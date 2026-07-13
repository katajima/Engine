#include "ParticleManager.h"
#include "DirectXGame/engine/base/Texture/TextureManager.h"
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include"DirectXGame/engine/Manager/SRV/SrvManager.h"
#include"DirectXGame/engine/Line/LineCommon.h"
#include "DirectXGame/engine/Effect/Primitive/Primitive.h"
#include"DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/engine/Material/Material.h"
#include "DirectXGame/engine/Light/LightCommon.h"

#include "imgui.h"
#include <execution>
#undef max
#include <limits>
#include <windows.h>

namespace {
	std::unique_ptr<Engine::BasePrimitive> CreateEditorPrimitive(Engine::ShapeParameter::ShapeType shapeType) {
		// エディタで選ばれた形状に合わせ、ParticleManagerが寿命を持つプリミティブを生成する。
		switch (shapeType)
		{
		case Engine::ShapeParameter::ShapeType::Triangle:
			return std::make_unique<Engine::TrianglePrimitive>();
		case Engine::ShapeParameter::ShapeType::Cross:
			return std::make_unique<Engine::CrossPrimitive>();
		case Engine::ShapeParameter::ShapeType::Cube:
			return std::make_unique<Engine::CubePrimitive>();
		case Engine::ShapeParameter::ShapeType::Circle:
			return std::make_unique<Engine::CirclePrimitive>();
		case Engine::ShapeParameter::ShapeType::Star:
			return std::make_unique<Engine::StarPrimitive>();
		case Engine::ShapeParameter::ShapeType::Crescent:
			return std::make_unique<Engine::CrescentPrimitive>();
		case Engine::ShapeParameter::ShapeType::Ring:
			return std::make_unique<Engine::RingPrimitive>();
		case Engine::ShapeParameter::ShapeType::Sphere:
			return std::make_unique<Engine::SpherePrimitive>();
		case Engine::ShapeParameter::ShapeType::Arrow:
			return std::make_unique<Engine::ArrowPrimitive>();
		case Engine::ShapeParameter::ShapeType::Cylinder:
			return std::make_unique<Engine::CylinderPrimitive>();
		case Engine::ShapeParameter::ShapeType::Tube:
			return std::make_unique<Engine::TubePrimitive>();
		case Engine::ShapeParameter::ShapeType::Pyramid:
			return std::make_unique<Engine::PyramidPrimitive>();
		case Engine::ShapeParameter::ShapeType::Torus:
			return std::make_unique<Engine::TorusPrimitive>();
		case Engine::ShapeParameter::ShapeType::Plane:
		default:
			return std::make_unique<Engine::PlanePrimitive>();
		}
	}

	void ApplyPrimitiveShapeData(Engine::BasePrimitive* primitive, const Engine::ParticleGroupEditorData& data) {
		if (primitive == nullptr) {
			return;
		}

		// 保存済み形状パラメータを、現在のプリミティブ型に合わせて反映する。
		switch (data.shapeType)
		{
		case Engine::ShapeParameter::ShapeType::Plane:
			if (auto* typed = dynamic_cast<Engine::PlanePrimitive*>(primitive)) typed->Data() = data.plane;
			break;
		case Engine::ShapeParameter::ShapeType::Triangle:
			if (auto* typed = dynamic_cast<Engine::TrianglePrimitive*>(primitive)) typed->Data() = data.triangle;
			break;
		case Engine::ShapeParameter::ShapeType::Cross:
			if (auto* typed = dynamic_cast<Engine::CrossPrimitive*>(primitive)) typed->Data() = data.cross;
			break;
		case Engine::ShapeParameter::ShapeType::Cube:
			if (auto* typed = dynamic_cast<Engine::CubePrimitive*>(primitive)) typed->Data() = data.cube;
			break;
		case Engine::ShapeParameter::ShapeType::Circle:
			if (auto* typed = dynamic_cast<Engine::CirclePrimitive*>(primitive)) typed->Data() = data.circle;
			break;
		case Engine::ShapeParameter::ShapeType::Star:
			if (auto* typed = dynamic_cast<Engine::StarPrimitive*>(primitive)) typed->Data() = data.star;
			break;
		case Engine::ShapeParameter::ShapeType::Crescent:
			if (auto* typed = dynamic_cast<Engine::CrescentPrimitive*>(primitive)) typed->Data() = data.crescent;
			break;
		case Engine::ShapeParameter::ShapeType::Ring:
			if (auto* typed = dynamic_cast<Engine::RingPrimitive*>(primitive)) typed->Data() = data.ring;
			break;
		case Engine::ShapeParameter::ShapeType::Sphere:
			if (auto* typed = dynamic_cast<Engine::SpherePrimitive*>(primitive)) typed->Data() = data.sphere;
			break;
		case Engine::ShapeParameter::ShapeType::Arrow:
			if (auto* typed = dynamic_cast<Engine::ArrowPrimitive*>(primitive)) typed->Data() = data.arrow;
			break;
		case Engine::ShapeParameter::ShapeType::Cylinder:
			if (auto* typed = dynamic_cast<Engine::CylinderPrimitive*>(primitive)) typed->Data() = data.cylinder;
			break;
		case Engine::ShapeParameter::ShapeType::Tube:
			if (auto* typed = dynamic_cast<Engine::TubePrimitive*>(primitive)) typed->Data() = data.tube;
			break;
		case Engine::ShapeParameter::ShapeType::Pyramid:
			if (auto* typed = dynamic_cast<Engine::PyramidPrimitive*>(primitive)) typed->Data() = data.pyramid;
			break;
		case Engine::ShapeParameter::ShapeType::Torus:
			if (auto* typed = dynamic_cast<Engine::TorusPrimitive*>(primitive)) typed->Data() = data.torus;
			break;
		default:
			break;
		}
		primitive->MeshInitialize();
	}

	void CapturePrimitiveShapeData(const Engine::BasePrimitive* primitive, Engine::ParticleGroupEditorData& data) {
		if (primitive == nullptr) {
			return;
		}

		// エディタ所有プリミティブの現在値を保存用データへ吸い上げる。
		switch (data.shapeType)
		{
		case Engine::ShapeParameter::ShapeType::Plane:
			if (auto* typed = dynamic_cast<const Engine::PlanePrimitive*>(primitive)) data.plane = const_cast<Engine::PlanePrimitive*>(typed)->Data();
			break;
		case Engine::ShapeParameter::ShapeType::Triangle:
			if (auto* typed = dynamic_cast<const Engine::TrianglePrimitive*>(primitive)) data.triangle = const_cast<Engine::TrianglePrimitive*>(typed)->Data();
			break;
		case Engine::ShapeParameter::ShapeType::Cross:
			if (auto* typed = dynamic_cast<const Engine::CrossPrimitive*>(primitive)) data.cross = const_cast<Engine::CrossPrimitive*>(typed)->Data();
			break;
		case Engine::ShapeParameter::ShapeType::Cube:
			if (auto* typed = dynamic_cast<const Engine::CubePrimitive*>(primitive)) data.cube = const_cast<Engine::CubePrimitive*>(typed)->Data();
			break;
		case Engine::ShapeParameter::ShapeType::Circle:
			if (auto* typed = dynamic_cast<const Engine::CirclePrimitive*>(primitive)) data.circle = const_cast<Engine::CirclePrimitive*>(typed)->Data();
			break;
		case Engine::ShapeParameter::ShapeType::Star:
			if (auto* typed = dynamic_cast<const Engine::StarPrimitive*>(primitive)) data.star = const_cast<Engine::StarPrimitive*>(typed)->Data();
			break;
		case Engine::ShapeParameter::ShapeType::Crescent:
			if (auto* typed = dynamic_cast<const Engine::CrescentPrimitive*>(primitive)) data.crescent = const_cast<Engine::CrescentPrimitive*>(typed)->Data();
			break;
		case Engine::ShapeParameter::ShapeType::Ring:
			if (auto* typed = dynamic_cast<const Engine::RingPrimitive*>(primitive)) data.ring = const_cast<Engine::RingPrimitive*>(typed)->Data();
			break;
		case Engine::ShapeParameter::ShapeType::Sphere:
			if (auto* typed = dynamic_cast<const Engine::SpherePrimitive*>(primitive)) data.sphere = const_cast<Engine::SpherePrimitive*>(typed)->Data();
			break;
		case Engine::ShapeParameter::ShapeType::Arrow:
			if (auto* typed = dynamic_cast<const Engine::ArrowPrimitive*>(primitive)) data.arrow = const_cast<Engine::ArrowPrimitive*>(typed)->Data();
			break;
		case Engine::ShapeParameter::ShapeType::Cylinder:
			if (auto* typed = dynamic_cast<const Engine::CylinderPrimitive*>(primitive)) data.cylinder = const_cast<Engine::CylinderPrimitive*>(typed)->Data();
			break;
		case Engine::ShapeParameter::ShapeType::Tube:
			if (auto* typed = dynamic_cast<const Engine::TubePrimitive*>(primitive)) data.tube = const_cast<Engine::TubePrimitive*>(typed)->Data();
			break;
		case Engine::ShapeParameter::ShapeType::Pyramid:
			if (auto* typed = dynamic_cast<const Engine::PyramidPrimitive*>(primitive)) data.pyramid = const_cast<Engine::PyramidPrimitive*>(typed)->Data();
			break;
		case Engine::ShapeParameter::ShapeType::Torus:
			if (auto* typed = dynamic_cast<const Engine::TorusPrimitive*>(primitive)) data.torus = const_cast<Engine::TorusPrimitive*>(typed)->Data();
			break;
		default:
			break;
		}
	}

	Engine::ParticleGroupEditorData CaptureParticleGroupData(
		const Engine::ParticleGroup& group, const Engine::ParticleGroupEditorData& baseData) {
		// 既存の保存メタデータに、現在のパーティクル群実体の編集値を重ねて外部保存用データにする。
		Engine::ParticleGroupEditorData data = baseData;
		data.rasterizerType = group.rasteType;
		data.blendType = group.blendType;
		data.isUVClamp = group.isUVClamp;
		data.uvTransformVelocity = group.uvTransformVeloctiy_;
		data.isFlag = group.isFlag;
		data.emitType = group.emitType;
		data.topBottom = group.topBottom;
		data.gravitationalAcceleration = group.kGravitationalAcceleration;

		if (group.material) {
			// マテリアルはGPU転送用の実データから取得し、次回起動時に同じ見た目へ戻せるようにする。
			MaterialInstance& material = group.material->GetMaterialInstance();
			data.texturePath = group.material->tex_.diffuseFilePath;
			data.materialTransform = material.transform;
			data.materialColor = material.color.ToVector4();
			data.materialEnableLighting = material.enableLighting_ != 0;
			data.materialEnvironmentCoefficient = material.environmentCoefficient_;
			data.materialShininess = material.shininess_;
			data.materialUseLig = material.useLig_ != 0;
			data.materialUseNormalMap = material.useNormalMap_ != 0;
			data.materialUseSpecularMap = material.useSpecularMap_ != 0;
			data.materialUseEnvironment = material.useEnvironment_;
			data.materialAlphaClipping = material.alphaClipping_;
			data.materialAlpha = material.alpha_;
		}
		return data;
	}
}

void Engine::ParticleManager::Initialize(DirectXCommon* dxCommon, LightManager* lightManager, EffectManager* efectManager)
{
	this->dxCommon = dxCommon;							// DX共通クラス
	this->efectManager = efectManager;					// エフェクト管理クラス
	this->lineCommon = efectManager->GetLineCommon();	// ライン共通クラス
	this->srvManager = dxCommon->GetSrvManager();		// SRV管理クラス
	this->lightManager = lightManager;					// ライト管理クラス

	// PSOマネージャー初期化
	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon->GetCommand(), dxCommon->GetDXGIDevice(), dxCommon->GetDXCCompiler());

	// パイプライン生成
	CreateGraphicsPipeline();
}

void Engine::ParticleManager::DrawCommonSetting(EmitData::RasterizerType rasteType, EmitData::BlendType blendType, bool uvClamp)
{
	if (!uvClamp) {
		switch (blendType)
		{
		case EmitData::BlendType::MODE_ADD:
			if (rasteType == EmitData::RasterizerType::MODE_SOLID_BACK) {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[0].Get());
			}
			else {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[1].Get());
			}
			break;
		case EmitData::BlendType::MODE_SUBTRACT:
			if (rasteType == EmitData::RasterizerType::MODE_SOLID_BACK) {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[2].Get());
			}
			else {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[3].Get());
			}
			break;
		case EmitData::BlendType::MODE_MULTIPLY:
			if (rasteType == EmitData::RasterizerType::MODE_SOLID_BACK) {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[4].Get());
			}
			else {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[5].Get());
			}
			break;
		default:
			break;
		}
		//// RootSignatureを設定。PSOに設定しているけど別途設定が必要
		dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
	}
	else {

		switch (blendType)
		{
		case EmitData::BlendType::MODE_ADD:
			if (rasteType == EmitData::RasterizerType::MODE_SOLID_BACK) {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[6].Get());
			}
			else {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[7].Get());
			}
			break;
		case EmitData::BlendType::MODE_SUBTRACT:
			if (rasteType == EmitData::RasterizerType::MODE_SOLID_BACK) {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[8].Get());
			}
			else {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[9].Get());
			}
			break;
		case EmitData::BlendType::MODE_MULTIPLY:
			if (rasteType == EmitData::RasterizerType::MODE_SOLID_BACK) {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[10].Get());
			}
			else {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[11].Get());
			}
			break;
		default:
			break;
		}
		//// RootSignatureを設定。PSOに設定しているけど別途設定が必要
		dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature2.Get());
	}

	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Engine::ParticleManager::Update() {

	if (particleGroups.empty()) {
		return;
	}
#ifdef _DEBUG
	ImGui::Begin("Field");
	for (auto& acc : fieldEffect_) {
		//	acc->DebugImgui();
	}
	ImGui::End();
#endif // _DEBUG

	float deltaTime = MyGame::GameTime(); // 毎フレーム一定時間と仮定

	std::for_each(std::execution::par, particleGroups.begin(), particleGroups.end(),
		[&](auto& pair) {

			ParticleGroup& group = pair.second;
			group.instanceCount = 0;



			// パーティクル粒子更新
			for (auto particleIterator = group.particle.begin(); particleIterator != group.particle.end(); ) {

				// ライフタイム
				if (particleIterator->lifeTime <= particleIterator->currentTime) {
					particleIterator = group.particle.erase(particleIterator);
					continue;
				}

				// 上限まで
				if (group.instanceCount < kNumMaxInstance) {

					// 場所による影響
					Field::Effect(group, particleIterator, fieldEffect_, deltaTime);

					// パーティクル影響
					ParticleFunction::Effect(group, particleIterator, deltaTime);

					// パーティクルデータをGPUに送る
					ParticleFunction::WorldDataForGPU(group, particleIterator, camera);

					// 加算
					++group.instanceCount;
				}

				++particleIterator;
			}
			// マテリアルデータ
			if(group.material)
			ParticleFunction::MaterialEffect(group);
		});
}

void Engine::ParticleManager::Draw()
{
	auto commandList = dxCommon->GetCommandList();

	for (auto& pair : particleGroups) {
		ParticleGroup& group = pair.second;
		if (group.instanceCount == 0) {
			continue;
		}
		DrawCommonSetting(group.rasteType, group.blendType, group.isUVClamp);

		group.material->GetCommandListTexture(2);

		group.material->GetCommandListMaterial(0);

		lightManager->DrawLight({ true,false,false }, 3);

		group.sbParticleResource_.SetGraphicsRootDescriptorTable(1);

		group.mesh->GetCommandList();

		// インスタンシング描画
		uint32_t instanceCount = (std::min)(group.instanceCount, kNumMaxInstance);
		commandList->DrawIndexedInstanced(static_cast<UINT>(group.mesh->GetIndices().size()), instanceCount, 0, 0, 0);
	}
}

void Engine::ParticleManager::CreateParticleGroup(const std::string name, const std::string textureFilePath, Model* model, EmitData::RasterizerType rasteType, EmitData::BlendType blendType)
{
	debugTimer_.StartTimer(); // デバッグ用タイマー開始
	// ランダムエンジンの初期化
	std::random_device seedGenerator;
	randomEngine_.seed(seedGenerator()); // randomEngine_ にシードを設定

	// あるなら
	if (particleGroups.Contains(name)) {
		return;
	}

	// パーティクルグループ生成
	ParticleFunction::Create(particleGroups[name], name, textureFilePath, kNumMaxInstance, dxCommon, model->GetModelData().mesh[0].get(), rasteType, blendType);
	editorParticleGroupDatas_[name] = ParticleGroupEditorData{
		.texturePath = textureFilePath,
		.shapeType = ShapeParameter::ShapeType::Plane,
		.rasterizerType = rasteType,
		.blendType = blendType,
		.isEditorPrimitive = false,
	};
	debugTimer_.EndTimer(); // デバッグ用タイマー終了
	debugTimer_.LogTimeSec("CreateParticleGroup ", " name");
}

void Engine::ParticleManager::CreateParticleGroup(const std::string name, const std::string textureFilePath, BasePrimitive* primitive, EmitData::RasterizerType rasteType, EmitData::BlendType blendType)
{
	debugTimer_.StartTimer(); // デバッグ用タイマー開始
	// ランダムエンジンの初期化
	std::random_device seedGenerator;
	randomEngine_.seed(seedGenerator()); // randomEngine_ にシードを設定

	// あるなら
	if (particleGroups.Contains(name)) {
		return;
	}

	// パーティクルグループ生成
	ParticleFunction::Create(particleGroups[name], name, textureFilePath, kNumMaxInstance, dxCommon, primitive->GetModelMesh(), rasteType, blendType);
	editorParticleGroupDatas_[name] = ParticleGroupEditorData{
		.texturePath = textureFilePath,
		.shapeType = ShapeParameter::ShapeType::Plane,
		.rasterizerType = rasteType,
		.blendType = blendType,
		.isEditorPrimitive = false,
	};

	debugTimer_.EndTimer(); // デバッグ用タイマー終了
	debugTimer_.LogTimeSec("CreateParticleGroup ", " name");
}

bool Engine::ParticleManager::CreateEditorParticleGroup(const std::string& name, const ParticleGroupEditorData& data)
{
	if (name.empty() || particleGroups.Contains(name) || primitiveCommon == nullptr) {
		return false;
	}

	// パーティクル群が参照するメッシュの寿命を保つため、プリミティブをManager側で所有する。
	auto primitive = CreateEditorPrimitive(data.shapeType);
	primitive->Initialize(primitiveCommon, data.texturePath);
	ApplyPrimitiveShapeData(primitive.get(), data);

	BasePrimitive* primitivePtr = primitive.get();
	editorParticlePrimitives_[name] = std::move(primitive);
	ParticleFunction::Create(particleGroups[name], name, data.texturePath, kNumMaxInstance,
		dxCommon, primitivePtr->GetModelMesh(), data.rasterizerType, data.blendType);
	editorParticleGroupDatas_[name] = data;
	editorParticleGroupDatas_[name].isEditorPrimitive = true;
	ApplyEditorParticleGroupData(name, editorParticleGroupDatas_[name]);
	return true;
}

bool Engine::ParticleManager::RecreateEditorParticleGroup(const std::string& name, const ParticleGroupEditorData& data)
{
	if (name.empty() || !particleGroups.Contains(name)) {
		return false;
	}

	RemoveParticleGroup(name);
	return CreateEditorParticleGroup(name, data);
}

bool Engine::ParticleManager::RemoveParticleGroup(const std::string& name)
{
	if (!particleGroups.Contains(name)) {
		return false;
	}

	particleGroups.data.erase(name);
	editorParticlePrimitives_.erase(name);
	editorParticleGroupDatas_.erase(name);
	return true;
}

bool Engine::ParticleManager::RenameParticleGroup(const std::string& oldName, const std::string& newName)
{
	if (oldName.empty() || newName.empty() || oldName == newName ||
		!particleGroups.Contains(oldName) || particleGroups.Contains(newName)) {
		return false;
	}

	auto node = particleGroups.data.extract(oldName);
	node.key() = newName;
	particleGroups.data.insert(std::move(node));
	particleGroups.data[newName].name = newName;

	auto primitiveIt = editorParticlePrimitives_.find(oldName);
	if (primitiveIt != editorParticlePrimitives_.end()) {
		editorParticlePrimitives_[newName] = std::move(primitiveIt->second);
		editorParticlePrimitives_.erase(primitiveIt);
	}

	auto dataIt = editorParticleGroupDatas_.find(oldName);
	if (dataIt != editorParticleGroupDatas_.end()) {
		editorParticleGroupDatas_[newName] = dataIt->second;
		editorParticleGroupDatas_.erase(dataIt);
	}
	return true;
}

Engine::ParticleGroupEditorData Engine::ParticleManager::GetEditorParticleGroupData(const std::string& name) const
{
	ParticleGroupEditorData data;
	auto dataIt = editorParticleGroupDatas_.find(name);
	if (dataIt != editorParticleGroupDatas_.end()) {
		data = dataIt->second;
	}

	auto groupIt = particleGroups.data.find(name);
	if (groupIt != particleGroups.data.end()) {
		data = CaptureParticleGroupData(groupIt->second, data);
		auto primitiveIt = editorParticlePrimitives_.find(name);
		if (primitiveIt != editorParticlePrimitives_.end()) {
			CapturePrimitiveShapeData(primitiveIt->second.get(), data);
		}
		return data;
	}
	return data;
}

void Engine::ParticleManager::SetEditorParticleGroupData(const std::string& name, const ParticleGroupEditorData& data)
{
	editorParticleGroupDatas_[name] = data;
}

void Engine::ParticleManager::ApplyEditorParticleGroupData(const std::string& name, const ParticleGroupEditorData& data)
{
	if (!particleGroups.Contains(name)) {
		return;
	}

	// 保存データをパーティクル群の実体へ反映し、次回保存時にも同じ値を取り出せるよう保持する。
	ParticleGroup& group = particleGroups[name];
	auto primitiveIt = editorParticlePrimitives_.find(name);
	if (primitiveIt != editorParticlePrimitives_.end()) {
		// プリミティブ形状を変更した場合は、パーティクル群が参照するメッシュも再生成する。
		ApplyPrimitiveShapeData(primitiveIt->second.get(), data);
		group.mesh = primitiveIt->second->GetModelMesh();
	}
	group.rasteType = data.rasterizerType;
	group.blendType = data.blendType;
	group.isUVClamp = data.isUVClamp;
	group.uvTransformVeloctiy_ = data.uvTransformVelocity;
	group.isFlag = data.isFlag;
	group.emitType = data.emitType;
	group.topBottom = data.topBottom;
	group.kGravitationalAcceleration = data.gravitationalAcceleration;

	if (group.material) {
		// テクスチャとマテリアル値を再設定し、GPU転送データにも反映する。
		MaterialInstance& material = group.material->GetMaterialInstance();
		if (group.material->tex_.diffuseFilePath != data.texturePath) {
			group.material->tex_.diffuseFilePath = data.texturePath;
			group.material->LoadTex();
		}
		material.transform = data.materialTransform;
		material.color = Color(data.materialColor.x, data.materialColor.y, data.materialColor.z, data.materialColor.w);
		material.enableLighting_ = data.materialEnableLighting;
		material.environmentCoefficient_ = data.materialEnvironmentCoefficient;
		material.shininess_ = data.materialShininess;
		material.useLig_ = data.materialUseLig;
		material.useNormalMap_ = data.materialUseNormalMap;
		material.useSpecularMap_ = data.materialUseSpecularMap;
		material.useEnvironment_ = data.materialUseEnvironment;
		material.alphaClipping_ = data.materialAlphaClipping;
		material.alpha_ = data.materialAlpha;
		group.material->GPUData();
	}

	editorParticleGroupDatas_[name] = CaptureParticleGroupData(group, data);
}

#pragma region PSO

void Engine::ParticleManager::ClearParticle(std::string name)
{
	particleGroups[name].particle.clear();
}

void Engine::ParticleManager::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[2] = {};
	PSOFunction::SetDescriptorRange(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // テクスチャ用
	PSOFunction::SetDescriptorRange(descriptorRange[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // インスタンシング用


	// RootParameter作成。複数指定できるのではい
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	// マテリアル (b0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// インスタンシング(t1) をバーテックシェーダ使用する
	PSOFunction::SetRootParameter(rootParameters[1], descriptorRange[1], D3D12_SHADER_VISIBILITY_VERTEX);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(rootParameters[2], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);
	// 方向ライト (b1) をバーテックスシェーダで使用する
	PSOFunction::SetRootParameter(rootParameters[3], 1, D3D12_SHADER_VISIBILITY_VERTEX, D3D12_ROOT_PARAMETER_TYPE_CBV);


	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	PSOFunction::SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);// バイリニアフィルタ


	// ルートシグネチャ作成
	psoManager_->SetRootSignature(rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));

	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	psoManager_->SetRootSignature(rootSignature2, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));

}

void Engine::ParticleManager::CreateGraphicsPipeline()
{
	CreateRootSignature();

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = PSOFunction::CreateDepthStencilDesc();
	// 透明オブジェクトの場合はデプス書き込みを無効化
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	// インプットレイアウト
	psoManager_->AddInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	psoManager_->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	psoManager_->AddInputElementDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);


	psoManager_->SetShaderFileName(ShaderFileName::VS, L"resources/shaders/Particle/CPU/Particle.VS.hlsl");
	psoManager_->SetShaderFileName(ShaderFileName::PS, L"resources/shaders/Particle/CPU/Particle.PS.hlsl");



	BlendAdd();
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[0], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[1], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	BlendSubtract();
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[2], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[3], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	BlendMuliply();
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[4], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[5], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);


	BlendAdd();
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature2, graphicsPipelineState[6], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature2, graphicsPipelineState[7], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	BlendSubtract();
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature2, graphicsPipelineState[8], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature2, graphicsPipelineState[9], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	BlendMuliply();
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature2, graphicsPipelineState[10], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature2, graphicsPipelineState[11], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);




}

#pragma region Blend

void Engine::ParticleManager::BlendAdd()
{
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
}

void Engine::ParticleManager::BlendSubtract()
{
	// 減算ブレンドの設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// RGB の減算ブレンド
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;      // ソースの影響度
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE; // デスティネーションの影響度
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_SUBTRACT;     // 減算

	// アルファブレンドの設定（通常の設定）
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
}

void Engine::ParticleManager::BlendMuliply()
{

	// 加算ブレンドの設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	// 通常のカラー（RGB）加算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;      // ソースカラーをそのまま
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;     // デスティネーションカラーをそのまま
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;    // 加算（SrcColor + DestColor）
	// アルファブレンドの設定
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;


}

#pragma endregion // ブレンド

#pragma endregion // パイプライン関係
