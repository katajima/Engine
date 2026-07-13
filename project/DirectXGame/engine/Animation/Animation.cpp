#include "Animation.h"
#include"DirectXGame/engine/3d/Model/Model.h"
#include <algorithm>
#include <execution> // C++17 以降
#include <numeric>

void Engine::SkinCluster::Finalize()
{
	// パレット用Uploadリソースは常時Mapして更新するため、破棄前に必ずUnmapする
	if (paletteResource && !mappedPalette.empty()) {
		paletteResource->Unmap(0, nullptr);
		mappedPalette = {};
	}

	// インフルエンス用UploadリソースもMapしたまま保持しているので、ここで対応して閉じる
	if (influenceResource && !mappedInfluence.empty()) {
		influenceResource->Unmap(0, nullptr);
		mappedInfluence = {};
	}

	// スキニング情報用UploadリソースのCPU書き込みポインタを無効化する
	if (skinningInfomation && skinningInfomationDeta) {
		skinningInfomation->Unmap(0, nullptr);
		skinningInfomationDeta = nullptr;
	}

	// GPUリソース本体を明示的に解放して、LiveObjectの残りを減らす
	influenceResource.Reset();
	paletteResource.Reset();
	outputVertexResource.Reset();
	skinningInfomation.Reset();
}

void Engine::AnimationFunction::ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime)
{
	
	for (Joint& joint : skeleton.joints) {
		// 対象のJointのAnimationがあれば、値の適用を行う。下記のif文はC++17から可能になった初期化付きif文。
		if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
			const NodeAnimation& rootNodeAnimation = (*it).second;
			joint.transform.translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);
			joint.transform.rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime);
			joint.transform.scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);
		}
	}
}

Vector3 Engine::AnimationFunction::CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time)
{
	assert(!keyframes.empty()); // キーがないものは返す値がわからないのでダメ
	if (keyframes.size() == 1 || time <= keyframes[0].time) { // キーが一つか、時刻がキーフレーム前なら最初の値を返す
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexの2つの keyFrameを取得して範囲内に時刻があるかを判断
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	// ここまで来た場合は一番最後の時刻よりも後ろなので最後の値を返すことにする
	return (*keyframes.rbegin()).value;
}

Quaternion Engine::AnimationFunction::CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time)
{
	assert(!keyframes.empty()); // キーがないものは返す値がわからないのでダメ
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value; // 最初のキーを返す
	}

	if (time >= keyframes.back().time) {
		return keyframes.back().value; // 最後のキーを返す
	}


	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexの2つのkeyframeを取得して範囲内に時刻があるかを判断
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	// ここまで来た場合は一番最後の時刻よりも後ろなので最後の値を返すことにする
	return (*keyframes.rbegin()).value;
}

void Engine::AnimationFunction::UpdateSkeleton(Skeleton& skeleton)
{
	// すべてのJointを更新。親が先に処理されるので通常ループで処理可能になっている
	for (Joint& joint : skeleton.joints) {
		// ローカル行列を計算
		
		joint.localMatrix = MakeAffineMatrix(joint.transform.scale, joint.transform.rotate, joint.transform.translate);
		
		// 親が存在する場合は親の変換を適用
		if (joint.parent) {
			// 親の行列を先に掛ける
			joint.skeletonSpaceMatrix = joint.localMatrix * skeleton.joints[*joint.parent].skeletonSpaceMatrix;
		}
		else {
			// 親がいない場合はローカル行列がそのままスケルトンスペース行列になる
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}
}

void Engine::AnimationFunction::BlendSkeletons(Skeleton& outSkeleton,
	const Skeleton& fromSkeleton,
	const Skeleton& toSkeleton,
	float t)
{
	size_t jointCount = fromSkeleton.joints.size();
	outSkeleton.joints.resize(jointCount);

	for (size_t i = 0; i < jointCount; ++i) {
		const Joint& fromJoint = fromSkeleton.joints[i];
		const Joint& toJoint = toSkeleton.joints[i];

		Joint& outJoint = outSkeleton.joints[i];

		// Transformの補間
		outJoint.transform.translate = Lerp(fromJoint.transform.translate, toJoint.transform.translate, t);
		outJoint.transform.scale = Lerp(fromJoint.transform.scale, toJoint.transform.scale, t);
		outJoint.transform.rotate = Slerp(fromJoint.transform.rotate, toJoint.transform.rotate, t);

		// 残りの固定情報はどちらでもOK
		outJoint.name = fromJoint.name;
		outJoint.index = fromJoint.index;
		outJoint.parent = fromJoint.parent;
		outJoint.children = fromJoint.children;
	}

	// 補間後、localMatrix / skeletonSpaceMatrix を更新する
	AnimationFunction::UpdateSkeleton(outSkeleton);
}

void Engine::AnimationFunction::DrawSkeleton(LineCommon* lineCommon,const std::vector<Joint>& joints, const Vector3& pos, const Vector3& scaleconst ,const Matrix4x4& rotationMatrix)
{
	// ライン描画が未初期化、またはジョイントが無い場合は描画しない
	if (!lineCommon || joints.empty()) {
		return;
	}

	std::vector<int> depths(joints.size(), 0);
	int maxDepth = 0;
	for (size_t i = 0; i < joints.size(); ++i) {
		depths[i] = CalculateDepth(joints, static_cast<int>(i));
		maxDepth = (std::max)(maxDepth, depths[i]);
	}

	for (const Joint& joint : joints) {
		if (joint.parent.has_value()) {
			const int32_t parentIndex = joint.parent.value();
			if (parentIndex < 0 || static_cast<size_t>(parentIndex) >= joints.size()) {
				continue; // 親Indexが不正なジョイントは描画しない
			}

			// ワールド座標を取得
			Vector3 parentPos = joints[parentIndex].skeletonSpaceMatrix.GetWorldPosition();
			Vector3 childPos = joint.skeletonSpaceMatrix.GetWorldPosition();

			// スケール適用
			parentPos = Multiply(parentPos, scaleconst);
			childPos = Multiply(childPos, scaleconst);

			// 回転適用
			parentPos = Transforms(parentPos, rotationMatrix);
			childPos = Transforms(childPos, rotationMatrix);

			// 平行移動適用
			parentPos = Add(parentPos, pos);
			childPos = Add(childPos, pos);

			// ライン描画
			lineCommon->GetDebugLineMeshData().AddLine(parentPos, childPos, {1,1,1,1});
		}
	}

}

void Engine::AnimationFunction::SetAnimation(ModelData& modelData, const std::string& newAnimName, float blendDuration)
{
	const auto& animations = modelData.animations;
	auto it = animations.find(newAnimName);
	if (it != animations.end()) {
		if (modelData.currentAnimName != newAnimName) {
			modelData.previousAnimName = modelData.currentAnimName;
			modelData.currentAnimName = newAnimName;
			modelData.blendTime = 0.0f;
			modelData.blendDuration = blendDuration;
			modelData.isBlending = true;
		}
	}

	
}

Engine::Joint* Engine::AnimationFunction::FindJointByName(Skeleton& skeleton, const std::string& name)
{
	auto it = skeleton.jointMap.find(name);
	if (it != skeleton.jointMap.end()) {
		int32_t index = it->second;
		if (index >= 0 && index < skeleton.joints.size()) {
			return &skeleton.joints[index];
		}
	}
	return nullptr;
}

const Engine::Joint* Engine::AnimationFunction::FindJointByNameConst(const Skeleton& skeleton, const std::string& name)
{
	auto it = skeleton.jointMap.find(name);
	if (it != skeleton.jointMap.end()) {
		int32_t index = it->second;
		if (index >= 0 && index < skeleton.joints.size()) {
			return &skeleton.joints[index];
		}
	}
	return nullptr;
}

Matrix4x4 Engine::AnimationFunction::GetWorldMatrixOfJoint(const Skeleton& skeleton, const std::string& jointName, const Matrix4x4& modelWorldMatrix)
{
	auto it = skeleton.jointMap.find(jointName);
	if (it != skeleton.jointMap.end()) {
		const Joint& joint = skeleton.joints[it->second];
		return Multiply(joint.skeletonSpaceMatrix,modelWorldMatrix);
	}
	return Matrix4x4::Identity(); // 見つからなければ単位行列
}

void Engine::AnimationFunction::UpdateSkinCluster(SkinCluster& skinCluster, const Skeleton& skeleton, std::vector<Matrix4x4>& cachedSkeletonMatrices)
{
	// スキニングに必要な配列数が揃っていない場合は、存在する範囲だけ更新する
	size_t jointCount = (std::min)({ skeleton.joints.size(), skinCluster.inverseBindPoseMatrices.size(), skinCluster.mappedPalette.size() });
	if (jointCount == 0) {
		return;
	}

	if (cachedSkeletonMatrices.size() != jointCount) {
		cachedSkeletonMatrices.resize(jointCount);
	}

	// スケルトン行列をキャッシュして並列更新中の参照を安定させる
	for (size_t i = 0; i < jointCount; ++i) {
		cachedSkeletonMatrices[i] = skeleton.joints[i].skeletonSpaceMatrix;
	}

	

	std::vector<size_t> indices(jointCount);
	std::iota(indices.begin(), indices.end(), 0);

	std::for_each(std::execution::par, indices.begin(), indices.end(), [&](size_t i) {
		auto skinnedMat = Multiply(skinCluster.inverseBindPoseMatrices[i], cachedSkeletonMatrices[i]);
		skinCluster.mappedPalette[i].skeletonSpaceMatrix = skinnedMat;
		skinCluster.mappedPalette[i].skeletonSpaceInverseTransposeMatrix = Transpose(Inverse(skinnedMat));
		});
}

void Engine::AnimationFunction::ValidateTransform(Joint& joint)
{
	if (joint.transform.scale.x == 0.0f || joint.transform.scale.y == 0.0f || joint.transform.scale.z == 0.0f) {
		//Logger::Log("Warning: Zero scale detected. Adjusting to default value.");
		joint.transform.scale = { 1.0f, 1.0f, 1.0f }; // デフォルト値に置き換え
	}
}

void Engine::AnimationFunction::ImGuiJoint(const std::vector<Joint>& joints)
{
	ImGui::Begin("Joint Info");
	for (const Joint& joint : joints) {
		//ImGui::Text("Joint Name: %s", joint.name.c_str());
		if (ImGui::CollapsingHeader(joint.name.c_str())) {

			ImGui::Text("Joint Index: %d", joint.index);


			Vector3 pos = joint.skeletonSpaceMatrix.GetWorldPosition();
			ImGui::InputFloat3("Position", &pos.x, "%.3f");
			ImGui::Separator();
			Matrix4x4 mat4x4 = joint.skeletonSpaceMatrix;
			ImGui::InputFloat4("mat[0][~]", &mat4x4.m[0][0], "%.3f");
			ImGui::InputFloat4("mat[1][~]", &mat4x4.m[1][0], "%.3f");
			ImGui::InputFloat4("mat[2][~]", &mat4x4.m[2][0], "%.3f");
			ImGui::InputFloat4("mat[3][~]", &mat4x4.m[3][0], "%.3f");

		}
	}
	ImGui::End();
}

void Engine::AnimationFunction::ImGuiNode(const std::vector<Node>& nodes)
{
	ImGui::Begin("Node Info");
	for (const Node& node : nodes) {
		ImGui::Text("Node Name: %s", node.name.c_str());


		Vector3 pos = node.localMatrix.GetWorldPosition();
		ImGui::InputFloat4("Position", &pos.x, "%.3f");
		Matrix4x4 mat4x4 = node.localMatrix;
		ImGui::InputFloat4("mat[0][~]", &mat4x4.m[0][0], "%.3f");
		ImGui::InputFloat4("mat[1][~]", &mat4x4.m[1][0], "%.3f");
		ImGui::InputFloat4("mat[2][~]", &mat4x4.m[2][0], "%.3f");
		ImGui::InputFloat4("mat[3][~]", &mat4x4.m[3][0], "%.3f");
		ImGui::Separator();
	}
	ImGui::End();
}

int Engine::AnimationFunction::CalculateDepth(const std::vector<Joint>& joints, int index)
{
	// 範囲外のIndexは深さ0として扱い、壊れたデータでの再帰クラッシュを防ぐ
	if (index < 0 || static_cast<size_t>(index) >= joints.size()) {
		return 0;
	}

	const Joint& joint = joints[index];
	if (!joint.parent.has_value()) {
		return 0; // ルートジョイントの深さは0
	}
	if (joint.parent.value() < 0 || static_cast<size_t>(joint.parent.value()) >= joints.size()) {
		return 0; // 親Indexが不正な場合はここで打ち切る
	}
	return 1 + AnimationFunction::CalculateDepth(joints, joint.parent.value());
}
