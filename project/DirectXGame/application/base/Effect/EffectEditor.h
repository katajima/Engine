#pragma once
#include <DirectXGame/engine/Effect/EffectComponent.h>
#include <DirectXGame/engine/Effect/EffectGlovalData.h>
#include "DirectXGame/application/base/Effect/EffectEditorSerializer.h"

#include <array>

/// <summary>
/// エフェクトとパーティクル群の追加、編集、削除、保存を行うデバッグエディターです。
/// </summary>
class EffectEditor {
public:

	/// <summary>
	/// エフェクト編集に必要な参照と保存ヘルパーを初期化します。
	/// </summary>
	/// <param name="effectComponent">編集対象のエフェクトコンポーネントです。所有権は受け取りません。</param>
	/// <param name="globalVariables">保存・読み込みに使うグローバル変数管理です。所有権は受け取りません。</param>
	void Initialize(Engine::EffectComponent* effectComponent, Engine::GlobalVariables* globalVariables);

	/// <summary>
	/// エディターUI、デバッグ発生、保存対象データを更新します。
	/// </summary>
	/// <param name="dt">前フレームからの経過時間です。単位は秒です。</param>
	void Update(float dt);

	/// <summary>
	/// 初期登録やUI追加で使うエフェクトデータを登録します。
	/// </summary>
	/// <param name="name">登録するエフェクト名です。既存名の場合は既存データを優先します。</param>
	/// <param name="particleName">エフェクトが参照するパーティクル群名です。</param>
	void AddEffectGlobalData(const std::string& name, const std::string& particleName);

	/// <summary>
	/// 保存データを実際のエミッターへ反映します。
	/// </summary>
	/// <param name="name">反映先のエフェクト名です。</param>
	/// <param name="shapeType">エミッターの形状種別です。</param>
	/// <param name="data">反映するエフェクト設定です。</param>
	void SetEffectGlobalData(const std::string& name, EmitterShapeType shapeType, const EffectGlobalData& data);

	/// <summary>
	/// 保存済みレジストリにあるUI追加エフェクトを読み込み、エディターに復元します。
	/// </summary>
	void LoadRegisteredEffectGlobalDatas();

	/// <summary>
	/// 保存済みレジストリにあるUI追加パーティクル群を読み込み、エフェクトコンポーネントに復元します。
	/// </summary>
	void LoadRegisteredParticleGroups();

	/// <summary>
	/// エディターが保持しているエフェクト設定一覧を取得します。
	/// </summary>
	/// <returns>エフェクト名をキーにした設定マップのコピーを返します。</returns>
	std::map<std::string, EffectGlobalData> GetEffectGlobalDatas() const { return effectGlobalDatas_; };
private:
	/// <summary>
	/// エフェクトの追加、名前変更、削除を行う管理UIを描画します。
	/// </summary>
	void DrawEffectManagement();

	/// <summary>
	/// 新しいエフェクト名が使用できるか検証します。
	/// </summary>
	/// <param name="effectName">検証するエフェクト名です。</param>
	/// <returns>空文字や重複がなく使用できる場合はtrue、それ以外はfalseです。</returns>
	bool ValidateNewEffectName(const std::string& effectName);

	/// <summary>
	/// UI入力内容から新しいエフェクトを追加し、保存レジストリへ登録します。
	/// </summary>
	void AddEffectFromEditor();

	/// <summary>
	/// 選択中のエフェクト名を変更し、保存レジストリを更新します。
	/// </summary>
	/// <param name="oldName">変更前のエフェクト名です。</param>
	/// <param name="newName">変更後のエフェクト名です。</param>
	void RenameEffect(const std::string& oldName, const std::string& newName);

	/// <summary>
	/// 選択中のエフェクトを削除し、初期登録で復活しないよう削除済みリストへ記録します。
	/// </summary>
	/// <param name="effectName">削除するエフェクト名です。</param>
	void DeleteEffect(const std::string& effectName);

	/// <summary>
	/// エフェクト一覧の保存グループへ名前を登録します。
	/// </summary>
	/// <param name="name">登録するエフェクト名です。</param>
	void RegisterEffectName(const std::string& name);

	/// <summary>
	/// エフェクト一覧の保存グループから名前を登録解除します。
	/// </summary>
	/// <param name="name">登録解除するエフェクト名です。</param>
	void UnregisterEffectName(const std::string& name);

	/// <summary>
	/// 削除済みリストへ登録し、初期登録処理で同名エフェクトが復活しないようにします。
	/// </summary>
	/// <param name="name">削除済みとして扱うエフェクト名です。</param>
	void RegisterDeletedEffectName(const std::string& name);

	/// <summary>
	/// 削除済みリストから外し、同名エフェクトの再追加を許可します。
	/// </summary>
	/// <param name="name">削除済み扱いを解除するエフェクト名です。</param>
	void UnregisterDeletedEffectName(const std::string& name);

	/// <summary>
	/// 指定名が削除済みとして保存されているか確認します。
	/// </summary>
	/// <param name="name">確認するエフェクト名です。</param>
	/// <returns>削除済みリストに含まれる場合はtrue、それ以外はfalseです。</returns>
	bool IsDeletedEffectName(const std::string& name) const;

	/// <summary>
	/// パーティクル群の追加、名前変更、削除、編集を行う管理UIを描画します。
	/// </summary>
	void DrawParticleGroupEditor();

	/// <summary>
	/// パーティクルへ適用するプリミティブ定義の追加、編集、保存を行うUIを描画します。
	/// </summary>
	void DrawPrimitiveLibraryEditor();

	/// <summary>
	/// 選択中プリミティブ定義を画面上で確認するプレビューUIを描画します。
	/// </summary>
	/// <param name="primitiveName">プレビュー元のプリミティブ定義名です。</param>
	/// <param name="data">プレビューへ反映するプリミティブ定義データです。</param>
	void DrawPrimitivePreviewControls(const std::string& primitiveName, const Engine::ParticleGroupEditorData& data);

	/// <summary>
	/// プレビュー用の一時パーティクル群とエミッターを必要に応じて生成します。
	/// </summary>
	/// <param name="primitiveName">プレビュー元のプリミティブ定義名です。</param>
	/// <param name="data">生成・反映に使うプリミティブ定義データです。</param>
	void EnsurePrimitivePreview(const std::string& primitiveName, const Engine::ParticleGroupEditorData& data);

	/// <summary>
	/// プレビュー用の一時パーティクルを発生させ、編集中形状を画面へ表示します。
	/// </summary>
	void EmitPrimitivePreview();

	/// <summary>
	/// プレビュー用に作った内部パーティクル群とエミッターを削除します。
	/// </summary>
	void ClearPrimitivePreview();

	/// <summary>
	/// プリミティブ定義をパーティクル群へ反映します。
	/// </summary>
	/// <param name="particleName">反映先のパーティクル群名です。</param>
	/// <param name="primitiveName">反映元のプリミティブ定義名です。</param>
	void ApplyPrimitiveDefinitionToParticleGroup(const std::string& particleName, const std::string& primitiveName);

	/// <summary>
	/// パーティクル群の詳細設定UIを描画します。
	/// </summary>
	/// <param name="particleName">編集対象のパーティクル群名です。</param>
	/// <param name="group">編集対象のパーティクル群データです。</param>
	void DrawParticleGroupDetail(const std::string& particleName, Engine::ParticleGroup& group);

	/// <summary>
	/// 選択中パーティクル群を、エフェクト定義なしで発生確認するUIを描画します。
	/// </summary>
	/// <param name="particleName">発生確認するパーティクル群名です。</param>
	void DrawParticleGroupPreviewControls(const std::string& particleName);

	/// <summary>
	/// パーティクル群確認用の一時エミッターを生成、または参照先を更新します。
	/// </summary>
	/// <param name="particleName">一時エミッターが参照するパーティクル群名です。</param>
	void EnsureParticleGroupPreviewEmitter(const std::string& particleName);

	/// <summary>
	/// パーティクル群確認用の一時エミッターからパーティクルを発生させます。
	/// </summary>
	/// <param name="particleName">発生させるパーティクル群名です。</param>
	void EmitParticleGroupPreview(const std::string& particleName);

	/// <summary>
	/// パーティクル群確認用の一時エミッターを削除します。
	/// </summary>
	void ClearParticleGroupPreview();

	/// <summary>
	/// 新しいパーティクル群名が使用できるか検証します。
	/// </summary>
	/// <param name="particleName">検証するパーティクル群名です。</param>
	/// <returns>空文字や重複がなく使用できる場合はtrue、それ以外はfalseです。</returns>
	bool ValidateNewParticleGroupName(const std::string& particleName);

	/// <summary>
	/// UI入力内容から新しいパーティクル群を追加し、保存レジストリへ登録します。
	/// </summary>
	void AddParticleGroupFromEditor();

	/// <summary>
	/// 選択中のパーティクル群名を変更し、参照しているエフェクト設定も更新します。
	/// </summary>
	/// <param name="oldName">変更前のパーティクル群名です。</param>
	/// <param name="newName">変更後のパーティクル群名です。</param>
	void RenameParticleGroup(const std::string& oldName, const std::string& newName);

	/// <summary>
	/// 選択中のパーティクル群を削除し、削除済みリストへ記録します。
	/// </summary>
	/// <param name="particleName">削除するパーティクル群名です。</param>
	void DeleteParticleGroup(const std::string& particleName);

	/// <summary>
	/// パーティクル群一覧の保存グループへ名前を登録します。
	/// </summary>
	/// <param name="name">登録するパーティクル群名です。</param>
	void RegisterParticleGroupName(const std::string& name);

	/// <summary>
	/// パーティクル群一覧の保存グループから名前を登録解除します。
	/// </summary>
	/// <param name="name">登録解除するパーティクル群名です。</param>
	void UnregisterParticleGroupName(const std::string& name);

	/// <summary>
	/// 削除済みパーティクル群リストへ登録します。
	/// </summary>
	/// <param name="name">削除済みとして扱うパーティクル群名です。</param>
	void RegisterDeletedParticleGroupName(const std::string& name);

	/// <summary>
	/// 削除済みパーティクル群リストから解除し、同名再追加を許可します。
	/// </summary>
	/// <param name="name">削除済み扱いを解除するパーティクル群名です。</param>
	void UnregisterDeletedParticleGroupName(const std::string& name);

	/// <summary>
	/// 指定名が削除済みパーティクル群として保存されているか確認します。
	/// </summary>
	/// <param name="name">確認するパーティクル群名です。</param>
	/// <returns>削除済みリストに含まれる場合はtrue、それ以外はfalseです。</returns>
	bool IsDeletedParticleGroupName(const std::string& name) const;

	/// <summary>
	/// パーティクル名変更を参照中のエフェクト設定へ反映します。
	/// </summary>
	/// <param name="oldName">変更前のパーティクル群名です。</param>
	/// <param name="newName">変更後のパーティクル群名です。</param>
	void RenameParticleReferences(const std::string& oldName, const std::string& newName);

	/// <summary>
	/// 新しいプリミティブ定義名が使用できるか検証します。
	/// </summary>
	/// <param name="primitiveName">検証するプリミティブ定義名です。</param>
	/// <returns>使用できる場合はtrue、それ以外はfalseです。</returns>
	bool ValidateNewPrimitiveDefinitionName(const std::string& primitiveName);

	/// <summary>
	/// UI入力内容からプリミティブ定義を追加します。
	/// </summary>
	void AddPrimitiveDefinitionFromEditor();

	/// <summary>
	/// 保存済みプリミティブ定義を読み込みます。
	/// </summary>
	void LoadRegisteredPrimitiveDefinitions();

	/// <summary>
	/// プリミティブ定義名を保存レジストリへ登録します。
	/// </summary>
	/// <param name="name">登録するプリミティブ定義名です。</param>
	void RegisterPrimitiveDefinitionName(const std::string& name);

	/// <summary>
	/// プリミティブ定義名を保存レジストリから外します。
	/// </summary>
	/// <param name="name">登録解除するプリミティブ定義名です。</param>
	void UnregisterPrimitiveDefinitionName(const std::string& name);

	/// <summary>
	/// プリミティブ定義を削除します。
	/// </summary>
	/// <param name="name">削除するプリミティブ定義名です。</param>
	void DeletePrimitiveDefinition(const std::string& name);

	/// <summary>
	/// プリミティブ定義を保存します。
	/// </summary>
	/// <param name="name">保存するプリミティブ定義名です。</param>
	/// <param name="data">保存するプリミティブ定義データです。</param>
	void SavePrimitiveDefinitionData(const std::string& name, const Engine::ParticleGroupEditorData& data);

	/// <summary>
	/// プリミティブ定義を読み込みます。
	/// </summary>
	/// <param name="name">読み込むプリミティブ定義名です。</param>
	/// <param name="data">読み込んだプリミティブ定義データです。</param>
	void LoadPrimitiveDefinitionData(const std::string& name, Engine::ParticleGroupEditorData& data);

	/// <summary>
	/// エディタ全体の設定を保存します。
	/// </summary>
	void SaveAllEditorData();

	/// <summary>
	/// 指定エミッタの設定を個別に保存します。
	/// </summary>
	/// <param name="name">保存するエミッタ名です。</param>
	/// <param name="data">保存するエミッタ設定です。</param>
	void SaveEmitterData(const std::string& name, const EffectGlobalData& data);

	/// <summary>
	/// 指定パーティクル群の設定を個別に保存します。
	/// </summary>
	/// <param name="particleName">保存するパーティクル群名です。</param>
	/// <param name="data">保存するパーティクル群設定です。</param>
	void SaveParticleGroupData(const std::string& particleName, const Engine::ParticleGroupEditorData& data);

	/// <summary>
	/// EffectエディタのUI状態を保存します。
	/// </summary>
	void SaveEditorUiSettings();

	/// <summary>
	/// EffectエディタのUI状態を読み込みます。
	/// </summary>
	void LoadEditorUiSettings();
private:
	/// <summary>
	/// 選択中エフェクトの詳細編集UIを描画します。
	/// </summary>
	/// <param name="name">編集対象のエフェクト名です。</param>
	/// <param name="data">編集対象のエフェクト設定です。</param>
	void DrawEffectDetail(const std::string& name, EffectGlobalData& data);

	/// <summary>
	/// 指定名のエフェクトをデバッグ用に発生させます。
	/// </summary>
	/// <param name="name">発生させるエフェクト名です。</param>
	/// <param name="pos">発生位置です。ワールド座標で指定します。</param>
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
	// エミッタ設定UIの保存結果やエラー文
	std::string emitterSettingsMessage_;
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
	// 選択中のプリミティブ定義名
	std::string selectedPrimitiveDefinitionName_;
	// パーティクルへ適用するプリミティブ定義名
	std::string selectedApplyPrimitiveName_;
	// 新規追加するプリミティブ定義名入力
	std::array<char, 128> newPrimitiveDefinitionNameBuffer_{};
	// 新規追加・編集に使うプリミティブ定義データ
	Engine::ParticleGroupEditorData newPrimitiveDefinitionData_{};
	// 保存済みプリミティブ定義一覧
	std::map<std::string, Engine::ParticleGroupEditorData> primitiveDefinitionDatas_;
	// プリミティブ定義UIの結果やエラー文
	std::string primitiveManagementMessage_;
	// プリミティブ定義の画面プレビューを有効にするか
	bool isPrimitivePreviewEnabled_ = false;
	// プレビュー用パーティクルを発生させるワールド位置
	Vector3 primitivePreviewPosition_ = { 0.0f, 2.0f, 0.0f };
	// プレビュー用パーティクルの表示サイズ
	Vector3 primitivePreviewSize_ = { 1.0f, 1.0f, 1.0f };
	// プレビュー用パーティクルの回転
	Vector3 primitivePreviewRotate_ = {};
	// プレビュー用パーティクルの色
	Vector4 primitivePreviewColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	// プレビューを再発生させるまでの経過時間
	float primitivePreviewTimer_ = 0.0f;
	// 現在プレビューへ反映済みのプリミティブ定義名
	std::string previewPrimitiveName_;
	// 現在プレビューへ反映済みのプリミティブ形状
	Engine::ShapeParameter::ShapeType previewPrimitiveShapeType_ = Engine::ShapeParameter::ShapeType::None;
	// 選択中パーティクル群の発生確認を有効にするか
	bool isParticleGroupPreviewEnabled_ = false;
	// パーティクル群確認用エミッターが現在参照しているパーティクル群名
	std::string previewParticleGroupName_;
	// パーティクル群確認用エミッターの発生位置
	Vector3 particleGroupPreviewPosition_ = {};
	// パーティクル群確認用パーティクルのサイズ
	Vector3 particleGroupPreviewSize_ = { 1.0f, 1.0f, 1.0f };
	// パーティクル群確認用パーティクルの回転
	Vector3 particleGroupPreviewRotate_ = {};
	// パーティクル群確認用パーティクルの色
	Vector4 particleGroupPreviewColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	// パーティクル群確認用の発生頻度
	float particleGroupPreviewFrequency_ = 0.2f;
	// パーティクル群確認用の発生タイマー
	float particleGroupPreviewTimer_ = 0.0f;
	// パーティクル群確認用の一回あたり発生数
	int particleGroupPreviewCount_ = 1;
	// パーティクル群確認用の寿命
	float particleGroupPreviewLifeTime_ = 1.0f;
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
	// エフェクトとパーティクル設定の永続化を担当する
	EffectEditorSerializer serializer_;
};
