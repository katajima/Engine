#include "BaseParticleEmitter.h"
// engine
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/engine/Line/LineCommon.h"
#include "DirectXGame/engine/Effect/Particle/CPU/ParticleField.h"
#include "DirectXGame/engine/Utility/RangeUtility.h"
#include "DirectXGame/engine/Math/Random.h"


void BaseParticleEmitter::CommonParticleInit(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName)
{
	particleManager_ = particleManager;					// パーティクルマネージャー
	globalVariables_ = globalVariables;					// グローバル変数
	lineCommon_ = particleManager_->GetLineCommon();	// ライン共通
	emitName_ = emitName;								// エミッタ名
	particleName_ = particleName;						// パーティクル名

	transform_.Initialize();							// トランスフォーム初期化
	frequency_ = 0.1f;									// 発生頻度
	frequencyTime_ = 0.0f;								// 発生頻度タイマー

	emitData_.colorRange = { {0,0,0,0} ,{1,1,1,1} };	// 色範囲


	emitData_.size.median = { 1,1,1 };	// サイズ
	emitData_.lifeTime.median =  3.0f ;	// 生存時間
	emitData_.count.median = 5;			// 個数

	isFlag.isLifeTimeScale_ = false;	// スケール
	isFlag.isLifeTimeVelocity = false;	// 速度
	enableLighting_ = false;			// ライティング
	isFlag.isGravity = false;			// 重力
	isFlag.usebillboard = true;			// ビルボード
	isFlag.isAlpha = false;				// 透明度

	isEmit = false;						// 発生フラグ
	isLine = false;						// ライン
	// uv
	uvTransformVeloctiy_.rotate = { 0,0,0 };
	uvTransformVeloctiy_.scale = { 0,0,0 };
	uvTransformVeloctiy_.translate = { 0,0,0 };


	//globalVariables_->CreateGroup(emitName);
	//globalVariables_->AddItem(emitName, "transform.translate", transform_.translate_);	// トランスフォーム登録
	//globalVariables_->AddItem(emitName, "transform.rotate", transform_.rotate_);		// トランスフォーム登録		
	//globalVariables_->AddItem(emitName, "Gravity", isFlag.isGravity);					// 重力	
	//globalVariables_->AddItem(emitName, "RotateVelocity", isFlag.isRotateVelocity);		// 回転するか

	//globalVariables_->AddItem(emitName, "usebillboard", isFlag.usebillboard);			// ビルボード
	//globalVariables_->AddItem(emitName, "usebillboardY", isFlag.usebillboardY);			// Y軸ビルボード
	//globalVariables_->AddItem(emitName, "billboardRotZ", isFlag.billboardRotZ);			// Z回転ビルボード
	//globalVariables_->AddItem(emitName, "Alpha", isFlag.isAlpha);						// 透明度
	//globalVariables_->AddItem(emitName, "Bounce", isFlag.isBounce);						// バウンス
	//globalVariables_->AddItem(emitName, "Acceleration", isFlag.isAcceleration);			// 加速度
	//globalVariables_->AddItem(emitName, "LineInterpolation", isFlag.isLineInterpolation); // 線形補間
	//globalVariables_->AddItem(emitName, "LifeTimeScale_", isFlag.isLifeTimeScale_);		// スケール
	//globalVariables_->AddItem(emitName, "LifeTimeVelocity", isFlag.isLifeTimeVelocity); // 速度

	//globalVariables_->AddItem(emitName, "rotate.median", emitData_.rotate.median);		// 回転量
	//globalVariables_->AddItem(emitName, "rotate.range", emitData_.rotate.range);		// 回転量範囲
	//globalVariables_->AddItem(emitName, "size.median", emitData_.size.median);			// サイズ
	//globalVariables_->AddItem(emitName, "size.range", emitData_.size.range);			// サイズ範囲
	//globalVariables_->AddItem(emitName, "rotateVelocity.median", emitData_.rotateVelocity.median); // 回転速度
	//globalVariables_->AddItem(emitName, "rotateVelocity.range", emitData_.rotateVelocity.range);	// 回転速度範囲
	//globalVariables_->AddItem(emitName, "velocity.median", emitData_.velocity.median);			// 速度
	//globalVariables_->AddItem(emitName, "velocity.range", emitData_.velocity.range);				// 速度範囲
	//globalVariables_->AddItem(emitName, "acceleration.median", emitData_.acceleration.median);	// 加速度
	//globalVariables_->AddItem(emitName, "acceleration.range", emitData_.acceleration.range);		// 加速度範囲
	//globalVariables_->AddItem(emitName, "lifeTime.median", emitData_.lifeTime.median);			// 生存時間
	//globalVariables_->AddItem(emitName, "lifeTime.range", emitData_.lifeTime.range);				// 生存時間範囲
	//globalVariables_->AddItem(emitName, "count.median", emitData_.count.median);					// 個数
	//globalVariables_->AddItem(emitName, "count.range", emitData_.count.range);						// 個数範囲
	//globalVariables_->AddItem(emitName, "colorRange.min", emitData_.colorRange.min);				// 色範囲最小
	//globalVariables_->AddItem(emitName, "colorRange.max", emitData_.colorRange.max);				// 色範囲最大
	//globalVariables_->AddItem(emitName, "frequency", frequency_);									// 発生頻度

	//
	//ApplyGlobalVariables(); // グローバル変数適用
}

void BaseParticleEmitter::ApplyGlobalVariables() {
	/*transform_.translate_ = globalVariables_->GetValue<Vector3>(emitName_, "transform.translate");
	transform_.rotate_ = globalVariables_->GetValue<Vector3>(emitName_, "transform.rotate");
	isFlag.isGravity = globalVariables_->GetValue<bool>(emitName_, "Gravity");
	isFlag.isRotateVelocity = globalVariables_->GetValue<bool>(emitName_, "RotateVelocity");
	isFlag.usebillboard = globalVariables_->GetValue<bool>(emitName_, "usebillboard");
	isFlag.usebillboardY = globalVariables_->GetValue<bool>(emitName_, "usebillboardY");
	isFlag.billboardRotZ = globalVariables_->GetValue<bool>(emitName_, "billboardRotZ");
	isFlag.isAlpha = globalVariables_->GetValue<bool>(emitName_, "Alpha");
	isFlag.isBounce = globalVariables_->GetValue<bool>(emitName_, "Bounce");
	isFlag.isAcceleration = globalVariables_->GetValue<bool>(emitName_, "Acceleration");
	isFlag.isLineInterpolation = globalVariables_->GetValue<bool>(emitName_, "LineInterpolation");
	isFlag.isLifeTimeScale_ = globalVariables_->GetValue<bool>(emitName_, "LifeTimeScale_");
	isFlag.isLifeTimeVelocity = globalVariables_->GetValue<bool>(emitName_, "LifeTimeVelocity");
	emitData_.rotate.median = globalVariables_->GetValue<Vector3>(emitName_, "rotate.median");
	emitData_.rotate.range = globalVariables_->GetValue<Vector3>(emitName_, "rotate.range");
	emitData_.size.median = globalVariables_->GetValue<Vector3>(emitName_, "size.median");
	emitData_.size.range = globalVariables_->GetValue<Vector3>(emitName_, "size.range");
	emitData_.rotateVelocity.median = globalVariables_->GetValue<Vector3>(emitName_, "rotateVelocity.median");
	emitData_.rotateVelocity.range = globalVariables_->GetValue<Vector3>(emitName_, "rotateVelocity.range");
	emitData_.velocity.median = globalVariables_->GetValue<Vector3>(emitName_, "velocity.median");
	emitData_.velocity.range = globalVariables_->GetValue<Vector3>(emitName_, "velocity.range");
	emitData_.acceleration.median = globalVariables_->GetValue<Vector3>(emitName_, "acceleration.median");
	emitData_.acceleration.range = globalVariables_->GetValue<Vector3>(emitName_, "acceleration.range");
	emitData_.lifeTime.median = globalVariables_->GetValue<float>(emitName_, "lifeTime.median");
	emitData_.lifeTime.range = globalVariables_->GetValue<float>(emitName_, "lifeTime.range");
	emitData_.count.median = globalVariables_->GetValue<int>(emitName_, "count.median");
	emitData_.count.range = globalVariables_->GetValue<int>(emitName_, "count.range");
	emitData_.colorRange.min = globalVariables_->GetValue<Vector4>(emitName_, "colorRange.min");
	emitData_.colorRange.max = globalVariables_->GetValue<Vector4>(emitName_, "colorRange.max");
	frequency_ = globalVariables_->GetValue<float>(emitName_, "frequency");
*/


}


// 共通発生処理
void BaseParticleEmitter::Emit()
{
	if (isEmit) {
		particleManager_->GetParticleGroups(particleName_).isFlag = isFlag;
		particleManager_->GetParticleGroups(particleName_).material->alphaClipping_ = alphaClipping_; // αクリッピング
		particleManager_->GetParticleGroups(particleName_).material->enableLighting_ = enableLighting_; // aライト
		particleManager_->GetParticleGroups(particleName_).topBottom = topBottom_; // 拡縮方向
		particleManager_->GetParticleGroups(particleName_).uvTransformVeloctiy_.scale = uvTransformVeloctiy_.scale; // UV
		particleManager_->GetParticleGroups(particleName_).uvTransformVeloctiy_.rotate = uvTransformVeloctiy_.rotate; // UV
		particleManager_->GetParticleGroups(particleName_).uvTransformVeloctiy_.translate = uvTransformVeloctiy_.translate; // UV

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


void BaseParticleEmitter::Update() {
	ParticleGroup& particleGroup = particleManager_->GetParticleGroups(particleName_);

	ApplyGlobalVariables(); // グローバル変数適用

	// トランスフォーム更新
	transform_.Update();
#ifdef _DEBUG
	ImGui::Begin("EmitParticle");
	if (ImGui::TreeNode(emitName_.c_str())) {
		ImGui::SeparatorText("Particle");
		int instanceCount = static_cast<int>(particleGroup.instanceCount);
		ImGui::InputInt("instanceCount", &instanceCount);

		ImGui::SeparatorText("Emitter");
		ImGui::DragFloat3("transform.translate", &transform_.translate_.x, 0.1f);
		ImGui::DragFloat3("transform.rotate", &transform_.rotate_.x, 0.1f);
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
		ImGui::Separator();
		ImGui::Text("emitPos");
		if (ImGui::Button("Random")) {
			emitType_ = EmitData::EmitType::kRandom;
		}
		if (ImGui::Button("Surface")) {
			emitType_ = EmitData::EmitType::kSurface;
		}
		if (ImGui::Button("Edge")) {
			emitType_ = EmitData::EmitType::kEdge;
		}
		ImGui::Text("flag");
		ImGui::Checkbox("Emit", &isEmit);
		ImGui::Checkbox("UseLine", &isLine);
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
		ImGui::DragFloat3("rotate", &emitData_.rotate.median.x, 0.01f);
		ImGui::DragFloat3("rotate.range", &emitData_.rotate.range.x, 0.01f);

		ImGui::DragFloat3("size", &emitData_.size.median.x, 0.1f);
		ImGui::DragFloat3("size.range", &emitData_.size.range.x, 0.1f);

		ImGui::DragFloat3("rotateVelocity", &emitData_.rotateVelocity.median.x, 0.01f);
		ImGui::DragFloat3("rotateVelocity.range", &emitData_.rotateVelocity.range.x, 0.01f);
		
		ImGui::DragFloat3("velocity", &emitData_.velocity.median.x, 0.1f);
		ImGui::DragFloat3("velocity.range", &emitData_.velocity.range.x, 0.1f);
		
		ImGui::DragFloat3("acceleration", &emitData_.acceleration.median.x, 0.1f);
		ImGui::DragFloat3("acceleration.range", &emitData_.acceleration.range.x, 0.1f);

		ImGui::DragFloat("lifeTime", &emitData_.lifeTime.median, 0.1f);
		ImGui::DragFloat("lifeTime.range", &emitData_.lifeTime.range, 0.1f);

		ImGui::DragInt("count", &emitData_.count.median, 1.0f);
		ImGui::DragInt("count.range", &emitData_.count.range, 1.0f);
		
		ImGui::DragFloat("frequency", &frequency_, 0.01f);

		ImGui::Separator(); // 水平線を引く
		ImGui::ColorEdit4("colorMax", &emitData_.colorRange.max.x);
		ImGui::ColorEdit4("colorMin", &emitData_.colorRange.min.x);

		ImGui::Separator(); // 水平線を引く


		DebugImGui();

		ImGui::TreePop();
	}
	ImGui::End();
#endif // _DEBUG


	EmitMinMax();

	UpdateUniqe();

	// 出現
	frequencyTime_ += MyGame::GameTime();
	if (frequency_ <= frequencyTime_) {
		Emit();
		frequencyTime_ -= frequency_;
	}
	if (isLine) {
		DrawEmitterLine();
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

// 最小最大値算出
void BaseParticleEmitter::EmitMinMax() {
	//	範囲 
	ConversionRange(emitData_.colorRange);
};


#pragma region 設定

// 発生頻度設定
void BaseParticleEmitter::SetCount(int count, int range)
{
	emitData_.count.median = count;
	emitData_.count.range = range;
}
// サイズ設定
void BaseParticleEmitter::SetSize(Vector3 size, Vector3 range)
{
	emitData_.size.median = size;
	emitData_.size.range = range;
}
// 回転設定
void BaseParticleEmitter::SetRotate(Vector3 rotate, Vector3 range)
{
	emitData_.rotate.median = rotate;
	emitData_.rotate.range = range;
}
// 生存時間設定
void BaseParticleEmitter::SetLifeTime(float life, float range)
{
	emitData_.lifeTime.median = life;
	emitData_.lifeTime.range = range;
}

// 速度設定
void BaseParticleEmitter::SetVelocity(Vector3 velocity, Vector3 range)
{
	emitData_.velocity.median = velocity;
	emitData_.velocity.range = range;
}
// 回転速度設定
void BaseParticleEmitter::SetRotateVelocity(Vector3 rotateVelocity, Vector3 range)
{
	emitData_.rotateVelocity.median = rotateVelocity;
	emitData_.rotateVelocity.range = range;
}
// 加速度設定
void BaseParticleEmitter::SetAcceleration(Vector3 acceleration, Vector3 range)
{
	emitData_.acceleration.median = acceleration;
	emitData_.acceleration.range = range;
}


#pragma endregion // 設定


