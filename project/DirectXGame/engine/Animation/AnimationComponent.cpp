#include "AnimationComponent.h"
#include"DirectXGame/engine/Skinning/Skinning.h"


void Engine::AnimationComponent::Init(LineCommon* lineCommon)
{
	this->lineCommon = lineCommon;
}

void Engine::AnimationComponent::UpdateSkin(float deltatime,WorldTransform worldTransform)
{

	// モデルが存在する場合
	if (model) {
		const auto& animations = model->modelData.animations;
		auto& modelData = model->modelData;

		const std::string& currentName = modelData.currentAnimName;
		auto itCurrent = animations.find(currentName);

		if (itCurrent != animations.end()) {
			// アニメーション時間更新（毎フレーム）
			float deltaTime = deltatime * animationSpeed;
			if (isPlaying) {
				if (isReversePlayback) { // 逆再生なら
					modelData.animationTime -= deltaTime;
				}
				else {
					modelData.animationTime += deltaTime;
				}
			}

			// ループするなら
			if (isLoop) {
				modelData.animationTime = std::fmod(modelData.animationTime, itCurrent->second.duration);

				// 負の値を返す可能性があるので
				if (modelData.animationTime < 0.0f) {
					modelData.animationTime += itCurrent->second.duration;
				}
			}
			else { // しないなら
				if (isReversePlayback) {
					if (modelData.animationTime <= 0) {
						modelData.animationTime = 0; // 最終フレームで止める
						isPlaying = false; // 自動停止
					}
				}
				else {
					if (modelData.animationTime >= itCurrent->second.duration) {
						modelData.animationTime = itCurrent->second.duration; // 最終フレームで止める
						isPlaying = false; // 自動停止
					}
				}
			}


			// アニメーションブレンド中か？
			if (modelData.isBlending && modelData.previousAnimName != "") {
				auto itPrev = animations.find(modelData.previousAnimName);
				if (itPrev != animations.end()) {
					const Animation& prevAnim = itPrev->second;
					const Animation& currAnim = itCurrent->second;

					auto WrapTime = [](float time, float duration) {
						float wrapped = std::fmod(time, duration);
						if (wrapped < 0.0f) wrapped += duration;
						return wrapped;
						};

					float prevTime = WrapTime(modelData.animationTime, prevAnim.duration);
					float currTime = WrapTime(modelData.animationTime, currAnim.duration);


					// ① 各スケルトン姿勢を取得
					Skeleton prevSkeleton = modelData.skeleton;
					Skeleton currSkeleton = modelData.skeleton;
					Animetion::ApplyAnimation(prevSkeleton, prevAnim, prevTime);
					Animetion::ApplyAnimation(currSkeleton, currAnim, currTime);

					// ② 補間割合を更新（EaseInOutでなめらかに）
					modelData.blendTime += deltaTime;
					float t = modelData.blendTime / modelData.blendDuration;
					t = std::clamp(t, 0.0f, 1.0f);
					t = t * t * (3.0f - 2.0f * t); // Hermite補間（EaseInOut）

					// ③ スケルトン補間
					Animetion::BlendSkeletons(modelData.skeleton, prevSkeleton, currSkeleton, t);

					// ブレンド完了判定
					if (modelData.blendTime >= modelData.blendDuration) {
						modelData.isBlending = false;
						modelData.previousAnimName.clear();
					}
				}
				else {
					// 前アニメーションが見つからなければ通常再生
					Animetion::ApplyAnimation(modelData.skeleton, itCurrent->second, modelData.animationTime);
				}
			}
			else {
				// ブレンドしていない通常の再生
				Animetion::ApplyAnimation(modelData.skeleton, itCurrent->second, modelData.animationTime);
			}




			// スケルトン姿勢更新
			Animetion::UpdateSkeleton(modelData.skeleton);

			std::vector<Matrix4x4> cachedSkeletonMatrices;
			for (auto& mesh : modelData.mesh) {
				Animetion::UpdateSkinCluster(*mesh->skinCluster, modelData.skeleton, cachedSkeletonMatrices);
			}

			// ルートの変換行列反映
			localMatrix_ = modelData.skeleton.joints[0].skeletonSpaceMatrix;
			// デバッグ用：スケルトン描画
			Animetion::DrawSkeleton(
				lineCommon,
				modelData.skeleton.joints,
				worldTransform.worldMat_.GetWorldPosition(),
				worldTransform.scale_,
				MakeRotateXYZ(worldTransform.rotate_)
			);
		}
		else {
			// アニメーションが見つからない場合のフォールバック
			localMatrix_ = model->modelData.rootNode.localMatrix;

			std::vector<Matrix4x4> cachedSkeletonMatrices;
			for (auto& mesh : modelData.mesh) {
				Animetion::UpdateSkinCluster(*mesh->skinCluster, modelData.skeleton, cachedSkeletonMatrices);
			}

			// デバッグ用：スケルトン描画
			Animetion::DrawSkeleton(
				lineCommon,
				modelData.skeleton.joints,
				worldTransform.worldMat_.GetWorldPosition(),
				worldTransform.scale_,
				MakeRotateXYZ(worldTransform.rotate_)
			);
		}



		std::vector<std::future<void>> futures;
		for (auto& mesh : model->modelData.mesh) {
			futures.push_back(std::async(std::launch::async, [&mesh]() {
				mesh->material->GPUData();
				}));
		}

		// 全スレッドの終了を待つ
		for (auto& f : futures) {
			f.get();
		}

	}


}

void Engine::AnimationComponent::Update(float deltatime, WorldTransform worldTransform)
{
	if (model) {
		const auto& animations = model->modelData.animations;
		auto& modelData = model->modelData;

		const std::string& currentName = modelData.currentAnimName;
		auto itCurrent = animations.find(currentName);

		if (itCurrent != animations.end()) {
			// アニメーション時間更新（毎フレーム）
			float deltaTime = deltatime * animationSpeed;
			if (isPlaying) {
				if (isReversePlayback) { // 逆再生なら
					modelData.animationTime -= deltaTime;
				}
				else {
					modelData.animationTime += deltaTime;
				}
			}

			// ループするなら
			if (isLoop) {
				modelData.animationTime = std::fmod(modelData.animationTime, itCurrent->second.duration);

				// 負の値を返す可能性があるので
				if (modelData.animationTime < 0.0f) {
					modelData.animationTime += itCurrent->second.duration;
				}
			}
			else { // しないなら
				if (isReversePlayback) {
					if (modelData.animationTime <= 0) {
						modelData.animationTime = 0; // 最終フレームで止める
						isPlaying = false; // 自動停止
					}
				}
				else {
					if (modelData.animationTime >= itCurrent->second.duration) {
						modelData.animationTime = itCurrent->second.duration; // 最終フレームで止める
						isPlaying = false; // 自動停止
					}
				}
			}
			// アニメーションブレンド中か？
			if (modelData.isBlending && modelData.previousAnimName != "") {
				auto itPrev = animations.find(modelData.previousAnimName);
				if (itPrev != animations.end()) {
					const Animation& prevAnim = itPrev->second;
					const Animation& currAnim = itCurrent->second;

					auto WrapTime = [](float time, float duration) {
						float wrapped = std::fmod(time, duration);
						if (wrapped < 0.0f) wrapped += duration;
						return wrapped;
						};

					float prevTime = WrapTime(modelData.animationTime, prevAnim.duration);
					float currTime = WrapTime(modelData.animationTime, currAnim.duration);
				}
			}

			// 各ノード（ボーンではなく、単なるノード）のトランスフォーム補間
			for (auto& [nodeName, nodeAnim] : itCurrent->second.nodeAnimations) {
				// 時間に対応する補間結果を取得
				Vector3 interpTranslate = Animetion::CalculateValue(nodeAnim.translate.keyframes, modelData.animationTime);
				Quaternion interpRotate = Animetion::CalculateValue(nodeAnim.rotate.keyframes,modelData.animationTime);
				Vector3 interpScale = Animetion::CalculateValue(nodeAnim.scale.keyframes,modelData.animationTime);

				// TRSマトリクスの生成
				Matrix4x4 localMatrix = MakeAffineMatrix(interpScale, interpRotate, interpTranslate);
			
				localMatrix_ = localMatrix;
			}
			
		}
		else {

			// アニメーションが見つからない場合のフォールバック
			localMatrix_ = model->modelData.rootNode.localMatrix;
		}

		std::vector<std::future<void>> futures;
		for (auto& mesh : model->modelData.mesh) {
			futures.push_back(std::async(std::launch::async, [&mesh]() {
				mesh->material->GPUData();
				}));
		}

		// 全スレッドの終了を待つ
		for (auto& f : futures) {
			f.get();
		}

	}
}

bool Engine::AnimationComponent::IsAnimationFinished()
{
	if (!model) return false;

	const auto& animations = model->modelData.animations;
	auto& modelData = model->modelData;

	const std::string& currentName = modelData.currentAnimName;
	auto itCurrent = animations.find(currentName);

	// 現在のアニメーション名が見つからなかった場合は再生終了扱い
	if (itCurrent == animations.end()) {
		return true;
	}

	// アニメ再生中かつアニメ時間が duration に達していれば終了
	if (isReversePlayback) {

		return !isPlaying && modelData.animationTime <= 0;
	}
	else {
		return !isPlaying && modelData.animationTime >= itCurrent->second.duration;
	}
}