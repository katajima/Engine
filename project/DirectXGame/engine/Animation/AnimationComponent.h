#pragma once
#include "DirectXGame/engine/math/MathFunctions.h"
#include "DirectXGame/engine/Animation/Animation.h"
#include "DirectXGame/engine/Mesh/ModelMesh.h"


namespace Engine {

	class Model;

	// アニメーションコンポーネント
/// <summary>
/// AnimationComponentを管理・実装するクラス。
/// </summary>
	class AnimationComponent
	{
	public:

		// 初期化
		void Init(LineCommon* lineCommon);

		// アニメーションするモデルを設定
		void SetModel(Model* mod) { model = mod; }

		// スキニング更新
		void UpdateSkin(float deltatime, WorldTransform worldTransform);

		// アニメーション更新
		void Update(float deltatime, WorldTransform worldTransform);


	public:
		// 再生するか
		void SetIsPlaying(bool is) { isPlaying = is; }
		// 逆再生
		void SetIsReversePlayback(bool is) { isReversePlayback = is; };
		// アニメーションスピード
		void SetAnimationSpeed(float speed) { animationSpeed = speed; }
		// ループ再生するか
		void SetIsLoop(bool is) { isLoop = is; };
		// ローカル行列取得
		Matrix4x4 GetLocalMatrix() const { return localMatrix_; }
		// アニメーション変更
		void SetAnimation(const std::string& name, float time);
		// アニメーションが終了しているか
		bool IsAnimationFinished();
		// 初期時間に戻す
		void SetStratAnimeTime() { animationTime = 0.0f; }
		// アニメーション時間設定
		void SetAnimationTime(float time) { animationTime = time; }
		// アニメーション時間取得
		float GetAnimationTime() const { return animationTime; }
		// 終了時間に合わせる
		void SetEndAnimeTime();
		// 終了時間取得
		float GetEndAnimeTime(std::string name) const;

	private:
		// アニメーション時間を再生状態に合わせて進める
		void AdvanceAnimationTime(const Animation& currentAnimation, float deltaTime);
		// 前アニメーションのブレンド用時間を再生方向に合わせて進める
		void AdvancePreviousAnimationTime(const Animation& previousAnimation, float deltaTime);
		// アニメーション時間を尺内に丸める
		float WrapAnimationTime(float time, float duration) const;
		// ブレンド率を更新して0.0f～1.0fの補間値を返す
		float UpdateBlendRate(ModelData& modelData, float deltaTime);
		// スキニング用のアニメーション姿勢を反映する
		void ApplySkinAnimation(ModelData& modelData, const Animation& currentAnimation, float deltaTime);
		// 通常ノード用のアニメーション姿勢を反映する
		void ApplyNodeAnimation(ModelData& modelData, const Animation& currentAnimation, float deltaTime);
		// ノードアニメーションからローカル行列を作成する
		Matrix4x4 CalculateNodeLocalMatrix(const NodeAnimation& nodeAnimation, float animationTime) const;
		// スキンクラスターのパレットを安全に更新する
		void UpdateSkinClusters(ModelData& modelData);
		// マテリアルのGPU転送を安全に更新する
		void UpdateMaterialGPUData(ModelData& modelData);

	private:
		// アニメーション再生しているか
		bool isPlaying = true;
		// 逆再生するか
		bool isReversePlayback = false;
		// アニメーション速度
		float animationSpeed = 1.0f;
		// ループさせるか
		bool isLoop = true;
		// デバッグ表示するか
		bool isDebug = true;

		// ローカル行列
		Matrix4x4 localMatrix_{};
		// アニメーション時間
		float animationTime = 0.0f;
		// ブレンド開始時点の前アニメーション時間
		float previousAnimationTime_ = 0.0f;
	private:
		// モデル
		Model* model = nullptr;
		// デバッグ用のライン描画クラスへのポインタ
		LineCommon* lineCommon = nullptr;
	};
}
