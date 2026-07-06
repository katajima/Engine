#pragma once

#include <cstdint>
#include <string>
#include <xaudio2.h>

#pragma comment(lib, "xaudio2.lib")

namespace Engine {
	// 音源データを識別するハンドル型。0 は無効な音源を表す。
	using SoundHandle = uint32_t;

	// 1回ごとの再生を識別するハンドル型。多重再生の個別停止に使用する。
	using VoiceHandle = uint32_t;

	// RIFFファイル内の各チャンクに共通するヘッダー情報。
	struct ChunkHeader {
		char id[4];       // チャンクの種類を表す4文字のID。
		int32_t size;     // チャンク本体のバイト数。
	};

	// WAVファイル先頭に格納されているRIFFヘッダー情報。
	struct RiffHeader {
		ChunkHeader chunk; // RIFFチャンクのヘッダー。
		char type[4];      // WAVでは「WAVE」が格納される。
	};

	// WAVファイルの再生フォーマットを保持するfmtチャンク情報。
	struct FormatChunk {
		ChunkHeader chunk; // fmtチャンクのヘッダー。
		WAVEFORMATEX fmt;  // XAudio2へ渡す波形フォーマット。
	};

	// メモリへ読み込んだ1つのWAV音源データ。
	struct SoundData {
		WAVEFORMATEX wfex{};          // 音源の波形フォーマット。
		BYTE* pBuffer = nullptr;       // PCMデータを保持するバッファ。
		unsigned int bufferSize = 0;   // PCMデータのバイト数。
	};

	// エディタの音一覧表示と保存に使用する音源情報。
	struct SoundFileInfo {
		SoundHandle handle = 0; // 実行中に音源へアクセスするためのハンドル。
		std::string id;         // Soundフォルダ基準の相対パス。保存用の安定したID。
		std::string name;       // 拡張子を含むファイル名。エディタ表示に使用する。
		std::string fullPath;   // 音源ロードに使用したファイルパス。
	};
}
