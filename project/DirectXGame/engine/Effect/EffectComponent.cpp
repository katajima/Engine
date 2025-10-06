#include "EffectComponent.h"
#include <DirectXGame/engine/DirectX/Common/DirectXCommon.h>
#include <DirectXGame/engine/Manager/Entity3D/Entity3DManager.h>
#include <DirectXGame/engine/MyGame/MyGame.h>
void EffectComponent::Init(Entity3DManager* entity3dManager, GlobalVariables* globalVariables)
{
	assert(entity3dManager && globalVariables);
	entity3dManager_ = entity3dManager;
	globalVariables_ = globalVariables;
	particleManager_ = entity3dManager_->GetEffectManager()->GetParticleManager();
	gpuParticleManager_ = entity3dManager_->GetEffectManager()->GetGpuParticleManager();
	primitiveCommon_ = entity3dManager_->GetPrimitiveCommon();
}

void EffectComponent::AddEmitter(const std::string& name, const std::string& particleName, EmitterShapeType type, EmitData::EmitType emittype, WorldTransform* parent)
{

	// 既に同じ名前のEmitterが存在する場合は追加しない
	if (emitters_.find(name) != emitters_.end()) {
		throw std::runtime_error("Emitter with name '" + name + "' already exists.");
	}


	std::unique_ptr<BaseParticleEmitter> emitter;

	// 種類によって生成するEmitterを変える
	switch (type)
	{
	case EmitterShapeType::AABB:
		emitter = std::make_unique<AABBParticleEmitter>();
		break;
	case EmitterShapeType::SPHERE:
		emitter = std::make_unique<SphereParticleEmitter>();
		break;
	case EmitterShapeType::CIRCLE:
		emitter = std::make_unique<CornerParticleEmitter>();
		break;
	case EmitterShapeType::POINT:
		emitter = std::make_unique<PointParticleEmitter>();
		break;
	case EmitterShapeType::LINE:
		emitter = std::make_unique<LineParticleEmitter>();
		break;
	case EmitterShapeType::SPLINE:
		emitter = std::make_unique<SplineParticleEmitter>();
		break;
	case EmitterShapeType::TRIANGLE:
		emitter = std::make_unique<TriangleParticleEmitter>();
		break;
	case EmitterShapeType::MESH:
		emitter = std::make_unique<MeshParticleEmitter>();
		break;
	default:
		throw std::runtime_error("Unsupported EmitterShapeType.");
	}

	// なかったら
	if (!emitter) {
		return; 
	}

	// 初期化
	emitter->Initialize(particleManager_, globalVariables_, name, particleName);

	// 親子関係を設定
	if(parent) {
		emitter->transform_.parent_ = parent;
	}
	// エミットタイプを設定
	emitter->SetEmitType(emittype);


	// マップに追加
	emitters_[name] = std::move(emitter);

}

void EffectComponent::AddGPUParticleEmitter(const std::string& name, const std::string& particleName, EmitterType type, WorldTransform* parent)
{
	switch (type)
	{
	case EmitterType::Sphere:
		gpuParticleManager_->CreateEmitter<GpuParticleEmitterSphere>(name);
		gpuParticleManager_->SetEmitteToGroup(name, particleName);

		gpuParticleManager_->GetGpuParticleEmitter<GpuParticleEmitterSphere>(name)->GetWorldTransform().parent_ = parent;
		break;
	case EmitterType::AABB:
		gpuParticleManager_->CreateEmitter<GpuParticleEmitterAABB>(name);
		gpuParticleManager_->SetEmitteToGroup(name, particleName);
		gpuParticleManager_->GetGpuParticleEmitter<GpuParticleEmitterAABB>(name)->GetWorldTransform().parent_ = parent;
		break;
	case EmitterType::Point:
		gpuParticleManager_->CreateEmitter<GpuParticleEmitterPoint>(name);
		gpuParticleManager_->SetEmitteToGroup(name, particleName);
		gpuParticleManager_->GetGpuParticleEmitter<GpuParticleEmitterPoint>(name)->GetWorldTransform().parent_ = parent;
		break;
	default:
		break;
	}
	


}

void EffectComponent::AddTrailEffect(const std::string name, const std::string tex, float maxTime, WorldTransform& parent, Color color, Vector3 offsetStr, Vector3 offsetEnd)
{
	// 既に同じ名前のTrailが存在する場合は追加しない
	if (trails_.find(name) != trails_.end()) {
		throw std::runtime_error("Trail with name '" + name + "' already exists.");
	}


	// トレイルエフェクトの生成
	std::unique_ptr<TrailEffect> trail = std::make_unique<TrailEffect>();
	trail->Initialize(entity3dManager_->GetEffectManager(), tex, maxTime, color);
	trail->SetCamera(camera_);
	trail->SetOffset(offsetStr, offsetEnd, parent);

	// マップに追加
	trails_[name] = std::move(trail);

}

void EffectComponent::AddPrimitive(const std::string& name, const std::string tex, ShapeParameter::ShapeType type)
{
	// 既に同じtypeとnameのプリミティブが存在する場合はreturn
	auto typeIt = primitives3D_.find(type);
	if (typeIt != primitives3D_.end()) {
		auto& nameMap = typeIt->second;
		if (nameMap.find(name) != nameMap.end()) {
			// 同じ名前のプリミティブが存在
			return;
		}
	}

	// 
	std::unique_ptr<BasePrimitive> primitive;

	switch (type)
	{
	case ShapeParameter::ShapeType::Plane:
		primitive = std::make_unique<PlanePrimitive>();
		break;
	case ShapeParameter::ShapeType::Triangle:
		primitive = std::make_unique<TrianglePrimitive>();
		break;
	case ShapeParameter::ShapeType::Cross:
		primitive = std::make_unique<CrossPrimitive>();
		break;
	case ShapeParameter::ShapeType::Cube:
		primitive = std::make_unique<CubePrimitive>();
		break;
	case ShapeParameter::ShapeType::Circle:
		primitive = std::make_unique<CirclePrimitive>();
		break;
	case ShapeParameter::ShapeType::Star:
		primitive = std::make_unique<StarPrimitive>();
		break;
	case ShapeParameter::ShapeType::Crescent:
		primitive = std::make_unique<CrescentPrimitive>();
		break;
	case ShapeParameter::ShapeType::Ring:
		primitive = std::make_unique<RingPrimitive>();
		break;
	case ShapeParameter::ShapeType::Sphere:
		primitive = std::make_unique<SpherePrimitive>();
		break;
	case ShapeParameter::ShapeType::Arrow:
		primitive = std::make_unique<ArrowPrimitive>();
		break;
	case ShapeParameter::ShapeType::Cylinder:
		primitive = std::make_unique<CylinderPrimitive>();
		break;
	case ShapeParameter::ShapeType::Tube:
		primitive = std::make_unique<TubePrimitive>();
		break;
	case ShapeParameter::ShapeType::Pyramid:
		primitive = std::make_unique<PyramidPrimitive>();
		break;
	case ShapeParameter::ShapeType::Torus:
		primitive = std::make_unique<TorusPrimitive>();
		break;
	default:
		return; // 未対応の型なら何もしない
		break;
	}

	// 初期化
	primitive->Initialize(primitiveCommon_, tex, Color(1, 1, 1, 1));
	
	// マップに格納
	primitives3D_[type][name] = std::move(primitive);
}

void EffectComponent::RemovePrimitive(const std::string& name, ShapeParameter::ShapeType type)
{
	// 指定typeが存在するか確認
	auto typeIt = primitives3D_.find(type);
	if (typeIt == primitives3D_.end()) {
		// 指定されたtypeのプリミティブ群が存在しない
		return;
	}

	// 指定nameが存在するか確認
	auto& nameMap = typeIt->second;
	auto nameIt = nameMap.find(name);
	if (nameIt == nameMap.end()) {
		// 該当するプリミティブが存在しない
		return;
	}

	// プリミティブ削除
	nameMap.erase(nameIt);

	// もしtypeに属するプリミティブが全てなくなったらtype自体も削除
	if (nameMap.empty()) {
		primitives3D_.erase(typeIt);
	}
}





void EffectComponent::Update() {

	for(auto& [name, emitter] : emitters_) {
		emitter->Update();
	}

	for(auto& [name, trail] : trails_) {
		trail->Update();
	}
	for (auto& [type, primitiveMap] : primitives3D_) {
		for (auto& [name, primitive] : primitiveMap) {
			primitive->Update(MyGame::GameTime());
		}
	}

};

void EffectComponent::Draw() {


	// トレイルエフェクトの描画
	for(auto& [name, trail] : trails_) {
		trail->Draw();
	}
	
};

void EffectComponent::DrawEffect() {
	for (auto& [type, primitiveMap] : primitives3D_) {
		for (auto& [name, primitive] : primitiveMap) {
			primitive->Draw();
		}
	}
}