#pragma once

#include "AudioData.h"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>
#include <wrl.h>

namespace Engine {
	/// <summary>
	/// 音源の検索、ロード、再生、停止をゲーム全体で一元管理するクラス。
	/// </summary>
	class AudioManager {
	public:
		// XAudio2を初期化し、音声を再生できる状態にする。
		void Initialize(const std::string& directoryPath = "Resources/Sound/");

		// 再生Voiceと読み込み済み音源をすべて解放する。
		void Finalize();

		// 再生終了済みVoiceを検出して解放する。ゲームループから毎フレーム呼び出す。
		void Update();

		// Soundディレクトリを再走査し、音源一覧を最新状態へ更新する。
		void ReloadSoundFiles();

		// 指定したWAVファイルを読み込み、利用可能な音源ハンドルを返す。
		SoundHandle LoadWave(const std::string& filename);

		// 指定した音源を解放する。再生中の場合は該当するVoiceも停止する。
		void UnloadWave(SoundHandle soundHandle);

		// 指定音源を再生し、個別停止に使える再生ハンドルを返す。
		VoiceHandle Play(SoundHandle soundHandle, bool loop = false, float volume = 1.0f);

		// 保存用音IDから音源を検索して再生する。
		VoiceHandle Play(const std::string& soundId, bool loop = false, float volume = 1.0f);

		// 指定した1回の再生を停止してVoiceを解放する。
		void Stop(VoiceHandle voiceHandle);

		// 指定音源から開始されたすべての再生を停止する。
		void StopAll(SoundHandle soundHandle);

		// 現在再生しているすべてのVoiceを停止する。
		void StopAll();

		// エディタ表示に使用できる、読み込み済み音源情報の一覧を返す。
		const std::vector<SoundFileInfo>& GetSoundFiles() const { return soundFiles_; }

		// 保存用音IDに対応する音源情報を返す。見つからない場合はnullptrを返す。
		const SoundFileInfo* FindSound(const std::string& soundId) const;

		// 保存用音IDに対応するハンドルを返す。見つからない場合は0を返す。
		SoundHandle FindSoundHandle(const std::string& soundId) const;

		// 音源一覧の基準となるSoundディレクトリを返す。
		const std::string& GetDirectoryPath() const { return directoryPath_; }

	private:
		// 再生中のSourceVoiceと元音源の対応を保持する内部データ。
		struct PlayingVoice {
			IXAudio2SourceVoice* sourceVoice = nullptr; // XAudio2の再生Voice。
			SoundHandle soundHandle = 0;                // Voiceが参照している音源。
		};

		// WAVファイルをメモリへ読み込む。
		bool LoadWaveData(const std::string& filename, SoundData& soundData) const;

		// SoundDataが所有するPCMバッファを解放する。
		void UnloadSoundData(SoundData& soundData) const;

		// Voiceを停止し、安全に破棄する。
		void DestroyVoice(IXAudio2SourceVoice*& sourceVoice) const;

		// パス区切りと大文字小文字を統一して検索用音IDを生成する。
		std::string NormalizeSoundId(const std::string& soundId) const;

		Microsoft::WRL::ComPtr<IXAudio2> xAudio2_; // XAudio2エンジン本体。
		IXAudio2MasteringVoice* masterVoice_ = nullptr; // 全音声の出力先となるマスターボイス。
		std::unordered_map<SoundHandle, SoundData> soundDatas_; // ハンドル別の音源データ。
		std::unordered_map<VoiceHandle, PlayingVoice> playingVoices_; // 再生ハンドル別のVoice。
		std::vector<SoundFileInfo> soundFiles_; // エディタへ公開する音源一覧。
		std::unordered_map<std::string, SoundHandle> soundIdToHandle_; // 正規化IDから音源への索引。
		SoundHandle nextSoundHandle_ = 1; // 次に発行する音源ハンドル。
		VoiceHandle nextVoiceHandle_ = 1; // 次に発行する再生ハンドル。
		std::string directoryPath_ = "Resources/Sound/"; // 音源を検索する基準ディレクトリ。
	};

	/// <summary>
	/// 旧コードとの互換性を保つためのAudioManager派生クラス。
	/// </summary>
	class Audio : public AudioManager {
	};
}
