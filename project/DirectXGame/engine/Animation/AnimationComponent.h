#pragma once
#include "DirectXGame/engine/math/MathFanctions.h"
#include "DirectXGame/engine/Animation/Animation.h"
#include "DirectXGame/engine/Mesh/ModelMesh.h"
#include "DirectXGame/engine/3d/Model/ModelData.h"
#include"DirectXGame/engine/3d/Model/Model.h"

namespace Engine {
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
		void SetAnimetion(const std::string& name, float time) {
			Animetion::SetAnimation(model->modelData, name, time);
		}
		// アニメーションが終了しているか
		bool IsAnimationFinished();
		// 初期時間に戻す
		void SetStratAnimeTime() { model->modelData.animationTime = 0.0f; }
		// 終了時間に合わせる
		void SetEndAnimeTime() {
			const auto& animations = model->modelData.animations;
			auto& modelData = model->modelData;
			const std::string& currentName = modelData.currentAnimName;
			auto itCurrent = animations.find(currentName);

			modelData.animationTime = itCurrent->second.duration;
		}
		// 終了時間取得
		float GetEndAnimeTime(std::string name) const {
			const auto& animations = model->modelData.animations;
			auto it = animations.find(name);
			if (it != animations.end()) {
				return it->second.duration;
			}
			// 見つからない場合は 0.0f や -1.0f を返す
			return 0.0f;
		}

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
		Matrix4x4 localMatrix_;

	private:
		// モデル
		Model* model;
		LineCommon* lineCommon;
	};
}
