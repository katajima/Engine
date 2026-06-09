#pragma once

#include "DirectXGame/engine/Camera/Camera.h"
#include "DirectXGame/engine/Offscreen/PostEffect.h"
#include "DirectXGame/engine/Offscreen/PostEffectBlock.h"

namespace Character {

	// 回避成功時のポストエフェクトを管理するクラス
	class DodgeSuccessEffect {
	public:
		// 破棄時にポストエフェクトが残らないよう停止する
		~DodgeSuccessEffect() { Stop(); }

		// 使用するカメラへ回避成功用ポストエフェクトを登録する
		void Initialize(Engine::Camera* camera) {
			camera_ = camera;	// 演出を適用するカメラを保持する
			if (!camera_) {
				return;
			}

			Engine::PostEffectPipeline* pipeline = camera_->GetPostEffectPipeline();
			if (!pipeline) {
				return;
			}

			// 既に登録済みなら再利用し、未登録なら今回の演出用パスを追加する
			grayPass_ = pipeline->FindPass(kGrayPassName_);
			if (!grayPass_) {
				camera_->AddEffectBlock(kGrayEffectName_, Engine::PostEffectBlockType::kGrayScale, false);
				grayPass_ = pipeline->FindPass(kGrayPassName_);
			}

			// 画面端を締めるビネットを追加し、回避成功の瞬間を強調する
			vignettePass_ = pipeline->FindPass(kVignettePassName_);
			if (!vignettePass_) {
				camera_->AddEffectBlock(kVignetteEffectName_, Engine::PostEffectBlockType::kVignette, false);
				vignettePass_ = pipeline->FindPass(kVignettePassName_);
			}

			ConfigureVignette();		// 回避成功用のビネット強度を設定する
			SetPostEffectUse(false);	// 初期状態では演出を非表示にしておく
		}

		// 回避成功ポストエフェクトを開始する
		void Start() {
			isActive_ = true;
			timer_ = 0.0f;	// 連続で成功した場合はポストエフェクト時間を最初から数え直す
			SetPostEffectUse(true);
		}

		// ポストエフェクト時間を進め、時間切れで解除する
		void Update(float deltaTime) {
			if (!isActive_) {
				return;
			}

			// スロー演出とは別に、ポストエフェクトだけの寿命を進める
			timer_ += deltaTime;

			if (timer_ >= effectDuration_) {
				Stop();
			}
		}

		// ポストエフェクトを即座に停止する
		void Stop() {
			isActive_ = false;
			timer_ = 0.0f;
			SetPostEffectUse(false);
		}

	private:
		// 回避成功用ビネットのパラメータを調整する
		void ConfigureVignette() {
			if (!vignettePass_ || !vignettePass_->GetPostEffectData() ||
				!vignettePass_->GetPostEffectData()->GetVignette()) {
				return;
			}

			// scaleは端の暗さの広がり、squaredは落ち方の鋭さとして扱う
			auto* vignetteData = vignettePass_->GetPostEffectData()->GetVignette()->Data();
			vignetteData->scale = 9.5f;
			vignetteData->squared = 0.55f;
		}

		// 登録済みポストエフェクトの使用状態をまとめて切り替える
		void SetPostEffectUse(bool use) {
			if (grayPass_) {
				grayPass_->SetUse(use);
			}
			if (vignettePass_) {
				vignettePass_->SetUse(use);
			}
		}

	private:
		static constexpr const char* kGrayEffectName_ = "DodgeSuccessGray";				// グレースケール登録名
		static constexpr const char* kGrayPassName_ = "GrayScale_DodgeSuccessGray";		// グレースケールパス検索名
		static constexpr const char* kVignetteEffectName_ = "DodgeSuccessVignette";		// ビネット登録名
		static constexpr const char* kVignettePassName_ = "Vignette_DodgeSuccessVignette";	// ビネットパス検索名

		Engine::Camera* camera_ = nullptr;						// 演出を追加するカメラ
		Engine::PostEffectPass* grayPass_ = nullptr;			// 回避成功中に使用するグレースケールパス
		Engine::PostEffectPass* vignettePass_ = nullptr;		// 回避成功中に使用するビネットパス
		bool isActive_ = false;									// 演出再生中か
		float timer_ = 0.0f;									// 演出の経過時間
		float effectDuration_ = 0.35f;							// ポストエフェクトを維持する時間
	};
}
