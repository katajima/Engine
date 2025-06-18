#include "ParticleData.h"

Matrix4x4 ParticleFanction::Billboard(ParticleGroup& group, std::list<Particle>::iterator& particleIterator, Camera* camera)
{
	Matrix4x4 worldMatrix;

	Matrix4x4 cameraWorldMatrix = camera->GetWorldMatrix();

	Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix = Multiply(backToFrontMatrix, cameraWorldMatrix);
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;
	Matrix4x4 billboardMatrixY = MakeBillboardMatrixY(billboardMatrix);

	// ビルボードするか
	if (group.isFlag.usebillboard) {
		Matrix4x4 billb;
		if (group.isFlag.billboardRotZ) {
			if (group.isFlag.usebillboardY) {
				billb = billboardMatrixY;
			}
			else {
				billb = billboardMatrix;
			}
			// Z軸回転だけの行列（パーティクル独自の回転）
			Matrix4x4 rotateZMatrix = MakeRotateZMatrix(particleIterator->transform.rotate.z);
			worldMatrix = Multiply(
				Multiply(
					Multiply(MakeScaleMatrix(particleIterator->transform.scale), rotateZMatrix),
					billb),
				MakeTranslateMatrix(particleIterator->transform.translate)
			);
		}
		else {
			if (group.isFlag.usebillboardY) {
				billb = billboardMatrixY;
			}
			else {
				billb = billboardMatrix;
			}

			worldMatrix = Multiply(Multiply(MakeScaleMatrix(particleIterator->transform.scale), billb),
				MakeTranslateMatrix(particleIterator->transform.translate));
		}
	}
	else {
		worldMatrix = MakeAffineMatrix(particleIterator->transform.scale, particleIterator->transform.rotate, particleIterator->transform.translate);
	}

	return worldMatrix;
}

void ParticleFanction::Effect(ParticleGroup& group, std::list<Particle>::iterator& particleIterator, float deltaTime)
{
	// 重力の影響を受けるか
	if (group.isFlag.isGravity) {
		particleIterator->velocity.y -= group.kGravitationalAcceleration * deltaTime;
	}
	// 加速度の影響
	if (group.isFlag.isAcceleration) {
		particleIterator->velocity += particleIterator->acceleration * deltaTime;
	}

	// 生存時間によって大きさが変化するか
	if (group.isFlag.isLifeTimeScale_) {
		float t = particleIterator->currentTime / particleIterator->lifeTime;
		float scaling = (group.topBottom == ParticleData::TopBottom::kBottom) ? (1.0f - t) : t;
		particleIterator->transform.scale = Lerp({}, particleIterator->strtTransform.scale, scaling);
	}

	// 回転させるか
	if (group.isFlag.isRotateVelocity) {
		particleIterator->transform.rotate += particleIterator->rotateVelocity;
	}

	// バウンドさせるか
	if (group.isFlag.isBounce && particleIterator->transform.translate.y < 0) {
		particleIterator->transform.translate.y = 0;
		particleIterator->velocity = Reflect(particleIterator->velocity, { 0,1,0 }, 0.85f);
	}

	// 時間
	particleIterator->currentTime += deltaTime;

	float t = particleIterator->currentTime / particleIterator->lifeTime;

	if (particleIterator->isEvent) {
	}

	// 透過するか
	if (group.isFlag.isAlpha) {
		float alpha = 1.0f - (particleIterator->currentTime / particleIterator->lifeTime);
		group.sbParticleResource_.Data()[group.instanceCount].color = particleIterator->color;
		group.sbParticleResource_.Data()[group.instanceCount].color.w = alpha;
	}
	else {
		group.sbParticleResource_.Data()[group.instanceCount].color = particleIterator->color;
	}

	// 移置加算
	particleIterator->transform.translate += particleIterator->velocity * deltaTime;
}

void ParticleFanction::WorldDataForGPU(ParticleGroup& group, std::list<Particle>::iterator& particleIterator, Camera* camera)
{
	Matrix4x4 projectionMatrix = camera->GetProjectionMatrix();
	Matrix4x4 viewMatrix = camera->GetViewMatrix();

	// ビルボード関係の関数
	Matrix4x4 worldMatrix = ParticleFanction::Billboard(group, particleIterator, camera);

	// ビュープロジェクション
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	// データ送る
	group.sbParticleResource_.Data()[group.instanceCount].World = worldMatrix;
	group.sbParticleResource_.Data()[group.instanceCount].WVP = worldViewProjectionMatrix;

}

void ParticleFanction::MaterialEffect(ParticleGroup& group)
{
	group.material->transform.translate += group.uvTransformVeloctiy_.translate;
	group.material->transform.rotate += group.uvTransformVeloctiy_.rotate;
	group.material->transform.scale += group.uvTransformVeloctiy_.scale;
	group.material->GPUData();
}

void ParticleFanction::Create(ParticleGroup& particleGroup, const std::string name, const std::string textureFilePath, uint32_t kNumMaxInstance,
	DirectXCommon* dxCommon, ModelMesh* mesh, ParticleData::RasterizerType rasteType, ParticleData::BlendType blendType)
{
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
	// メッシュ
	particleGroup.mesh = mesh;
	particleGroup.mesh->UpdateVertexBuffer();
	particleGroup.mesh->UpdateIndexBuffer();

	// マテリアル
	particleGroup.material = std::make_unique<Material>();
	particleGroup.material->Initialize(dxCommon);
	particleGroup.material->tex_.diffuseFilePath = textureFilePath;
	particleGroup.material->LoadTex();
	particleGroup.material->enableLighting_ = false;
	particleGroup.material->useEnvironment_ = false;


	// パーティクルリソース生成
	particleGroup.sbParticleResource_.CreateBuffer(dxCommon, kNumMaxInstance);

	// 初期化
	for (uint32_t i = 0; i < kNumMaxInstance; ++i) {
		particleGroup.sbParticleResource_.Data()[i].World = MakeIdentity4x4();
		particleGroup.sbParticleResource_.Data()[i].WVP = MakeIdentity4x4();
		particleGroup.sbParticleResource_.Data()[i].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// ブレンド
	particleGroup.blendType = blendType;

	// ラスタライザ
	particleGroup.rasteType = rasteType;
}
