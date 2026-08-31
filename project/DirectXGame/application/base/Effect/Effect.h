#pragma once
#include "EffectEditor.h"
// 前方宣言
namespace Engine {
	class EntityManager;
}
/// <summary>
/// エフェクトクラス
/// </summary>
class EffectSystem {
public:
	/// <summary>エフェクト定義と実行用コンポーネントを初期化する。</summary>
	/// <param name="entityManager">エフェクトを生成するエンティティ管理。利用中は有効であること。</param>
	/// <param name="globalVariables">エフェクト定義の登録・保存先。</param>
	void Initialize(Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables);
	/// <summary>生成済みエフェクトとエディター状態を更新する。</summary>
	/// <param name="dt">秒単位のフレーム時間。</param>
	void Update(float dt);
	// コンボ演出が所有するエフェクトをすべて描画する。
	void Draw();
	/// <summary>指定位置へエフェクトを生成する。</summary>
	/// <param name="name">登録済みエフェクト名。</param>
	/// <param name="pos">ワールド座標。</param>
	void Emit(const std::string& name, const Vector3& pos);
	/// <summary>方向と放出範囲を指定してエフェクトを生成する。</summary>
	/// <param name="name">登録済みエフェクト名。</param>
	/// <param name="pos">ワールド座標。</param>
	/// <param name="dir">放出方向。</param>
	/// <param name="range">各軸の放出範囲。</param>
	void Emit(const std::string& name, const Vector3& pos, const Vector3& dir, const Vector3& range);
	// 動的に生成するトレイルで使用するカメラを設定する。
	void SetCamera(Engine::Camera* camera);

	// コンボ演出エントリ用のトレイルをエフェクトコンポーネント側で生成する。
	void CreateTrailEffect(const std::string& name, const std::string& texture, float lifeTime,
		Engine::WorldTransform& parent, Engine::Camera* camera, const Color& color,
		const Vector3& offsetStart, const Vector3& offsetEnd,
		const Engine::TrailTrajectorySettings& trajectory);

	// 動的に生成したコンボ用トレイルを停止して削除する。
	void RemoveTrailEffect(const std::string& name);

	// エフェクトコンポーネントの所有権をComboEffectへ公開せず、発生状態だけ変更する。
	void SetTrailEmit(const std::string& name, bool isEmit);

public:
	/// <summary>エディターが管理している全エフェクト定義を取得する。</summary>
	/// <returns>呼び出し側で安全に参照できる定義マップのコピー。</returns>
	std::map<std::string, EffectGlobalData> GetEffectGlobalDatas() const { return effectEditor_->GetEffectGlobalDatas(); };
private:
	/// <summary>1件のエフェクト定義から実行用エフェクトを生成する。</summary>
	/// <param name="name">生成するエフェクト名。</param>
	/// <param name="shapeType">エミッター形状。</param>
	/// <param name="data">生成に利用する保存済みパラメータ。</param>
	void CreateEffect(const std::string& name, EmitterShapeType shapeType, const EffectGlobalData& data);

	/// <summary>
	/// パーティクル初期化
	/// </summary>
	void InitParticle();
	/// <summary>
	/// パーティクル初期化(範囲攻撃弾)
	/// </summary>
	void InitRangeBombingBullet();
	/// <summary>
	/// パーティクル初期化(弾)
	/// </summary>
	void InitBullet();
private:
	// エフェクトコンポーネント
	std::unique_ptr<Engine::EffectComponent> effectComponent_ = nullptr;	
	// エフェクトエディタ
	std::unique_ptr<EffectEditor> effectEditor_ = nullptr;
private:
	// エンティティマネージャー
	Engine::EntityManager* entityManager = nullptr;	
	// グローバル変数保存
	Engine::GlobalVariables* globalVariables = nullptr;	
private: // デバッグ用

};
