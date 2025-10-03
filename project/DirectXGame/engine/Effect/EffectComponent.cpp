#include "EffectComponent.h"
#include <DirectXGame/engine/DirectX/Common/DirectXCommon.h>
#include <DirectXGame/engine/Manager/Entity3D/Entity3DManager.h>

void EffectComponent::Init(Entity3DManager* entity3dManager)
{
	entity3dManager_ = entity3dManager;

	particleManager_ = entity3dManager_->GetEffectManager()->GetParticleManager();

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
	emitter->Initialize(particleManager_, name, particleName);

	// 親子関係を設定
	if(parent) {
		emitter->transform_.parent_ = parent;
	}
	// エミットタイプを設定
	emitter->SetEmitType(emittype);


	// マップに追加
	emitters_[name] = std::move(emitter);

}

void EffectComponent::AddTrailEffect(const std::string name, const std::string tex, float maxTime, WorldTransform* parent, Color color, Vector3 offsetStr, Vector3 offsetEnd)
{
	// 既に同じ名前のTrailが存在する場合は追加しない
	if (trails_.find(name) != trails_.end()) {
		throw std::runtime_error("Trail with name '" + name + "' already exists.");
	}


	// トレイルエフェクトの生成
	std::unique_ptr<TrailEffect> trail = std::make_unique<TrailEffect>();
	trail->Initialize(entity3dManager_->GetEffectManager(), tex, maxTime, color);
	trail->SetCamera(camera_);
	trail->SetOffset(offsetStr, offsetEnd, *parent);

	// マップに追加
	trails_[name] = std::move(trail);

}

void EffectComponent::Update() {

	for(auto& [name, emitter] : emitters_) {
		emitter->Update();
	}

	for(auto& [name, trail] : trails_) {
		trail->Update();
	}

};

void EffectComponent::Draw() {


	// トレイルエフェクトの描画
	for(auto& [name, trail] : trails_) {
		trail->Draw();
	}

};