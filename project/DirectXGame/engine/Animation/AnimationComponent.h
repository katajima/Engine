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

		/// <summary>
		/// 初期化
		/// </summary>
		void Init(LineCommon* lineCommon);

		/// <summary>
		/// アニメーションするモデルを設定
		/// </summary>
		void SetModel(Model* mod) { model = mod; }

		/// <summary>
		/// スキニング更新
		/// </summary>
		void UpdateSkin(float deltatime, WorldTransform worldTransform);

		/// <summary>
		/// アニメーション更新
		/// </summary>
		void Update(float deltatime, WorldTransform worldTransform);


	public:
		/// <summary>
		/// 再生するか
		/// </summary>
		void SetIsPlaying(bool is) { isPlaying = is; }
		/// <summary>
		/// 逆再生
		/// </summary>
		void SetIsReversePlayback(bool is) { isReversePlayback = is; };
		/// <summary>
		/// アニメーションスピード
		/// </summary>
		void SetAnimationSpeed(float speed) { animationSpeed = speed; }
		/// <summary>
		/// ループ再生するか
		/// </summary>
		void SetIsLoop(bool is) { isLoop = is; };
		/// <summary>
		/// ローカル行列取得
		/// </summary>
		Matrix4x4 GetLocalMatrix() const { return localMatrix_; }
		/// <summary>
		/// アニメーション変更
		/// </summary>
		void SetAnimation(const std::string& name, float time);
		/// <summary>
		/// アニメーションが終了しているか
		/// </summary>
		bool IsAnimationFinished();
		/// <summary>
		/// 初期時間に戻す
		/// </summary>
		void SetStratAnimeTime() { animationTime = 0.0f; }
		/// <summary>
		/// アニメーション時間設定
		/// </summary>
		void SetAnimationTime(float time) { animationTime = time; }
		/// <summary>
		/// アニメーション時間取得
		/// </summary>
		float GetAnimationTime() const { return animationTime; }
		/// <summary>
		/// 終了時間に合わせる
		/// </summary>
		void SetEndAnimeTime();
		/// <summary>
		/// 終了時間取得
		/// </summary>
		float GetEndAnimeTime(std::string name) const;

	private:
		/// <summary>
		/// アニメーション時間を再生状態に合わせて進める
		/// </summary>
		void AdvanceAnimationTime(const Animation& currentAnimation, float deltaTime);
		/// <summary>
		/// 前アニメーションのブレンド用時間を再生方向に合わせて進める
		/// </summary>
		void AdvancePreviousAnimationTime(const Animation& previousAnimation, float deltaTime);
		/// <summary>
		/// アニメーション時間を尺内に丸める
		/// </summary>
		float WrapAnimationTime(float time, float duration) const;
		/// <summary>
		/// ブレンド率を更新して0.0f～1.0fの補間値を返す
		/// </summary>
		float UpdateBlendRate(ModelData& modelData, float deltaTime);
		/// <summary>
		/// スキニング用のアニメーション姿勢を反映する
		/// </summary>
		void ApplySkinAnimation(ModelData& modelData, const Animation& currentAnimation, float deltaTime);
		/// <summary>
		/// 通常ノード用のアニメーション姿勢を反映する
		/// </summary>
		void ApplyNodeAnimation(ModelData& modelData, const Animation& currentAnimation, float deltaTime);
		/// <summary>
		/// ノードアニメーションからローカル行列を作成する
		/// </summary>
		Matrix4x4 CalculateNodeLocalMatrix(const NodeAnimation& nodeAnimation, float animationTime) const;
		/// <summary>
		/// スキンクラスターのパレットを安全に更新する
		/// </summary>
		void UpdateSkinClusters(ModelData& modelData);
		/// <summary>
		/// マテリアルのGPU転送を安全に更新する
		/// </summary>
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
