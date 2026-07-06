#include"SceneManager.h"
#include"cassert"

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include <algorithm>

void Engine::SceneManager::Init() {
	fade_ = std::make_unique<Fade>();
	fade_->Initialize(entityManager);
	finished_ = true;
};

void Engine::SceneManager::Update()
{
	// 通常シーン処理
	if (scene_) {
		scene_->AllUpdate();
	}

	// 各フェーズ処理
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
				scene_.reset();
			}

			scene_ = std::move(nextScene_);
			//nextScene_ = nullptr;

			// シーンマネージャをセット
			scene_->SetSceneManader(this);
			scene_->SetInput(input);
			scene_->SetGlobalVariables(globalVariables);
			scene_->SetDirectXCommon(dxCommon);
			scene_->SetEntityManager(entityManager);
			scene_->SetWinApp(winApp);
			// シーンやエディタから音源一覧と再生機能を利用できるよう共有する。
			scene_->SetAudioManager(audioManager);

			dxCommon->GetPostEffectManager()->ClearPostEffectBlock();
			entityManager->GetObject3dInstansManager()->ClearObject();
			entityManager->GetObject3dInstansManager()->AllClear();
			entityManager->GetEffectManager()->GetParticleManager()->ClearParticle();
			entityManager->ObjectClean();
			entityManager->GetLightManager()->ClearLights();
			
			
			scene_->Initialize();
			SetCamera(scene_->GetCamara());

			// フェードイン開始
			fade_->Start(Fade::Status::FadeIn, 0.5f, Color(0, 0, 0, 1));
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

void Engine::SceneManager::Draw3D()
{
	if (scene_)
		scene_->Draw3D();
}

void Engine::SceneManager::Draw2D()
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

void Engine::SceneManager::DrawForeground2D() {
	if (scene_) {
		scene_->DrawForeground2D();
	}
};

Engine::SceneManager::~SceneManager()
{
	// Finalize中に scene_ が変更・reset されても安全なように、所有権を退避
	std::unique_ptr<BaseScene> dying = std::move(scene_);
	if (dying)
	{
		dying->Finalize();
		// dying がスコープを抜けるときに確実に delete される
	}
	// scene_ は既に nullptr（move済み）
}

void Engine::SceneManager::ChangeScene(const std::string& sceneName, float duration)
{
	assert(sceneFactory);
	if (nextScene_ == nullptr && phase_ == Phase::kMain) {
		// ★ unique_ptr をそのまま受け取る（reset を使わない）
		nextScene_ = sceneFactory->CreateScene(sceneName);
		if (!nextScene_) {
			assert(false && "Invalid scene name");
			return;
		}

		this->sceneName = sceneName;

		// フェードアウト開始
		fade_->Start(Fade::Status::FadeOut, duration, Color(0, 0, 0, 1));
		fade_->SetSize({ static_cast<float>(WinApp::GetClientWidth()), static_cast<float>(WinApp::GetClientHeight()) });
		phase_ = Phase::kFadeOut;
	}
}
