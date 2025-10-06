#pragma once

#include "GpuParticleData.h"

class DirectXCommon;
class LineCommon;
class GpuParticleGroup;


class BaseGpuParticleEmitter
{
public:
	virtual ~BaseGpuParticleEmitter() {}


	void Init(DirectXCommon* dxCommon, LineCommon* lineCommon, GpuParticleGroup* group, std::string name);

	void Update(float deltaTime);

	void UpdateImGui();

	void SetParticleGroup(GpuParticleGroup* group) { group_ = group;}

	EmitterCommon* GetCommonData() { return cbEmitterCommon_.Data(); }
	EmitterTrail* GetTrailData() { return cbEmitterTrail_.Data(); }

	EmitterType GetType() const { return type_; }

	WorldTransform& GetWorldTransform() { return worldTransform_; }

protected:
	
	// 固有の初期化
	virtual void InitUniqe() = 0;

	virtual void UpdateUniqe(float deltaTime) = 0;

	virtual void UpdateImGuiUniqe() = 0;

	virtual void DrawLine() = 0;

protected:
	std::string name_;
	bool isEmitte_ = true; 
	int count_ = 64;


	float time_ = 0.0f;			// 経過時間()
	float interval_ = 0.02f;	// 発生間隔()


	EmitterType type_ = EmitterType::Sphere;

	ConstantBuffer<EmitterCommon> cbEmitterCommon_;	// 共通データ
	ConstantBuffer<EmitterTrail> cbEmitterTrail_;	// トレイルエミッター用データ

	WorldTransform worldTransform_;	// ワールド変換情報


	DirectXCommon* dxCommon_ = nullptr;		// DirectX共通クラス
	GpuParticleGroup* group_ = nullptr;		// GPUパーティクルグループ
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

	EmitterSphereCS* GetData() { return cbEmitterSphere_.Data(); }

private:
	// 球エミッター
	ConstantBuffer<EmitterSphereCS> cbEmitterSphere_;
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

	EmitterPointCS* GetData() { return cbEmitterPoint_.Data(); }

private:
	// 球エミッター
	ConstantBuffer<EmitterPointCS> cbEmitterPoint_;
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

	EmitterAABBCS* GetData() { return cbEmitterAABB_.Data(); }

private:
	// 球エミッター
	ConstantBuffer<EmitterAABBCS> cbEmitterAABB_;
};
