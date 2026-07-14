#include "EffectScene.h"

#include "DirectXGame/engine/Effect/Primitive/Primitive.h"
#include "DirectXGame/engine/input/Input.h"
#include "DirectXGame/engine/math/MathFunctions.h"

#include <cmath>

void EffectScene::Initialize() {
	// デバッグシーンで使用する入力管理を一括初期化する
	inputCoordinator_ = std::make_unique<InputCoordinator>();
	inputCoordinator_->Initialize(GetInput());

	// 確認用シーンの土台を順番に生成する
	InitializeCamera();
	InitializeEnvironment();

	// 保存済みエフェクトとエディタを持つ実行システムを初期化する
	effect_ = std::make_unique<EffectSystem>();
	effect_->Initialize(GetEntityManager(), GetGlobalVariables());

	// エフェクト試射用の3D/2D確認物を生成する
	InitializePreviewObjects();
	InitializePreviewEffects();
	InitializeParticle2D();

	// カメラ設定
	SetCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetEffectManager()->GetParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetEffectManager()->GetGpuParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetObject3dInstanceManager()->SetCamera(cameraManager_->GetCamera());
}

void EffectScene::Finalize()
{
	// 2Dパーティクルはカメラ参照を持つため、カメラ破棄より前に解放する
	particleEmitter2d_.reset();

	// ライトはLightManagerにも共有されるため、シーン側の所有参照を明示的に外す
	directionalLight_.reset();
	pointLight_.reset();
	spotLight_.reset();

	// EntityManager所有のオブジェクトは非所有ポインタだけ無効化する
	for (EffectPreviewSlot& slot : previewSlots_) {
		slot.marker = nullptr;
	}
	previewSlots_.clear();
	sky_ = nullptr;
	skyBox.reset();
	effect_.reset();

	// CameraManagerが描画系へ渡したカメラ参照を外してから破棄する
	if (cameraManager_) {
		cameraManager_->Finalize();
		cameraManager_.reset();
	}
	effectCamera_.reset();
	inputCoordinator_.reset();
}

void EffectScene::Update()
{
	// 時間
	float dt = GetTime();

	// 入力更新とデバッグ対象へのコマンド実行を一括で行う
	inputCoordinator_->Update(dt, nullptr);
	UpdateInput();

	// エフェクトの高さや広がりを確認しやすいようにグリッドを描画する
	GetEntityManager()->Get3DLineCommon()->GetDebugLineMeshData().AddGrid(1000.0f, 1000.0f, 1.0f, {1,1,1,1});

	// カメラ管理の更新
	cameraManager_->Update();
	SetCamera(cameraManager_->GetCamera());

	// Effect更新
	effect_->Update(dt);
	UpdateAutoEmit(dt);
	UpdatePreviewObjects(dt);

	// 2Dパーティクルの確認表示を更新する
	if (particleEmitter2d_) {
	//	particleEmitter2d_->Update();
	}

	UpdateImGui();
}

void EffectScene::Draw3D() {}

void EffectScene::Draw2D() {}

void EffectScene::InitializeCamera()
{
	// エフェクト全体を見下ろす固定カメラを生成する
	effectCamera_ = std::make_unique<EffectCamera>();
	effectCamera_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables(), {});

	// カメラ管理クラスを初期化し、エフェクト用カメラを使用中にする
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables());
	cameraManager_->AddCamera({ effectCamera_.get(), true }, "effectCamera");
	cameraManager_->SetUseCamera("effectCamera", 0.0f);
	cameraManager_->Update();
}

void EffectScene::InitializeEnvironment()
{
	// スカイボックスを生成して、エフェクトの輪郭を確認しやすい背景を用意する
	skyBox = std::make_unique<Engine::SkyBox>();
	skyBox->Initialize(GetEntityManager(), "resources/Texture/hdr/sky.dds");

	// 空用の3Dオブジェクトにスカイボックスリソースを接続する
	sky_ = GetEntityManager()->CreateObject3D("skyBox", Engine::ObjectModelType::kSkyBox, {}, cameraManager_->GetCamera());
	sky_->GetWorldTransform().scale_ = skyBoxScale_;
	sky_->SetSkyBox(skyBox.get());

	// エフェクトの色が潰れないように弱めの平行光源を追加する
	directionalLight_ = std::make_shared<Engine::DirectionalLight>();
	directionalLight_->directional.color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLight_->directional.direction = { 0.2f,-1.0f,0.3f };
	directionalLight_->directional.intensity = 0.8f;
	directionalLight_->directional.lig = 0.1f;
	directionalLight_->directional.isLight = true;
	GetEntityManager()->GetLightManager()->AddLight(directionalLight_);

	// 爆発地点付近を照らすポイントライトを追加する
	pointLight_ = std::make_shared<Engine::PointLight>();
	pointLight_->point.color = { 1.0f,0.7f,0.35f,1.0f };
	pointLight_->point.position = { 0.0f,8.0f,0.0f };
	pointLight_->point.intensity = 2.0f;
	pointLight_->point.radius = 35.0f;
	pointLight_->point.decay = 1.5f;
	pointLight_->point.lig = 0.1f;
	pointLight_->point.isLight = true;
	GetEntityManager()->GetLightManager()->AddLight(pointLight_);

	// 奥側の煙やリングを浮かび上がらせるスポットライトを追加する
	spotLight_ = std::make_shared<Engine::SpotLight>();
	spotLight_->spot.color = { 0.6f,0.8f,1.0f,1.0f };
	spotLight_->spot.position = { -18.0f,18.0f,-24.0f };
	spotLight_->spot.direction = { 0.4f,-0.7f,0.6f };
	spotLight_->spot.intensity = 1.4f;
	spotLight_->spot.distance = 80.0f;
	spotLight_->spot.decay = 1.2f;
	spotLight_->spot.cosAngle = 0.65f;
	spotLight_->spot.cosFalloffStart = 0.85f;
	spotLight_->spot.isLight = true;
	GetEntityManager()->GetLightManager()->AddLight(spotLight_);
}

void EffectScene::InitializePreviewObjects()
{
	// 爆発の中心を示すリングを生成する
	Engine::Object3d* centerRing = GetEntityManager()->CreatePrimitiveObject3D<Engine::RingPrimitive>(
		"EffectPreview_CenterRing",
		"resources/Texture/effect/gradationLine.dds",
		cameraManager_->GetCamera());
	centerRing->GetWorldTransform().translate_ = { 0.0f,0.05f,0.0f };
	centerRing->GetWorldTransform().rotate_.x = Math::DegreesToRadians(90.0f);
	centerRing->GetWorldTransform().scale_ = { 1.2f,1.2f,1.2f };
	centerRing->GetPrimitive()->SetPsoType(Engine::BasePrimitive::PsoType::kNoCullRingClamp);
	centerRing->GetPrimitive()->GetMaterial()->GetMaterialInstance().color = { 1.0f,0.55f,0.1f,0.85f };

	// 左側に煙と薬莢系を確認する小さなリングを生成する
	Engine::Object3d* leftRing = GetEntityManager()->CreatePrimitiveObject3D<Engine::RingPrimitive>(
		"EffectPreview_LeftRing",
		"resources/Texture/effect/gradationLine.dds",
		cameraManager_->GetCamera());
	leftRing->GetWorldTransform().translate_ = { -12.0f,0.05f,2.0f };
	leftRing->GetWorldTransform().rotate_.x = Math::DegreesToRadians(90.0f);
	leftRing->GetWorldTransform().scale_ = { 0.8f,0.8f,0.8f };
	leftRing->GetPrimitive()->SetPsoType(Engine::BasePrimitive::PsoType::kNoCullRingClamp);
	leftRing->GetPrimitive()->GetMaterial()->GetMaterialInstance().color = { 0.45f,0.8f,1.0f,0.75f };

	// 右側に方向付きスパークを確認する小さなリングを生成する
	Engine::Object3d* rightRing = GetEntityManager()->CreatePrimitiveObject3D<Engine::RingPrimitive>(
		"EffectPreview_RightRing",
		"resources/Texture/effect/gradationLine.dds",
		cameraManager_->GetCamera());
	rightRing->GetWorldTransform().translate_ = { 12.0f,0.05f,2.0f };
	rightRing->GetWorldTransform().rotate_.x = Math::DegreesToRadians(90.0f);
	rightRing->GetWorldTransform().scale_ = { 0.8f,0.8f,0.8f };
	rightRing->GetPrimitive()->SetPsoType(Engine::BasePrimitive::PsoType::kNoCullRingClamp);
	rightRing->GetPrimitive()->GetMaterial()->GetMaterialInstance().color = { 1.0f,0.35f,0.4f,0.75f };

	// 上方向の高さ確認用に半透明キューブを生成する
	Engine::Object3d* heightCube = GetEntityManager()->CreatePrimitiveObject3D<Engine::CubePrimitive>(
		"EffectPreview_HeightCube",
		"resources/Texture/uvChecker.png",
		cameraManager_->GetCamera());
	heightCube->GetWorldTransform().translate_ = { 0.0f,6.0f,10.0f };
	heightCube->GetWorldTransform().scale_ = { 0.4f,0.4f,0.4f };
	heightCube->GetPrimitive()->GetMaterial()->GetMaterialInstance().color = { 0.3f,0.9f,0.7f,0.45f };
}

void EffectScene::InitializePreviewEffects()
{
	// ミサイル着弾用の一式を中心で確認する
	previewSlots_.push_back({ "missileHitCylinder", { 0.0f,0.0f,0.0f }, {}, {}, 1.8f, 0.0f, false, nullptr });
	previewSlots_.push_back({ "missileHit", { 0.0f,8.0f,0.0f }, {}, {}, 1.8f, 0.3f, false, nullptr });
	previewSlots_.push_back({ "smokePlaneExpSmoke", { 0.0f,0.0f,0.0f }, {}, {}, 2.8f, 0.6f, false, nullptr });
	previewSlots_.push_back({ "expPlane01", { 0.0f,3.0f,0.0f }, {}, {}, 2.0f, 0.9f, false, nullptr });
	previewSlots_.push_back({ "expSpark", { 0.0f,2.0f,0.0f }, {}, {}, 1.6f, 1.2f, false, nullptr });
	previewSlots_.push_back({ "AnimatedCube", { 0.0f,1.0f,0.0f }, {}, {}, 3.0f, 1.5f, false, nullptr });
	previewSlots_.push_back({ "ringEmit", { 0.0f,0.0f,0.0f }, {}, {}, 0.7f, 0.0f, false, nullptr });

	// 左右のリングで弾や煙の小物系を確認する
	previewSlots_.push_back({ "stratSmoke01", { -12.0f,0.0f,2.0f }, {}, {}, 2.0f, 0.0f, false, nullptr });
	previewSlots_.push_back({ "bulletSmoke", { -12.0f,1.5f,2.0f }, {}, {}, 1.1f, 0.0f, false, nullptr });
	previewSlots_.push_back({ "cartridge", { -12.0f,1.5f,2.0f }, { 0.0f,6.0f,0.0f }, { 3.0f,2.0f,3.0f }, 1.4f, 0.2f, true, nullptr });
	previewSlots_.push_back({ "expSpark", { 12.0f,2.0f,2.0f }, { 0.0f,0.0f,0.0f }, {}, 1.0f, 0.0f, true, nullptr });

	// 代表地点に近いマーカーを関連付けて、ImGuiやアニメーションから参照できるようにする
	if (!previewSlots_.empty()) {
		previewSlots_[0].marker = GetEntityManager()->CreatePrimitiveObject3D<Engine::SpherePrimitive>(
			"EffectPreview_CenterMarker",
			"resources/Texture/Image.dds",
			cameraManager_->GetCamera());
		previewSlots_[0].marker->GetWorldTransform().translate_ = { 0.0f,0.7f,0.0f };
		previewSlots_[0].marker->GetWorldTransform().scale_ = { 0.12f,0.12f,0.12f };
		previewSlots_[0].marker->GetPrimitive()->GetMaterial()->GetMaterialInstance().color = { 1.0f,0.55f,0.1f,0.8f };
	}
}

void EffectScene::InitializeParticle2D()
{
	// 2Dパーティクルの描画にも同じカメラを渡す
	GetEntityManager()->GetEffectManager()->GetParticleManager2d()->SetCamera(cameraManager_->GetCamera());

	// 画面右上寄りで軽い火花のように散る2Dパーティクルを生成する
	particleEmitter2d_ = std::make_unique<Engine::ParticleEmitter2d>();
	particleEmitter2d_->Initialize(GetEntityManager()->GetEffectManager()->GetParticleManager2d(), "EffectScene2D", "testParticle2d");
	particleEmitter2d_->SetPos({ 1080.0f,180.0f });
	particleEmitter2d_->SetCount(8, 3);
	particleEmitter2d_->SetLifeTime(0.8f, 0.25f);
	particleEmitter2d_->SetSize({ 18.0f,18.0f }, { 8.0f,8.0f });
	particleEmitter2d_->SetVelocity({ -60.0f,20.0f }, { 90.0f,80.0f });
	particleEmitter2d_->SetAcceleration({ 0.0f,80.0f }, { 0.0f,20.0f });
	particleEmitter2d_->SetColorMinMax({ 1.0f,0.55f,0.2f,1.0f }, { 1.0f,1.0f,0.5f,1.0f });
	particleEmitter2d_->GetEmitData().isEmit = true;
}

void EffectScene::UpdateInput()
{
	Engine::Input* input = GetInput();
	if (!input) {
		return;
	}

	// Spaceで選択中のエフェクトを単発発火する
	if (input->IsTriggerKey(DIK_SPACE)) {
		EmitPreview(selectedPreviewIndex_);
	}

	// Enterで登録済みエフェクトをまとめて発火する
	if (input->IsTriggerKey(DIK_RETURN)) {
		EmitAllPreview();
	}

	// Tabで手動発火対象を次へ進める
	if (input->IsTriggerKey(DIK_TAB) && !previewSlots_.empty()) {
		selectedPreviewIndex_ = (selectedPreviewIndex_ + 1) % previewSlots_.size();
	}

	// Aキーで自動発火を切り替える
	if (input->IsTriggerKey(DIK_A)) {
		isAutoEmit_ = !isAutoEmit_;
	}

	// Escapeでタイトルへ戻れるようにして、確認シーンから抜けやすくする
	if (input->IsTriggerKey(DIK_ESCAPE)) {
		GetSceneManager()->ChangeScene("TITLE");
	}
}

void EffectScene::UpdateAutoEmit(float dt)
{
	if (!isAutoEmit_) {
		return;
	}

	// 各スロットの個別周期で発火する
	for (size_t i = 0; i < previewSlots_.size(); ++i) {
		EffectPreviewSlot& slot = previewSlots_[i];
		slot.timer += dt;
		if (slot.timer >= slot.interval) {
			slot.timer = 0.0f;
			EmitPreview(i);
		}
	}

	// 一定周期で全体を同時発火し、合成時の見え方を確認する
	allEmitTimer_ += dt;
	if (allEmitTimer_ >= allEmitInterval_) {
		allEmitTimer_ = 0.0f;
		EmitAllPreview();
	}
}

void EffectScene::UpdatePreviewObjects(float dt)
{
	// 確認用オブジェクトにゆっくりした鼓動を付けて、発火地点を見つけやすくする
	previewAnimationTimer_ += dt;
	const float pulse = 1.0f + std::sin(previewAnimationTimer_ * 2.0f) * 0.08f;

	for (EffectPreviewSlot& slot : previewSlots_) {
		if (slot.marker) {
			slot.marker->GetWorldTransform().scale_ = { 0.12f * pulse,0.12f * pulse,0.12f * pulse };
		}
	}

	// ライトも少しだけ明滅させて、煙の陰影変化を確認しやすくする
	if (pointLight_) {
		pointLight_->point.intensity = 1.8f + std::sin(previewAnimationTimer_ * 3.0f) * 0.4f;
	}
}

void EffectScene::UpdateImGui()
{
#ifdef _DEBUG
	// エフェクト確認用の手動操作パネル
	ImGui::Begin("EffectScene");
	ImGui::Checkbox("Auto Emit", &isAutoEmit_);
	ImGui::DragFloat("All Emit Interval", &allEmitInterval_, 0.05f, 0.5f, 30.0f);

	if (ImGui::Button("Emit Selected")) {
		EmitPreview(selectedPreviewIndex_);
	}
	ImGui::SameLine();
	if (ImGui::Button("Emit All")) {
		EmitAllPreview();
	}

	if (!previewSlots_.empty()) {
		const char* previewName = previewSlots_[selectedPreviewIndex_].name.c_str();
		if (ImGui::BeginCombo("Preview Slot", previewName)) {
			for (size_t i = 0; i < previewSlots_.size(); ++i) {
				const bool selected = (selectedPreviewIndex_ == i);
				ImGui::PushID(static_cast<int>(i));
				if (ImGui::Selectable(previewSlots_[i].name.c_str(), selected)) {
					selectedPreviewIndex_ = i;
				}
				if (selected) {
					ImGui::SetItemDefaultFocus();
				}
				ImGui::PopID();
			}
			ImGui::EndCombo();
		}

		EffectPreviewSlot& slot = previewSlots_[selectedPreviewIndex_];
		ImGui::DragFloat3("Position", &slot.position.x, 0.1f);
		ImGui::DragFloat("Interval", &slot.interval, 0.05f, 0.1f, 30.0f);
		ImGui::Checkbox("Use Velocity", &slot.useVelocity);
		if (slot.useVelocity) {
			ImGui::DragFloat3("Velocity", &slot.velocity.x, 0.1f);
			ImGui::DragFloat3("Velocity Range", &slot.velocityRange.x, 0.1f);
		}
	}
	ImGui::Text("Space: Emit Selected / Enter: Emit All / Tab: Next / A: Auto / Esc: Title");
	ImGui::End();
#endif
}

void EffectScene::EmitPreview(size_t index)
{
	if (!effect_ || index >= previewSlots_.size()) {
		return;
	}

	// スロット設定に応じて通常Emitか方向付きEmitを呼び分ける
	EffectPreviewSlot& slot = previewSlots_[index];
	if (slot.useVelocity) {
		effect_->Emit(slot.name, slot.position, slot.velocity, slot.velocityRange);
	}
	else {
		effect_->Emit(slot.name, slot.position);
	}
}

void EffectScene::EmitAllPreview()
{
	// 登録済みスロットをすべて発火して、複合演出としての見え方を確認する
	for (size_t i = 0; i < previewSlots_.size(); ++i) {
		EmitPreview(i);
	}
}
