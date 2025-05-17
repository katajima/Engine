#include "ModelData.h"
#include "ModelCommon.h"

#pragma region Laod

// メッシュ読み込み
void LoadModel::LoadMesh(const aiScene* scene, ModelData& modelData, DirectXCommon* dxCommon)
{
	modelData.allMesh = std::make_unique<ModelMesh>();
	uint32_t vertexOffset = 0;

	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals()); // 法線がないMeshは今回は非対応
		//assert(mesh->HasTextureCoords(0)); //TexcoordがないMeshは今回は非対応
		std::unique_ptr<ModelMesh> pMesh = std::make_unique<ModelMesh>();

		pMesh->meshIndex = meshIndex;

		Vector3 min = { 10000,10000,10000 };
		Vector3 max = { -10000,-10000,-10000 };
		pMesh->vertices.resize(mesh->mNumVertices);
		pMesh->verticesline.resize(mesh->mNumVertices);

		if (mesh->HasTangentsAndBitangents()) { // 追加: タンジェント・ビタンジェントの確認
			modelData.isTangent = true;
		}
		else {
			modelData.isTangent = false;
		};

		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

			if (modelData.isTangent) {
				aiVector3D& tangent = mesh->mTangents[vertexIndex];
				aiVector3D& bitangent = mesh->mBitangents[vertexIndex];

				Vector3 nrm = { -normal.x, normal.y, normal.z };
				Vector3 tan = { -tangent.x, tangent.y, tangent.z };
				Vector3 bin = { -bitangent.x, bitangent.y, bitangent.z };

				// ハンドネス計算
				float w = (Dot(Cross(nrm, tan), bin) < 0.0f) ? -1.0f : 1.0f;

				pMesh->vertices[vertexIndex].tangent = { tan.x, tan.y, tan.z, w }; // ← w を追加
			}
			else {
				pMesh->vertices[vertexIndex].tangent = {};
			}

			Vector3 offset = modelData.meshOffsetMap[(scene->mNumMeshes - 1) - meshIndex];

			pMesh->vertices[vertexIndex].position = { -position.x + offset.x ,position.y + offset.y,position.z + offset.z,1.0f };
			pMesh->vertices[vertexIndex].normal = { -normal.x,normal.y,normal.z };
			if (mesh->HasTextureCoords(0)) {
				pMesh->vertices[vertexIndex].texcoord = { texcoord.x, texcoord.y };
			}
			else {
				pMesh->vertices[vertexIndex].texcoord = { 0.5f,0.5f };
			}

			pMesh->verticesline[vertexIndex].position = pMesh->vertices[vertexIndex].position;

			modelData.allMesh->vertices.push_back(pMesh->vertices[vertexIndex]);

			min = Min(min, pMesh->vertices[vertexIndex].position.xyz());
			max = Max(max, pMesh->vertices[vertexIndex].position.xyz());
		}

		pMesh->SetMin(min);
		pMesh->SetMax(max);

		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3); // 三角形のみサポート

			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				pMesh->indices.push_back(vertexIndex);
				modelData.allMesh->indices.push_back(vertexIndex + vertexOffset); // ★補正
			}
		}
		vertexOffset += mesh->mNumVertices; // ★次メッシュの頂点オフセット更新
		pMesh->Initialize(dxCommon);

		modelData.mesh.push_back(std::move(pMesh));
	}
	modelData.allMesh->Initialize(dxCommon);
}

// ボーン読み込み
void LoadModel::LoadBone(const aiScene* scene, ModelData& modelData, DirectXCommon* dxCommon)
{


	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];

		if (mesh->mNumBones == 0) {
			modelData.isAmimetion = false;
		}
		else {
			modelData.isAmimetion = true;
		}

		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = modelData.skinClusterData[jointName];

			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
			aiVector3D scale, translate;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
			Matrix4x4 bindPoseMatrix = MakeAffineMatrix(Vector3{ scale.x, scale.y, scale.z }, Quaternion{ rotate.x,-rotate.y,-rotate.z,rotate.w }, Vector3{ -translate.x,translate.y,translate.z });
			jointWeightData.inverseBindPoseMatrix = Inverse(bindPoseMatrix);

			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight,bone->mWeights[weightIndex].mVertexId });
			}
		}
	}
	modelData.skinning.wellSrvIndex = dxCommon->GetSrvManager()->Allocate();
	modelData.skinning.influencesIndex = dxCommon->GetSrvManager()->Allocate();
	modelData.skinning.inputVerticesIndex = dxCommon->GetSrvManager()->Allocate();
	modelData.skinning.outputVerticesUavIndex = dxCommon->GetSrvManager()->Allocate();
}

// マテリアル読み込み
void LoadModel::LoadMaterial(const aiScene* scene, ModelData& modelData, DirectXCommon* dxCommon, const std::string& directoryPath)
{
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		uint32_t materialIndex = mesh->mMaterialIndex;

		if (materialIndex >= scene->mNumMaterials) {
			std::cerr << "Invalid material index: " << materialIndex << std::endl;
			continue;
		}

		aiMaterial* material = scene->mMaterials[materialIndex];

		// メッシュに対するマテリアル作成
		std::unique_ptr<Material> pMaterial = std::make_unique<Material>();
		pMaterial->Initialize(dxCommon);

		// Diffuse
		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			std::cout << "Mesh[" << meshIndex << "] Diffuse: " << textureFilePath.C_Str() << std::endl;
			pMaterial->tex_.diffuseFilePath = directoryPath + "/" + textureFilePath.C_Str();
		}
		else {
			// テクスチャがない → ベースカラーを取得
			aiColor3D baseColor(1.0f, 1.0f, 1.0f); // デフォルト白
			if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor)) {
				std::cout << "Mesh[" << meshIndex << "] BaseColor: " << baseColor.r << "," << baseColor.g << "," << baseColor.b << std::endl;
				pMaterial->color = { baseColor.r, baseColor.g, baseColor.b, 1.0f };
				pMaterial->tex_.diffuseFilePath = "resources/Texture/Image.png";
			}
		}

		// Specular
		if (material->GetTextureCount(aiTextureType_SPECULAR) > 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_SPECULAR, 0, &textureFilePath);
			std::cout << "Mesh[" << meshIndex << "] Specular: " << textureFilePath.C_Str() << std::endl;
			pMaterial->tex_.speculerFilePath = directoryPath + "/" + textureFilePath.C_Str();
		}

		// Normal / Height
		if (material->GetTextureCount(aiTextureType_NORMALS) > 0 || material->GetTextureCount(aiTextureType_HEIGHT) > 0) {
			aiString textureFilePath;
			if (material->GetTextureCount(aiTextureType_NORMALS) > 0) {
				material->GetTexture(aiTextureType_NORMALS, 0, &textureFilePath);
			}
			else {
				material->GetTexture(aiTextureType_HEIGHT, 0, &textureFilePath);
			}
			std::cout << "Mesh[" << meshIndex << "] Normal/Height: " << textureFilePath.C_Str() << std::endl;
			pMaterial->tex_.normalFilePath = directoryPath + "/" + textureFilePath.C_Str();
		}

		//// テクスチャがない → ベースカラーを取得
		//aiColor3D baseColor(1.0f, 1.0f, 1.0f); // デフォルト白
		//if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor)) {
		//	std::cout << "Mesh[" << meshIndex << "] BaseColor: " << baseColor.r << "," << baseColor.g << "," << baseColor.b << std::endl;
		//	pMaterial->baseColor = { baseColor.r, baseColor.g, baseColor.b, 1.0f };
		//	pMaterial->useColor = true;
		//}

		// モデルデータ内のメッシュへマテリアルを割り当て
		assert(meshIndex < modelData.mesh.size());
		modelData.mesh[meshIndex]->material = std::move(pMaterial);
	}
}

// アニメーション読み込み
void LoadModel::LoadAnimation(ModelData& modelData, const std::string& directoryPath, const std::string& filename)
{
	//Animation animation; // 今回作るアニメーション
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);

	if (scene->mAnimations == 0) {
		modelData.animation.flag = false;
		return;
	}
	else {
		modelData.animation.flag = true;
	}

	assert(scene->mAnimations != 0); // アニメーションがない
	aiAnimation* animationAssimp = scene->mAnimations[0]; // 最初のアニメーションだけ採用。もちろん作数対応するに越したことはない
	modelData.animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);// 時間の単位を秒に変換

	// assimpでは個々のNodeのAnimationをchannelと呼んでいるのでchannelを回してNodeAnimationの情報をとってくる
	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = modelData.animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];

		// Position Keysの処理
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // ここも秒に変換
			keyframe.value = { -keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }; // 右手->左手
			nodeAnimation.translate.keyframes.push_back(keyframe);
		}

		// Rotate Keysの処理
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyframeQuaternion keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // ここも秒に変換
			keyframe.value = { keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w }; // 右手->左手
			nodeAnimation.rotate.keyframes.push_back(keyframe);
		}

		// Scale Keysの処理
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // ここも秒に変換
			keyframe.value = { keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }; // Scaleはそのまま
			nodeAnimation.scale.keyframes.push_back(keyframe);
		}
	}
}

// ノード読み込み
Node LoadModel::ReadNode(aiNode* node, std::unordered_map<uint32_t, Vector3>& meshOffsetMap)
{
	Node result;

	aiVector3D scale{}, translate{};
	aiQuaternion rotate{};
	node->mTransformation.Decompose(scale, rotate, translate); // assimpの行列からSRTを抽出する関数を利用

	result.transform.scale = { scale.x,scale.y,scale.z }; // Scaleはそのまま
	result.transform.rotate = { rotate.x,-rotate.y,-rotate.z,rotate.w }; // x軸を反転、さらに回転方向が逆なので軸を反転させる	
	result.transform.translate = { -translate.x,translate.y,translate.z }; // x軸を反転

	result.localMatrix = MakeAffineMatrix(result.transform.scale, result.transform.rotate, result.transform.translate);
	result.name = node->mName.C_Str(); // Node名を格納

	// ★追加：このノードに紐づくメッシュインデックスを記録
	result.meshIndices.resize(node->mNumMeshes);
	for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
		result.meshIndices[i] = node->mMeshes[i];
		meshOffsetMap[i] = { -translate.x, translate.y, translate.z };
	}


	result.children.resize(node->mNumChildren); // 子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		// 再帰的に読んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex], meshOffsetMap);
	}
	return result;
}

#pragma endregion // 読み込み系

#pragma region Create

void CreateModel::CreateMeshLine(ModelData& modelData, const std::vector<uint32_t>& indices)
{
	if (indices.empty() || indices.size() % 3 != 0) {
		// indicesが空 or 不正なサイズなら処理をスキップ
		std::cerr << "Error: Invalid indices size (" << indices.size() << ")" << std::endl;
		return;
	}

	std::unordered_map<uint64_t, bool> edgeMap;

	for (size_t i = 0; i < indices.size(); i += 3) {
		for (int j = 0; j < 3; ++j) {
			uint32_t v0 = indices[i + j];
			uint32_t v1 = indices[i + (j + 1) % 3];

			if (v0 > v1) std::swap(v0, v1);
			uint64_t edgeKey = (static_cast<uint64_t>(v0) << 32) | v1;

			if (edgeMap.find(edgeKey) == edgeMap.end()) {
				edgeMap[edgeKey] = true;
				modelData.cachedLineIndices_.push_back(v0);
				modelData.cachedLineIndices_.push_back(v1);
			}
		}
	}
}

void CreateModel::CreateSkeleton(ModelData& modelData)
{
	modelData.skeleton.root = CreateModel::CreateJoint(modelData.rootNode, {}, modelData.skeleton.joints);

	// 名前とindexのマッピングを行いアクセスしやすくする
	for (const Joint& joint : modelData.skeleton.joints) {
		modelData.skeleton.jointMap.emplace(joint.name, joint.index);
	}

	Animetion::UpdateSkeleton(modelData.skeleton);
}

void CreateModel::CreateSkinCluster(ModelData& modelData, ModelCommon* modelCommon)
{
	SkinCluster skinCluster;

	// palette用のResourceを確保
	skinCluster.paletteResource = modelCommon->GetDXGIDevice()->CreateBufferResource(sizeof(WellForGPU) * modelData.skeleton.joints.size());
	WellForGPU* mappedPalette = nullptr;
	skinCluster.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
	std::memset(mappedPalette, 0, sizeof(WellForGPU) * modelData.skeleton.joints.size());
	skinCluster.mappedPalette = { mappedPalette, modelData.skeleton.joints.size() }; // spanを使ってアクセスするようにする

	modelCommon->GetSrvManager()->CreateSRVforStructuredBuffer(modelData.skinning.wellSrvIndex, skinCluster.paletteResource.Get(), UINT(modelData.skeleton.joints.size()), sizeof(WellForGPU));
	skinCluster.paletteSrvHandle.first = modelCommon->GetSrvManager()->GetCPUDescriptorHandle(modelData.skinning.wellSrvIndex);
	skinCluster.paletteSrvHandle.second = modelCommon->GetSrvManager()->GetGPUDescriptorHandle(modelData.skinning.wellSrvIndex);



	// influence用のResourceを確保。頂点ごとにinfluence情報を追加できるようにする
	skinCluster.influenceResource = modelCommon->GetDXGIDevice()->CreateBufferResource(sizeof(VertexInfluence) * modelData.mesh[0]->vertices.size());
	VertexInfluence* mappedInfluence = nullptr;
	skinCluster.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData.mesh[0]->vertices.size()); // 仮埋め。weightを0にしておく。
	skinCluster.mappedInfluence = { mappedInfluence, modelData.mesh[0]->vertices.size() };

	// Influence用のVB作成
	skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGPUVirtualAddress();
	skinCluster.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData.mesh[0]->vertices.size());
	skinCluster.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

	modelCommon->GetSrvManager()->CreateSRVforStructuredBuffer(modelData.skinning.influencesIndex, skinCluster.influenceResource.Get(), UINT(modelData.mesh[0]->vertices.size()), sizeof(VertexInfluence));
	skinCluster.influenceSrvHandle.first = modelCommon->GetSrvManager()->GetCPUDescriptorHandle(modelData.skinning.influencesIndex);
	skinCluster.influenceSrvHandle.second = modelCommon->GetSrvManager()->GetGPUDescriptorHandle(modelData.skinning.influencesIndex);




	// inputVertex用
	modelCommon->GetSrvManager()->CreateSRVforStructuredBuffer(modelData.skinning.inputVerticesIndex, modelData.mesh[0]->GetVertexResource().Get(), UINT(modelData.mesh[0]->vertices.size()), sizeof(VertexData));
	skinCluster.inputVertexSrvHandle.first = modelCommon->GetSrvManager()->GetCPUDescriptorHandle(modelData.skinning.inputVerticesIndex);
	skinCluster.inputVertexSrvHandle.second = modelCommon->GetSrvManager()->GetGPUDescriptorHandle(modelData.skinning.inputVerticesIndex);




	// outputVertex用のResourceを確保。
	skinCluster.outputVertexResource = modelCommon->GetDXGIDevice()->CreateBufferResourceUAV(sizeof(VertexData) * modelData.mesh[0]->vertices.size());
	VertexData* mappedOutputVertex = nullptr;

	// 初期状態を UAV 用に遷移させる
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = skinCluster.outputVertexResource.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	modelCommon->GetCommand()->GetList()->ResourceBarrier(1, &barrier);


	// outputVertex用のVB作成
	skinCluster.outputBufferView.BufferLocation = skinCluster.outputVertexResource->GetGPUVirtualAddress();
	skinCluster.outputBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.mesh[0]->vertices.size());
	skinCluster.outputBufferView.StrideInBytes = sizeof(VertexData);

	modelCommon->GetSrvManager()->CreateUAVforStructuredBuffer(modelData.skinning.outputVerticesUavIndex, skinCluster.outputVertexResource.Get(), UINT(modelData.mesh[0]->vertices.size()), sizeof(VertexData));
	skinCluster.outputVertexUavHandle.first = modelCommon->GetSrvManager()->GetCPUDescriptorHandle(modelData.skinning.outputVerticesUavIndex);
	skinCluster.outputVertexUavHandle.second = modelCommon->GetSrvManager()->GetGPUDescriptorHandle(modelData.skinning.outputVerticesUavIndex);



	// skinningInfomation
	skinCluster.skinningInfomation = modelCommon->GetDXGIDevice()->CreateBufferResource(sizeof(SkinningInfomation));
	skinCluster.skinningInfomation->Map(0, nullptr, reinterpret_cast<void**>(&skinCluster.skinningInfomationDeta));
	skinCluster.skinningInfomationDeta->numVertices = static_cast<uint32_t>(modelData.mesh[0]->vertices.size());



	// InverseBindPoseMatrixを格納する場所を作成して、単位行列で埋める
	skinCluster.inverseBindPoseMatrices.resize(modelData.skeleton.joints.size());
	std::generate(skinCluster.inverseBindPoseMatrices.begin(), skinCluster.inverseBindPoseMatrices.end(), MakeIdentity4x4);


	for (const auto& jointWeight : modelData.skinClusterData) {
		auto it = modelData.skeleton.jointMap.find(jointWeight.first);
		if (it == modelData.skeleton.jointMap.end()) {
			continue;
		}
		skinCluster.inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
			auto& currentInfluence = skinCluster.mappedInfluence[vertexWeight.vertexIndex];
			bool weightSet = false;
			for (uint32_t index = 0; index < kNumMaxInfluence; ++index) {
				if (currentInfluence.weights[index] == 0.0f) {
					currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					weightSet = true;
					break;
				}
			}
			if (!weightSet) {
				// デバッグ用出力
				Logger::Log(StringUtility::ConvertString(std::format(L"Warning: Vertex %d has more influences than supported.\n", vertexWeight.vertexIndex)));
			}
		}
	}


	modelData.skinCluster = skinCluster;
}

int32_t CreateModel::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints)
{
	Joint joint{};
	joint.name = node.name;
	joint.localMatrix = node.localMatrix;
	joint.skeletonSpaceMatrix = MakeIdentity4x4();
	joint.transform = node.transform;
	joint.index = int32_t(joints.size()); // 登録されている数をIndexに
	joint.parent = parent;
	joints.push_back(joint); // SkeletonのJoint列に追加
	for (const Node& child : node.children) {
		// 子Jointを作成し、そのIndexを登録
		int32_t childIndex = CreateModel::CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}
	// 自身のIndexを返す
	return joint.index;
}

#pragma endregion // 生成系

#pragma region Debug

void DebugModel::ImguiSkin(ModelData& modelData)
{
	if (ImGui::CollapsingHeader("SkinnigData")) {
		int index = static_cast<int>(modelData.skinning.influencesIndex);
		ImGui::InputInt("influencesIndex", &index);
		index = static_cast<int>(modelData.skinning.wellSrvIndex);
		ImGui::InputInt("wellSrvIndex", &index);
		index = static_cast<int>(modelData.skinning.inputVerticesIndex);
		ImGui::InputInt("inputVerticesIndex", &index);
		index = static_cast<int>(modelData.skinning.outputVerticesUavIndex);
		ImGui::InputInt("outputVerticesUavIndex", &index);
		index = static_cast<int>(modelData.skinCluster.skinningInfomationDeta->numVertices);
		ImGui::InputInt("numVertices", &index);
	}
}

void DebugModel::ImguiModel(ModelData& modelData)
{
	if (ImGui::CollapsingHeader("Modeldata")) {
		if (ImGui::TreeNode("rootNode")) {
			for (size_t i = 0; i < modelData.rootNode.children.size(); ++i) {
				std::string name = "children" + std::to_string(i);
				if (ImGui::TreeNode(name.c_str())) {
					ImGui::DragFloat3("translate", &modelData.rootNode.children[i].transform.translate.x);
					ImGui::DragFloat3("rotate", &modelData.rootNode.children[i].transform.rotate.x);
					ImGui::DragFloat3("scale", &modelData.rootNode.children[i].transform.scale.x);

					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}
}

#pragma endregion // デバッグ系