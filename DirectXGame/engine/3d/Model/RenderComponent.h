#pragma once
#include "DirectXGame/engine/Mesh/ModelMesh.h"
#include "DirectXGame/engine/3d/Model/ModelData.h"
#include"DirectXGame/engine/3d/Model/Model.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"

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

	Transparent,
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

class BasePrimitive;
class SkyBox;
class Ocean;
class Transfomation;
class Entity3DManager;
class RenderComponent
{
public:

	void Init(Entity3DManager* entity3DManager, ObjectModelType objectType, ObjectRasterizerType rasterizerType);


	void Update();

	void Draw();

	// カメラ
	void SetCamera(Camera* camera) { camera_ = camera; }
	// トランスフォーム
	void SetTransfomation(Transfomation* transfomation) { transfomation_ = transfomation; }

	// モデル
	void SetModel(Model* model) { this->model = model; }
	// スカイボックス
	void SetSkyBox(SkyBox* skyBox) { skyBox_ = skyBox; }
	// 波セット
	void SetOcean(Ocean* ocean) { ocean_ = ocean; }
	// プリミティブ
	void SetPrimitive(BasePrimitive* primitive) { primitive_ = primitive; }


public:
	// 何かしらの見た目があるか
	bool GetIsSkin() const { return isSkin_; }
	// 描画するか
	void SetIsDraw(bool is) { isDraw = is; }
	// オブジェクトタイプ
	ObjectModelType GetObjectType() const { return objectType_; }
	// オブジェクトの描画順
	ObjectDrawType GetObjectDrawType() const { return objectDrawType_; }
	// 描画順の設定
	void SetObjectDrawType(ObjectDrawType type) { objectDrawType_ = type; };
	// 映り方タイプ設定
	void SetObjectRasterizerType(ObjectRasterizerType type) { rasterizerType_ = type; }
	// オブジェクト型名前
	std::string GetObjectTypeName() const { return objectTypeName; }


public:

	float GetAlpha();


private:
	void ObjectNormalTypeDiscrimination(ObjectRasterizerType type);

	void ObjectSkinningTypeDiscrimination(ObjectRasterizerType type);

	// 描画セッティング(デフォルト)
	void DrawSetting();
	// 描画セッティング(スキニング)
	void DrawSettingSkin();
	// 描画セッティング(海)
	void DrawSettingOcean();


private:
	// オブジェクトの描画順
	ObjectDrawType objectDrawType_ = ObjectDrawType::kOpaque;
	// オブジェクトの映り方タイプ
	ObjectRasterizerType rasterizerType_ = ObjectRasterizerType::NoUvInterpolation_MODE_SOLID_BACK;
	// オブジェクトのタイプ
	ObjectModelType objectType_ = ObjectModelType::kNormal;
	// 何かしらの見た目があるか
	bool isSkin_ = false;
	// 描画するかのフラグ
	bool isDraw = true;

	// オブジェクトタイプ名前
	std::string objectTypeName = "";
private:
	Entity3DManager* entity3DManager_ = nullptr;
	Model* model = nullptr;
	// プリミティブ
	BasePrimitive* primitive_ = nullptr;
	// スカイボックス
	SkyBox* skyBox_ = nullptr;
	// 波
	Ocean* ocean_ = nullptr;
	// トランスフォーム
	Transfomation* transfomation_ = nullptr;
	// カメラ
	Camera* camera_ = nullptr;
};

