#include "DodgeSuccessEffect.h"

#include "DirectXGame/engine/Camera/Camera.h"
#include "DirectXGame/engine/Offscreen/PostEffect.h"
#include "DirectXGame/engine/Offscreen/PostEffectBlock.h"

namespace Character {

	DodgeSuccessEffect::~DodgeSuccessEffect()
	{
		// プレイヤー破棄時に画面効果だけ残らないよう停止する
		Stop();
	}

	void DodgeSuccessEffect::Initialize(Engine::Camera* camera)
	{
		camera_ = camera;	// 演出を適用するカメラを保持する
		if (!camera_) {
			return;
		}

		Engine::PostEffectPipeline* pipeline = camera_->GetPostEffectPipeline();
		if (!pipeline) {
			return;
		}

		// スロー中の速度感を出すため、画面中心へ向かうラジアルブラーを追加する
		radialBlurPass_ = pipeline->FindPass(kRadialBlurPassName_);
		if (!radialBlurPass_) {
			camera_->AddEffectBlock(kRadialBlurEffectName_, Engine::PostEffectBlockType::kRadialBlur, false);
			radialBlurPass_ = pipeline->FindPass(kRadialBlurPassName_);
		}

		ConfigureRadialBlur();		// 回避成功用のラジアルブラー強度を設定する
		SetPostEffectUse(false);	// 初期状態では演出を非表示にしておく
	}

	void DodgeSuccessEffect::Start()
	{
		isActive_ = true;
		timer_ = 0.0f;	// 連続で成功した場合はポストエフェクト時間を最初から数え直す
		SetPostEffectUse(true);
	}

	void DodgeSuccessEffect::Update(float deltaTime)
	{
		if (!isActive_) {
			return;
		}

		// スロー演出とは別に、ポストエフェクトだけの寿命を進める
		timer_ += deltaTime;

		if (timer_ >= effectDuration_) {
			Stop();
		}
	}

	void DodgeSuccessEffect::Stop()
	{
		isActive_ = false;
		timer_ = 0.0f;
		SetPostEffectUse(false);
	}

	void DodgeSuccessEffect::ConfigureRadialBlur()
	{
		if (!radialBlurPass_ || !radialBlurPass_->GetPostEffectData() ||
			!radialBlurPass_->GetPostEffectData()->GetRadialBlur()) {
			return;
		}

		// centerは画面中心、numSamplesはサンプリング数、blurWidthはブラーの伸び幅として扱う
		auto* radialBlurData = radialBlurPass_->GetPostEffectData()->GetRadialBlur()->Data();
		radialBlurData->center = Vector2{ 0.5f, 0.5f };
		radialBlurData->numSamples = 6;
		radialBlurData->blurWidth = 0.035f;
	}

	void DodgeSuccessEffect::SetPostEffectUse(bool use)
	{
		if (grayPass_) {
			grayPass_->SetUse(use);
		}
		
		if (radialBlurPass_) {
			radialBlurPass_->SetUse(use);
		}
	}
}
