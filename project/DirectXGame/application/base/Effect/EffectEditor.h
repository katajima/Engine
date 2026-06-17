#pragma once
#include <DirectXGame/engine/Effect/EffectComponent.h>
#include <DirectXGame/engine/Effect/EffectGlovalData.h>

#include <array>

/// <summary>
/// effectエディタークラス
/// </summary>
class EffectEditor {
public:

	// 初期化
	void Initialize(Engine::EffectComponent* effectComponent, Engine::GlobalVariables* globalVariables);

	// 更新
	void Update(float dt);
	// 初期登録やUI追加で使うエフェクトデータを登録
	void AddEffectGlobalData(const std::string& name, const std::string& particleName);
	// 保存データを実際のエミッターへ反映
	void SetEffectGlobalData(const std::string& name, EmitterShapeType shapeType, const EffectGlobalData& data);

	// 保存済みレジストリにあるUI追加エフェクトを読み込む
	void LoadRegisteredEffectGlobalDatas();
	// 保存済みレジストリにあるUI追加パーティクル群を読み込む
	void LoadRegisteredParticleGroups();

	std::map<std::string, EffectGlobalData> GetEffectGlobalDatas() const { return effectGlobalDatas_; };
private:
	// エフェクトの追加、名前変更、削除を行う管理UI
	void DrawEffectManagement();
	// 新しいエフェクト名が使用できるか検証
	bool ValidateNewEffectName(const std::string& effectName);
	// UI入力から新しいエフェクトを追加
	void AddEffectFromEditor();
	// 選択中のエフェクト名を変更
	void RenameEffect(const std::string& oldName, const std::string& newName);
	// 選択中のエフェクトを削除
	void DeleteEffect(const std::string& effectName);
	// エフェクト一覧の保存グループへ登録
	void RegisterEffectName(const std::string& name);
	// エフェクト一覧の保存グループから登録解除
	void UnregisterEffectName(const std::string& name);
	// 削除済みリストへ登録し、初期登録で復活しないようにする
	void RegisterDeletedEffectName(const std::string& name);
	// 削除済みリストから外し、同名再追加を許可する
	void UnregisterDeletedEffectName(const std::string& name);
	// 削除済みとして保存されているか確認
	bool IsDeletedEffectName(const std::string& name) const;
	// パーティクル群の追加、名前変更、削除、編集を行う管理UI
	void DrawParticleGroupEditor();
	// パーティクル群の詳細設定を描画
	void DrawParticleGroupDetail(const std::string& particleName, Engine::ParticleGroup& group);
	// 新しいパーティクル群名が使用できるか検証
	bool ValidateNewParticleGroupName(const std::string& particleName);
	// UI入力から新しいパーティクル群を追加
	void AddParticleGroupFromEditor();
	// 選択中のパーティクル群名を変更
	void RenameParticleGroup(const std::string& oldName, const std::string& newName);
	// 選択中のパーティクル群を削除
	void DeleteParticleGroup(const std::string& particleName);
	// パーティクル群メタデータを保存項目へ追加
	void AddParticleGroupItem(const std::string& particleName, const Engine::ParticleGroupEditorData& data);
	// パーティクル群メタデータを保存項目から取得
	void GetParticleGroupValue(const std::string& particleName, Engine::ParticleGroupEditorData& data);
	// パーティクル群メタデータを保存項目へ反映
	void SetParticleGroupValue(const std::string& particleName, const Engine::ParticleGroupEditorData& data);
	// パーティクル群一覧の保存グループへ登録
	void RegisterParticleGroupName(const std::string& name);
	// パーティクル群一覧の保存グループから登録解除
	void UnregisterParticleGroupName(const std::string& name);
	// 削除済みパーティクル群リストへ登録
	void RegisterDeletedParticleGroupName(const std::string& name);
	// 削除済みパーティクル群リストから解除
	void UnregisterDeletedParticleGroupName(const std::string& name);
	// 削除済みパーティクル群として保存されているか確認
	bool IsDeletedParticleGroupName(const std::string& name) const;
	// パーティクル名変更を参照中のエフェクトへ反映
	void RenameParticleReferences(const std::string& oldName, const std::string& newName);
	// 保存項目に追加
	void AddItem(const std::string& name, const EffectGlobalData& data);
	// 保存項目のデータを取得
	void GetValue(const std::string& name, EffectGlobalData& data);
	// 保存項目設定
	void SetValue(const std::string& name, const EffectGlobalData& data);

private:

	void AAAA(const std::string& name,EffectGlobalData& data);

	void Emit(const std::string& name, const Vector3& pos);
private:// エフェクトのグローバルデータ
	std::map<std::string, EffectGlobalData> effectGlobalDatas_;
	// 選択中のエフェクトエディターブロック名
	std::string selectedBlockName_;
	// 新規追加するエフェクト名入力
	std::array<char, 128> newEffectNameBuffer_{};
	// 名前変更後のエフェクト名入力
	std::array<char, 128> renameEffectNameBuffer_{};
	// 新規追加時に紐づけるパーティクル名
	std::string newEffectParticleName_;
	// 名前変更確認中の古いエフェクト名
	std::string pendingRenameEffectName_;
	// 削除確認中のエフェクト名
	std::string pendingDeleteEffectName_;
	// 管理UIの結果やエラー文
	std::string managementMessage_;
	// 選択中のパーティクル群名
	std::string selectedParticleGroupName_;
	// 新規追加するパーティクル群名入力
	std::array<char, 128> newParticleGroupNameBuffer_{};
	// 名前変更後のパーティクル群名入力
	std::array<char, 128> renameParticleGroupNameBuffer_{};
	// 新規追加するパーティクル群のテクスチャ入力
	std::array<char, 256> newParticleTexturePathBuffer_{};
	// 編集中パーティクル群のテクスチャ入力
	std::array<char, 256> editParticleTexturePathBuffer_{};
	// 名前変更確認中の古いパーティクル群名
	std::string pendingRenameParticleGroupName_;
	// 削除確認中のパーティクル群名
	std::string pendingDeleteParticleGroupName_;
	// パーティクル群管理UIの結果やエラー文
	std::string particleManagementMessage_;
	// 新規追加時のパーティクル群メタデータ
	Engine::ParticleGroupEditorData newParticleGroupData_{};
private:
	// 出現させるか
	bool isSpawnEmit = false;
	// デバッグエミッター位置
	Vector3 spawnEmitPos{};
	// 頻度
	float frequency = 0.2f;
	// 時間
	float timer = 0.0f;
private:
	// エフェクトコンポーネント
	Engine::EffectComponent* effectComponent = nullptr;
	// グローバル変数保存
	Engine::GlobalVariables* globalVariables = nullptr;
};
