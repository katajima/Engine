#pragma once

namespace Engine {
	class Camera;
	class PostEffectPass;
}

namespace Character {

	// 回避成功時のポストエフェクトを管理するクラス
/// <summary>
/// DodgeSuccessEffectを管理・実装するクラス。
/// </summary>
	class DodgeSuccessEffect {
	public:
		/// <summary>
		/// 破棄時にポストエフェクトが残らないよう停止する
		/// </summary>
		~DodgeSuccessEffect();

		/// <summary>
		/// 使用するカメラへ回避成功用ポストエフェクトを登録する
		/// </summary>
		void Initialize(Engine::Camera* camera);

		/// <summary>
		/// 回避成功ポストエフェクトを開始する
		/// </summary>
		void Start();

		/// <summary>
		/// ポストエフェクト時間を進め、時間切れで解除する
		/// </summary>
		void Update(float deltaTime);

		/// <summary>
		/// ポストエフェクトを即座に停止する
		/// </summary>
		void Stop();

	private:
		/// <summary>
		/// 回避成功用ラジアルブラーのパラメータを調整する
		/// </summary>
		void ConfigureRadialBlur();

		/// <summary>
		/// 登録済みポストエフェクトの使用状態をまとめて切り替える
		/// </summary>
		void SetPostEffectUse(bool use);

	private:
		static constexpr const char* kGrayEffectName_ = "DodgeSuccessGray";					// グレースケール登録名
		static constexpr const char* kGrayPassName_ = "GrayScale_DodgeSuccessGray";			// グレースケールパス検索名
		static constexpr const char* kRadialBlurEffectName_ = "DodgeSuccessRadialBlur";		// ラジアルブラー登録名
		static constexpr const char* kRadialBlurPassName_ = "RadialBlur_DodgeSuccessRadialBlur";	// ラジアルブラーパス検索名

		Engine::Camera* camera_ = nullptr;					// 演出を追加するカメラ
		Engine::PostEffectPass* grayPass_ = nullptr;		// 回避成功中に使用するグレースケールパス
		Engine::PostEffectPass* radialBlurPass_ = nullptr;	// 回避成功中に使用するラジアルブラーパス
		bool isActive_ = false;								// 演出再生中か
		float timer_ = 0.0f;								// 演出の経過時間
		float effectDuration_ = 0.35f;						// ポストエフェクトを維持する時間
	};
}
