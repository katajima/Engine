#pragma once
#include"DirectXGame/engine/struct/Light.h"

#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"
#include "DirectXGame/engine/SkyBox/SkyBox.h"
#include "DirectXGame/engine/Effect/Ocean/Ocean.h"
#include "DirectXGame/engine/Effect/Primitive/Primitive.h"
#include "DirectXGame/engine/Effect/Trail/TrailEffect.h"
using namespace Microsoft::WRL;

#include "DirectXGame/engine/Animation/AnimationComponent.h"
#include "DirectXGame/engine/collider/3d/ColliderComponent.h"
#include "DirectXGame/engine/Transform/TransformComponent.h"
#include "DirectXGame/engine/Move/RigidBodyComponent.h"
#include "DirectXGame/engine/3d/Model/RenderComponent.h"

#include <future>

class Entity3DManager;
class Object3dCommon;
class SkinningConmmon;
class ImGuiManager;
class SkyBox;
class SkyBoxCommon;
class OceanManager;
class Object3d
{
public:
	// 初期化
	void Initialize(Entity3DManager* entity3DManager, ObjectModelType objectType = ObjectModelType::kNormal, ObjectRasterizerType rasterizerType = ObjectRasterizerType::NoUvInterpolation_MODE_SOLID_BACK);
	// 更新
	void Update();

	// 描画通常
	void Draw();

	void DrawTrailEffect();
	// セッター

	// モデル設定
	void SetModel(Model* model) { 
		this->model = model; 
		renderComponent_->SetModel(model);
	}

	// モデル指定
	void SetModel(const std::string& filePath);

	// カメラ設定
	void SetCamera(Camera* camera) { this->individualCamera_ = camera; }

	// 名前設定
	void SetName(const std::string& name) { this->name = name; }
	// タグ設定
	void SetNameTag(const std::string& name) { nameTag = name; }



	// プリミティブ形状
	void SetPrimitive(std::unique_ptr<Primitive> primitive)
	{
		primitive_ = std::move(primitive);
		renderComponent_->SetPrimitive(primitive_.get());
	};
	// スカイボックス
	void SetSkyBox(SkyBox* skyBox) {
		skyBox_ = skyBox;
		renderComponent_->SetSkyBox(skyBox_);
	}

	// 波セット
	void SetOcean(Ocean* ocean) { 
		ocean_ = ocean; 
		renderComponent_->SetOcean(ocean_);
	}

	void SetIsIndividualCamera(bool isIndividualCamera) { isIndividualCamera_ = isIndividualCamera; }

	void UseTrailEffect(const std::string tex, float maxTime, Color color = { 1,1,1,1 }, Vector3 offsetStr = { 0,0.5f,0 }, Vector3 offsetEnd = { 0,-0.5f,0 });




	// オブジェクトがカメラ内に映っているか
	bool IsInFrustum(const Matrix4x4& viewProjectionMatrix, const Vector3& position);

	// メッシュ取得
	ModelMesh* GetMesh(int index) { return model->modelData.mesh[index].get(); }

	// マテリアル取得
	Material* GetMaterial(int index) { return model->modelData.mesh[index]->material.get(); }

	// モデル取得
	Model* GetModel() const { return model; }

	// プリミティブ取得
	Primitive* GetPrimitive() const { return primitive_.get();};
	// 波取得
	Ocean* GetOcean() const { return ocean_; }

	// タグ
	std::string GetNameTag() const { return nameTag; }


	void SetIsDraw(bool is) { renderComponent_->SetIsDraw(is); }

	void DebugImguiModel();

	void DebugImguiSkin();

	void IsDelete() { isDelete = true; }

	bool GetIsDelete() const { return isDelete; }

	float GetAlpha() { return renderComponent_->GetAlpha(); };


	void SetIsEmitTrailEffect(bool isTrailEffect) { isEmitTrailEffect = isTrailEffect; }
private:
	// カメラ
	Camera* defaltCamera = nullptr;
	Camera* individualCamera_ = nullptr;
	// 個人的にカメラを使用するか
	bool isIndividualCamera_ = false;

	// トランスフォームデータ
	std::unique_ptr<Transfomation> transformation = nullptr;



	// ImGuiを表示するか
	bool imguiFlag_ = false;

	// 削除フラグ
	bool isDelete = false;
private: // コンポネント

	/// <summary>
	/// コライダー
	/// </summary>	

	// コライダーコンポーネント
	std::unique_ptr<ColliderComponent> colliderComponent_;
	// コライダーコンポーネントをObject3d内で更新するかのフラグ
	bool isColliderComponenyUpdate_ = false;

	/// <summary>
	///  トランスフォーム
	/// </summary>

	std::unique_ptr<TransformComponent> transformComponent_ = nullptr;

	/// <summary>
	///  物理
	/// </summary>

	std::unique_ptr<RigidBodyComponent> rigidBodyComponent_ = nullptr;

	/// <summary>
	/// アニメーション
	/// </summary>

	std::unique_ptr<AnimationComponent> animationComponent_ = nullptr;

	/// <summary>
	/// 描画
	/// </summary>

	std::unique_ptr<RenderComponent> renderComponent_ = nullptr;

public:
	// コライダーコンポーネントを初期化
	void InitColliderComponent();
	// Object3d内でコライダーコンポーネントを更新するか
	void SetIsUpdateColliderComponent(bool is) { isColliderComponenyUpdate_ = is; };
	// コライダーコンポーネントを取得
	ColliderComponent* GetColliderComponent() { return colliderComponent_.get(); };
	// コライダーコンポーネントの接触情報を取得
	ContactRecord& GetContactRecord() { return colliderComponent_->contactRecord_; };

	/// <summary>
	///  トランスフォーム
	/// </summary>

	// トランスフォームコンポーネント
	TransformComponent* GetTransformComponent() { return transformComponent_.get(); }
	// ワールド座標
	Vector3 GetWorldPosition() const { return transformComponent_->GetWorldPosition(); };
	// １フレーム前のワールド座標
	Vector3 GetPreWorldPosition() const { return transformComponent_->GetPreWorldPosition(); };
	// スクリーン座標
	Vector2 GetScreenPosition();
	// ワールド座標
	WorldTransform& GetWorldTransform() { return transformComponent_->GetWorldTransform(); }
	// 座標更新
	void UpdateWorldTransform() { transformComponent_->GetWorldTransform().Update(); }



	/// <summary>
	/// 物理
	/// </summary>

	// 初期化
	void InitRigidBodyComponent() {
		rigidBodyComponent_ = std::make_unique<RigidBodyComponent>();
	}
	// 物理取得
	RigidBodyComponent* GetRigidBodyComponent() { return rigidBodyComponent_.get(); };


	/// <summary>
	/// アニメーション
	/// </summary>

	// アニメーションコンポーネント初期化

	void InitAnimationComponent() {
		animationComponent_ = std::make_unique<AnimationComponent>();
		animationComponent_->Init(lineCommon_);
		animationComponent_->SetModel(model);
	}
	AnimationComponent* GetAnimationComponent() { return animationComponent_.get(); }

	/// <summary>
	/// 描画
	/// </summary>
	/// <returns></returns>

	RenderComponent* GetRenderComponent() { return renderComponent_.get(); }

public:


	// モデル
	Model* model = nullptr;
	// プリミティブ
	std::unique_ptr<Primitive> primitive_ = nullptr;
	// スカイボックス
	SkyBox* skyBox_ = nullptr;
	// 波
	Ocean* ocean_ = nullptr;

	//
	std::unique_ptr<TrailEffect> trailEffect_ = nullptr;
	// trailエフェクトを使用するかのフラグ
	bool isEmitTrailEffect = false;




	// オブジェクト名前
	std::string name = "";
	// オブジェクトタグ
	std::string nameTag = "";
private:
	Object3dCommon* object3dCommon_;
	SkinningConmmon* skinningConmmon_;
	ImGuiManager* imGuiManager_;
	Entity3DManager* entity3DManager_;
	LineCommon* lineCommon_;
	SkyBoxCommon* skyBoxCommon_;
	OceanManager* oceanManager_;
};


