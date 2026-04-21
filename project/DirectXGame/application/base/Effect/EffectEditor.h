#pragma once
#include <DirectXGame/engine/Effect/EffectComponent.h>
#include <DirectXGame/engine/Effect/EffectGlovalData.h>

/// <summary>
/// effectエディタークラス
/// </summary>
class EffectEditor {
public:

	// 初期化
	void Initialize(Engine::EffectComponent* effectComponent, Engine::GlobalVariables* globalVariables);

	// 更新
	void Update(float dt);
	//
	void AddEffectGlobalData(const std::string& name, const std::string& particleName);
	//
	void SetEffectGlobalData(const std::string& name, EmitterShapeType shapeType, const EffectGlobalData& data);

	

	std::map<std::string, EffectGlobalData> GetEffectGlobalDatas() const { return effectGlobalDatas_; };
private:
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