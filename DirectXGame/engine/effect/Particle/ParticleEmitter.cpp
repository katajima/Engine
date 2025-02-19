#include "ParticleEmitter.h"

#include "DirectXGame/engine/MyGame/MyGame.h"

// count: パーティクルの最大生成数を指定する値。frequency: パーティクルの発射間隔を秒単位で指定する値。frequencyTime: 現在の発射間隔の経過時間を追跡する値。
//ParticleEmitter::ParticleEmitter(std::string name, Transform transform, uint32_t count, float frequency, float frequencyTime)
//{
//	// name: パーティクルエミッターの名前を示す文字列。
//	// transform: パーティクルエミッターの位置、回転、スケールなどを表す構造体。	
//	// count: パーティクルの最大生成数を指定する値。
//	// frequency: パーティクルの発射間隔を秒単位で指定する値。
//	// frequencyTime: 現在の発射間隔の経過時間を追跡する値。
//
//
//
//	//name_ = name;
//	//count_ = count;
//	//frequency_ = frequency;
//	//frequencyTime_ = frequencyTime;
//	//transform_ = transform;
//}

void ParticleEmitter::Initialize(std::string emitName, std::string particleName, EmitSpawnShapeType spawnType)
{

	spawnShapeType_ = spawnType;
	emitType_ = ParticleManager::EmitType::kRandom;

	emitName_ = emitName;
	particleName_ = particleName;
	transform_.Initialize();
	frequency_ = 1.0f;
	frequencyTime_ = 0.0f;

	emitter_.color = { {1,1,1,1} ,{1,1,1,1} };
	emitter_.renge.max = Vector3{ 1.0f,1.0f,1.0f };
	emitter_.renge.min = Vector3{ -1.0f,-1.0f,-1.0f };
	emitter_.color.max = Vector4{ 1,1,1,1 };
	emitter_.color.min = Vector4{ 0,0,0,0 };
	emitter_.rotate.min = Vector3{ 0,0,0 };
	emitter_.rotate.max = Vector3{ 0,0,0 };
	emitter_.size.min = Vector3{ 1.0f,1.0f,1.0f };
	emitter_.size.max = Vector3{ 1.0f,1.0f,1.0f };
	emitter_.lifeTime.min = 1.0f;
	emitter_.lifeTime.max = 3.0f;
	emitter_.velocity.min = Vector3{ -1.0f,-1.0f,-1.0f };
	emitter_.velocity.max = Vector3{ 1.0f,1.0f,1.0f };
	emitter_.count = 10;

	isLifeTimeScale_ = false;	// スケール
	isLifeTimeAlpha_ = false;	// 透明度
	isLifeTimeVelocity = false;// 速度

	isGravity = false; // 重力
	isWind = false;  // 風

	usebillboard = true;// ビルボード
	isAlpha = false;    // 透明度
}

void ParticleEmitter::Update()
{
	ParticleManager::ParticleGroup& particleGroup = ParticleManager::GetInstance()->GetParticleGroups(particleName_);


#ifdef _DEBUG
	ImGui::Begin("engine");

		if (ImGui::TreeNode(emitName_.c_str())) {
			ImGui::SeparatorText("Emitter");
			ImGui::DragFloat3("translate", &transform_.translate_.x, 0.1f);
			ImGui::DragFloat3("rotate", &transform_.rotate_.x, 0.1f);
			ImGui::Separator(); 
			ImGui::Text("flag");
			ImGui::Separator(); 
			ImGui::Checkbox("Wind", &isWind);
			ImGui::Checkbox("Gravity", &isGravity);
			ImGui::Checkbox("RotateVelocity", &isRotateVelocity);
			ImGui::Checkbox("usebillboard", &usebillboard);
			ImGui::Checkbox("Alpha", &isAlpha);
			ImGui::Checkbox("Bounce", &isBounce);

			ImGui::Separator(); 
			ImGui::Checkbox("LifeTimeScale_", &isLifeTimeScale_);
			ImGui::Checkbox("LifeTimeAlpha", &isLifeTimeAlpha_);
			ImGui::Checkbox("LifeTimeVelocity", &isLifeTimeVelocity);
			ImGui::Separator(); // 水平線を引く
			ImGui::DragFloat3("renge.max", &emitter_.renge.max.x, 0.1f);
			ImGui::DragFloat3("renge.min", &emitter_.renge.min.x, 0.1f);
			ImGui::Separator(); // 水平線を引く
			ImGui::DragFloat3("rotate.max", &emitter_.rotate.max.x, 0.1f);
			ImGui::DragFloat3("rotate.min", &emitter_.rotate.min.x, 0.1f);
			ImGui::DragFloat3("size.max", &emitter_.size.max.x, 0.1f);
			ImGui::DragFloat3("size.min", &emitter_.size.min.x, 0.1f);
			ImGui::DragFloat3("velocity.max", &emitter_.velocity.max.x, 0.1f);
			ImGui::DragFloat3("velocity.min", &emitter_.velocity.min.x, 0.1f);
			ImGui::DragFloat("lifeTime.max", &emitter_.lifeTime.max, 0.1f);
			ImGui::DragFloat("lifeTime.min", &emitter_.lifeTime.min, 0.1f);

			ImGui::DragFloat("count", &emitter_.count, 1.0f);

			ImGui::Separator(); // 水平線を引く
			ImGui::ColorEdit4("colorMax", &emitter_.color.max.x);
			ImGui::ColorEdit4("colorMin", &emitter_.color.min.x);
			ImGui::TreePop();

			EmitMinMax();
		}


	ImGui::End();
#endif




	transform_.Update();
	emitter_.worldtransform = transform_;
	particleGroup.emiter.worldtransform = emitter_.worldtransform;
	particleGroup.emiter.worldtransform.Update();



	frequencyTime_ += MyGame::GameTime();
	if (frequency_ <= frequencyTime_) {
		Emit();
		frequencyTime_ -= frequency_;
	}

	DrawEmitterLine();

	// 寿命が尽きたパーティクルを削除する処理
	particleGroup.particle.remove_if([](const ParticleManager::Particle& p)
		{
			return p.currentTime >= p.lifeTime;
		});



}

void ParticleEmitter::Emit()
{
	ParticleManager::GetInstance()->GetParticleGroups(particleName_).usebillboard = usebillboard; // ビルボード
	ParticleManager::GetInstance()->GetParticleGroups(particleName_).isAlpha = isAlpha; // 透明度
	ParticleManager::GetInstance()->GetParticleGroups(particleName_).isGravity = isGravity; // 重力
	ParticleManager::GetInstance()->GetParticleGroups(particleName_).isLifeTimeScale_ = isLifeTimeScale_; // 重力
	ParticleManager::GetInstance()->GetParticleGroups(particleName_).isRotateVelocity = isRotateVelocity; // 回転速度
	ParticleManager::GetInstance()->GetParticleGroups(particleName_).isBounce = isBounce; // 回転速度

	if (emitType_ == ParticleManager::EmitType::kRandom) {
		ParticleManager::GetInstance()->GetParticleGroups(particleName_).emiter = emitter_;
		ParticleManager::GetInstance()->Emit(particleName_, emitType_);
	}
}

void ParticleEmitter::SetParent(WorldTransform& parent)
{
	transform_.parent_ = &parent;
}

void ParticleEmitter::SetIsAll(bool billboard, bool alpha, bool gravity, bool isLifeTimeScale, bool rotateVelocity)
{
	usebillboard = billboard;
	isAlpha = alpha;
	isGravity = gravity;
	isLifeTimeScale_ = isLifeTimeScale;
	isRotateVelocity = rotateVelocity;
}

void ParticleEmitter::DrawEmitterLine()
{
	switch (spawnShapeType_)
	{
	case ParticleEmitter::EmitSpawnShapeType::kAABB:
		LineCommon::GetInstance()->AddLineAABB({ emitter_.renge.min,emitter_.renge.max }, emitter_.worldtransform.translate_);
		break;
	case ParticleEmitter::EmitSpawnShapeType::kOBB:
		break;
	case ParticleEmitter::EmitSpawnShapeType::kSphere:
		break;
	case ParticleEmitter::EmitSpawnShapeType::kSegment:
		break;
	case ParticleEmitter::EmitSpawnShapeType::kLine:
		break;
	default:
		break;
	}
}

void ParticleEmitter::EmitMinMax()
{
	//	範囲 
	emitter_.renge.min.x = (std::min)(emitter_.renge.min.x, emitter_.renge.max.x);
	emitter_.renge.max.x = (std::max)(emitter_.renge.min.x, emitter_.renge.max.x);
	emitter_.renge.min.y = (std::min)(emitter_.renge.min.y, emitter_.renge.max.y);
	emitter_.renge.max.y = (std::max)(emitter_.renge.min.y, emitter_.renge.max.y);
	emitter_.renge.min.z = (std::min)(emitter_.renge.min.z, emitter_.renge.max.z);
	emitter_.renge.max.z = (std::max)(emitter_.renge.min.z, emitter_.renge.max.z);

	// 回転
	emitter_.rotate.min.x = (std::min)(emitter_.rotate.min.x, emitter_.rotate.max.x);
	emitter_.rotate.max.x = (std::max)(emitter_.rotate.min.x, emitter_.rotate.max.x);
	emitter_.rotate.min.y = (std::min)(emitter_.rotate.min.y, emitter_.rotate.max.y);
	emitter_.rotate.max.y = (std::max)(emitter_.rotate.min.y, emitter_.rotate.max.y);
	emitter_.rotate.min.z = (std::min)(emitter_.rotate.min.z, emitter_.rotate.max.z);
	emitter_.rotate.max.z = (std::max)(emitter_.rotate.min.z, emitter_.rotate.max.z);

	// 
	emitter_.velocity.min.x = (std::min)(emitter_.velocity.min.x, emitter_.velocity.max.x);
	emitter_.velocity.max.x = (std::max)(emitter_.velocity.min.x, emitter_.velocity.max.x);
	emitter_.velocity.min.y = (std::min)(emitter_.velocity.min.y, emitter_.velocity.max.y);
	emitter_.velocity.max.y = (std::max)(emitter_.velocity.min.y, emitter_.velocity.max.y);
	emitter_.velocity.min.z = (std::min)(emitter_.velocity.min.z, emitter_.velocity.max.z);
	emitter_.velocity.max.z = (std::max)(emitter_.velocity.min.z, emitter_.velocity.max.z);


	//	サイズ 
	emitter_.size.min.x = (std::min)(emitter_.size.min.x, emitter_.size.max.x);
	emitter_.size.max.x = (std::max)(emitter_.size.min.x, emitter_.size.max.x);
	emitter_.size.min.y = (std::min)(emitter_.size.min.y, emitter_.size.max.y);
	emitter_.size.max.y = (std::max)(emitter_.size.min.y, emitter_.size.max.y);
	emitter_.size.min.z = (std::min)(emitter_.size.min.z, emitter_.size.max.z);
	emitter_.size.max.z = (std::max)(emitter_.size.min.z, emitter_.size.max.z);

	if (emitter_.size.min.x < 0) {
		emitter_.size.min.x = 0;
		emitter_.size.max.x = 0;
	}
	if (emitter_.size.min.y < 0) {
		emitter_.size.min.y = 0;
		emitter_.size.max.y = 0;
	}
	if (emitter_.size.min.z < 0) {
		emitter_.size.min.z = 0;
		emitter_.size.max.z = 0;
	}


	//　色
	emitter_.color.min.x = (std::min)(emitter_.color.min.x, emitter_.color.max.x);
	emitter_.color.max.x = (std::max)(emitter_.color.min.x, emitter_.color.max.x);
	emitter_.color.min.y = (std::min)(emitter_.color.min.y, emitter_.color.max.y);
	emitter_.color.max.y = (std::max)(emitter_.color.min.y, emitter_.color.max.y);
	emitter_.color.min.z = (std::min)(emitter_.color.min.z, emitter_.color.max.z);
	emitter_.color.max.z = (std::max)(emitter_.color.min.z, emitter_.color.max.z);
	emitter_.color.min.w = (std::min)(emitter_.color.min.w, emitter_.color.max.w);
	emitter_.color.max.w = (std::max)(emitter_.color.min.w, emitter_.color.max.w);


	emitter_.lifeTime.min = (std::min)(emitter_.lifeTime.min, emitter_.lifeTime.max);
	emitter_.lifeTime.max = (std::max)(emitter_.lifeTime.min, emitter_.lifeTime.max);


	if (emitter_.lifeTime.min < 0) {
		emitter_.lifeTime.min = 0;
	}
	if (emitter_.lifeTime.max < 0) {
		emitter_.lifeTime.max = 0;
	}
}

