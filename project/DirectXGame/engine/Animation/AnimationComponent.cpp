#include "AnimationComponent.h"
#include"DirectXGame/engine/Skinning/Skinning.h"
#include"DirectXGame/engine/3d/Model/Model.h"
#include"DirectXGame/engine/Material/Material.h"

#include <algorithm>
#include <cmath>
#include <future>

void Engine::AnimationComponent::Init(LineCommon* lineCommon) {
	this->lineCommon = lineCommon;
}

void Engine::AnimationComponent::AdvanceAnimationTime(const Animation& currentAnimation, float deltaTime)
{
	// 尺が0秒以下のアニメーションではfmodを使わず、先頭フレーム固定にする
	if (currentAnimation.duration <= 0.0f) {
		animationTime = 0.0f;
		if (!isLoop) {
			isPlaying = false;
		}
		return;
	}

	// 再生中だけ現在時間を進める
	if (isPlaying) {
		if (isReversePlayback) {
			animationTime -= deltaTime; // 逆再生時は時間を戻す
		}
		else {
			animationTime += deltaTime; // 通常再生時は時間を進める
		}
	}

	// ループ設定に合わせて時間を尺内へ丸める
	if (isLoop) {
		animationTime = WrapAnimationTime(animationTime, currentAnimation.duration);
	}
	else if (isReversePlayback) {
		if (animationTime <= 0.0f) {
			animationTime = 0.0f; // 逆再生の終端で停止する
			isPlaying = false;
		}
	}
	else if (animationTime >= currentAnimation.duration) {
		animationTime = currentAnimation.duration; // 通常再生の終端で停止する
		isPlaying = false;
	}
}

void Engine::AnimationComponent::AdvancePreviousAnimationTime(const Animation& previousAnimation, float deltaTime)
{
	// ブレンド元アニメーションの尺がない場合は先頭フレームを使う
	if (previousAnimation.duration <= 0.0f) {
		previousAnimationTime_ = 0.0f;
		return;
	}

	// ブレンド元も再生方向に合わせて別時間で進める
	if (isPlaying) {
		if (isReversePlayback) {
			previousAnimationTime_ -= deltaTime;
		}
		else {
			previousAnimationTime_ += deltaTime;
		}
	}

	// 前アニメーションはブレンド中の参照用なので常に尺内へ丸める
	previousAnimationTime_ = WrapAnimationTime(previousAnimationTime_, previousAnimation.duration);
}

float Engine::AnimationComponent::WrapAnimationTime(float time, float duration) const
{
	// 0秒アニメーションでfmodしないための安全ガード
	if (duration <= 0.0f) {
		return 0.0f;
	}

	// fmodは負値を返す可能性があるため、負の時間を正の尺内に戻す
	float wrapped = std::fmod(time, duration);
	if (wrapped < 0.0f) {
		wrapped += duration;
	}
	return wrapped;
}

float Engine::AnimationComponent::UpdateBlendRate(ModelData& modelData, float deltaTime)
{
	// ブレンド時間が0以下なら即座に現在アニメーションへ切り替える
	if (modelData.blendDuration <= 0.0f) {
		modelData.isBlending = false;
		modelData.previousAnimName.clear();
		return 1.0f;
	}

	// ブレンド経過時間を進めて0.0f～1.0fへ正規化する
	modelData.blendTime += deltaTime;
	float t = modelData.blendTime / modelData.blendDuration;
	t = std::clamp(t, 0.0f, 1.0f);

	// Hermite補間でブレンド開始・終了をなめらかにする
	float easedT = t * t * (3.0f - 2.0f * t);

	// ブレンドが終わったら前アニメーション名を消して通常再生へ戻す
	if (modelData.blendTime >= modelData.blendDuration) {
		modelData.isBlending = false;
		modelData.previousAnimName.clear();
	}

	return easedT;
}

void Engine::AnimationComponent::ApplySkinAnimation(ModelData& modelData, const Animation& currentAnimation, float deltaTime)
{
	const auto& animations = modelData.animations;

	// ブレンド中は前アニメーションと現在アニメーションを別々の時刻で評価する
	if (modelData.isBlending && !modelData.previousAnimName.empty()) {
		auto itPrev = animations.find(modelData.previousAnimName);
		if (itPrev != animations.end()) {
			const Animation& previousAnimation = itPrev->second;
			AdvancePreviousAnimationTime(previousAnimation, deltaTime);

			Skeleton previousSkeleton = modelData.skeleton; // ブレンド元の姿勢を作る一時スケルトン
			Skeleton currentSkeleton = modelData.skeleton;  // ブレンド先の姿勢を作る一時スケルトン
			AnimationFunction::ApplyAnimation(previousSkeleton, previousAnimation, previousAnimationTime_);
			AnimationFunction::ApplyAnimation(currentSkeleton, currentAnimation, animationTime);

			float t = UpdateBlendRate(modelData, deltaTime);
			AnimationFunction::BlendSkeletons(modelData.skeleton, previousSkeleton, currentSkeleton, t);
			return;
		}

		// 前アニメーションが存在しなければブレンドを解除して通常再生へ戻す
		modelData.isBlending = false;
		modelData.previousAnimName.clear();
	}

	// ブレンドしていない場合は現在アニメーションだけを適用する
	AnimationFunction::ApplyAnimation(modelData.skeleton, currentAnimation, animationTime);
}

void Engine::AnimationComponent::ApplyNodeAnimation(ModelData& modelData, const Animation& currentAnimation, float deltaTime)
{
	const auto& animations = modelData.animations;

	// 通常ノードアニメーションでも、前後アニメーションを別時刻で評価してブレンドする
	if (modelData.isBlending && !modelData.previousAnimName.empty()) {
		auto itPrev = animations.find(modelData.previousAnimName);
		if (itPrev != animations.end()) {
			const Animation& previousAnimation = itPrev->second;
			AdvancePreviousAnimationTime(previousAnimation, deltaTime);
			float t = UpdateBlendRate(modelData, deltaTime);

			for (const auto& [nodeName, currentNodeAnimation] : currentAnimation.nodeAnimations) {
				auto itPrevNode = previousAnimation.nodeAnimations.find(nodeName);
				if (itPrevNode != previousAnimation.nodeAnimations.end()) {
					// 同名ノードがある場合はTRSを成分ごとに補間する
					const NodeAnimation& previousNodeAnimation = itPrevNode->second;
					Vector3 previousTranslate = AnimationFunction::CalculateValue(previousNodeAnimation.translate.keyframes, previousAnimationTime_);
					Quaternion previousRotate = AnimationFunction::CalculateValue(previousNodeAnimation.rotate.keyframes, previousAnimationTime_);
					Vector3 previousScale = AnimationFunction::CalculateValue(previousNodeAnimation.scale.keyframes, previousAnimationTime_);
					Vector3 currentTranslate = AnimationFunction::CalculateValue(currentNodeAnimation.translate.keyframes, animationTime);
					Quaternion currentRotate = AnimationFunction::CalculateValue(currentNodeAnimation.rotate.keyframes, animationTime);
					Vector3 currentScale = AnimationFunction::CalculateValue(currentNodeAnimation.scale.keyframes, animationTime);

					Vector3 blendTranslate = Lerp(previousTranslate, currentTranslate, t);
					Quaternion blendRotate = Slerp(previousRotate, currentRotate, t);
					Vector3 blendScale = Lerp(previousScale, currentScale, t);
					localMatrix_ = MakeAffineMatrix(blendScale, blendRotate, blendTranslate);
				}
				else {
					// ブレンド元に無いノードは現在アニメーションだけを反映する
					localMatrix_ = CalculateNodeLocalMatrix(currentNodeAnimation, animationTime);
				}
			}
			return;
		}

		// 前アニメーションが存在しなければブレンドを解除して通常再生へ戻す
		modelData.isBlending = false;
		modelData.previousAnimName.clear();
	}

	// 各ノードのアニメーション行列を作る。既存仕様に合わせて最後に評価したノードをlocalMatrix_へ保持する
	for (const auto& [nodeName, nodeAnimation] : currentAnimation.nodeAnimations) {
		(void)nodeName; // ノード名はmap走査の識別用で、現在の行列計算には使わない
		localMatrix_ = CalculateNodeLocalMatrix(nodeAnimation, animationTime);
	}
}

Matrix4x4 Engine::AnimationComponent::CalculateNodeLocalMatrix(const NodeAnimation& nodeAnimation, float animationTime) const
{
	// 指定時刻のTRSを取得してローカル行列を作成する
	Vector3 interpTranslate = AnimationFunction::CalculateValue(nodeAnimation.translate.keyframes, animationTime);
	Quaternion interpRotate = AnimationFunction::CalculateValue(nodeAnimation.rotate.keyframes, animationTime);
	Vector3 interpScale = AnimationFunction::CalculateValue(nodeAnimation.scale.keyframes, animationTime);
	return MakeAffineMatrix(interpScale, interpRotate, interpTranslate);
}

void Engine::AnimationComponent::UpdateSkinClusters(ModelData& modelData)
{
	std::vector<Matrix4x4> cachedSkeletonMatrices;
	for (auto& mesh : modelData.mesh) {
		// メッシュまたはスキンクラスターが無い場合は更新を飛ばす
		if (!mesh || !mesh->skinCluster) {
			continue;
		}
		AnimationFunction::UpdateSkinCluster(*mesh->skinCluster, modelData.skeleton, cachedSkeletonMatrices);
	}
}

void Engine::AnimationComponent::UpdateMaterialGPUData(ModelData& modelData)
{
	std::vector<std::future<void>> futures;
	for (auto& mesh : modelData.mesh) {
		// マテリアルが無いメッシュはGPU転送できないためスキップする
		if (!mesh || !mesh->material) {
			continue;
		}

		Material* material = mesh->material.get(); // 非同期更新で使うマテリアル実体
		futures.push_back(std::async(std::launch::async, [material]() {
			material->GPUData();
			}));
	}

	// 全スレッドの終了を待ってから次の描画処理へ進む
	for (auto& f : futures) {
		f.get();
	}
}

void Engine::AnimationComponent::UpdateSkin(float deltatime, WorldTransform worldTransform)
{
	// モデルが設定されていない場合は更新できない
	if (!model) {
		return;
	}

	auto& modelData = model->GetModelData();
	const auto& animations = modelData.animations;
	auto itCurrent = animations.find(modelData.currentAnimName);

	if (itCurrent != animations.end()) {
		const Animation& currentAnimation = itCurrent->second;
		float deltaTime = deltatime * animationSpeed;
		AdvanceAnimationTime(currentAnimation, deltaTime);
		ApplySkinAnimation(modelData, currentAnimation, deltaTime);

		// スケルトン姿勢を更新してスキニング用パレットへ反映する
		AnimationFunction::UpdateSkeleton(modelData.skeleton);
		UpdateSkinClusters(modelData);

		// スキニング後の頂点は既にスケルトン空間へ変換済みなので、描画時にルート行列を二重に掛けない
		localMatrix_ = MakeIdentity4x4();
	}
	else {
		// アニメーションが見つからない場合はモデルのルート行列を使う
		localMatrix_ = modelData.rootNode.localMatrix;
		UpdateSkinClusters(modelData);
	}

	// デバッグ用にスケルトンを描画する
	AnimationFunction::DrawSkeleton(
		lineCommon,
		modelData.skeleton.joints,
		worldTransform.worldMat_.GetWorldPosition(),
		worldTransform.scale_,
		MakeRotateXYZ(worldTransform.rotate_)
	);

	// メッシュのマテリアルGPUデータを更新する
	UpdateMaterialGPUData(modelData);
}

void Engine::AnimationComponent::Update(float deltatime, WorldTransform worldTransform)
{
	(void)worldTransform; // 通常ノード更新ではワールド変換を直接使わない

	// モデルが設定されていない場合は更新できない
	if (!model) {
		return;
	}

	auto& modelData = model->GetModelData();
	const auto& animations = modelData.animations;
	auto itCurrent = animations.find(modelData.currentAnimName);

	if (itCurrent != animations.end()) {
		const Animation& currentAnimation = itCurrent->second;
		float deltaTime = deltatime * animationSpeed;
		AdvanceAnimationTime(currentAnimation, deltaTime);
		ApplyNodeAnimation(modelData, currentAnimation, deltaTime);
	}
	else {
		// アニメーションが見つからない場合はモデルのルート行列を使う
		localMatrix_ = modelData.rootNode.localMatrix;
	}

	// メッシュのマテリアルGPUデータを更新する
	UpdateMaterialGPUData(modelData);
}

bool Engine::AnimationComponent::IsAnimationFinished()
{
	if (!model) {
		return false;
	}

	const auto& animations = model->GetModelData().animations;
	const auto& modelData = model->GetModelData();
	auto itCurrent = animations.find(modelData.currentAnimName);

	// 現在のアニメーション名が見つからなかった場合は再生終了扱い
	if (itCurrent == animations.end()) {
		return true;
	}

	// 0秒アニメーションは停止状態なら終了扱いにする
	if (itCurrent->second.duration <= 0.0f) {
		return !isPlaying;
	}

	// アニメ再生中かつアニメ時間が終端に達していれば終了
	if (isReversePlayback) {
		return !isPlaying && animationTime <= 0.0f;
	}
	return !isPlaying && animationTime >= itCurrent->second.duration;
}

void Engine::AnimationComponent::SetAnimation(const std::string& name, float time) {
	// モデルが未設定ならアニメーションを切り替えられない
	if (!model) {
		return;
	}

	auto& modelData = model->GetModelData();
	if (modelData.animations.find(name) == modelData.animations.end()) {
		return;
	}

	// 切り替え前の時刻をブレンド元アニメーション用に保持する
	if (modelData.currentAnimName != name) {
		previousAnimationTime_ = animationTime;
		animationTime = 0.0f;
		isPlaying = true;
	}

	AnimationFunction::SetAnimation(modelData, name, time);
}

void Engine::AnimationComponent::SetEndAnimeTime() {
	// モデルが未設定なら終了時刻を取得できない
	if (!model) {
		return;
	}

	const auto& animations = model->GetModelData().animations;
	const auto& modelData = model->GetModelData();
	auto itCurrent = animations.find(modelData.currentAnimName);
	if (itCurrent == animations.end()) {
		return;
	}

	// 現在アニメーションの終端時間へ合わせる
	animationTime = itCurrent->second.duration;
}

float Engine::AnimationComponent::GetEndAnimeTime(std::string name) const {
	// モデルが未設定なら終了時刻を取得できない
	if (!model) {
		return 0.0f;
	}

	const auto& animations = model->GetModelData().animations;
	auto it = animations.find(name);
	if (it != animations.end()) {
		return it->second.duration;
	}

	// 見つからない場合は0秒として扱う
	return 0.0f;
}
