#include "ParticleData.h"

#include <DirectXGame/engine/Base/WinApp/WinApp.h>
#include <DirectXGame/engine/Math/Random.h>
#include <DirectXGame/engine/MyGame/MyGame.h>

Matrix4x4 Engine::ParticleFunction::Billboard(ParticleGroup& group, std::list<Particle>::iterator& particleIterator, Camera* camera)
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

void Engine::ParticleFunction::Effect(ParticleGroup& group, std::list<Particle>::iterator& particleIterator, float deltaTime)
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
		float scaling = (group.topBottom == EmitData::TopBottom::kBottom) ? (1.0f - t) : t;
		particleIterator->transform.scale = Lerp({}, particleIterator->strtTransform.scale, scaling);
	}
	else if (group.isFlag.isScaling_) {// サイズ変化させるか(生存時間によって大きさが変化と混ざらないようにelse if)
		particleIterator->transform.scale += particleIterator->sizeAmount;
	}



	// 回転させるか
	if (group.isFlag.isRotateVelocity) {
		particleIterator->transform.rotate += particleIterator->rotateVelocity;
	}

	// バウンドさせるか
	if (group.isFlag.isBounce && particleIterator->transform.translate.y < 0) {
		particleIterator->transform.translate.y = 0;
		particleIterator->velocity = Math::Reflect(particleIterator->velocity, { 0,1,0 }, 0.85f);
	}

	// 時間
	particleIterator->currentTime += deltaTime;

	float t = particleIterator->currentTime / particleIterator->lifeTime;

	if (particleIterator->isEvent) {
	}

	if (particleIterator->isNoise) {
		Vector3 noize = Random::RandomVector3(-1.9f, 1.9f);

		if (particleIterator->direction == 0) {		// ↑
			noize.y = 0;
		}
		else if (particleIterator->direction == 1) {// →
			noize.x = 0;
		}
		else if (particleIterator->direction == 2) {// ↓
			noize.x = 0;
		}
		else if (particleIterator->direction == 3) {// ←
			noize.y = 0;
		}
		particleIterator->velocity += noize * MyGame::GameTime();
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

void Engine::ParticleFunction::Effect(ParticleGroup2d& group, std::list<Particle>::iterator& particleIterator, float deltaTime)
{
	// 重力の影響を受けるか
	if (group.isGravity_) {
		particleIterator->velocity.y -= group.kGravitationalAcceleration * deltaTime;
	}
	// 加速度の影響
	if (group.isAcceleration_) {
		particleIterator->velocity += particleIterator->acceleration * deltaTime;
	}

	// 時間
	particleIterator->currentTime += deltaTime;

	float t = particleIterator->currentTime / particleIterator->lifeTime;

	if (particleIterator->isEvent) {
	}

	// 透過するか
	if (group.isAlpha_) {
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

void Engine::ParticleFunction::WorldDataForGPU(ParticleGroup& group, std::list<Particle>::iterator& particleIterator, Camera* camera)
{
	Matrix4x4 projectionMatrix = camera->GetProjectionMatrix();
	Matrix4x4 viewMatrix = camera->GetViewMatrix();

	// ビルボード関係の関数
	Matrix4x4 worldMatrix = ParticleFunction::Billboard(group, particleIterator, camera);

	// ビュープロジェクション
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	// データ送る
	group.sbParticleResource_.Data()[group.instanceCount].World = worldMatrix;
	group.sbParticleResource_.Data()[group.instanceCount].WVP = worldViewProjectionMatrix;

}

void Engine::ParticleFunction::WorldDataForGPU(ParticleGroup2d& group, std::list<Particle>::iterator& particleIterator, Camera* camera)
{
	float winWidth = static_cast<float>(WinApp::GetClientWidth(false));
	float winHeight = static_cast<float>(WinApp::GetClientHeight(false));

	// 変換行列

	Vector3 pos = particleIterator->transform.translate;
	pos.z = 1.0f;

	Matrix4x4 worldMatrix = MakeAffineMatrix(particleIterator->transform.scale, particleIterator->transform.rotate, pos);

	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, winWidth, winHeight, 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));


	// WVP = Projection * View * World
	Matrix4x4 wvp = worldViewProjectionMatrix;

	auto& dst = group.sbParticleResource_.Data()[group.instanceCount];
	dst.World = worldMatrix;
	dst.WVP = wvp;
	dst.color = particleIterator->color;
}

void Engine::ParticleFunction::MaterialEffect(ParticleGroup& group)
{
	// UV本体に速度を加算し、その結果をマテリアルへ反映する。
	group.uvTransform_.translate += group.uvTransformVeloctiy_.translate;
	group.uvTransform_.rotate += group.uvTransformVeloctiy_.rotate;
	group.uvTransform_.scale += group.uvTransformVeloctiy_.scale;
	group.material->GetMaterialInstance().transform = group.uvTransform_;
	group.material->GPUData();
}

void Engine::ParticleFunction::Create(ParticleGroup& particleGroup, const std::string name, const std::string textureFilePath, uint32_t kNumMaxInstance,
	DirectXCommon* dxCommon, ModelMesh* mesh, EmitData::RasterizerType rasteType, EmitData::BlendType blendType)
{
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
	particleGroup.material->GetMaterialInstance().enableLighting_ = false;
	particleGroup.material->GetMaterialInstance().useEnvironment_ = false;
	// マテリアルの初期UV値を、保存・速度更新の基準値にも保持する。
	particleGroup.uvTransform_ = particleGroup.material->GetMaterialInstance().transform;

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

void Engine::ParticleFunction::Create(ParticleGroup2d& particleGroup, const std::string name, const std::string textureFilePath, uint32_t kNumMaxInstance, DirectXCommon* dxCommon, Sprite* sprite)
{
	// 名前
	particleGroup.name = name;
	// メッシュ
	particleGroup.sprite = sprite;

	// マテリアル
	particleGroup.material = std::make_unique<Material>();
	particleGroup.material->Initialize(dxCommon);
	particleGroup.material->tex_.diffuseFilePath = textureFilePath;
	particleGroup.material->LoadTex();
	particleGroup.material->GetMaterialInstance().enableLighting_ = false;
	particleGroup.material->GetMaterialInstance().useEnvironment_ = false;


	// パーティクルリソース生成
	particleGroup.sbParticleResource_.CreateBuffer(dxCommon, kNumMaxInstance);

	// 初期化
	for (uint32_t i = 0; i < kNumMaxInstance; ++i) {
		particleGroup.sbParticleResource_.Data()[i].World = MakeIdentity4x4();
		particleGroup.sbParticleResource_.Data()[i].WVP = MakeIdentity4x4();
		particleGroup.sbParticleResource_.Data()[i].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

void Engine::ParticleFunction::Create(ParticleGroup2d& particleGroup, const std::string name, const std::string textureFilePath, uint32_t kNumMaxInstance, DirectXCommon* dxCommon, ModelMesh* sprite)
{
	// 名前
	particleGroup.name = name;
	// メッシュ
	particleGroup.mesh = sprite;
	particleGroup.mesh->UpdateVertexBuffer();
	particleGroup.mesh->UpdateIndexBuffer();


	// マテリアル
	particleGroup.material = std::make_unique<Material>();
	particleGroup.material->Initialize(dxCommon);
	particleGroup.material->tex_.diffuseFilePath = textureFilePath;
	particleGroup.material->LoadTex();
	particleGroup.material->GetMaterialInstance().enableLighting_ = false;
	particleGroup.material->GetMaterialInstance().useEnvironment_ = false;


	// パーティクルリソース生成
	particleGroup.sbParticleResource_.CreateBuffer(dxCommon, kNumMaxInstance);

	// 初期化
	for (uint32_t i = 0; i < kNumMaxInstance; ++i) {
		particleGroup.sbParticleResource_.Data()[i].World = MakeIdentity4x4();
		particleGroup.sbParticleResource_.Data()[i].WVP = MakeIdentity4x4();
		particleGroup.sbParticleResource_.Data()[i].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}
