#include"SceneManager.h"
#include"cassert"

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include <algorithm>

void SceneManager::Init() {
	fade_ = std::make_unique<Fade>();
	fade_->Initialize(entity2DManager_);
	finished_ = true;
};

void SceneManager::Update()
{
	// 通常シーン処理
	if (scene_) {
		scene_->AllUpdate();
	}

	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;

	case Phase::kMain:

		break;
	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			// フェードアウト完了 → シーン切り替え
			if (scene_) {
				scene_->Finalize();
				delete scene_;
				scene_ = nullptr;
			}

			scene_ = nextScene_;
			nextScene_ = nullptr;

			// シーンマネージャをセット
			scene_->SetSceneManader(this);
			scene_->SetInput(input_);
			scene_->SetGlobalVariables(globalVariables_);
			scene_->SetDirectXCommon(directXCommon_);
			scene_->SetEntity3DManager(entity3DManager_);

			directXCommon_->GetPostEffectManager()->ClearPostEffectBlock();
			entity3DManager_->GetObject3dInstansManager()->ClearObject();
			entity3DManager_->GetObject3dInstansManager()->AllClear();
			entity3DManager_->ObjectClean();
			entity3DManager_->GetLightManager()->ClearLights();
			scene_->SetEntity2DManager(entity2DManager_);

			
			scene_->Initialize();
			SetCamera(scene_->GetCamara());

			// フェードイン開始
			fade_->Start(Fade::Status::FadeIn, 1.5f, Color(0, 0, 0, 1));
			phase_ = Phase::kFadeIn;
		}
		break;
	case Phase::kLoading:
		fade_->Update();
		break;
	}

	if (!scene_) {
		fade_->SetColorUpdate({ 0,0,0,1 });
	}



}

void SceneManager::Draw3D()
{
	if (scene_)
		scene_->Draw3D();
}

void SceneManager::Draw2D()
{
	if (scene_) {
		scene_->AllDraw2D();
	}

	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Draw2D();
		break;
	case Phase::kMain:

		break;
	case Phase::kFadeOut:
		fade_->Draw2D();
		break;
	case Phase::kLoading:

		break;
	default:
		break;
	}
	if (!scene_) {
		fade_->DrawCompulsion2D();
	}

}

void SceneManager::DrawForeground2D() {
	if (scene_) {
		scene_->DrawForeground2D();
	}
};

SceneManager::~SceneManager()
{
	// 最後のシーンの終了と解放
	scene_->Finalize();
	delete scene_;
}

void SceneManager::ChangeScene(const std::string& sceneName)
{
	assert(sceneFactory_);
	if (nextScene_ == nullptr && phase_ == Phase::kMain) {
		nextScene_ = sceneFactory_->CreateScene(sceneName);
		this->sceneName = sceneName;

		// フェードアウト開始
		fade_->Start(Fade::Status::FadeOut, 1.5f, Color(0, 0, 0, 1));
		fade_->SetSize({ static_cast<float>(WinApp::GetClientWidth()), static_cast<float>(WinApp::GetClientHeight()) });
		phase_ = Phase::kFadeOut;
	}
}
