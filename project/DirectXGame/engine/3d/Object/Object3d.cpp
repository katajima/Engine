#include"Object3dCommon.h"
#include"Object3d.h"
#include"DirectXGame/engine/Skinning/Skinning.h"

#include"DirectXGame/engine/base/Texture/TextureManager.h"
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFunctions.h"
#include"DirectXGame/engine/MyGame/MyGame.h"

#include"DirectXGame/engine/Animation/Animation.h"
#include"DirectXGame/engine/Light/LightCommon.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/engine/Move/RigidBodyComponent.h"
#include "DirectXGame/engine/Animation/AnimationComponent.h"
#include "DirectXGame/engine/collider/3d/ColliderComponent.h"
#include "DirectXGame/engine/Transform/TransformComponent.h"

#include "DirectXGame/engine/Effect/Primitive/Primitive.h"
#include "DirectXGame/engine/SkyBox/SkyBox.h"
#include "DirectXGame/engine/Effect/Ocean/Ocean.h"
#include "DirectXGame/engine/Effect/Trail/TrailEffect.h"

#pragma region Init

Engine::Object3d::Object3d() = default;

Engine::Object3d::~Object3d() = default;

void Engine::Object3d::Initialize(EntityManager* entityManager, ObjectModelType objectType, PSOType rasterizerType)
{
	this->entityManager = entityManager;														// エンティティ3d
	this->object3dCommon = entityManager->GetObject3dCommon();								// オブジェクト共通クラス
	this->skinningConmmon = entityManager->GetSkinningConmmon();								// スキニング共通クラス
	this->imGuiManager = entityManager->GetObject3dCommon()->GetDxCommon()->GetImGuiManager();// ImGui管理クラス
	this->skyBoxCommon = entityManager->GetSkyBoxCommon();									// スカイボックス共通クラス
	this->oceanManager = entityManager->GetOceanManager();									// 波管理クラス
	this->lineCommon = entityManager->Get3DLineCommon();										// ライン共通クラス

	// 位置コンポーネント初期化
	transformComponent_ = std::make_unique<TransformComponent>();
	transformComponent_->Init();


	name = "object" + std::to_string(object3dCommon->GetObjectCount());

	// 位置初期化
	transformation = std::make_unique<Transfomation>();
	transformation->Initialize(object3dCommon->GetDxCommon());


	// 方向用トランスフォーム初期化
	direWorldTransform_.Initialize();
	direWorldTransform_.translate_.z = 1.0f;
	direWorldTransform_.parent_ = &transformComponent_->GetWorldTransform();

	defaltCamera = entityManager->GetObject3dCommon()->GetDefaltCamera();




	// レンダーコンポーネント初期化
	renderComponent_ = std::make_unique<RenderComponent>();
	renderComponent_->Init(entityManager, objectType, rasterizerType);
	renderComponent_->SetTransfomation(transformation.get());


	isColliderComponenyUpdate_ = true;

	// オブジェクト数
	object3dCommon->AddObjectCount();
}

void Engine::Object3d::InitColliderComponent()
{
	// コライダーコンポーネントの初期化
	colliderComponent_ = std::make_unique<ColliderComponent>();
	colliderComponent_->SetOwner(colliderComponent_.get());
	// ラインコモンをセット
	colliderComponent_->SetLineCommon(entityManager->Get3DLineCommon());
	// 登録（IDを取得したければ変数で受ける）
	colliderComponent_->SetUniqueId(UniqueIdGenerator::Generate());
	isColliderComponenyUpdate_ = true;
}

void Engine::Object3d::UseTrailEffect(const std::string tex, float maxTime, Color color, Vector3 offsetStr, Vector3 offsetEnd)
{
	trailEffect_ = std::make_unique<TrailEffect>();
	trailEffect_->Initialize(entityManager->GetEffectManager(), tex, maxTime, color);
	trailEffect_->SetCamera(defaltCamera);
	trailEffect_->SetOffset(offsetStr, offsetEnd, transformComponent_->GetWorldTransform());
}

void Engine::Object3d::InitAnimationComponent() {
	animationComponent_ = std::make_unique<AnimationComponent>();
	animationComponent_->Init(lineCommon);
	animationComponent_->SetModel(renderComponent_->GetModel());
}

// 初期化
void  Engine::Object3d::InitRigidBodyComponent() {
	rigidBodyComponent_ = std::make_unique<RigidBodyComponent>();
}

#pragma endregion // 初期化系

#pragma region Update

void Engine::Object3d::Update()
{
	if (isDelete) return;

	Matrix4x4 localMatrix = MakeIdentity4x4();



	Camera* cameraPtr;
	if (isIndividualCamera_) {
		cameraPtr = individualCamera;
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
		if (renderComponent_->GetModel()) {
			localMatrix = renderComponent_->GetModel()->GetModelData().rootNode.localMatrix;

			for (auto& mesh : renderComponent_->GetModel()->GetModelData().mesh) {
				mesh->material->GPUData();
			}
		}

		// トランスフォームデータ
		transformation->Update(GetModel(), cameraPtr, localMatrix, transformComponent_->GetWorldTransform().worldMat_);
		break;
	case ObjectModelType::kAnimation:

		// アニメーションコンポーネント更新
		animationComponent_->Update(MyGame::GameTime(), transformComponent_->GetWorldTransform());
		localMatrix = animationComponent_->GetLocalMatrix();

		// トランスフォームデータ
		transformation->Update(GetModel(), cameraPtr, localMatrix, transformComponent_->GetWorldTransform().worldMat_);
		break;
	case ObjectModelType::kSkinning:

		// アニメーションコンポーネント更新
		animationComponent_->UpdateSkin(MyGame::GameTime(), transformComponent_->GetWorldTransform());
		localMatrix = animationComponent_->GetLocalMatrix();

		// トランスフォームデータ
		transformation->UpdateSkinning(GetModel(), cameraPtr, localMatrix, transformComponent_->GetWorldTransform().worldMat_);
		break;
	case ObjectModelType::kPrimitive:
		if (GetPrimitive()) {
			GetPrimitive()->Update(MyGame::GameTime());

			transformation->Update(GetPrimitive(), cameraPtr, localMatrix, transformComponent_->GetWorldTransform().worldMat_);
		}
		break;
	case ObjectModelType::kSkyBox:
		if (renderComponent_->GetSkyBox()) {
			renderComponent_->GetSkyBox()->Update();

			transformation->Update(GetSkyBox(), cameraPtr, localMatrix, transformComponent_->GetWorldTransform().worldMat_);
		}
		break;
	case ObjectModelType::kOcean:
		if (renderComponent_->GetOcean()) {
			renderComponent_->GetOcean()->Update();

			transformation->Update(renderComponent_->GetOcean(), cameraPtr, localMatrix, transformComponent_->GetWorldTransform().worldMat_);
		}
		break;
	default:
		break;
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

void Engine::Object3d::RigidBodyUpdate()
{
	if (isDelete) return;
	// 物理
	if (rigidBodyComponent_ && isRigidUpdate_) {
		rigidBodyComponent_->Integrate(MyGame::GameTime(), transformComponent_->GetWorldTransform());
	}
}

#pragma endregion //更新系

#pragma region Draw

void Engine::Object3d::Draw()
{
	if (isDelete) return;

	renderComponent_->Draw();
}

void Engine::Object3d::DrawTrailEffect()
{
	if (trailEffect_) {
		// トレイルエフェクトの描画
		trailEffect_->Draw();
	}
}

void Engine::Object3d::DebugImguiSkin()
{
	DebugModel::ImguiSkin(renderComponent_->GetModel()->GetModelData());
}

#pragma endregion // 描画系

#pragma region Other

// アニメーションコンポーネント取得
Engine::AnimationComponent* Engine::Object3d::GetAnimationComponent() { return animationComponent_.get(); }

/// <summary>
/// 描画
/// </summary>
/// <returns></returns>
// レンダーコンポーネント取得
Engine::RenderComponent* Engine::Object3d::GetRenderComponent() { return renderComponent_.get(); }

// 物理取得
Engine::RigidBodyComponent* Engine::Object3d::GetRigidBodyComponent() { return rigidBodyComponent_.get(); };
//
void  Engine::Object3d::SetIsRigidUpdate(bool isRigidUpdate) { isRigidUpdate_ = isRigidUpdate; };

// カメラ設定
void  Engine::Object3d::SetCamera(Camera* camera) { this->individualCamera = camera; }
// 名前設定
void  Engine::Object3d::SetName(const std::string& name) { this->name = name; }
// タグ設定
void  Engine::Object3d::SetNameTag(const std::string& name) { nameTag = name; }
// プリミティブ形状
void  Engine::Object3d::SetPrimitive(std::unique_ptr< Engine::BasePrimitive> primitive) { renderComponent_->SetPrimitive(std::move(primitive)); };
// スカイボックス
void  Engine::Object3d::SetSkyBox(Engine::SkyBox* skyBox) { renderComponent_->SetSkyBox(skyBox); }
// 波セット
void  Engine::Object3d::SetOcean(Engine::Ocean* ocean) { renderComponent_->SetOcean(ocean); }
// オブジェクト固有に映すカメラを使用するか設定
void  Engine::Object3d::SetIsIndividualCamera(bool isIndividualCamera) { isIndividualCamera_ = isIndividualCamera; }
// メッシュ取得
Engine::ModelMesh* Engine::Object3d::GetMesh(int index) { return renderComponent_->GetModel()->GetModelData().mesh[index].get(); }
// マテリアル取得
Engine::Material* Engine::Object3d::GetMaterial(int index) { return renderComponent_->GetModel()->GetModelData().mesh[index]->material.get(); }
// モデル取得
Engine::Model* Engine::Object3d::GetModel() const { return renderComponent_->GetModel(); }
// プリミティブ取得
Engine::BasePrimitive* Engine::Object3d::GetPrimitive() const { return renderComponent_->GetPrimitive(); };
// 波取得
Engine::Ocean* Engine::Object3d::GetOcean() const { return renderComponent_->GetOcean(); }
// スカイボックス取得
Engine::SkyBox* Engine::Object3d::GetSkyBox() const { return renderComponent_->GetSkyBox(); }
// マテリアルインスタンス取得
std::vector<MaterialInstance>& Engine::Object3d::GetMaterialInstance() { return renderComponent_->GetMaterialInstance(); }
// タグ取得
std::string  Engine::Object3d::GetNameTag() const { return nameTag; }
// 描画するか設定
void  Engine::Object3d::SetIsDraw(bool is) { renderComponent_->SetIsDraw(is); }
// 削除する
void Engine::Object3d::IsDelete() { isDelete = true; }
// 削除されているか取得
bool Engine::Object3d::GetIsDelete() const { return isDelete; }
// オブジェクトに使われているモデルの透明度取得
float Engine::Object3d::GetAlpha() { return renderComponent_->GetAlpha(); };

// Object3d内でコライダーコンポーネントを更新するか
void Engine::Object3d::SetIsUpdateColliderComponent(bool is) { isColliderComponenyUpdate_ = is; };
// コライダーコンポーネントを取得
Engine::ColliderComponent* Engine::Object3d::GetColliderComponent() { return colliderComponent_.get(); };
// コライダーコンポーネントの接触情報を取得
Engine::ContactRecord& Engine::Object3d::GetContactRecord() { return colliderComponent_->contactRecord_; };

// トランスフォームコンポーネント
Engine::TransformComponent* Engine::Object3d::GetTransformComponent() { return transformComponent_.get(); }
// ワールド座標
Vector3 Engine::Object3d::GetWorldPosition() const { return transformComponent_->GetWorldPosition(); };
// １フレーム前のワールド座標
Vector3 Engine::Object3d::GetPreWorldPosition() const { return transformComponent_->GetPreWorldPosition(); };
// ワールド座標
Engine::WorldTransform& Engine::Object3d::GetWorldTransform() { return transformComponent_->GetWorldTransform(); }
// 座標更新
void Engine::Object3d::UpdateWorldTransform() { transformComponent_->GetWorldTransform().Update(); }
// 向いている方向
Vector3 Engine::Object3d::ObjectDirection() const { return direction_; }

Vector2 Engine::Object3d::GetScreenPosition()
{
	if (transformComponent_.get()) {
		if (isIndividualCamera_) {
			return ScreenPosition(transformComponent_->GetWorldTransform(), individualCamera);
		}
		else {
			return ScreenPosition(transformComponent_->GetWorldTransform(), defaltCamera);
		}
	}
	else {
		return Vector2{ 0.0f,0.0f };
	}
}

void Engine::Object3d::DebugImguiModel()
{
	DebugModel::ImguiModel(renderComponent_->GetModel()->GetModelData());
}

void Engine::Object3d::SetModel(Model* model){
	renderComponent_->SetModel(model);
}

void Engine::Object3d::SetModel(const std::string& filePath)
{
	//モデルを検索してセット
	Model* findModel = object3dCommon->GetDxCommon()->GetModelManager()->FindModel(filePath);
	renderComponent_->SetModel(findModel);
}

#pragma endregion // その他


Vector2 Engine::ScreenPosition(const Engine::WorldTransform world, Engine::Camera* camera)
{

	Vector3 wPos = world.GetWorldPosition();

	// カメラのビュープロジェクション行列を取得
	Matrix4x4 matViewProjection;

	matViewProjection = Multiply( camera->GetViewMatrix(), camera->GetProjectionMatrix());

	// ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, static_cast<float>(Engine::WinApp::GetClientWidth()), static_cast<float>(Engine::WinApp::GetClientHeight()), 0, 1);

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