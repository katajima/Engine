#pragma once

#include "ParticleData.h"

//class ModelMesh;
//class DirectXCommon;
//class ParticleManager;
//class ParticleGroup
//{
//public:
//	void Create(ParticleManager* particleManager, DirectXCommon* dxCommon, int MaxInstance, std::string name, std::string textureName);
//
//	void Update();
//
//	void Draw();
//
//
//private:
//	std::string name;														// 名前
//	std::unique_ptr<Material> material = nullptr;							// マテリアルデータ
//	std::list<Particle> particle;											// パーティクル
//
//
//	StructuredBuffer<ParticleForGPU> sbParticleResource_;					// パーティクルリソース(sBuffer)
//	uint32_t instanceCount;													// インスタンス数
//	ModelMesh* mesh;														// メッシュ
//	Emiter emiter;															// エミッター
//	EmitData::IsFlag isFlag;												// 各フラグ
//	EmitData::EmitType emitType = EmitData::EmitType::kRandom;				// エミッターでの出方
//	EmitData::TopBottom topBottom = EmitData::TopBottom::kBottom;			// 
//	EmitData::RasterizerType rasteType;										// ラスタライザタイプ
//	EmitData::BlendType blendType;											// ブレンドタイプ
//	Transform uvTransformVeloctiy_{ {},{},{0,0,0} };						// uvトランスフォーム
//	float kGravitationalAcceleration = 9.8f;								// 重力
//
//	SrvManager* srvManager_ = nullptr;		// SRVマネージャー
//	DirectXCommon* dxCommon_ = nullptr;		// DirectX共通クラス
//	Camera* camera_ = nullptr;				// カメラ
//	ParticleManager* particleManager_;		// パーティクルマネージャー
//};