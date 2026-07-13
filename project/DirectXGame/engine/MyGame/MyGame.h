#pragma once
#include"Framework.h"

namespace Engine {
	/// <summary>
	/// ゲームのフレームワークを拡張したクラスで、ゲームの初期化、更新、描画、リソース管理などを行う。
	/// </summary>
	class  MyGame : public Framework
	{
	public:

		// 初期化
		void Initialize() override;

		// 終了
		void Finalize() override;

		// 毎フレーム更新
		void Update() override;

		// 描画
		void Draw() override;

		// リソース初期化
		void InitializeResource();

		// パーティクル生成
		void CreateParticle();

		// モデル読み込み
		void LoadModel();

		// ゲーム内時間取得
		static float GameTime() { return kDeltaTime_ * kTimeSpeed_; };
		// 時間倍率を掛けていない固定デルタタイム取得
		static float BaseDeltaTime() { return kDeltaTime_; };
		// 現在のゲーム内時間倍率取得
		static float GetTimeScale() { return kTimeSpeed_; };
		// ゲーム内時間倍率設定
		static void SetTimeScale(float timeScale);
		// 現在の時間
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
