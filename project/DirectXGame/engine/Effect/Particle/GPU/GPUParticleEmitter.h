#pragma once

#include "GpuParticleData.h"


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

		// 初期化
		void Init(DirectXCommon* dxCommon, LineCommon* lineCommon, GpuParticleGroup* group, std::string name);
		// 更新
		void Update(float deltaTime);
		// ImGui更新
		void UpdateImGui();
		// グループ設定
		void SetParticleGroup(GpuParticleGroup* group) { group_ = group; }
		// エミッターデータ
		EmitterCommon& GetCommonData() { return cbEmitterCommon_; }
		// トレイルデータ
		EmitterTrail& GetTrailData() { return cbEmitterTrail_; }
		// エミッター形状
		EmitterType GetType() const { return type_; }
		// トランスフォーム
		WorldTransform& GetWorldTransform() { return worldTransform_; }
		// 名前
		std::string GetName() { return name_; }
		// エミッター時間設定
		void SetDeltaTime(float delta) { deltaTime_ = delta; }
		// エミッター時間取得
		float GetDeltaTime() const { return deltaTime_; }


	protected:

		// 固有の初期化
		virtual void InitUniqe() = 0;
		// 固有の更新
		virtual void UpdateUniqe(float deltaTime) = 0;
		// 固有のImGui更新
		virtual void UpdateImGuiUniqe() = 0;
		// ライン描画
		virtual void DrawLine() = 0;

	protected:
		std::string name_;
		bool isEmitte_ = true;
		int count_ = 64;


		float time_ = 0.0f;			// 経過時間()
		float interval_ = 0.02f;	// 発生間隔()
		float deltaTime_ = 1.0f / 60.0f;

		EmitterType type_ = EmitterType::Sphere;

		EmitterCommon cbEmitterCommon_;	// 共通データ
		EmitterTrail  cbEmitterTrail_;	// トレイルエミッター用データ

		WorldTransform worldTransform_;	// ワールド変換情報


		DirectXCommon* dxCommon_ = nullptr;		// DirectX共通クラス
		Engine::GpuParticleGroup* group_ = nullptr;		// GPUパーティクルグループ
		LineCommon* lineCommon_ = nullptr;		// ライン共通クラス
	};

	// 球エミッター
	class GpuParticleEmitterSphere : public BaseGpuParticleEmitter
	{
	public:
		// 球エミッターのパラメータ
		void InitUniqe() override;
		// 固有の更新
		void UpdateUniqe(float deltaTime) override;
		// 固有のImGui更新
		void UpdateImGuiUniqe() override;
		// ライン描画
		void DrawLine() override;

	private:
	};

	// 点エミッター
	class GpuParticleEmitterPoint : public BaseGpuParticleEmitter
	{
	public:
		// 球エミッターのパラメータ
		void InitUniqe() override;
		// 固有の更新
		void UpdateUniqe(float deltaTime) override;
		// 固有のImGui更新
		void UpdateImGuiUniqe() override;
		// ライン描画
		void DrawLine() override;

	private:
	};

	// 球エミッター
	class GpuParticleEmitterAABB : public BaseGpuParticleEmitter
	{
	public:
		// 球エミッターのパラメータ
		void InitUniqe() override;
		// 固有の更新
		void UpdateUniqe(float deltaTime) override;
		// 固有のImGui更新
		void UpdateImGuiUniqe() override;
		// ライン描画
		void DrawLine() override;
	private:
	};
}