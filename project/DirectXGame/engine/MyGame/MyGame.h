#pragma once
#include"Framework.h"

namespace Engine {
	/// <summary>
	/// ゲームのフレームワークを拡張したクラスで、ゲームの初期化、更新、描画、リソース管理などを行う。
	/// </summary>
	class  MyGame : public Framework
	{
	public:

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize() override;

		/// <summary>
		/// 終了
		/// </summary>
		void Finalize() override;

		/// <summary>
		/// 毎フレーム更新
		/// </summary>
		void Update() override;

		/// <summary>
		/// 描画
		/// </summary>
		void Draw() override;

		/// <summary>
		/// リソース初期化
		/// </summary>
		void InitializeResource();

		/// <summary>
		/// パーティクル生成
		/// </summary>
		void CreateParticle();

		/// <summary>
		/// モデル読み込み
		/// </summary>
		void LoadModel();

		/// <summary>
		/// ゲーム内時間取得
		/// </summary>
		static float GameTime() { return kDeltaTime_ * kTimeSpeed_; };
		/// <summary>
		/// 時間倍率を掛けていない固定デルタタイム取得
		/// </summary>
		static float BaseDeltaTime() { return kDeltaTime_; };
		/// <summary>
		/// 現在のゲーム内時間倍率取得
		/// </summary>
		static float GetTimeScale() { return kTimeSpeed_; };
		/// <summary>
		/// ゲーム内時間倍率設定
		/// </summary>
		static void SetTimeScale(float timeScale);
		/// <summary>
		/// 現在の時間
		/// </summary>
		static float NowTime() { return nowTime; };

	private:
		static float const kDeltaTime_;
		static float kTimeSpeed_;
		static float nowTime;
	private:
		std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();

		float fps = 0.0f;

		DebugTimer debugTimer_;

		std::unique_ptr<TorusPrimitive> primi = nullptr;
		std::unique_ptr<PlanePrimitive> primiPlane = nullptr;
		std::unique_ptr<StarPrimitive> primiStar = nullptr;
		std::unique_ptr<TrianglePrimitive> primitiveTriangle = nullptr;

		std::unique_ptr<RingPrimitive> primiRing = nullptr;
		std::unique_ptr<CylinderPrimitive> cylinder_ = nullptr;

	private:
		// ゲーム終了フラグ
		bool endRequest_ = false;
		//
		std::vector<std::string> scenenames_;

	};
}
