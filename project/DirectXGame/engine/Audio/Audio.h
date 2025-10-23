#pragma once
#include<cstdint>
#include <xaudio2.h>
#pragma comment(lib,"xaudio2.lib")
#include<fstream>
#include<sstream>
#include<wrl.h>
#include<cassert>
#include<unordered_map>

// チャンクヘッダ
struct ChunkHeader
{
	char id[4]; // チャンク毎のID
	int32_t size; // チャンクサイズ
};


// RIFFヘッダチャンク
struct RiffHeader
{
	ChunkHeader chunk; // "RIFF"
	char type[4];
};

// FMIチャンク
struct FormatChunk
{
	ChunkHeader chunk; // "fmt"
	WAVEFORMATEX fmt; // 波形フォーマット
};

// 音声データ
struct SoundData
{
	// 波形フォーマット
	WAVEFORMATEX wfex;
	// バッファの先頭アドレス
	BYTE* pBuffer;
	// バッファのサイズ
	unsigned int bufferSize;
};


// 前方宣言
class WinApp;
class DirectXCommon;

/// <summary>
/// 音クラス
/// </summary>
class Audio
{
public:
	Audio() = default;
	~Audio() {
		Finalize();
	};

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	// サウンドデータ取得
	SoundData SoundLoadWave(const char* filename);

	// サウンドアップロード
	void SoundUnload(SoundData* soundData);

	// 再生
	void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData);

	// 返り値: サウンドハンドル（0 は無効）
	uint32_t LoadWave(const char* filename);

	// アンロード
	void UnloadWave(uint32_t soundDataHandle);

	// 多重再生可能
	void PlayWave(uint32_t soundDataHandle, bool loop = false, float volume = 1.0f);

	// ストップ
	void StopWave(uint32_t soundDataHandle);
private:
	//////------音--------///////

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;


	std::unordered_map<uint32_t, SoundData> soundDatas;
	std::unordered_map<uint32_t, IXAudio2SourceVoice*> playingVoices;
	uint32_t nextHandle = 1;

	std::string directoryPath = "Resources/sound/";

};

