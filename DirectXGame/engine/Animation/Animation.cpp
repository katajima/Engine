#include "Animation.h"

void Animetion::ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime)
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

Vector3 Animetion::CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time)
{
	assert(!keyframes.empty()); // キーがないものは返す値がわからないのでダメ
	if (keyframes.size() == 1 || time <= keyframes[0].time) { // キーが一つか、時刻がキーフレーム前なら最初の値を返す
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexの2つのkeyframeを取得して範囲内に時刻があるかを判断
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	// ここまで来た場合は一番最後の時刻よりも後ろなので最後の値を返すことにする
	return (*keyframes.rbegin()).value;
}

Quaternion Animetion::CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time)
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

void Animetion::UpdateSkeleton(Skeleton& skeleton)
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

void Animetion::DrawSkeleton(LineCommon* lineCommon,const std::vector<Joint>& joints, const Vector3& pos, const Vector3& scale)
{
	// ジョイントごとの深さを計算して保存
	std::vector<int> depths(joints.size(), 0);
	int maxDepth = 0;
	for (size_t i = 0; i < joints.size(); ++i) {
		depths[i] = CalculateDepth(joints, static_cast<int>(i));
		maxDepth = (std::max)(maxDepth, depths[i]);
	}


	for (const Joint& joint : joints) {
		if (joint.parent.has_value()) {
			const int32_t parentIndex = joint.parent.value();
			const Vector3& parentPosition = joints[parentIndex].skeletonSpaceMatrix.GetWorldPosition() * scale;
			const Vector3& childPosition = joint.skeletonSpaceMatrix.GetWorldPosition() * scale;


			Vector3 offsetParentPosition = Add(parentPosition, pos);
			Vector3 offsetChildPosition = Add(childPosition, pos);
			lineCommon->AddLine(offsetParentPosition, offsetChildPosition, { 1,1,1,1 });

		}
	}

}

void Animetion::UpdateSkinCluster(SkinCluster& skinCluster, const Skeleton& skeleton)
{
	// サイズチェック
	assert(skinCluster.inverseBindPoseMatrices.size() == skeleton.joints.size());
	assert(skinCluster.mappedPalette.size() == skeleton.joints.size());

	for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex) {
		// スケルトンスペース行列を計算
		skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix =
			skinCluster.inverseBindPoseMatrices[jointIndex] * skeleton.joints[jointIndex].skeletonSpaceMatrix;

		// 逆転置行列を計算
		skinCluster.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix =
			Transpose(Inverse(skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix));
	}
}

void Animetion::ValidateTransform(Joint& joint)
{
	if (joint.transform.scale.x == 0.0f || joint.transform.scale.y == 0.0f || joint.transform.scale.z == 0.0f) {
		//Logger::Log("Warning: Zero scale detected. Adjusting to default value.");
		joint.transform.scale = { 1.0f, 1.0f, 1.0f }; // デフォルト値に置き換え
	}
}

void Animetion::ImGuiJoint(const std::vector<Joint>& joints)
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

void Animetion::ImGuiNode(const std::vector<Node>& nodes)
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

int Animetion::CalculateDepth(const std::vector<Joint>& joints, int index)
{
	const Joint& joint = joints[index];
	if (!joint.parent.has_value()) {
		return 0; // ルートジョイントの深さは0
	}
	return 1 + Animetion::CalculateDepth(joints, joint.parent.value());
}
