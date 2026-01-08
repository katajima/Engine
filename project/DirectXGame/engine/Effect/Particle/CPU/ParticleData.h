#pragma once
#include "DirectXGame/engine/3d/Model/Model.h"
#include <DirectXGame/engine/2d/Sprite.h>
#include "DirectXGame/engine/Effect/Particle/Emit/EmitData.h"

#include "DirectXGame/engine/DirectX/Resource/StructuredBuffer.h"
#include "DirectXGame/engine/DirectX/Resource/ConstantBuffer.h"


namespace Engine {

	class Material;


	// GPUに送るデータ
	struct ParticleForGPU
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
		Vector4 color;
	};

	// パーティクル(一粒)
	struct Particle
	{
		Transform transform;			// SRT
		Vector3 velocity;				// 速度
		Vector3 acceleration;			// 加速
		Vector4 color;					// 色
		float lifeTime;					// 生存時間
		float currentTime;				// 経過時間

		Transform strtTransform;		// 初期SRT
		Vector3 rotateVelocity;			// 回転速度
		Vector3 sizeAmount;				// サイズ変化量

		bool isDestroy = false;			// 破棄フラグ

		bool isEvent = false;			// イベントフラグ


		bool isNoise = false;
		int direction = 0; // 0↑、1→、2↓,3←


		std::vector<std::string> fieldEffect; // 
	};


	// パーティクルグループ
	struct ParticleGroup
	{
		std::string name;														// 名前
		std::unique_ptr<Engine::Material> material = nullptr;							// マテリアルデータ
		std::list<Particle> particle;											// パーティクル


		Engine::StructuredBuffer<ParticleForGPU> sbParticleResource_;					// パーティクルリソース(sBuffer)
		uint32_t instanceCount;													// インスタンス数
		ModelMesh* mesh;														// メッシュ
		//Emiter emiter;															// エミッター
		EmitData::IsFlag isFlag;											// 各フラグ
		EmitData::EmitType emitType = EmitData::EmitType::kRandom;		// エミッターでの出方
		EmitData::TopBottom topBottom = EmitData::TopBottom::kBottom;	// 
		EmitData::RasterizerType rasteType;									// ラスタライザタイプ
		EmitData::BlendType blendType;										// ブレンドタイプ
		Transform uvTransformVeloctiy_{ {},{},{0,0,0} };						// uvトランスフォーム
		float kGravitationalAcceleration = 9.8f;								// 重力
		bool isUVClamp = false;												// UVを
	};


	// パーティクルグループ
	struct ParticleGroup2d
	{
		std::string name;														// 名前
		std::unique_ptr<Engine::Material> material = nullptr;							// マテリアルデータ
		std::list<Particle> particle;											// パーティクル


		Engine::StructuredBuffer<ParticleForGPU> sbParticleResource_;					// パーティクルリソース(sBuffer)
		uint32_t instanceCount;													// インスタンス数
		Sprite* sprite;															// スプライト
		ModelMesh* mesh;
		float kGravitationalAcceleration = 9.8f;								// 重力
		bool isAlpha_ = true;
		bool isGravity_ = false;
		bool isAcceleration_ = false;
	};





	class DirectXCommon;
	namespace ParticleFanction {

		// ビルボード
		Matrix4x4 Billboard(ParticleGroup& group, std::list<Particle>::iterator& particleIterator, Camera* camera);

		// パーティクル影響
		void Effect(ParticleGroup& group, std::list<Particle>::iterator& particleIterator, float deltaTime);

		// パーティクル影響
		void Effect(ParticleGroup2d& group, std::list<Particle>::iterator& particleIterator, float deltaTime);



		// パーティクルデータをGPUに送る
		void WorldDataForGPU(ParticleGroup& group, std::list<Particle>::iterator& particleIterator, Camera* camera);

		// パーティクルデータをGPUに送る
		void WorldDataForGPU(ParticleGroup2d& group, std::list<Particle>::iterator& particleIterator, Camera* camera);


		// マテリアルのエフェクト処理
		void MaterialEffect(ParticleGroup& group);

		// パーティクル3dグループ生成
		void Create(ParticleGroup& particleGrou, const std::string name, const std::string textureFilePath, uint32_t kNumMaxInstance,
			DirectXCommon* dxCommon, ModelMesh* mesh, EmitData::RasterizerType rasteType, EmitData::BlendType blendType);

		// パーティクル2dグループ生成(スプライト)
		void Create(ParticleGroup2d& particleGrou, const std::string name, const std::string textureFilePath, uint32_t kNumMaxInstance,
			DirectXCommon* dxCommon, Sprite* sprite);

		// パーティクル2dグループ生成(モデル)
		void Create(ParticleGroup2d& particleGrou, const std::string name, const std::string textureFilePath, uint32_t kNumMaxInstance,
			DirectXCommon* dxCommon, ModelMesh* sprite);


	};
}