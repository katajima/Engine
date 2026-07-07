#pragma once

#include "DirectXGame/engine/Effect/EffectComponent.h"
#include "DirectXGame/engine/Effect/EffectGlovalData.h"

namespace Engine {
	class GlobalVariables;
}

/// <summary>
/// EffectEditorが扱うエフェクト・パーティクル設定の永続化を担当するクラス
/// </summary>
class EffectEditorSerializer {
public:
	// 保存先のグローバル変数を設定する
	void Initialize(Engine::GlobalVariables* globalVariables) { globalVariables_ = globalVariables; }

	// エフェクト設定の未登録項目を追加する
	void RegisterEffectData(const std::string& name, const EffectGlobalData& data);
	// エフェクト設定を読み込む
	void LoadEffectData(const std::string& name, EffectGlobalData& data) const;
	// エフェクト設定を現在値で保存する
	void SaveEffectData(const std::string& name, const EffectGlobalData& data);

	// パーティクル群設定の未登録項目を追加する
	void RegisterParticleGroupData(const std::string& name, const Engine::ParticleGroupEditorData& data);
	// パーティクル群設定を読み込む
	void LoadParticleGroupData(const std::string& name, Engine::ParticleGroupEditorData& data) const;
	// パーティクル群設定を現在値で保存する
	void SaveParticleGroupData(const std::string& name, const Engine::ParticleGroupEditorData& data);

private:
	// 登録と保存で共通のエフェクト項目一覧を書き込む
	void WriteEffectData(const std::string& name, const EffectGlobalData& data, bool overwrite);
	// 登録と保存で共通のパーティクル群項目一覧を書き込む
	void WriteParticleGroupData(const std::string& name, const Engine::ParticleGroupEditorData& data, bool overwrite);

private:
	Engine::GlobalVariables* globalVariables_ = nullptr; // 設定の登録・読込・保存先
};
