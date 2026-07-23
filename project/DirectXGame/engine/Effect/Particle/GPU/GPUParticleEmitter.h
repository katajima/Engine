#pragma once

#include "GpuParticleData.h"
#include "DirectXGame/engine/Utility/ConvertUtility.h"


namespace Engine {
	// 前方宣言
	class DirectXCommon;
	class LineCommon;
	class GpuParticleGroup;

	/// <summary>
	/// GPUパーティクルエミッター基底クラス
	/// </summary>
	class BaseGpuParticleEmitter
	{
	public:
		virtual ~BaseGpuParticleEmitter() {}

		/// <summary>
		/// 初期化
		/// </summary>
		void Init(DirectXCommon* dxCommon, LineCommon* lineCommon, GpuParticleGroup* group, std::string name);
		/// <summary>
		/// 更新
		/// </summary>
		void Update(float deltaTime);
		/// <summary>
		/// ImGui更新
		/// </summary>
		void UpdateImGui();
		/// <summary>
		/// グループ設定
		/// </summary>
		void SetParticleGroup(GpuParticleGroup* group) { this->group = group; }
		/// <summary>
		/// エミッターデータ
		/// </summary>
		EmitterCommon& GetCommonData() { return cbEmitterCommon_; }
		/// <summary>
		/// トレイルデータ
		/// </summary>
		EmitterTrail& GetTrailData() { return cbEmitterTrail_; }
		/// <summary>
		/// エミッター形状
		/// </summary>
		EmitterType GetType() const { return type_; }
		/// <summary>
		/// トランスフォーム
		/// </summary>
		WorldTransform& GetWorldTransform() { return worldTransform_; }
		/// <summary>
		/// 名前
		/// </summary>
		std::string GetName() { return name_; }
		/// <summary>
		/// エミッター時間設定
		/// </summary>
		void SetDeltaTime(float delta) { deltaTime_ = delta; }
		/// <summary>
		/// エミッター時間取得
		/// </summary>
		float GetDeltaTime() const { return deltaTime_; }


	protected:

		/// <summary>
		/// 固有の初期化
		/// </summary>
		virtual void InitUnique() = 0;
		/// <summary>
		/// 固有の更新
		/// </summary>
		virtual void UpdateUnique(float deltaTime) = 0;
		/// <summary>
		/// 固有のImGui更新
		/// </summary>
		virtual void UpdateImGuiUnique() = 0;
		/// <summary>
		/// ライン描画
		/// </summary>
		virtual void DrawLine() = 0;

	protected:
		std::string name_;
		bool isEmitte_ = true;
		int count_ = 64;


		float time_ = 0.0f;			// 経過時間()
		float interval_ = 0.02f;	// 発生間隔()
		float deltaTime_ = ConvertUtility::kDefaultDeltaTime;	// 未指定時に使う基準デルタタイム

		EmitterType type_ = EmitterType::Sphere;

		EmitterCommon cbEmitterCommon_;	// 共通データ
		EmitterTrail  cbEmitterTrail_;	// トレイルエミッター用データ

		WorldTransform worldTransform_;	// ワールド変換情報


		DirectXCommon* dxCommon = nullptr;		// DirectX共通クラス
		Engine::GpuParticleGroup* group = nullptr;		// GPUパーティクルグループ
		LineCommon* lineCommon = nullptr;		// ライン共通クラス
	};

	// 球エミッター
/// <summary>
/// GpuParticleEmitterSphereを管理・実装するクラス。
/// </summary>
	class GpuParticleEmitterSphere : public BaseGpuParticleEmitter
	{
	public:
		/// <summary>
		/// 球エミッターのパラメータ
		/// </summary>
		void InitUnique() override;
		/// <summary>
		/// 固有の更新
		/// </summary>
		void UpdateUnique(float deltaTime) override;
		/// <summary>
		/// 固有のImGui更新
		/// </summary>
		void UpdateImGuiUnique() override;
		/// <summary>
		/// ライン描画
		/// </summary>
		void DrawLine() override;

	private:
	};

	// 点エミッター
/// <summary>
/// GpuParticleEmitterPointを管理・実装するクラス。
/// </summary>
	class GpuParticleEmitterPoint : public BaseGpuParticleEmitter
	{
	public:
		/// <summary>
		/// 球エミッターのパラメータ
		/// </summary>
		void InitUnique() override;
		/// <summary>
		/// 固有の更新
		/// </summary>
		void UpdateUnique(float deltaTime) override;
		/// <summary>
		/// 固有のImGui更新
		/// </summary>
		void UpdateImGuiUnique() override;
		/// <summary>
		/// ライン描画
		/// </summary>
		void DrawLine() override;

	private:
	};

	// AABBエミッター
/// <summary>
/// GpuParticleEmitterAABBを管理・実装するクラス。
/// </summary>
	class GpuParticleEmitterAABB : public BaseGpuParticleEmitter
	{
	public:
		/// <summary>
		/// 球エミッターのパラメータ
		/// </summary>
		void InitUnique() override;
		/// <summary>
		/// 固有の更新
		/// </summary>
		void UpdateUnique(float deltaTime) override;
		/// <summary>
		/// 固有のImGui更新
		/// </summary>
		void UpdateImGuiUnique() override;
		/// <summary>
		/// ライン描画
		/// </summary>
		void DrawLine() override;
	private:
	};
}
