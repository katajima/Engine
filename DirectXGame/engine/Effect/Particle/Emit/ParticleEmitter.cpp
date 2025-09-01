
// engine
#include "ParticleEmitter.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/engine/Line/LineCommon.h"
#include "DirectXGame/engine/Effect/Particle/ParticleField.h"
#include "DirectXGame/engine/Utility/RangeUtility.h"

void ParticleEmitter::Initialize(ParticleManager* particleManager, std::string emitName, std::string particleName, EmitData::SpawnType spawnType)
{
	particleManager_ = particleManager;

	lineCommon_ = particleManager_->GetLineCommon();

	emitter_.controlPoints.clear(); // 初期化

	spawnShapeType_ = spawnType;
	emitType_ = EmitData::EmitType::kRandom;

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

	emitter_.velocity.min = Vector3{ 0,0,0 };
	emitter_.velocity.max = Vector3{ 0,0,0 };


	emitter_.count = 10;

	emitter_.corner.radius = 5;
	emitter_.corner.center = 0;
	emitter_.corner.segment = 3;



	isFlag.isLifeTimeScale_ = false;	// スケール
	isFlag.isLifeTimeVelocity = false;// 速度
	enableLighting_ = false;
	isFlag.isGravity = false; // 重力
	isFlag.usebillboard = true;// ビルボード
	isFlag.isAlpha = false;    // 透明度

	isEmit = true;


	uvTransformVeloctiy_.rotate = { 0,0,0 };
	uvTransformVeloctiy_.scale = { 0,0,0 };
	uvTransformVeloctiy_.translate = { 0,0,0 };
}

void ParticleEmitter::Update()
{
	ParticleGroup& particleGroup = particleManager_->GetParticleGroups(particleName_);
#ifdef _DEBUG
	ImGui::Begin("EmitParticle");
	if (ImGui::TreeNode(emitName_.c_str())) {
		ImGui::SeparatorText("Emitter");
		ImGui::DragFloat3("translate", &transform_.translate_.x, 0.1f);
		ImGui::DragFloat3("rotate", &transform_.rotate_.x, 0.1f);
		ImGui::Separator();
		if (ImGui::Button("MODE_ADD")) {
			particleGroup.blendType = EmitData::BlendType::MODE_ADD;
		}
		if (ImGui::Button("MODE_MUlLIPLY")) {
			particleGroup.blendType = EmitData::BlendType::MODE_MUlLIPLY;
		}
		if (ImGui::Button("MODE_SUBTRACT")) {
			particleGroup.blendType = EmitData::BlendType::MODE_SUBTRACT;
		}
		ImGui::Text("flag");
		ImGui::Checkbox("Emit", &isEmit);
		ImGui::Separator();
		ImGui::Checkbox("Gravity", &isFlag.isGravity);
		ImGui::Checkbox("RotateVelocity", &isFlag.isRotateVelocity);
		ImGui::Checkbox("usebillboard", &isFlag.usebillboard);
		ImGui::Checkbox("usebillboardY", &isFlag.usebillboardY);
		ImGui::Checkbox("billboardRotZ", &isFlag.billboardRotZ);
		ImGui::Checkbox("Alpha", &isFlag.isAlpha);
		ImGui::Checkbox("Bounce", &isFlag.isBounce);
		ImGui::Checkbox("Acceleration", &isFlag.isAcceleration);
		ImGui::Checkbox("LineInterpolation", &isFlag.isLineInterpolation);

		ImGui::Separator();
		ImGui::Checkbox("LifeTimeScale_", &isFlag.isLifeTimeScale_);
		ImGui::Checkbox("LifeTimeVelocity", &isFlag.isLifeTimeVelocity);
		ImGui::Separator(); // 水平線を引く
		ImGui::DragFloat3("renge.max", &emitter_.renge.max.x, 0.1f);
		ImGui::DragFloat3("renge.min", &emitter_.renge.min.x, 0.1f);
		ImGui::Separator(); // 水平線を引く
		ImGui::DragFloat3("rotate.max", &emitter_.rotate.max.x, 0.1f);
		ImGui::DragFloat3("rotate.min", &emitter_.rotate.min.x, 0.1f);
		ImGui::DragFloat3("size.max", &emitter_.size.max.x, 0.1f);
		ImGui::DragFloat3("size.min", &emitter_.size.min.x, 0.1f);
		ImGui::DragFloat3("rotateVelocity.max", &emitter_.rotateVelocity.max.x, 0.1f);
		ImGui::DragFloat3("rotateVelocity.min", &emitter_.rotateVelocity.min.x, 0.1f);
		ImGui::DragFloat3("velocity.max", &emitter_.velocity.max.x, 0.1f);
		ImGui::DragFloat3("velocity.min", &emitter_.velocity.min.x, 0.1f);
		ImGui::DragFloat3("acceleration.max", &emitter_.acceleration.max.x, 0.1f);
		ImGui::DragFloat3("acceleration.min", &emitter_.acceleration.min.x, 0.1f);


		ImGui::DragFloat("lifeTime.max", &emitter_.lifeTime.max, 0.1f);
		ImGui::DragFloat("lifeTime.min", &emitter_.lifeTime.min, 0.1f);

		ImGui::DragInt("count", &emitter_.count, 1.0f);

		if (spawnShapeType_ == EmitData::SpawnType::kCornerLine) {
			ImGui::Separator();
			ImGui::Text("CornerLine");
			ImGui::Separator();

			ImGui::DragFloat("corner.radius", &emitter_.corner.radius, 0.1f);
			ImGui::SliderInt("corner.segment", &emitter_.corner.segment, 3, 36);
			if (emitter_.corner.segment < 3) {
				emitter_.corner.segment = 3;
			}
			if (ImGui::Button("segment_3")) {
				emitter_.corner.segment = 3;
			}
			if (ImGui::Button("segment_4")) {
				emitter_.corner.segment = 4;
			}
			if (ImGui::Button("segment_5")) {
				emitter_.corner.segment = 5;
			}
			if (ImGui::Button("segment_16")) {
				emitter_.corner.segment = 16;
			}

		}
		if (spawnShapeType_ == EmitData::SpawnType::kSpline) {
			ImGui::Separator();
			ImGui::Text("spline");
			ImGui::Separator();
			for (int i = 0; i < emitter_.controlPoints.size(); i++) {
				std::string index = std::to_string(i);
				ImGui::DragFloat3(index.c_str(), &emitter_.controlPoints[i].x, 0.1f);
			}

		}
		ImGui::Separator(); // 水平線を引く
		ImGui::Separator(); // 水平線を引く
		ImGui::ColorEdit4("colorMax", &emitter_.color.max.x);
		ImGui::ColorEdit4("colorMin", &emitter_.color.min.x);

		ImGui::Separator(); // 水平線を引く


		ImGui::TreePop();
		EmitMinMax();
	}
	ImGui::End();
#endif

	transform_.Update();



	frequencyTime_ += MyGame::GameTime();
	if (frequency_ <= frequencyTime_) {
		Emit();
		frequencyTime_ -= frequency_;
	}

	DrawEmitterLine();

	// 寿命が尽きたパーティクルを削除する処理
	particleGroup.particle.remove_if([](const Particle& p)
		{
			return p.currentTime >= p.lifeTime;
		});

	particleGroup.particle.remove_if([](const Particle& p)
		{
			return p.isDestroy;
		});

}

void ParticleEmitter::Emit()
{
	if (isEmit) {
		particleManager_->GetParticleGroups(particleName_).isFlag = isFlag;

		particleManager_->GetParticleGroups(particleName_).material->alphaClipping_ = alphaClipping_; // αクリッピング
		particleManager_->GetParticleGroups(particleName_).material->enableLighting_ = enableLighting_; // aライト
		particleManager_->GetParticleGroups(particleName_).topBottom = topBottom_; // 拡縮方向
		particleManager_->GetParticleGroups(particleName_).uvTransformVeloctiy_.scale = uvTransformVeloctiy_.scale; // UV
		particleManager_->GetParticleGroups(particleName_).uvTransformVeloctiy_.rotate = uvTransformVeloctiy_.rotate; // UV
		particleManager_->GetParticleGroups(particleName_).uvTransformVeloctiy_.translate = uvTransformVeloctiy_.translate; // UV
		particleManager_->GetParticleGroups(particleName_).emiter = emitter_;



		particleManager_->Emit(particleName_, transform_, emitType_, spawnShapeType_);

	}
}

void ParticleEmitter::SetParent(WorldTransform& parent)
{
	transform_.parent_ = &parent;
}

void ParticleEmitter::SetIsAll(bool billboard, bool alpha, bool gravity, bool isLifeTimeScale, bool rotateVelocity)
{
	isFlag.usebillboard = billboard;
	isFlag.isAlpha = alpha;
	isFlag.isGravity = gravity;
	isFlag.isLifeTimeScale_ = isLifeTimeScale;
	isFlag.isRotateVelocity = rotateVelocity;
}

void ParticleEmitter::DrawEmitterLine()
{
	switch (spawnShapeType_)
	{
	case EmitData::SpawnType::kAABB:
		lineCommon_->AddLineAABB({ emitter_.renge.min,emitter_.renge.max }, transform_.translate_);
		break;
	case EmitData::SpawnType::kSphere:
		break;
	case EmitData::SpawnType::kSegmentLine:
		lineCommon_->AddLine(emitter_.renge.min + transform_.translate_, emitter_.renge.max + transform_.translate_, { 1,1,1,1 });
		break;
	case EmitData::SpawnType::kCornerLine:
		lineCommon_->AddLineCorner(emitter_.corner, transform_);
		break;
	case EmitData::SpawnType::kSpline:
		lineCommon_->AddSpline(emitter_.controlPoints, transform_);
		break;
	default:
		break;
	}
}

void ParticleEmitter::EmitMinMax()
{
	//	範囲 
	ConversionRange(emitter_.renge);

	// 回転
	ConversionRange(emitter_.rotate);

	// 速度 
	ConversionRange(emitter_.velocity);

	// 回転速度 
	ConversionRange(emitter_.rotateVelocity);

	// 加速度 
	ConversionRange(emitter_.acceleration);

	//　色
	ConversionRange(emitter_.color);


	//	サイズ 
	ConversionRange(emitter_.size);

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

	// 生存時間
	ConversionRangeFloat(emitter_.lifeTime);
	if (emitter_.lifeTime.min < 0) {
		emitter_.lifeTime.min = 0;
	}
	if (emitter_.lifeTime.max < 0) {
		emitter_.lifeTime.max = 0;
	}
}