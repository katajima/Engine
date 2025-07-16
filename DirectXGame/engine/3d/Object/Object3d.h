#pragma once
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/struct/Light.h"

#include"DirectXGame/engine/3d/Model/Model.h"
#include"DirectXGame/engine/3d/Model/ModelManager.h"
#include"DirectXGame/engine/Transfomation/Transfomation.h"
#include "DirectXGame/engine/WorldTransform/WorldTransform.h"
#include "DirectXGame/engine/SkyBox/SkyBox.h"
#include "DirectXGame/engine/Effect/Ocean/Ocean.h"
#include "DirectXGame/engine/Effect/Primitive/Primitive.h"
#include "DirectXGame/engine/Effect/Trail/TrailEffect.h"
using namespace Microsoft::WRL;


#include "DirectXGame/engine/collider/3d/ColliderComponent.h"

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

	// 描画するときの映り方を指定する
	enum class ObjectRasterizerType {
		UvInterpolation_MODE_SOLID_BACK,
		NoUvInterpolation_MODE_SOLID_BACK,
		UvInterpolation_MODE_WIREFRAME_BACK,
		NoUvInterpolation_MODE_WIREFRAME_BACK,

		UvInterpolation_MODE_SOLID_NONE,
		NoUvInterpolation_MODE_SOLID_NONE,
		UvInterpolation_MODE_WIREFRAME_NONE,
		NoUvInterpolation_MODE_WIREFRAME_NONE,
	};

	// オブジェクトのタイプを指定する
	enum class ObjectModelType { // オブジェクト種類
		kNormal,	// モデルを描画するオブジェクト
		kAnimation,	// モデルをアニメーション描画するオブジェクト
		kSkinning,	// モデルをスキニング描画するオブジェクト
		kPrimitive, // プリミティブを描画するオブジェクト
		kSkyBox,	// スカイボックスを描画するオブジェクト
		kOcean,		// 波を描画するオブジェクト
	};

	// 描画する順番
	enum class ObjectDrawType {
		kOpaque,		// 不透明
		kTranslucent01,	// 半透明最前
		kTranslucent02,	// 半透明中
		kTranslucent03,	// 半透明最後
	};

	// 初期化
	void Initialize(Entity3DManager* entity3DManager, ObjectModelType objectType = ObjectModelType::kNormal, ObjectRasterizerType rasterizerType = ObjectRasterizerType::NoUvInterpolation_MODE_SOLID_BACK);
	// 更新(アニメーション無し)
	void Update();

	// 描画通常
	void Draw();

	void DrawTrailEffect();
	// セッター

	// モデル設定
	void SetModel(Model* model) { this->model = model; }

	// モデル指定
	void SetModel(const std::string& filePath);

	// カメラ設定
	void SetCamera(Camera* camera) { this->individualCamera_ = camera; }

	// 名前設定
	void SetName(const std::string& name) { this->name = name; }
	// タグ設定
	void SetNameTag(const std::string& name) { nameTag = name; }
	// アニメーション変更
	void SetAnimetion(const std::string& name,float time) {
		Animetion::SetAnimation(model->modelData, name, time);
	}


	// プリミティブ形状
	void SetPrimitive(std::unique_ptr<Primitive> primitive);
	// スカイボックス
	void SetSkyBox(SkyBox* skyBox) { skyBox_ = skyBox; }

	// 波セット
	void SetOcean(Ocean* ocean) { ocean_ = ocean; }

	// 描画順
	void SetObjectDrawType(ObjectDrawType type) { objectDrawType_ = type; };

	// 映り方タイプ設定
	void SetObjectRasterizerType(ObjectRasterizerType type) { rasterizerType_ = type; }

	void SetIsIndividualCamera(bool isIndividualCamera) { isIndividualCamera_ = isIndividualCamera; }

	void UseTrailEffect(const std::string tex, float maxTime, Color color = { 1,1,1,1 }, Vector3 offsetStr = { 0,0.5f,0 }, Vector3 offsetEnd = { 0,-0.5f,0 });

	// ゲッター

	// ワールド座標
	Vector3 GetWorldPosition() const {
		// ワールド座標を入れる
		Vector3 worldPos;
		worldPos.x = worldtransform_.worldMat_.m[3][0];
		worldPos.y = worldtransform_.worldMat_.m[3][1];
		worldPos.z = worldtransform_.worldMat_.m[3][2];
		return worldPos;
	};

	// １フレーム前のワールド座標
	Vector3 GetPreWorldPosition() const {
		// ワールド座標を入れる
		Vector3 worldPos;
		worldPos.x = worldtransform_.worldPreMat_.m[3][0];
		worldPos.y = worldtransform_.worldPreMat_.m[3][1];
		worldPos.z = worldtransform_.worldPreMat_.m[3][2];
		return worldPos;
	};

	// スクリーン座標
	Vector2 GetScreenPosition();

	// オブジェクトがカメラ内に映っているか
	bool IsInFrustum(const Matrix4x4& viewProjectionMatrix, const Vector3& position);

	// メッシュ取得
	ModelMesh* GetMesh(int index) { return model->modelData.mesh[index].get(); }

	// マテリアル取得
	Material* GetMaterial(int index) { return model->modelData.mesh[index]->material.get(); }

	// モデル取得
	Model* GetModel() const { return model; }

	// プリミティブ取得
	Primitive* GetPrimitive() const;
	// 波取得
	Ocean* GetOcean() { return ocean_; }

	// オブジェクト型名前
	std::string GetObjectTypeName() const { return objectTypeName; }

	// タグ
	std::string GetNameTag() const { return nameTag; }

	bool GetIsColliderComponent() const {return isColliderComponent_;}

	ObjectModelType GetObjectType() { return objectType_; }

	ObjectDrawType GetObjectDrawType() { return objectDrawType_; }


	void DebugImguiModel();

	void DebugImguiSkin();

	void IsDelete() { isDelete = true; }

	bool GetIsDelete() { return isDelete; }

	void SetIsDraw(bool is) { isDraw = is; }

	float GetAlpha();

	bool GetIsSkin() const { return isSkin_; }

	void SetIsEmitTrailEffect(bool isTrailEffect) { isEmitTrailEffect = isTrailEffect; }

private:
	// 各コマンドリスト
	void DrawSetting();

	// スキニング設定
	void DrawSettingSkin();

	// 波
	void DrawSettingOcean();

	//
	void ObjectTypeDiscrimination(ObjectRasterizerType type);
	void ObjectSkinTypeDiscrimination(ObjectRasterizerType type);


	void UpdateSkinCluster(SkinCluster& skinCluster, const Skeleton& skeleton);
	


private:
	// カメラ
	Camera* defaltCamera = nullptr;

	Camera* individualCamera_ = nullptr;

	// トランスフォームデータ
	std::unique_ptr<Transfomation> transformation = nullptr;

	// 何かしらの見た目があるか
	bool isSkin_ = false;

	// 個人的にカメラを使用するか
	bool isIndividualCamera_ = false;

	// ImGuiを表示するか
	bool imguiFlag_ = false;

	// 削除フラグ
	bool isDelete = false;

	// 描画するかのフラグ
	bool isDraw = true;

	// trailエフェクトを使用するかのフラグ
	bool isTrailEffect = false;
	bool isEmitTrailEffect = false;


	// オブジェクトのタイプ
	ObjectModelType objectType_ = ObjectModelType::kNormal;

	// オブジェクトの描画順
	ObjectDrawType objectDrawType_ = ObjectDrawType::kOpaque;

	// オブジェクトの映り方タイプ
	ObjectRasterizerType rasterizerType_ = ObjectRasterizerType::NoUvInterpolation_MODE_SOLID_BACK;

private:
	// コライダーコンポーネント
	std::unique_ptr<ColliderComponent> colliderComponent_;

	// コライダーコンポーネントを使用するかのフラグ
	bool isColliderComponent_ = false;

	// コライダーコンポーネントをObject3d内で更新するかのフラグ
	bool isColliderComponenyUpdate_ = false;

public:
	// コライダーコンポーネントを初期化
	void InitColliderComponent();

	// Object3d内でコライダーコンポーネントを更新するか
	void SetIsUpdateColliderComponent(bool is) { isColliderComponenyUpdate_ = is; };

	// コライダーコンポーネントを取得
	ColliderComponent* GetColliderComponent() { return colliderComponent_.get(); };

	// コライダーコンポーネントの接触情報を取得
	ContactRecord& GetContactRecord() { return colliderComponent_->contactRecord_; };

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


	// 位置
	WorldTransform worldtransform_;

	// オブジェクト名前
	std::string name = "";

	// オブジェクトタグ
	std::string nameTag = "";

	// オブジェクトタイプ名前
	std::string objectTypeName = "";

	WorldTransform worldtransformTstr_;
	WorldTransform worldtransformTend_;
private:
	Object3dCommon* object3dCommon_;
	SkinningConmmon* skinningConmmon_;
	ImGuiManager* imGuiManager_;
	Entity3DManager* entity3DManager_;
	SkyBoxCommon* skyBoxCommon_;
	OceanManager* oceanManager_;
};


