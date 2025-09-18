#include "ParticleEmitter2d.h"
// engine
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/engine/Line/LineCommon.h"
#include "DirectXGame/engine/Effect/Particle/ParticleField.h"
#include "DirectXGame/engine/Utility/RangeUtility.h"
#include "DirectXGame/engine/Math/Random.h"


void ParticleEmitter2d::Initialize(ParticleManager2d* particleManager, std::string emitName, std::string particleName)
{
	particleManager_ = particleManager;					// パーティクルマネージャー
	emitName_ = emitName;								// エミッタ名
	particleName_ = particleName;						// パーティクル名

	transform_.Initialize();							// トランスフォーム初期化
	frequency_ = 0.1f;									// 発生頻度
	frequencyTime_ = 0.0f;								// 発生頻度タイマー

	emitData_.colorRange = { {0,0,0,0} ,{1,1,1,1} };	// 色範囲


	emitData_.size.median = { 1,1 };	// サイズ
	emitData_.lifeTime.median = 3.0f;	// 生存時間
	emitData_.count.median = 5;			// 個数


	//isFlag.isAlpha = false;				// 透明度

	isEmit = true;						// 発生フラグ
}


void ParticleEmitter2d::Update() {
	ParticleGroup2d& particleGroup = particleManager_->GetParticleGroups(particleName_);

	// トランスフォーム更新
	transform_.Update();
#ifdef _DEBUG
	ImGui::Begin("EmitParticle");
	if (ImGui::TreeNode(emitName_.c_str())) {
		ImGui::TreePop();
	}
	ImGui::End();
#endif // _DEBUG

	// 出現
	frequencyTime_ += MyGame::GameTime();
	if (frequency_ <= frequencyTime_) {
		Emit();
		frequencyTime_ -= frequency_;
	}
	// 寿命が尽きたパーティクルを削除する処理
	particleGroup.particle.remove_if([](const Particle& p)
		{
			return p.currentTime >= p.lifeTime;
		});

	particleGroup.particle.remove_if([](const Particle& p)
		{
			return p.isDestroy;
		});
};

void ParticleEmitter2d::Emit() {
	if (isEmit) {
		particleManager_->GetParticleGroups(particleName_).material->alphaClipping_ = alphaClipping_; // αクリッピング
		particleManager_->GetParticleGroups(particleName_).material->enableLighting_ = enableLighting_; // aライト

		auto& rng = particleManager_->GetRandomEngine();

		int index = static_cast<int>(Random::RandomInt32_t(static_cast<int32_t>(emitData_.count.Min()), static_cast<int32_t>(emitData_.count.Max())));
		if (index <= 0) {
			return;
		}
		for (int i = 0; i < index; i++) {
			EmitUniqe();
		}
	}
}

void ParticleEmitter2d::SetCount(int count, int range) {
	emitData_.count.median = count;
	emitData_.count.range = range;
}

void ParticleEmitter2d::SetRange(Vector2 min, Vector2 max)
{
	range_.min = min;
	range_.max = max;
}

void ParticleEmitter2d::SetSize(Vector2 size, Vector2 range)
{
	emitData_.size.median = size;
	emitData_.size.range = range;
}
void ParticleEmitter2d::SetRotate(float rotate, float range)
{
	emitData_.rotate.median = rotate;
	emitData_.rotate.range = range;
}
void ParticleEmitter2d::SetLifeTime(float life, float range)
{
	emitData_.lifeTime.median = life;
	emitData_.lifeTime.range = range;
}

void ParticleEmitter2d::SetVelocity(Vector2 velocity, Vector2 range)
{
	emitData_.velocity.median = velocity;
	emitData_.velocity.range = range;
}
void ParticleEmitter2d::SetRotateVelocity(float rotateVelocity, float range)
{
	emitData_.rotateVelocity.median = rotateVelocity;
	emitData_.rotateVelocity.range = range;
}
void ParticleEmitter2d::SetAcceleration(Vector2 acceleration, Vector2 range)
{
	emitData_.acceleration.median = acceleration;
	emitData_.acceleration.range = range;
}

void ParticleEmitter2d::EmitUniqe() {
	ParticleGroup2d& particleGroup = particleManager_->GetParticleGroups(particleName_);
	auto& rnd = particleManager_->GetRandomEngine();


	Vector3 pos{};

	Vector2 pa = transform_.worldMat_.GetWorldPosition();
	Vector2 ran = Random::RandVector2(range_, particleManager_->GetRandomEngine());
	pos.x = pa.x + ran.x;
	pos.y = pa.y + ran.y;
	pos.z = 0.0f;

	EmitFanction::CreateParticle(particleGroup, emitData_, particleManager_->GetRandomEngine(), pos);
};