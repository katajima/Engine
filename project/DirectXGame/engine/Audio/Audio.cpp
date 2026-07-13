#include "Audio.h"
#include "DirectXGame/engine/Base/Logger.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstring>
#include <fstream>
#include <format>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <limits>
#include <system_error>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

namespace {
	// AudioManagerが直接読み込める拡張子か判定する。
	bool IsSupportedAudioFile(const std::filesystem::path& filePath) {
		std::string extension = filePath.extension().string();
		std::transform(extension.begin(), extension.end(), extension.begin(),
			[](unsigned char character) { return static_cast<char>(std::tolower(character)); });
		return extension == ".wav" || extension == ".mp3";
	}

	// 音声処理のログへ共通プレフィックスと改行を付ける。
	void AudioLog(const std::string& message) {
		Logger::Log("[Audio] " + message + "\n");
	}
}

Engine::AudioManager::~AudioManager()
{
	// Frameworkから明示Finalizeされなかった場合でもVoiceとPCMバッファを解放する
	Finalize();
}

void Engine::AudioManager::Initialize(const std::string& directoryPath) {
	// 二重初期化された場合にもVoiceや音源を残さないよう、先に既存状態を解放する。
	Finalize();
	directoryPath_ = directoryPath;
	AudioLog(std::format("Initialize begin. directory={}", directoryPath_));

	// MP3デコードに使用するMedia FoundationをAudioManagerの寿命に合わせて開始する。
	HRESULT result = MFStartup(MF_VERSION);
	if (FAILED(result)) {
		AudioLog(std::format("ERROR: MFStartup failed. hr=0x{:08X}", static_cast<unsigned long>(result)));
		return;
	}
	isMediaFoundationStarted_ = true;
	AudioLog("Media Foundation created for MP3 decoding.");

	// XAudio2本体と最終出力先のマスターボイスを生成する。
	result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));
	if (FAILED(result)) {
		AudioLog(std::format("ERROR: XAudio2Create failed. hr=0x{:08X}", static_cast<unsigned long>(result)));
		return;
	}
	AudioLog("XAudio2 engine created.");

	result = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(result));
	if (FAILED(result)) {
		AudioLog(std::format("ERROR: MasteringVoice creation failed. hr=0x{:08X}", static_cast<unsigned long>(result)));
		xAudio2_.Reset();
		return;
	}
	AudioLog("MasteringVoice created.");

}

void Engine::AudioManager::Finalize() {
	const bool hadAudioSystem = xAudio2_ != nullptr || isMediaFoundationStarted_;
	if (hadAudioSystem) {
		AudioLog(std::format("Finalize begin. sounds={}, voices={}", soundDatas_.size(), playingVoices_.size()));
	}
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
	// AudioManagerで開始したMedia Foundationだけを対になる呼び出しで終了する。
	if (isMediaFoundationStarted_) {
		MFShutdown();
		isMediaFoundationStarted_ = false;
		AudioLog("Media Foundation shut down.");
	}
	nextSoundHandle_ = 1;
	nextVoiceHandle_ = 1;
	if (hadAudioSystem) {
		AudioLog("Finalize complete.");
	}
}

void Engine::AudioManager::Update() {
	// キュー内のバッファがなくなった非ループVoiceを再生終了として回収する。
	for (auto voiceIterator = playingVoices_.begin(); voiceIterator != playingVoices_.end();) {
		XAUDIO2_VOICE_STATE voiceState{};
		voiceIterator->second.sourceVoice->GetState(&voiceState);
		if (voiceState.BuffersQueued == 0) {
			AudioLog(std::format("Playback completed. voiceHandle={}, soundHandle={}",
				voiceIterator->first, voiceIterator->second.soundHandle));
			DestroyVoice(voiceIterator->second.sourceVoice);
			voiceIterator = playingVoices_.erase(voiceIterator);
		} else {
			++voiceIterator;
		}
	}
}

void Engine::AudioManager::ReloadSoundFiles() {
	AudioLog(std::format("Sound directory scan begin. directory={}", directoryPath_));
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
		AudioLog(std::format("WARNING: Sound directory was not found. directory={}", directoryPath_));
		return;
	}

	// サブフォルダも含めてWAVとMP3ファイルを収集する。
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
	AudioLog(std::format("Sound directory scan complete. discovered={}, loaded={}",
		filePaths.size(), soundFiles_.size()));
}

Engine::SoundHandle Engine::AudioManager::LoadWave(const std::string& filename) {
	// 初期化前、または読み込みに失敗したファイルには無効ハンドルを返す。
	if (xAudio2_ == nullptr) {
		AudioLog(std::format("ERROR: Load requested before XAudio2 initialization. file={}", filename));
		return 0;
	}

	SoundData soundData{};
	AudioLog(std::format("Load begin. file={}", filename));
	if (!LoadAudioData(filename, soundData)) {
		AudioLog(std::format("ERROR: Load failed. file={}", filename));
		return 0;
	}

	// 0を無効値として予約し、正常な音源には1以上のハンドルを割り当てる。
	const SoundHandle soundHandle = nextSoundHandle_++;
	soundDatas_.emplace(soundHandle, soundData);
	AudioLog(std::format("Load succeeded. handle={}, bytes={}, channels={}, sampleRate={}, file={}",
		soundHandle, soundData.bufferSize, soundData.wfex.nChannels,
		soundData.wfex.nSamplesPerSec, filename));
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
	AudioLog(std::format("Sound unloaded. soundHandle={}", soundHandle));

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
		AudioLog(std::format("WARNING: Play rejected. invalid soundHandle={}", soundHandle));
		return 0;
	}

	// 音量はXAudio2で扱いやすい0以上の値に制限する。
	volume = (std::max)(volume, 0.0f);
	IXAudio2SourceVoice* sourceVoice = nullptr;
	HRESULT result = xAudio2_->CreateSourceVoice(&sourceVoice, &soundIterator->second.wfex);
	if (FAILED(result)) {
		AudioLog(std::format("ERROR: SourceVoice creation failed. soundHandle={}, hr=0x{:08X}",
			soundHandle, static_cast<unsigned long>(result)));
		return 0;
	}
	AudioLog(std::format("SourceVoice created. soundHandle={}", soundHandle));

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
		AudioLog(std::format("ERROR: Playback setup failed. soundHandle={}, hr=0x{:08X}",
			soundHandle, static_cast<unsigned long>(result)));
		DestroyVoice(sourceVoice);
		return 0;
	}

	// 同じ音源の多重再生を個別に管理できるよう、再生ごとにハンドルを発行する。
	const VoiceHandle voiceHandle = nextVoiceHandle_++;
	playingVoices_.emplace(voiceHandle, PlayingVoice{ sourceVoice, soundHandle });
	AudioLog(std::format("Playback started. voiceHandle={}, soundHandle={}, loop={}, volume={:.2f}",
		voiceHandle, soundHandle, loop, volume));
	return voiceHandle;
}

Engine::VoiceHandle Engine::AudioManager::Play(const std::string& soundId, bool loop, float volume) {
	// JSON等へ保存された相対パスIDを実行時ハンドルへ変換して再生する。
	const SoundHandle soundHandle = FindSoundHandle(soundId);
	if (soundHandle == 0) {
		AudioLog(std::format("WARNING: Sound ID was not found. id={}", soundId));
		return 0;
	}
	AudioLog(std::format("Play requested by ID. id={}, soundHandle={}", soundId, soundHandle));
	return Play(soundHandle, loop, volume);
}

void Engine::AudioManager::Stop(VoiceHandle voiceHandle) {
	// 指定された1回分のVoiceだけを停止・破棄する。
	auto voiceIterator = playingVoices_.find(voiceHandle);
	if (voiceIterator == playingVoices_.end()) {
		return;
	}

	DestroyVoice(voiceIterator->second.sourceVoice);
	playingVoices_.erase(voiceIterator);
	AudioLog(std::format("Playback stopped. voiceHandle={}", voiceHandle));
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

bool Engine::AudioManager::LoadAudioData(const std::string& filename, SoundData& soundData) const {
	// 拡張子を小文字へ揃え、対応するデコーダーへ処理を振り分ける。
	std::string extension = std::filesystem::path(filename).extension().string();
	std::transform(extension.begin(), extension.end(), extension.begin(),
		[](unsigned char character) { return static_cast<char>(std::tolower(character)); });
	if (extension == ".wav") {
		return LoadWaveData(filename, soundData);
	}
	if (extension == ".mp3") {
		return LoadMp3Data(filename, soundData);
	}

	AudioLog(std::format("ERROR: Unsupported audio extension. extension={}, file={}", extension, filename));
	return false;
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

bool Engine::AudioManager::LoadMp3Data(const std::string& filename, SoundData& soundData) const {
	// MP3デコード基盤が開始できていない場合は、安全にロード失敗として扱う。
	if (!isMediaFoundationStarted_) {
		AudioLog(std::format("ERROR: MP3 decode requested before Media Foundation startup. file={}", filename));
		return false;
	}

	// Media Foundationへ渡すため、相対パスを絶対ワイド文字パスへ変換する。
	std::error_code pathError;
	const std::filesystem::path absolutePath = std::filesystem::absolute(filename, pathError);
	if (pathError) {
		AudioLog(std::format("ERROR: Failed to resolve MP3 path. file={}", filename));
		return false;
	}

	Microsoft::WRL::ComPtr<IMFSourceReader> sourceReader;
	HRESULT result = MFCreateSourceReaderFromURL(absolutePath.c_str(), nullptr, &sourceReader);
	if (FAILED(result)) {
		AudioLog(std::format("ERROR: MP3 SourceReader creation failed. hr=0x{:08X}, file={}",
			static_cast<unsigned long>(result), filename));
		return false;
	}
	AudioLog(std::format("MP3 SourceReader created. file={}", filename));

	// 出力形式を非圧縮PCMへ指定し、既存のXAudio2再生経路で扱えるようにする。
	Microsoft::WRL::ComPtr<IMFMediaType> requestedMediaType;
	result = MFCreateMediaType(&requestedMediaType);
	if (SUCCEEDED(result)) {
		result = requestedMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	}
	if (SUCCEEDED(result)) {
		result = requestedMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	}
	if (SUCCEEDED(result)) {
		result = sourceReader->SetCurrentMediaType(
			MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, requestedMediaType.Get());
	}
	if (FAILED(result)) {
		AudioLog(std::format("ERROR: MP3 PCM output format setup failed. hr=0x{:08X}, file={}",
			static_cast<unsigned long>(result), filename));
		return false;
	}

	// SourceReaderが決定したチャンネル数やサンプルレートをXAudio2形式へ変換する。
	Microsoft::WRL::ComPtr<IMFMediaType> decodedMediaType;
	result = sourceReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &decodedMediaType);
	WAVEFORMATEX* decodedWaveFormat = nullptr;
	UINT32 waveFormatSize = 0;
	if (SUCCEEDED(result)) {
		result = MFCreateWaveFormatExFromMFMediaType(
			decodedMediaType.Get(), &decodedWaveFormat, &waveFormatSize);
	}
	if (FAILED(result) || decodedWaveFormat == nullptr) {
		AudioLog(std::format("ERROR: MP3 wave format conversion failed. hr=0x{:08X}, file={}",
			static_cast<unsigned long>(result), filename));
		return false;
	}
	soundData.wfex = *decodedWaveFormat;
	CoTaskMemFree(decodedWaveFormat);

	// 全サンプルを順番に読み、1つの連続PCMバッファへ結合する。
	std::vector<BYTE> decodedBytes;
	while (true) {
		DWORD actualStreamIndex = 0;
		DWORD streamFlags = 0;
		LONGLONG timestamp = 0;
		Microsoft::WRL::ComPtr<IMFSample> sample;
		result = sourceReader->ReadSample(
			MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &actualStreamIndex,
			&streamFlags, &timestamp, &sample);
		if (FAILED(result)) {
			AudioLog(std::format("ERROR: MP3 sample read failed. hr=0x{:08X}, file={}",
				static_cast<unsigned long>(result), filename));
			return false;
		}
		if ((streamFlags & MF_SOURCE_READERF_ENDOFSTREAM) != 0) {
			break;
		}
		if (!sample) {
			continue;
		}

		Microsoft::WRL::ComPtr<IMFMediaBuffer> mediaBuffer;
		result = sample->ConvertToContiguousBuffer(&mediaBuffer);
		if (FAILED(result)) {
			AudioLog(std::format("ERROR: MP3 sample buffer conversion failed. hr=0x{:08X}, file={}",
				static_cast<unsigned long>(result), filename));
			return false;
		}

		BYTE* bufferData = nullptr;
		DWORD currentLength = 0;
		result = mediaBuffer->Lock(&bufferData, nullptr, &currentLength);
		if (FAILED(result)) {
			AudioLog(std::format("ERROR: MP3 sample buffer lock failed. hr=0x{:08X}, file={}",
				static_cast<unsigned long>(result), filename));
			return false;
		}
		decodedBytes.insert(decodedBytes.end(), bufferData, bufferData + currentLength);
		mediaBuffer->Unlock();
	}

	// XAudio2のバッファサイズ型へ収まらない異常に大きい音源はロードしない。
	if (decodedBytes.empty() || decodedBytes.size() > (std::numeric_limits<unsigned int>::max)()) {
		AudioLog(std::format("ERROR: MP3 decoded buffer is empty or too large. file={}", filename));
		soundData.wfex = {};
		return false;
	}

	soundData.bufferSize = static_cast<unsigned int>(decodedBytes.size());
	soundData.pBuffer = new BYTE[soundData.bufferSize];
	std::copy(decodedBytes.begin(), decodedBytes.end(), soundData.pBuffer);
	AudioLog(std::format("MP3 decoded to PCM. bytes={}, channels={}, sampleRate={}, file={}",
		soundData.bufferSize, soundData.wfex.nChannels, soundData.wfex.nSamplesPerSec, filename));
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
