#include"Audio.h"
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "DirectXGame/engine/base/WinApp/WinApp.h"

void Engine::Audio::Initialize()
{

	HRESULT hr;

	// XAudioエンジンのインスタンスを生成
	hr = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));
	// マスターボイスを生成
	hr = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(hr));
}

void Engine::Audio::Finalize()
{
	// 再生中の全てのソースボイスを破棄
	for (auto& kv : playingVoices) {
		if (kv.second) {
			kv.second->Stop();
			kv.second->FlushSourceBuffers();
			kv.second->DestroyVoice();
		}
	}
	playingVoices.clear();

	// サウンドデータ解放
	for (auto& kv : soundDatas) {
		SoundUnload(&kv.second);
	}
	soundDatas.clear();

	if (masterVoice) {
		masterVoice->DestroyVoice();
		masterVoice = nullptr;
	}
	xAudio2.Reset();
}

Engine::SoundData Engine::Audio::SoundLoadWave(const char* filename) {



	//HRESULT result;

	// ファイル入力ストリームのインスタンス
	std::ifstream file;
	// .wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	// ファイルオープン失敗を検出
	assert(file.is_open());

	////2
	// RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	// タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}
	// Formatチャンクの読み込み
	FormatChunk format = {};
	// チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	// チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	// Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	// JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		// 読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	// Dataチャンクのデータ部（波形データ）の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	// Waveファイルを閉じる
	file.close();


	////4
	// returnする為の音声データ
	SoundData soundData = {};
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;

	return soundData;
}

// 音声データ解放
void Engine::Audio::SoundUnload(SoundData* soundData)
{
	// バッファのメモリを解放
	delete[] soundData->pBuffer;
	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void Engine::Audio::SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData) {

	HRESULT result;

	// 波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	// 再生する波形データ設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	// 波形データ再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}

uint32_t Engine::Audio::LoadWave(const char* filename)
{
	SoundData sd = SoundLoadWave(filename);
	if (!sd.pBuffer || sd.bufferSize == 0) {
		return 0; // 失敗
	}
	uint32_t handle = nextHandle++;
	soundDatas.emplace(handle, sd);
	return handle;
}

void Engine::Audio::UnloadWave(uint32_t soundDataHandle)
{
	auto it = soundDatas.find(soundDataHandle);
	if (it == soundDatas.end()) return;
	SoundUnload(&it->second);
	soundDatas.erase(it);
}


void Engine::Audio::PlayWave(uint32_t soundDataHandle, bool loop, float volume)
{
	HRESULT hresult = S_FALSE;

	// 該当データが存在しなければ return
	auto it = soundDatas.find(soundDataHandle);
	if (it == soundDatas.end()) {
		return;
	}

	SoundData& data = it->second;

	// SourceVoice作成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	hresult = xAudio2->CreateSourceVoice(&pSourceVoice, &data.wfex);
	assert(SUCCEEDED(hresult));

	// バッファ設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = data.pBuffer;
	buf.AudioBytes = data.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	if (loop) {
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	// バッファ送信
	hresult = pSourceVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(hresult));

	// 音量設定（Start 前に）
	hresult = pSourceVoice->SetVolume(volume);
	assert(SUCCEEDED(hresult));

	// 再生開始
	hresult = pSourceVoice->Start();
	assert(SUCCEEDED(hresult));

	// 再生中リストに登録
	playingVoices[soundDataHandle] = pSourceVoice;
}

void Engine::Audio::StopWave(uint32_t soundDataHandle) {
	auto it = playingVoices.find(soundDataHandle);
	if (it == playingVoices.end()) return;

	it->second->Stop();

	IXAudio2SourceVoice* src = it->second;
	if (src) {
		src->Stop();
		src->FlushSourceBuffers();
		src->DestroyVoice();
	}
	playingVoices.erase(it);
}
