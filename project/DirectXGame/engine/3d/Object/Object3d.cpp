#include"Object3dCommon.h"
#include"Object3d.h"
#include"DirectXGame/engine/Skinning/Skinning.h"

#include"DirectXGame/engine/base/Texture/TextureManager.h"
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/MyGame/MyGame.h"



#include"DirectXGame/engine/Animation/Animation.h"
#include"DirectXGame/engine/Light/LightCommon.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Effect/Ocean/Ocean.h"

#pragma region Init

void Object3d::Initialize(Entity3DManager* entity3DManager, ObjectModelType objectType, PSOType rasterizerType)
{
	entity3DManager_ = entity3DManager;
	object3dCommon_ = entity3DManager_->GetObject3dCommon();
	skinningConmmon_ = entity3DManager_->GetSkinningConmmon();
	imGuiManager_ = entity3DManager_->GetObject3dCommon()->GetDxCommon()->GetImGuiManager();
	skyBoxCommon_ = entity3DManager_->GetSkyBoxCommon();
	oceanManager_ = entity3DManager_->GetOceanManager();
	lineCommon_ = entity3DManager_->Get3DLineCommon();

	transformComponent_ = std::make_unique<TransformComponent>();
	transformComponent_->Init();


	name = "object" + std::to_string(object3dCommon_->count);

	transformation = std::make_unique<Transfomation>();
	transformation->Initialize(object3dCommon_->GetDxCommon());


	direWorldTransform_.Initialize();
	direWorldTransform_.translate_.z = 1.0f;
	direWorldTransform_.parent_ = &transformComponent_->GetWorldTransform();

	defaltCamera = entity3DManager_->GetObject3dCommon()->GetDefaltCamera();





	renderComponent_ = std::make_unique<RenderComponent>();
	renderComponent_->Init(entity3DManager_, objectType, rasterizerType);
	renderComponent_->SetTransfomation(transformation.get());

	isColliderComponenyUpdate_ = true;

	// オブジェクト数
	object3dCommon_->count++;
}

void Object3d::InitColliderComponent()
{
	// コライダーコンポーネントの初期化
	colliderComponent_ = std::make_unique<ColliderComponent>();
	colliderComponent_->SetOwner(colliderComponent_.get());
	// ラインコモンをセット
	colliderComponent_->SetLineCommon(entity3DManager_->Get3DLineCommon());
	// 登録（IDを取得したければ変数で受ける）
	colliderComponent_->SetUniqueId(UniqueIdGenerator::Generate());
	isColliderComponenyUpdate_ = true;
}

void Object3d::UseTrailEffect(const std::string tex, float maxTime, Color color, Vector3 offsetStr, Vector3 offsetEnd)
{
	trailEffect_ = std::make_unique<TrailEffect>();
	trailEffect_->Initialize(entity3DManager_->GetEffectManager(), tex, maxTime, color);
	trailEffect_->SetCamera(defaltCamera);
	trailEffect_->SetOffset(offsetStr, offsetEnd, transformComponent_->GetWorldTransform());
}

#pragma endregion // 初期化系

#pragma region Update

void Object3d::Update()
{
	if (isDelete) return;

	Matrix4x4 localMatrix = MakeIdentity4x4();



	Camera* cameraPtr;
	if (isIndividualCamera_) {
		cameraPtr = individualCamera_;
	}
	else {
		cameraPtr = defaltCamera;
	}
	renderComponent_->Update();
	renderComponent_->SetCamera(cameraPtr);


	// トランスフォームコンポーネント
	transformComponent_->Update();

	switch (renderComponent_->GetObjectType())
	{
	case ObjectModelType::kNormal:
		// モデルが存在する場合
		if (model) {
			localMatrix = model->modelData.rootNode.localMatrix;

			for (auto& mesh : model->modelData.mesh) {
				mesh->material->GPUData();
			}
		}

		// トランスフォームデータ
		transformation->Update(model, cameraPtr, localMatrix, transformComponent_->GetWorldTransform().worldMat_);
		break;
	case ObjectModelType::kAnimation:

		// アニメーションコンポーネント更新
		animationComponent_->Update(MyGame::GameTime(), transformComponent_->GetWorldTransform());
		localMatrix = animationComponent_->GetLocalMatrix();

		// トランスフォームデータ
		transformation->Update(model, cameraPtr, localMatrix, transformComponent_->GetWorldTransform().worldMat_);
		break;
	case ObjectModelType::kSkinning:

		// アニメーションコンポーネント更新
		animationComponent_->UpdateSkin(MyGame::GameTime(), transformComponent_->GetWorldTransform());
		localMatrix = animationComponent_->GetLocalMatrix();

		// トランスフォームデータ
		transformation->UpdateSkinning(model, cameraPtr, localMatrix, transformComponent_->GetWorldTransform().worldMat_);
		break;
	case ObjectModelType::kPrimitive:
		if (primitive_) {
			primitive_->Update(MyGame::GameTime());

			transformation->Update(primitive_.get(), cameraPtr, localMatrix, transformComponent_->GetWorldTransform().worldMat_);
		}
		break;
	case ObjectModelType::kSkyBox:
		if (skyBox_) {
			skyBox_->Update();

			transformation->Update(primitive_.get(), cameraPtr, localMatrix, transformComponent_->GetWorldTransform().worldMat_);
		}
		break;
	case ObjectModelType::kOcean:
		if (ocean_) {
			ocean_->Update();

			transformation->Update(ocean_, cameraPtr, localMatrix, transformComponent_->GetWorldTransform().worldMat_);
		}
		break;
	default:
		break;
	}

	// 物理
	if (rigidBodyComponent_) {
		rigidBodyComponent_->Integrate(MyGame::GameTime(), transformComponent_->GetWorldTransform());
	}
	// トレイル
	if (trailEffect_) {
		trailEffect_->SetIsEmit(isEmitTrailEffect);
		trailEffect_->Update();
	}

	// コライダー
	if (colliderComponent_) {
		if (isColliderComponenyUpdate_) {
			colliderComponent_->UpdateAll(transformComponent_->GetWorldTransform());
		}
	}
	direWorldTransform_.Update();

	direction_ = Subtract(direWorldTransform_.worldMat_.GetWorldPosition(), GetWorldPosition()).Normalize();

}

#pragma endregion //更新系

#pragma region Draw

void Object3d::Draw()
{
	if (isDelete) return;

	renderComponent_->Draw();
}

void Object3d::DrawTrailEffect()
{
	if (trailEffect_) {
		// トレイルエフェクトの描画
		trailEffect_->Draw();
	}
}

void Object3d::DebugImguiSkin()
{
	DebugModel::ImguiSkin(model->modelData);
}

#pragma endregion // 描画系

#pragma region Other

Vector2 Object3d::GetScreenPosition()
{
	if (transformComponent_.get()) {
		if (isIndividualCamera_) {
			return ScreenPosition(transformComponent_->GetWorldTransform(), individualCamera_);
		}
		else {
			return ScreenPosition(transformComponent_->GetWorldTransform(), defaltCamera);
		}
	}
	else {
		return Vector2{ 0.0f,0.0f };
	}
}

void Object3d::DebugImguiModel()
{
	DebugModel::ImguiModel(model->modelData);
}

void Object3d::SetModel(const std::string& filePath)
{
	//モデルを検索してセット

	model = object3dCommon_->GetDxCommon()->GetModelManager()->FindModel(filePath);
	renderComponent_->SetModel(model);
}

#pragma endregion // その他


Vector2 ScreenPosition(const WorldTransform world, Camera* camera)
{

	Vector3 wPos = world.GetWorldPosition();

	// カメラのビュープロジェクション行列を取得
	Matrix4x4 matViewProjection;

	matViewProjection = Multiply( camera->GetViewMatrix(), camera->GetProjectionMatrix());

	// ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, static_cast<float>(WinApp::GetClientWidth()), static_cast<float>(WinApp::GetClientHeight()), 0, 1);

	// 視錐台内にオブジェクトがあるかチェック (matViewProjection を渡す)
	if (!IsInFrustum(matViewProjection, wPos)) {
		return Vector2{ -100, -100 }; // 視錐台外にある場合、無効なスクリーン座標を返す
	}

	// ワールド座標をクリップ空間座標へ変換
	Vector4 clipSpacePos = Transforms(Vector4(wPos.x, wPos.y, wPos.z, 1.0f), matViewProjection);

	// 透視除算 (NDC へ変換)
	if (clipSpacePos.w == 0.0f) {
		return Vector2{ -100, -100 }; // 透視除算エラー
	}
	Vector3 ndcPos = {
		clipSpacePos.x / clipSpacePos.w,
		clipSpacePos.y / clipSpacePos.w,
		clipSpacePos.z / clipSpacePos.w
	};

	// NDC → スクリーン座標変換
	Vector4 screenPos = Transforms(Vector4(ndcPos.x, ndcPos.y, ndcPos.z, 1.0f), matViewport);

	return Vector2{ screenPos.x, screenPos.y };
}