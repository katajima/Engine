#pragma once
#include"Framework.h"
#include "DirectXGame/engine/Utility/FrameTime.h"
#include "DirectXGame/engine/Base/Profiler/CpuProfilerWindow.h"

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
		/// @brief 実測された経過時間でフレームを更新する。
		/// @param deltaTime 前フレームから経過した実時間（秒）。
		void Update(float deltaTime) override;

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
		static float GameTime();
		/// <summary>
		/// 時間倍率を掛けていない実測デルタタイム取得
		/// </summary>
		static float BaseDeltaTime();
		/// <summary>
		/// 現在のゲーム内時間倍率取得
		/// </summary>
		static float GetTimeScale();
		/// <summary>
		/// ゲーム内時間倍率設定
		/// </summary>
		static void SetTimeScale(float timeScale);
		/// <summary>
		/// 現在の時間
		/// </summary>
		static float NowTime();

	private:
		static float mLegacyGameDeltaTime;
		static float mLegacyUnscaledDeltaTime;
		FrameTime mFrameTime;
		static float kTimeSpeed_;
		static float nowTime;
	private:


		float fps = 0.0f;

		DebugTimer debugTimer_;
		// CPUプロファイル結果を表示するデバッグウィンドウ。
		std::unique_ptr<CpuProfilerWindow> cpuProfilerWindow_ = nullptr;

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
