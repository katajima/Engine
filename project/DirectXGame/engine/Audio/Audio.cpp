#include "Audio.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstring>
#include <fstream>
#include <system_error>

namespace {
	// AudioManagerが直接読み込める拡張子か判定する。
	bool IsSupportedAudioFile(const std::filesystem::path& filePath) {
		std::string extension = filePath.extension().string();
		std::transform(extension.begin(), extension.end(), extension.begin(),
			[](unsigned char character) { return static_cast<char>(std::tolower(character)); });
		return extension == ".wav";
	}
}

void Engine::AudioManager::Initialize(const std::string& directoryPath) {
	// 二重初期化された場合にもVoiceや音源を残さないよう、先に既存状態を解放する。
	Finalize();
	directoryPath_ = directoryPath;

	// XAudio2本体と最終出力先のマスターボイスを生成する。
	HRESULT result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));
	if (FAILED(result)) {
		return;
	}

	result = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(result));
	if (FAILED(result)) {
		xAudio2_.Reset();
		return;
	}

}

void Engine::AudioManager::Finalize() {
	// SourceVoiceはXAudio2本体より先にすべて破棄する必要がある。
	StopAll();

	// 読み込んだPCMバッファをそれぞれ解放する。
	for (auto& [soundHandle, soundData] : soundDatas_) {
		(void)soundHandle;
		UnloadSoundData(soundData);
	}
	soundDatas_.clear();
	soundFiles_.clear();
	soundIdToHandle_.clear();

	// 出力VoiceとXAudio2エンジンを終了する。
	if (masterVoice_ != nullptr) {
		masterVoice_->DestroyVoice();
		masterVoice_ = nullptr;
	}
	xAudio2_.Reset();
	nextSoundHandle_ = 1;
	nextVoiceHandle_ = 1;
}

void Engine::AudioManager::Update() {
	// キュー内のバッファがなくなった非ループVoiceを再生終了として回収する。
	for (auto voiceIterator = playingVoices_.begin(); voiceIterator != playingVoices_.end();) {
		XAUDIO2_VOICE_STATE voiceState{};
		voiceIterator->second.sourceVoice->GetState(&voiceState);
		if (voiceState.BuffersQueued == 0) {
			DestroyVoice(voiceIterator->second.sourceVoice);
			voiceIterator = playingVoices_.erase(voiceIterator);
		} else {
			++voiceIterator;
		}
	}
}

void Engine::AudioManager::ReloadSoundFiles() {
	// 再走査前に既存のVoiceと音源を解放し、一覧と実データの対応を維持する。
	StopAll();
	for (auto& [soundHandle, soundData] : soundDatas_) {
		(void)soundHandle;
		UnloadSoundData(soundData);
	}
	soundDatas_.clear();
	soundFiles_.clear();
	soundIdToHandle_.clear();
	nextSoundHandle_ = 1;

	// フォルダが未作成でも起動を継続し、空の一覧として扱う。
	const std::filesystem::path rootPath(directoryPath_);
	std::error_code fileSystemError;
	if (!std::filesystem::exists(rootPath, fileSystemError)) {
		return;
	}

	// サブフォルダも含めてWAVファイルを収集する。
	std::vector<std::filesystem::path> filePaths;
	std::filesystem::recursive_directory_iterator iterator(
		rootPath, std::filesystem::directory_options::skip_permission_denied, fileSystemError);
	const std::filesystem::recursive_directory_iterator endIterator;
	for (; iterator != endIterator; iterator.increment(fileSystemError)) {
		if (fileSystemError) {
			fileSystemError.clear();
			continue;
		}
		if (iterator->is_regular_file(fileSystemError) && IsSupportedAudioFile(iterator->path())) {
			filePaths.push_back(iterator->path());
		}
	}

	// OSの列挙順に依存しない一覧にして、コンボボックスの並び順を安定させる。
	std::sort(filePaths.begin(), filePaths.end(),
		[](const std::filesystem::path& left, const std::filesystem::path& right) {
			return left.generic_string() < right.generic_string();
		});

	// 各ファイルをロードし、保存用IDと実行時ハンドルを関連付ける。
	for (const std::filesystem::path& filePath : filePaths) {
		const SoundHandle soundHandle = LoadWave(filePath.string());
		if (soundHandle == 0) {
			continue;
		}

		SoundFileInfo fileInfo{};
		fileInfo.handle = soundHandle;
		fileInfo.id = std::filesystem::relative(filePath, rootPath, fileSystemError).generic_string();
		if (fileSystemError) {
			fileSystemError.clear();
			fileInfo.id = filePath.filename().generic_string();
		}
		fileInfo.name = filePath.filename().string();
		fileInfo.fullPath = filePath.generic_string();

		soundIdToHandle_[NormalizeSoundId(fileInfo.id)] = soundHandle;
		soundFiles_.push_back(std::move(fileInfo));
	}
}

Engine::SoundHandle Engine::AudioManager::LoadWave(const std::string& filename) {
	// 初期化前、または読み込みに失敗したファイルには無効ハンドルを返す。
	if (xAudio2_ == nullptr) {
		return 0;
	}

	SoundData soundData{};
	if (!LoadWaveData(filename, soundData)) {
		return 0;
	}

	// 0を無効値として予約し、正常な音源には1以上のハンドルを割り当てる。
	const SoundHandle soundHandle = nextSoundHandle_++;
	soundDatas_.emplace(soundHandle, soundData);
	return soundHandle;
}

void Engine::AudioManager::UnloadWave(SoundHandle soundHandle) {
	// 音源バッファを解放する前に、それを参照するVoiceをすべて停止する。
	StopAll(soundHandle);
	auto soundIterator = soundDatas_.find(soundHandle);
	if (soundIterator == soundDatas_.end()) {
		return;
	}

	UnloadSoundData(soundIterator->second);
	soundDatas_.erase(soundIterator);

	// 公開一覧とID索引からも同じ音源を削除する。
	std::erase_if(soundFiles_, [soundHandle](const SoundFileInfo& fileInfo) {
		return fileInfo.handle == soundHandle;
	});
	for (auto idIterator = soundIdToHandle_.begin(); idIterator != soundIdToHandle_.end();) {
		if (idIterator->second == soundHandle) {
			idIterator = soundIdToHandle_.erase(idIterator);
		} else {
			++idIterator;
		}
	}
}

Engine::VoiceHandle Engine::AudioManager::Play(SoundHandle soundHandle, bool loop, float volume) {
	// 指定音源が存在しない場合は再生せず、無効な再生ハンドルを返す。
	auto soundIterator = soundDatas_.find(soundHandle);
	if (soundIterator == soundDatas_.end() || xAudio2_ == nullptr) {
		return 0;
	}

	// 音量はXAudio2で扱いやすい0以上の値に制限する。
	volume = (std::max)(volume, 0.0f);
	IXAudio2SourceVoice* sourceVoice = nullptr;
	HRESULT result = xAudio2_->CreateSourceVoice(&sourceVoice, &soundIterator->second.wfex);
	if (FAILED(result)) {
		return 0;
	}

	// ループ指定時だけ無限ループを設定し、通常再生はバッファ末尾で終了させる。
	XAUDIO2_BUFFER audioBuffer{};
	audioBuffer.pAudioData = soundIterator->second.pBuffer;
	audioBuffer.AudioBytes = soundIterator->second.bufferSize;
	audioBuffer.Flags = XAUDIO2_END_OF_STREAM;
	audioBuffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;

	result = sourceVoice->SubmitSourceBuffer(&audioBuffer);
	if (SUCCEEDED(result)) {
		result = sourceVoice->SetVolume(volume);
	}
	if (SUCCEEDED(result)) {
		result = sourceVoice->Start();
	}
	if (FAILED(result)) {
		DestroyVoice(sourceVoice);
		return 0;
	}

	// 同じ音源の多重再生を個別に管理できるよう、再生ごとにハンドルを発行する。
	const VoiceHandle voiceHandle = nextVoiceHandle_++;
	playingVoices_.emplace(voiceHandle, PlayingVoice{ sourceVoice, soundHandle });
	return voiceHandle;
}

Engine::VoiceHandle Engine::AudioManager::Play(const std::string& soundId, bool loop, float volume) {
	// JSON等へ保存された相対パスIDを実行時ハンドルへ変換して再生する。
	return Play(FindSoundHandle(soundId), loop, volume);
}

void Engine::AudioManager::Stop(VoiceHandle voiceHandle) {
	// 指定された1回分のVoiceだけを停止・破棄する。
	auto voiceIterator = playingVoices_.find(voiceHandle);
	if (voiceIterator == playingVoices_.end()) {
		return;
	}

	DestroyVoice(voiceIterator->second.sourceVoice);
	playingVoices_.erase(voiceIterator);
}

void Engine::AudioManager::StopAll(SoundHandle soundHandle) {
	// 同じ音源から作られたVoiceをすべて検索して破棄する。
	for (auto voiceIterator = playingVoices_.begin(); voiceIterator != playingVoices_.end();) {
		if (voiceIterator->second.soundHandle == soundHandle) {
			DestroyVoice(voiceIterator->second.sourceVoice);
			voiceIterator = playingVoices_.erase(voiceIterator);
		} else {
			++voiceIterator;
		}
	}
}

void Engine::AudioManager::StopAll() {
	// XAudio2終了時にも使えるよう、全Voiceを確実に破棄してコンテナを空にする。
	for (auto& [voiceHandle, playingVoice] : playingVoices_) {
		(void)voiceHandle;
		DestroyVoice(playingVoice.sourceVoice);
	}
	playingVoices_.clear();
}

const Engine::SoundFileInfo* Engine::AudioManager::FindSound(const std::string& soundId) const {
	// ID検索後、公開一覧から対応する詳細情報を返す。
	const SoundHandle soundHandle = FindSoundHandle(soundId);
	for (const SoundFileInfo& fileInfo : soundFiles_) {
		if (fileInfo.handle == soundHandle) {
			return &fileInfo;
		}
	}
	return nullptr;
}

Engine::SoundHandle Engine::AudioManager::FindSoundHandle(const std::string& soundId) const {
	// 大文字小文字と区切り文字の差を吸収して保存済みIDを検索する。
	const auto idIterator = soundIdToHandle_.find(NormalizeSoundId(soundId));
	return idIterator != soundIdToHandle_.end() ? idIterator->second : 0;
}

bool Engine::AudioManager::LoadWaveData(const std::string& filename, SoundData& soundData) const {
	// WAVをバイナリで開き、最低限必要なRIFF/WAVE構造を検証する。
	std::ifstream file(filename, std::ios_base::binary);
	if (!file.is_open()) {
		return false;
	}

	RiffHeader riffHeader{};
	file.read(reinterpret_cast<char*>(&riffHeader), sizeof(riffHeader));
	if (!file || std::memcmp(riffHeader.chunk.id, "RIFF", 4) != 0 ||
		std::memcmp(riffHeader.type, "WAVE", 4) != 0) {
		return false;
	}

	// fmtとdataを順不同のチャンクから探し、未知チャンクは読み飛ばす。
	WAVEFORMATEX waveFormat{};
	bool foundFormat = false;
	std::vector<BYTE> audioBytes;
	while (file && (!foundFormat || audioBytes.empty())) {
		ChunkHeader chunkHeader{};
		file.read(reinterpret_cast<char*>(&chunkHeader), sizeof(chunkHeader));
		if (!file || chunkHeader.size < 0) {
			break;
		}

		if (std::memcmp(chunkHeader.id, "fmt ", 4) == 0) {
			// WAVEFORMATEXへ収まる範囲だけ読み、拡張領域があれば後ろを読み飛ばす。
			const std::streamsize readSize = std::min<std::streamsize>(chunkHeader.size, sizeof(waveFormat));
			file.read(reinterpret_cast<char*>(&waveFormat), readSize);
			if (chunkHeader.size > readSize) {
				file.seekg(chunkHeader.size - readSize, std::ios_base::cur);
			}
			foundFormat = static_cast<bool>(file);
		} else if (std::memcmp(chunkHeader.id, "data", 4) == 0) {
			// PCMデータは一時vectorへ読み込み、検証成功後にSoundDataへ所有権を移す。
			audioBytes.resize(static_cast<size_t>(chunkHeader.size));
			file.read(reinterpret_cast<char*>(audioBytes.data()), chunkHeader.size);
		} else {
			// LISTやJUNKなど、再生に不要なチャンクを読み飛ばす。
			file.seekg(chunkHeader.size, std::ios_base::cur);
		}

		// RIFFチャンクは偶数バイト境界へパディングされる。
		if (chunkHeader.size % 2 != 0) {
			file.seekg(1, std::ios_base::cur);
		}
	}

	if (!foundFormat || audioBytes.empty() || !file) {
		return false;
	}

	// 読み込み成功後だけヒープを確保し、呼び出し側へ音源データを返す。
	soundData.wfex = waveFormat;
	soundData.bufferSize = static_cast<unsigned int>(audioBytes.size());
	soundData.pBuffer = new BYTE[soundData.bufferSize];
	std::copy(audioBytes.begin(), audioBytes.end(), soundData.pBuffer);
	return true;
}

void Engine::AudioManager::UnloadSoundData(SoundData& soundData) const {
	// 所有するPCMバッファを解放し、二重解放されない空状態へ戻す。
	delete[] soundData.pBuffer;
	soundData.pBuffer = nullptr;
	soundData.bufferSize = 0;
	soundData.wfex = {};
}

void Engine::AudioManager::DestroyVoice(IXAudio2SourceVoice*& sourceVoice) const {
	// nullptrを許容し、停止・キュー破棄・Voice破棄を一か所で行う。
	if (sourceVoice == nullptr) {
		return;
	}
	sourceVoice->Stop();
	sourceVoice->FlushSourceBuffers();
	sourceVoice->DestroyVoice();
	sourceVoice = nullptr;
}

std::string Engine::AudioManager::NormalizeSoundId(const std::string& soundId) const {
	// WindowsとJSONで区切り文字が異なっても同じ音源として検索できるよう統一する。
	std::string normalizedId = soundId;
	std::replace(normalizedId.begin(), normalizedId.end(), '\\', '/');
	std::transform(normalizedId.begin(), normalizedId.end(), normalizedId.begin(),
		[](unsigned char character) { return static_cast<char>(std::tolower(character)); });
	return normalizedId;
}
