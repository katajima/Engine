#include "ParticleManager.h"
#include "DirectXGame/engine/base/TextureManager.h"
#include "imgui.h"


#include"DirectXGame/engine/MyGame/MyGame.h"

ParticleManager* ParticleManager::instance = nullptr;

ParticleManager* ParticleManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new ParticleManager;
	}
	return instance;
}

void ParticleManager::Initialize(DirectXCommon* dxCommon)
{

	this->camera_ = Object3dCommon::GetInstance()->GetDefaltCamera();

	dxCommon_ = dxCommon;

	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon_);

	CreateGraphicsPipeline();
}

void ParticleManager::Finalize()
{
	delete instance;
	instance = nullptr;
}

void ParticleManager::DrawCommonSetting(RasterizerType rasteType, BlendType blendType)
{
	switch (blendType)
	{
	case ParticleManager::BlendType::MODE_ADD:
		if (rasteType == RasterizerType::MODE_SOLID_BACK) {
			dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[0].Get());
		}
		else {
			dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[1].Get());
		}
		break;
	case ParticleManager::BlendType::MODE_SUBTRACT:
		if (rasteType == RasterizerType::MODE_SOLID_BACK) {
			dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[2].Get());
		}
		else {
			dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[3].Get());
		}
		break;
	case ParticleManager::BlendType::MODE_MUlLIPLY:
		if (rasteType == RasterizerType::MODE_SOLID_BACK) {
			dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[4].Get());
		}
		else {
			dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[5].Get());
		}
		break;
	default:
		break;
	}




	//// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());


	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ParticleManager::Update()
{


	// カメラ設定
	Transform cameraTransform{
		{1.0f, 1.0f, 1.0f},
		{std::numbers::pi_v<float> / 3.0f, std::numbers::pi_v<float>, 0.0f},
		{0.0f, 23.0f, 10.0f}
	};




	// 全パーティクルグループに対する処理
	for (auto& pair : particleGroups) // 各パーティクルグループに対して
	{
		ParticleGroup& group = pair.second;
		group.instanceCount = 0; // 描画すべきインスタンスのカウント

		Matrix4x4 projectionMatrix = camera_->GetProjectionMatrix();
		Matrix4x4 viewMatrix = camera_->GetViewMatrix();


		Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
		Matrix4x4 billboardMatrix = Multiply(backToFrontMatrix, camera_->GetWorldMatrix());
		billboardMatrix.m[3][0] = 0.0f; // 平行移動成分は不要
		billboardMatrix.m[3][1] = 0.0f;
		billboardMatrix.m[3][2] = 0.0f;


		group.emiter.worldtransform.Update();

		for (auto particleIterator = group.particle.begin(); particleIterator != group.particle.end(); )
		{
			// パーティクルの寿命をチェック
			if (particleIterator->lifeTime <= particleIterator->currentTime) {
				particleIterator = group.particle.erase(particleIterator);
				continue;
			}

			if (group.instanceCount < kNumMaxInstance) {


				if (group.isGravity) { // 重力
					particleIterator->velocity.y -= kGravitationalAcceleration * MyGame::GameTime();
				};

				if (group.isLifeTimeScale_) { // スケール
					// アルファ値を計算
					float alpha = 1.0f - (particleIterator->currentTime / particleIterator->lifeTime);

					particleIterator->transform.scale = Lerp({}, particleIterator->strtTransform.scale, alpha);
				}

				if (group.isRotateVelocity) {// 回転
					particleIterator->transform.rotate += particleIterator->rotateVelocity;
				}
				// 移動処理 (速度を位置に加算)
				particleIterator->transform.translate += particleIterator->velocity * MyGame::GameTime();

				// 経過時間を加算
				particleIterator->currentTime += MyGame::GameTime();

				if (group.isBounce) {
					if (particleIterator->transform.translate.y < 0) {
						particleIterator->transform.translate.y = 0;
						particleIterator->velocity = Reflect(particleIterator->velocity, { 0,1,0 }, 0.85f);
					}
				}


				// ワールド行列を計算
				Matrix4x4 worldMatrix;
				if (group.usebillboard) {
					Matrix4x4 mat = group.emiter.worldtransform.worldMat_;

					worldMatrix = Multiply(Multiply(MakeScaleMatrix((*particleIterator).transform.scale), billboardMatrix), MakeTranslateMatrix((*particleIterator).transform.translate));

				}
				else {
					worldMatrix = MakeAffineMatrix(particleIterator->transform.scale, particleIterator->transform.rotate, particleIterator->transform.translate);

				}

				// ワールドビュー射影行列を合成
				Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

				// インスタンシング用データに情報を書き込み
				group.instanceData[group.instanceCount].World = worldMatrix;
				group.instanceData[group.instanceCount].WVP = worldViewProjectionMatrix;
				group.instanceData[group.instanceCount].color = particleIterator->color;

				if (group.isAlpha) {

					// アルファ値を計算
					float alpha = 1.0f - (particleIterator->currentTime / particleIterator->lifeTime);

					group.instanceData[group.instanceCount].color.w = alpha;

				}

				// インスタンス数をカウント
				++group.instanceCount;
			}

			++particleIterator;
		}
	}
	//}
}

void ParticleManager::LimitMaxMin()
{
	for (auto& pair : particleGroups) // 各パーティクルグループに対して
	{
		ParticleGroup& group = pair.second;

		//	範囲 
		group.emiter.renge.min.x = (std::min)(group.emiter.renge.min.x, group.emiter.renge.max.x);
		group.emiter.renge.max.x = (std::max)(group.emiter.renge.min.x, group.emiter.renge.max.x);
		group.emiter.renge.min.y = (std::min)(group.emiter.renge.min.y, group.emiter.renge.max.y);
		group.emiter.renge.max.y = (std::max)(group.emiter.renge.min.y, group.emiter.renge.max.y);
		group.emiter.renge.min.z = (std::min)(group.emiter.renge.min.z, group.emiter.renge.max.z);
		group.emiter.renge.max.z = (std::max)(group.emiter.renge.min.z, group.emiter.renge.max.z);

		// 回転
		group.emiter.rotate.min.x = (std::min)(group.emiter.rotate.min.x, group.emiter.rotate.max.x);
		group.emiter.rotate.max.x = (std::max)(group.emiter.rotate.min.x, group.emiter.rotate.max.x);
		group.emiter.rotate.min.y = (std::min)(group.emiter.rotate.min.y, group.emiter.rotate.max.y);
		group.emiter.rotate.max.y = (std::max)(group.emiter.rotate.min.y, group.emiter.rotate.max.y);
		group.emiter.rotate.min.z = (std::min)(group.emiter.rotate.min.z, group.emiter.rotate.max.z);
		group.emiter.rotate.max.z = (std::max)(group.emiter.rotate.min.z, group.emiter.rotate.max.z);

		// 
		group.emiter.velocity.min.x = (std::min)(group.emiter.velocity.min.x, group.emiter.velocity.max.x);
		group.emiter.velocity.max.x = (std::max)(group.emiter.velocity.min.x, group.emiter.velocity.max.x);
		group.emiter.velocity.min.y = (std::min)(group.emiter.velocity.min.y, group.emiter.velocity.max.y);
		group.emiter.velocity.max.y = (std::max)(group.emiter.velocity.min.y, group.emiter.velocity.max.y);
		group.emiter.velocity.min.z = (std::min)(group.emiter.velocity.min.z, group.emiter.velocity.max.z);
		group.emiter.velocity.max.z = (std::max)(group.emiter.velocity.min.z, group.emiter.velocity.max.z);


		//	サイズ 
		group.emiter.size.min.x = (std::min)(group.emiter.size.min.x, group.emiter.size.max.x);
		group.emiter.size.max.x = (std::max)(group.emiter.size.min.x, group.emiter.size.max.x);
		group.emiter.size.min.y = (std::min)(group.emiter.size.min.y, group.emiter.size.max.y);
		group.emiter.size.max.y = (std::max)(group.emiter.size.min.y, group.emiter.size.max.y);
		group.emiter.size.min.z = (std::min)(group.emiter.size.min.z, group.emiter.size.max.z);
		group.emiter.size.max.z = (std::max)(group.emiter.size.min.z, group.emiter.size.max.z);

		if (group.emiter.size.min.x < 0) {
			group.emiter.size.min.x = 0;
			group.emiter.size.max.x = 0;
		}
		if (group.emiter.size.min.y < 0) {
			group.emiter.size.min.y = 0;
			group.emiter.size.max.y = 0;
		}
		if (group.emiter.size.min.z < 0) {
			group.emiter.size.min.z = 0;
			group.emiter.size.max.z = 0;
		}


		//　色
		group.emiter.color.min.x = (std::min)(group.emiter.color.min.x, group.emiter.color.max.x);
		group.emiter.color.max.x = (std::max)(group.emiter.color.min.x, group.emiter.color.max.x);
		group.emiter.color.min.y = (std::min)(group.emiter.color.min.y, group.emiter.color.max.y);
		group.emiter.color.max.y = (std::max)(group.emiter.color.min.y, group.emiter.color.max.y);
		group.emiter.color.min.z = (std::min)(group.emiter.color.min.z, group.emiter.color.max.z);
		group.emiter.color.max.z = (std::max)(group.emiter.color.min.z, group.emiter.color.max.z);
		group.emiter.color.min.w = (std::min)(group.emiter.color.min.w, group.emiter.color.max.w);
		group.emiter.color.max.w = (std::max)(group.emiter.color.min.w, group.emiter.color.max.w);


		group.emiter.lifeTime.min = (std::min)(group.emiter.lifeTime.min, group.emiter.lifeTime.max);
		group.emiter.lifeTime.max = (std::max)(group.emiter.lifeTime.min, group.emiter.lifeTime.max);


		if (group.emiter.lifeTime.min < 0) {
			group.emiter.lifeTime.min = 0;
		}
		if (group.emiter.lifeTime.max < 0) {
			group.emiter.lifeTime.max = 0;
		}
	}


}

void ParticleManager::Draw()
{


	auto commandList = dxCommon_->GetCommandList();




	for (auto& pair : particleGroups) {
		ParticleGroup& group = pair.second;
		if (group.instanceCount == 0) {
			continue;
		}


		ParticleManager::GetInstance()->DrawCommonSetting(group.rasteType, group.blendType);


		group.material->GetCommandListTexture(2);

		//commandList->SetGraphicsRootConstantBufferView(0, group.resource->GetGPUVirtualAddress());

		// インスタンシングデータのSRVのDescriptorTableを設定
		commandList->SetGraphicsRootDescriptorTable(1, group.instancingSrvHandleGPU);


		group.mesh->GetCommandList();

		// インスタンシング描画
		uint32_t instanceCount = (std::min)(group.instanceCount, kNumMaxInstance);
		commandList->DrawIndexedInstanced(static_cast<UINT>(group.mesh->indices.size()), instanceCount, 0, 0, 0);
	}
}

void ParticleManager::Emit(const std::string name, EmitType type, SpawnType spawnType)
{
	// パーティクルグループが登録済みであることを確認
	assert(particleGroups.contains(name) && "Error: Particle group with this name is not registered.");


	RandParticle(name, spawnType);
}

void ParticleManager::CreateParticleGroup(const std::string name, const std::string textureFilePath, Model* model, RasterizerType rasteType, BlendType blendType)
{
	// ランダムエンジンの初期化
	std::random_device seedGenerator;
	randomEngine_.seed(seedGenerator()); // randomEngine_ にシードを設定

	if (particleGroups.contains(name)) {
		return;
	}

	ParticleGroup& particleGroup = particleGroups[name];
	particleGroup.emiter.renge.max = Vector3{ 1.0f,1.0f,1.0f };
	particleGroup.emiter.renge.min = Vector3{ -1.0f,-1.0f,-1.0f };
	particleGroup.emiter.color.max = Vector4{ 1,1,1,1 };
	particleGroup.emiter.color.min = Vector4{ 0,0,0,0 };
	particleGroup.emiter.rotate.min = Vector3{ 0,0,0 };
	particleGroup.emiter.rotate.max = Vector3{ 0,0,0 };
	particleGroup.emiter.size.min = Vector3{ 1.0f,1.0f,1.0f };
	particleGroup.emiter.size.max = Vector3{ 1.0f,1.0f,1.0f };
	particleGroup.emiter.lifeTime.min = 1.0f;
	particleGroup.emiter.lifeTime.max = 3.0f;
	particleGroup.emiter.velocity.min = Vector3{ -1.0f,-1.0f,-1.0f };
	particleGroup.emiter.velocity.max = Vector3{ 1.0f,1.0f,1.0f };
	particleGroup.emiter.count = 10;
	particleGroup.emiter.rotateVelocity.min = Vector3{ 0,0,0 };
	particleGroup.emiter.rotateVelocity.max = Vector3{ 0,0,0 };

	particleGroup.emiter.worldtransform.Initialize();

	// 名前
	particleGroup.name = name;
	// モデル
	//particleGroup.model = model;
	particleGroup.mesh = model->modelData.mesh[0].get();
	particleGroup.mesh->UpdateVertexBuffer();
	particleGroup.mesh->UpdateIndexBuffer();

	// マテリアル
	particleGroup.material = std::make_unique<Material>();
	particleGroup.material->Initialize(dxCommon_);
	particleGroup.material->tex_.diffuseFilePath = textureFilePath;
	particleGroup.material->LoadTex();


	// GPUリソースの作成
	particleGroup.resource = dxCommon_->CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance);
	// マッピング
	particleGroup.resource->Map(0, nullptr, reinterpret_cast<void**>(&particleGroup.instanceData));
	// 初期化
	for (uint32_t i = 0; i < kNumMaxInstance; ++i) {
		particleGroup.instanceData[i].World = MakeIdentity4x4();
		particleGroup.instanceData[i].WVP = MakeIdentity4x4();
		particleGroup.instanceData[i].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// SRVの設定
	// SRVインデックスの取得と設定
	particleGroup.srvIndex = SrvManager::GetInstance()->Allocate();
	particleGroup.instancingSrvHandleCPU = SrvManager::GetInstance()->GetCPUDescriptorHandle(particleGroup.srvIndex);
	particleGroup.instancingSrvHandleGPU = SrvManager::GetInstance()->GetGPUDescriptorHandle(particleGroup.srvIndex);
	SrvManager::GetInstance()->CreateSRVforStructuredBuffer(particleGroup.srvIndex, particleGroup.resource.Get(), kNumMaxInstance, sizeof(ParticleForGPU));

	// ブレンド
	particleGroup.blendType = blendType;

	// ラスタライザ
	particleGroup.rasteType = rasteType;

}

void ParticleManager::CreateParticleGroup(const std::string name, const std::string textureFilePath, Primitive* primitive, RasterizerType rasteType, BlendType blendType)
{
	// ランダムエンジンの初期化
	std::random_device seedGenerator;
	randomEngine_.seed(seedGenerator()); // randomEngine_ にシードを設定

	if (particleGroups.contains(name)) {
		return;
	}

	ParticleGroup& particleGroup = particleGroups[name];
	particleGroup.emiter.renge.max = Vector3{ 1.0f,1.0f,1.0f };
	particleGroup.emiter.renge.min = Vector3{ -1.0f,-1.0f,-1.0f };
	particleGroup.emiter.color.max = Vector4{ 1,1,1,1 };
	particleGroup.emiter.color.min = Vector4{ 0,0,0,0 };
	particleGroup.emiter.rotate.min = Vector3{ 0,0,0 };
	particleGroup.emiter.rotate.max = Vector3{ 0,0,0 };
	particleGroup.emiter.size.min = Vector3{ 1.0f,1.0f,1.0f };
	particleGroup.emiter.size.max = Vector3{ 1.0f,1.0f,1.0f };
	particleGroup.emiter.lifeTime.min = 1.0f;
	particleGroup.emiter.lifeTime.max = 3.0f;
	particleGroup.emiter.velocity.min = Vector3{ -1.0f,-1.0f,-1.0f };
	particleGroup.emiter.velocity.max = Vector3{ 1.0f,1.0f,1.0f };
	particleGroup.emiter.count = 10;
	particleGroup.emiter.rotateVelocity.min = Vector3{ 0,0,0 };
	particleGroup.emiter.rotateVelocity.max = Vector3{ 0,0,0 };

	particleGroup.emiter.worldtransform.Initialize();

	// 名前
	particleGroup.name = name;
	// モデル
	//particleGroup.model = model;
	particleGroup.mesh = primitive->GetMesh();
	particleGroup.mesh->UpdateVertexBuffer();
	particleGroup.mesh->UpdateIndexBuffer();

	// マテリアル
	particleGroup.material = std::make_unique<Material>();
	particleGroup.material->Initialize(dxCommon_);
	particleGroup.material->tex_.diffuseFilePath = textureFilePath;
	particleGroup.material->LoadTex();


	// GPUリソースの作成
	particleGroup.resource = dxCommon_->CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance);
	// マッピング
	particleGroup.resource->Map(0, nullptr, reinterpret_cast<void**>(&particleGroup.instanceData));
	// 初期化
	for (uint32_t i = 0; i < kNumMaxInstance; ++i) {
		particleGroup.instanceData[i].World = MakeIdentity4x4();
		particleGroup.instanceData[i].WVP = MakeIdentity4x4();
		particleGroup.instanceData[i].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// SRVの設定
	// SRVインデックスの取得と設定
	particleGroup.srvIndex = SrvManager::GetInstance()->Allocate();
	particleGroup.instancingSrvHandleCPU = SrvManager::GetInstance()->GetCPUDescriptorHandle(particleGroup.srvIndex);
	particleGroup.instancingSrvHandleGPU = SrvManager::GetInstance()->GetGPUDescriptorHandle(particleGroup.srvIndex);
	SrvManager::GetInstance()->CreateSRVforStructuredBuffer(particleGroup.srvIndex, particleGroup.resource.Get(), kNumMaxInstance, sizeof(ParticleForGPU));



}








void ParticleManager::RandParticle(const std::string name, SpawnType spawnType)
{


	if (SpawnType::kAABB == spawnType) {
		AABBEmit(particleGroups[name]);
	}
	else if (SpawnType::kSegmentLine == spawnType) {
		LineEmit(particleGroups[name]);
	}
	else if (SpawnType::kSpline == spawnType) {
		SplineEmit(particleGroups[name]);
	}
	else if (SpawnType::kCornerLine == spawnType) {
		CornerLineEmit(particleGroups[name]);
	}

}

#pragma region EmitType



void ParticleManager::AABBEmit(ParticleGroup& particleGroup)
{

	// 出る位置
	std::uniform_real_distribution<float> distributionX(particleGroup.emiter.renge.min.x, particleGroup.emiter.renge.max.x);
	std::uniform_real_distribution<float> distributionY(particleGroup.emiter.renge.min.y, particleGroup.emiter.renge.max.y);
	std::uniform_real_distribution<float> distributionZ(particleGroup.emiter.renge.min.z, particleGroup.emiter.renge.max.z);

	// 方向
	std::uniform_real_distribution<float> distributionVeloX(particleGroup.emiter.velocity.min.x, particleGroup.emiter.velocity.max.x);
	std::uniform_real_distribution<float> distributionVeloY(particleGroup.emiter.velocity.min.y, particleGroup.emiter.velocity.max.y);
	std::uniform_real_distribution<float> distributionVeloZ(particleGroup.emiter.velocity.min.z, particleGroup.emiter.velocity.max.z);

	// 色RGBA
	std::uniform_real_distribution<float> distColorR(particleGroup.emiter.color.min.x, particleGroup.emiter.color.max.x);
	std::uniform_real_distribution<float> distColorG(particleGroup.emiter.color.min.y, particleGroup.emiter.color.max.y);
	std::uniform_real_distribution<float> distColorB(particleGroup.emiter.color.min.z, particleGroup.emiter.color.max.z);
	std::uniform_real_distribution<float> distColorA(particleGroup.emiter.color.min.w, particleGroup.emiter.color.max.w);

	// 時間
	std::uniform_real_distribution<float> distTime(particleGroup.emiter.lifeTime.min, particleGroup.emiter.lifeTime.max);

	// 回転
	std::uniform_real_distribution<float> distributionRotateX(particleGroup.emiter.rotate.min.x, particleGroup.emiter.rotate.max.x);
	std::uniform_real_distribution<float> distributionRotateY(particleGroup.emiter.rotate.min.y, particleGroup.emiter.rotate.max.y);
	std::uniform_real_distribution<float> distributionRotateZ(particleGroup.emiter.rotate.min.z, particleGroup.emiter.rotate.max.z);

	// スケール
	std::uniform_real_distribution<float> distributionSizeX(particleGroup.emiter.size.min.x, particleGroup.emiter.size.max.x);
	std::uniform_real_distribution<float> distributionSizeY(particleGroup.emiter.size.min.y, particleGroup.emiter.size.max.y);
	std::uniform_real_distribution<float> distributionSizeZ(particleGroup.emiter.size.min.z, particleGroup.emiter.size.max.z);

	// 回転速度
	std::uniform_real_distribution<float> distributionRotateVelocityX(particleGroup.emiter.rotateVelocity.min.x, particleGroup.emiter.rotateVelocity.max.x);
	std::uniform_real_distribution<float> distributionRotateVelocityY(particleGroup.emiter.rotateVelocity.min.y, particleGroup.emiter.rotateVelocity.max.y);
	std::uniform_real_distribution<float> distributionRotateVelocityZ(particleGroup.emiter.rotateVelocity.min.z, particleGroup.emiter.rotateVelocity.max.z);


	// パーティクル
	for (int t = 0; t < particleGroup.emiter.count; ++t) {
		Particle newParticle;
		// パーティクルの初期化 (必要に応じて詳細を設定)
		newParticle.transform.scale =
		{
			distributionSizeX(randomEngine_),
			distributionSizeY(randomEngine_),
			distributionSizeZ(randomEngine_)
		};
		newParticle.transform.rotate =
		{
			distributionRotateX(randomEngine_),
			distributionRotateY(randomEngine_),
			distributionRotateZ(randomEngine_)
		};
		newParticle.transform.translate =
		{
			particleGroup.emiter.worldtransform.worldMat_.GetWorldPosition().x + distributionX(randomEngine_),
			particleGroup.emiter.worldtransform.worldMat_.GetWorldPosition().y + distributionY(randomEngine_),
			particleGroup.emiter.worldtransform.worldMat_.GetWorldPosition().z + distributionZ(randomEngine_)
		};
		newParticle.color =
		{
			distColorR(randomEngine_),
			distColorG(randomEngine_),
			distColorB(randomEngine_),
			1.0f
		};
		newParticle.rotateVelocity = {
			distributionRotateVelocityX(randomEngine_),
			distributionRotateVelocityY(randomEngine_),
			distributionRotateVelocityZ(randomEngine_)
		};

		newParticle.lifeTime = distTime(randomEngine_);
		newParticle.currentTime = 0;

		// 初期値
		newParticle.strtTransform = newParticle.transform;

		//速度
		newParticle.velocity =
		{
			distributionVeloX(randomEngine_),
			distributionVeloY(randomEngine_),
			distributionVeloZ(randomEngine_)
		};
		// パーティクルをグループに追加
		particleGroup.particle.push_back(newParticle);
	}
}

void ParticleManager::LineEmit(ParticleGroup& particleGroup)
{
	// 発生セグメントの設定
	Segment segment_{ Vector3{},Vector3{} };
	segment_.origin = particleGroup.emiter.renge.min;  // 始点
	segment_.end = particleGroup.emiter.renge.max;    // 終点



	// 方向
	std::uniform_real_distribution<float> distributionVeloX(particleGroup.emiter.velocity.min.x, particleGroup.emiter.velocity.max.x);
	std::uniform_real_distribution<float> distributionVeloY(particleGroup.emiter.velocity.min.y, particleGroup.emiter.velocity.max.y);
	std::uniform_real_distribution<float> distributionVeloZ(particleGroup.emiter.velocity.min.z, particleGroup.emiter.velocity.max.z);

	// 色RGBA
	std::uniform_real_distribution<float> distColorR(particleGroup.emiter.color.min.x, particleGroup.emiter.color.max.x);
	std::uniform_real_distribution<float> distColorG(particleGroup.emiter.color.min.y, particleGroup.emiter.color.max.y);
	std::uniform_real_distribution<float> distColorB(particleGroup.emiter.color.min.z, particleGroup.emiter.color.max.z);
	std::uniform_real_distribution<float> distColorA(particleGroup.emiter.color.min.w, particleGroup.emiter.color.max.w);

	// 時間
	std::uniform_real_distribution<float> distTime(particleGroup.emiter.lifeTime.min, particleGroup.emiter.lifeTime.max);

	// 回転
	std::uniform_real_distribution<float> distributionRotateX(particleGroup.emiter.rotate.min.x, particleGroup.emiter.rotate.max.x);
	std::uniform_real_distribution<float> distributionRotateY(particleGroup.emiter.rotate.min.y, particleGroup.emiter.rotate.max.y);
	std::uniform_real_distribution<float> distributionRotateZ(particleGroup.emiter.rotate.min.z, particleGroup.emiter.rotate.max.z);

	// スケール
	std::uniform_real_distribution<float> distributionSizeX(particleGroup.emiter.size.min.x, particleGroup.emiter.size.max.x);
	std::uniform_real_distribution<float> distributionSizeY(particleGroup.emiter.size.min.y, particleGroup.emiter.size.max.y);
	std::uniform_real_distribution<float> distributionSizeZ(particleGroup.emiter.size.min.z, particleGroup.emiter.size.max.z);

	// 回転速度
	std::uniform_real_distribution<float> distributionRotateVelocityX(particleGroup.emiter.rotateVelocity.min.x, particleGroup.emiter.rotateVelocity.max.x);
	std::uniform_real_distribution<float> distributionRotateVelocityY(particleGroup.emiter.rotateVelocity.min.y, particleGroup.emiter.rotateVelocity.max.y);
	std::uniform_real_distribution<float> distributionRotateVelocityZ(particleGroup.emiter.rotateVelocity.min.z, particleGroup.emiter.rotateVelocity.max.z);


	// 0.0 〜 1.0 のランダム値 (線分上の位置を決める)
	std::uniform_real_distribution<float> distributionT(0.0f, 1.0f);
	float t_2 = 0;
	Vector3 position{};

	// パーティクル
	for (int t = 0; t < particleGroup.emiter.count; ++t) {
		Particle newParticle;
		// パーティクルの初期化 (必要に応じて詳細を設定)
		newParticle.transform.scale =
		{
			distributionSizeX(randomEngine_),
			distributionSizeY(randomEngine_),
			distributionSizeZ(randomEngine_)
		};
		newParticle.transform.rotate =
		{
			distributionRotateX(randomEngine_),
			distributionRotateY(randomEngine_),
			distributionRotateZ(randomEngine_)
		};

		t_2 = distributionT(randomEngine_);
		position.x = (1.0f - t_2) * segment_.origin.x + t_2 * segment_.end.x;
		position.y = (1.0f - t_2) * segment_.origin.y + t_2 * segment_.end.y;
		position.z = (1.0f - t_2) * segment_.origin.z + t_2 * segment_.end.z;
		newParticle.transform.translate = particleGroup.emiter.worldtransform.worldMat_.GetWorldPosition() + position;

		newParticle.color =
		{
			distColorR(randomEngine_),
			distColorG(randomEngine_),
			distColorB(randomEngine_),
			1.0f
		};
		newParticle.rotateVelocity = {
			distributionRotateVelocityX(randomEngine_),
			distributionRotateVelocityY(randomEngine_),
			distributionRotateVelocityZ(randomEngine_)
		};

		newParticle.lifeTime = distTime(randomEngine_);
		newParticle.currentTime = 0;

		// 初期値
		newParticle.strtTransform = newParticle.transform;

		//速度
		newParticle.velocity =
		{
			distributionVeloX(randomEngine_),
			distributionVeloY(randomEngine_),
			distributionVeloZ(randomEngine_)
		};
		// パーティクルをグループに追加
		particleGroup.particle.push_back(newParticle);
	}
}

void ParticleManager::CornerLineEmit(ParticleGroup& particleGroup)
{
	

	// 頂点を計算
	std::vector<Vector3> vertices;
	float angleStep = DirectX::XM_2PI / particleGroup.emiter.corner.segment; // 360° を segment 分割

	// 回転行列を作成
	Matrix4x4 rotationMatrix = MakeRotateXYZ(particleGroup.emiter.worldtransform.rotate_);
	for (int i = 0; i < particleGroup.emiter.corner.segment; ++i)
	{
		float angle = i * angleStep; // 各頂点の角度
		Vector3 localVertex;
		localVertex.x = cos(angle) * particleGroup.emiter.corner.radius;
		localVertex.y = 0.0f;
		localVertex.z = sin(angle) * particleGroup.emiter.corner.radius;

		// 回転を適用
		Vector3 rotatedVertex = rotationMatrix.Transform(localVertex);

		// ワールド座標へ変換
		Vector3 worldVertex = rotatedVertex;
		vertices.push_back(worldVertex);
	}

	
	// 各辺の上にパーティクルを発生
	std::uniform_real_distribution<float> distributionT(0.0f, 1.0f);



	// 出る位置
	std::uniform_real_distribution<float> distributionX(particleGroup.emiter.renge.min.x, particleGroup.emiter.renge.max.x);
	std::uniform_real_distribution<float> distributionY(particleGroup.emiter.renge.min.y, particleGroup.emiter.renge.max.y);
	std::uniform_real_distribution<float> distributionZ(particleGroup.emiter.renge.min.z, particleGroup.emiter.renge.max.z);

	// 方向
	std::uniform_real_distribution<float> distributionVeloX(particleGroup.emiter.velocity.min.x, particleGroup.emiter.velocity.max.x);
	std::uniform_real_distribution<float> distributionVeloY(particleGroup.emiter.velocity.min.y, particleGroup.emiter.velocity.max.y);
	std::uniform_real_distribution<float> distributionVeloZ(particleGroup.emiter.velocity.min.z, particleGroup.emiter.velocity.max.z);

	// 色RGBA
	std::uniform_real_distribution<float> distColorR(particleGroup.emiter.color.min.x, particleGroup.emiter.color.max.x);
	std::uniform_real_distribution<float> distColorG(particleGroup.emiter.color.min.y, particleGroup.emiter.color.max.y);
	std::uniform_real_distribution<float> distColorB(particleGroup.emiter.color.min.z, particleGroup.emiter.color.max.z);
	std::uniform_real_distribution<float> distColorA(particleGroup.emiter.color.min.w, particleGroup.emiter.color.max.w);

	// 時間
	std::uniform_real_distribution<float> distTime(particleGroup.emiter.lifeTime.min, particleGroup.emiter.lifeTime.max);

	// 回転
	std::uniform_real_distribution<float> distributionRotateX(particleGroup.emiter.rotate.min.x, particleGroup.emiter.rotate.max.x);
	std::uniform_real_distribution<float> distributionRotateY(particleGroup.emiter.rotate.min.y, particleGroup.emiter.rotate.max.y);
	std::uniform_real_distribution<float> distributionRotateZ(particleGroup.emiter.rotate.min.z, particleGroup.emiter.rotate.max.z);

	// スケール
	std::uniform_real_distribution<float> distributionSizeX(particleGroup.emiter.size.min.x, particleGroup.emiter.size.max.x);
	std::uniform_real_distribution<float> distributionSizeY(particleGroup.emiter.size.min.y, particleGroup.emiter.size.max.y);
	std::uniform_real_distribution<float> distributionSizeZ(particleGroup.emiter.size.min.z, particleGroup.emiter.size.max.z);

	// 回転速度
	std::uniform_real_distribution<float> distributionRotateVelocityX(particleGroup.emiter.rotateVelocity.min.x, particleGroup.emiter.rotateVelocity.max.x);
	std::uniform_real_distribution<float> distributionRotateVelocityY(particleGroup.emiter.rotateVelocity.min.y, particleGroup.emiter.rotateVelocity.max.y);
	std::uniform_real_distribution<float> distributionRotateVelocityZ(particleGroup.emiter.rotateVelocity.min.z, particleGroup.emiter.rotateVelocity.max.z);


	// パーティクル
	for (int t = 0; t < particleGroup.emiter.count; ++t) {

		for (int i = 0; i < particleGroup.emiter.corner.segment; ++i)
		{
			Particle newParticle;

			// パーティクルの初期化 (必要に応じて詳細を設定)
			newParticle.transform.scale =
			{
				distributionSizeX(randomEngine_),
				distributionSizeY(randomEngine_),
				distributionSizeZ(randomEngine_)
			};
			newParticle.transform.rotate =
			{
				distributionRotateX(randomEngine_),
				distributionRotateY(randomEngine_),
				distributionRotateZ(randomEngine_)
			};
			newParticle.color =
			{
				distColorR(randomEngine_),
				distColorG(randomEngine_),
				distColorB(randomEngine_),
				1.0f
			};
			newParticle.rotateVelocity = {
				distributionRotateVelocityX(randomEngine_),
				distributionRotateVelocityY(randomEngine_),
				distributionRotateVelocityZ(randomEngine_)
			};

			newParticle.lifeTime = distTime(randomEngine_);
			newParticle.currentTime = 0;

			// 初期値
			newParticle.strtTransform = newParticle.transform;

			//速度
			newParticle.velocity =
			{
				distributionVeloX(randomEngine_),
				distributionVeloY(randomEngine_),
				distributionVeloZ(randomEngine_)
			};


			// 隣接する2点（N角形の1辺）
			Vector3 start = vertices[i];
			Vector3 end = vertices[(i + 1) % particleGroup.emiter.corner.segment]; // ループするように処理

			// 線分上のランダムな位置を求める
			float t = distributionT(randomEngine_);
			Vector3 position;
			position.x = (1.0f - t) * start.x + t * end.x;
			position.y = (1.0f - t) * start.y + t * end.y;
			position.z = (1.0f - t) * start.z + t * end.z;



			newParticle.transform.translate = position + particleGroup.emiter.worldtransform.worldMat_.GetWorldPosition();
			particleGroup.particle.push_back(newParticle);
		}
	}
}

void ParticleManager::SplineEmit(ParticleGroup& particleGroup)
{
	
	// 各辺の上にパーティクルを発生
	std::uniform_real_distribution<float> distributionT(0.0f, 1.0f);


	if (particleGroup.emiter.controlPoints.size() < 4) {
		return;
	}

	

	
	// 方向
	std::uniform_real_distribution<float> distributionVeloX(particleGroup.emiter.velocity.min.x, particleGroup.emiter.velocity.max.x);
	std::uniform_real_distribution<float> distributionVeloY(particleGroup.emiter.velocity.min.y, particleGroup.emiter.velocity.max.y);
	std::uniform_real_distribution<float> distributionVeloZ(particleGroup.emiter.velocity.min.z, particleGroup.emiter.velocity.max.z);

	// 色RGBA
	std::uniform_real_distribution<float> distColorR(particleGroup.emiter.color.min.x, particleGroup.emiter.color.max.x);
	std::uniform_real_distribution<float> distColorG(particleGroup.emiter.color.min.y, particleGroup.emiter.color.max.y);
	std::uniform_real_distribution<float> distColorB(particleGroup.emiter.color.min.z, particleGroup.emiter.color.max.z);
	std::uniform_real_distribution<float> distColorA(particleGroup.emiter.color.min.w, particleGroup.emiter.color.max.w);

	// 時間
	std::uniform_real_distribution<float> distTime(particleGroup.emiter.lifeTime.min, particleGroup.emiter.lifeTime.max);

	// 回転
	std::uniform_real_distribution<float> distributionRotateX(particleGroup.emiter.rotate.min.x, particleGroup.emiter.rotate.max.x);
	std::uniform_real_distribution<float> distributionRotateY(particleGroup.emiter.rotate.min.y, particleGroup.emiter.rotate.max.y);
	std::uniform_real_distribution<float> distributionRotateZ(particleGroup.emiter.rotate.min.z, particleGroup.emiter.rotate.max.z);

	// スケール
	std::uniform_real_distribution<float> distributionSizeX(particleGroup.emiter.size.min.x, particleGroup.emiter.size.max.x);
	std::uniform_real_distribution<float> distributionSizeY(particleGroup.emiter.size.min.y, particleGroup.emiter.size.max.y);
	std::uniform_real_distribution<float> distributionSizeZ(particleGroup.emiter.size.min.z, particleGroup.emiter.size.max.z);

	// 回転速度
	std::uniform_real_distribution<float> distributionRotateVelocityX(particleGroup.emiter.rotateVelocity.min.x, particleGroup.emiter.rotateVelocity.max.x);
	std::uniform_real_distribution<float> distributionRotateVelocityY(particleGroup.emiter.rotateVelocity.min.y, particleGroup.emiter.rotateVelocity.max.y);
	std::uniform_real_distribution<float> distributionRotateVelocityZ(particleGroup.emiter.rotateVelocity.min.z, particleGroup.emiter.rotateVelocity.max.z);


	// パーティクル
	for (int t = 0; t < particleGroup.emiter.count; ++t) {
		Particle newParticle;
		// パーティクルの初期化 (必要に応じて詳細を設定)
		newParticle.transform.scale =
		{
			distributionSizeX(randomEngine_),
			distributionSizeY(randomEngine_),
			distributionSizeZ(randomEngine_)
		};
		newParticle.transform.rotate =
		{
			distributionRotateX(randomEngine_),
			distributionRotateY(randomEngine_),
			distributionRotateZ(randomEngine_)
		};
		Vector3 pos = CatmullRom(particleGroup.emiter.controlPoints, distributionT(randomEngine_));

		newParticle.transform.translate = pos + particleGroup.emiter.worldtransform.worldMat_.GetWorldPosition();
		
		newParticle.color =
		{
			distColorR(randomEngine_),
			distColorG(randomEngine_),
			distColorB(randomEngine_),
			1.0f
		};
		newParticle.rotateVelocity = {
			distributionRotateVelocityX(randomEngine_),
			distributionRotateVelocityY(randomEngine_),
			distributionRotateVelocityZ(randomEngine_)
		};

		newParticle.lifeTime = distTime(randomEngine_);
		newParticle.currentTime = 0;

		// 初期値
		newParticle.strtTransform = newParticle.transform;

		//速度
		newParticle.velocity =
		{
			distributionVeloX(randomEngine_),
			distributionVeloY(randomEngine_),
			distributionVeloZ(randomEngine_)
		};
		// パーティクルをグループに追加
		particleGroup.particle.push_back(newParticle);
	}
}

void ParticleManager::SphereEmit(ParticleGroup& particleGroup)
{

}

#pragma endregion


#pragma region PSO


void ParticleManager::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[2] = {};
	psoManager_->SetDescriptorRenge(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // テクスチャ用
	psoManager_->SetDescriptorRenge(descriptorRange[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // インスタンシング用


	// RootParameter作成。複数指定できるのではい
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	// マテリアル (b4) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// インスタンシング(t1) をバーテックシェーダ使用する
	psoManager_->SetRootParameter(rootParameters[1], descriptorRange[1], D3D12_SHADER_VISIBILITY_VERTEX);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[2], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	psoManager_->SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);// バイリニアフィルタ


	// ルートシグネチャ作成
	psoManager_->SetRootSignature(rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));

}

void ParticleManager::CreateGraphicsPipeline()
{
	CreateRootSignature();

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 透明オブジェクトの場合はデプス書き込みを無効化
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// インプットレイアウト
	psoManager_->AddInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	psoManager_->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	psoManager_->AddInputElementDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);


	psoManager_->shderFile_.vertex.filePach = L"resources/shaders/Particle/Particle.VS.hlsl";
	psoManager_->shderFile_.pixel.filePach = L"resources/shaders/Particle/Particle.PS.hlsl";


	BlendAdd();
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[0], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[1], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	BlendSubtract();
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[2], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[3], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	BlendMuliply();
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[4], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[5], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
}

#pragma region Blend

void ParticleManager::BlendAdd()
{
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
}

void ParticleManager::BlendSubtract()
{
	// 減算ブレンドの設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// RGB の減算ブレンド
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;      // ソースの影響度
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE; // デスティネーションの影響度
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_SUBTRACT;     // 減算

	// アルファブレンドの設定（通常の設定）
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
}

void ParticleManager::BlendMuliply()
{
	
	// 加算ブレンドの設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	// 通常のカラー（RGB）加算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;      // ソースカラーをそのまま
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;     // デスティネーションカラーをそのまま
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;    // 加算（SrcColor + DestColor）
	// アルファブレンドの設定
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;


}

#pragma endregion // ブレンド

#pragma endregion // パイプライン関係

