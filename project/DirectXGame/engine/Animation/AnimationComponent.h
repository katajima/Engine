#pragma once
#include "DirectXGame/engine/math/MathFunctions.h"
#include "DirectXGame/engine/Animation/Animation.h"
#include "DirectXGame/engine/Mesh/ModelMesh.h"


namespace Engine {

	class Model;

	// アニメーションコンポーネント
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
		// 終了時間に合わせる
		void SetEndAnimeTime();
		// 終了時間取得
		float GetEndAnimeTime(std::string name) const;

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
	private:
		// モデル
		Model* model = nullptr;
		// デバッグ用のライン描画クラスへのポインタ
		LineCommon* lineCommon = nullptr;
	};
}
