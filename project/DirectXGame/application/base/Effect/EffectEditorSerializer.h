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
	/// <summary>
	/// 保存先のグローバル変数を設定する
	/// </summary>
	void Initialize(Engine::GlobalVariables* globalVariables) { globalVariables_ = globalVariables; }

	/// <summary>
	/// エフェクト設定の未登録項目を追加する
	/// </summary>
	void RegisterEffectData(const std::string& name, const EffectGlobalData& data);
	/// <summary>
	/// エフェクト設定を読み込む
	/// </summary>
	void LoadEffectData(const std::string& name, EffectGlobalData& data) const;
	/// <summary>
	/// エフェクト設定を現在値で保存する
	/// </summary>
	void SaveEffectData(const std::string& name, const EffectGlobalData& data);

	/// <summary>
	/// パーティクル群設定の未登録項目を追加する
	/// </summary>
	void RegisterParticleGroupData(const std::string& name, const Engine::ParticleGroupEditorData& data);
	/// <summary>
	/// パーティクル群設定を読み込む
	/// </summary>
	void LoadParticleGroupData(const std::string& name, Engine::ParticleGroupEditorData& data) const;
	/// <summary>
	/// パーティクル群設定を現在値で保存する
	/// </summary>
	void SaveParticleGroupData(const std::string& name, const Engine::ParticleGroupEditorData& data);

private:
	/// <summary>
	/// 登録と保存で共通のエフェクト項目一覧を書き込む
	/// </summary>
	void WriteEffectData(const std::string& name, const EffectGlobalData& data, bool overwrite);
	/// <summary>
	/// 登録と保存で共通のパーティクル群項目一覧を書き込む
	/// </summary>
	void WriteParticleGroupData(const std::string& name, const Engine::ParticleGroupEditorData& data, bool overwrite);

private:
	Engine::GlobalVariables* globalVariables_ = nullptr; // 設定の登録・読込・保存先
};
