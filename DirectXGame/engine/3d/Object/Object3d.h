#pragma once
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/struct/Light.h"
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>
#include<string>
#include<vector>
#include<format>
#include"DirectXGame/engine/3d/Model/Model.h"
#include"DirectXGame/engine/3d/Model/ModelManager.h"
#include"DirectXGame/engine/Transfomation/Transfomation.h"
#include "DirectXGame/engine/WorldTransform/WorldTransform.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3D.h"

using namespace Microsoft::WRL;

class Entity3DManager;
class Object3dCommon;
class SkinningConmmon;
class ImGuiManager;
class Primitive;
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
	enum class ObjectType { // オブジェクト種類
		kNormal,	// モデルを描画するオブジェクト
		kAnimation,	// モデルをアニメーション描画するオブジェクト
		kSkinning,	// モデルをスキニング描画するオブジェクト
		kPrimitive, // プリミティブを描画するオブジェクト
	};

	// 初期化
	void Initialize(Entity3DManager* entity3DManager, ObjectType objectType = ObjectType::kNormal, ObjectRasterizerType rasterizerType = ObjectRasterizerType::NoUvInterpolation_MODE_SOLID_BACK);
	// 更新(アニメーション無し)
	void Update();

	// 描画通常
	void Draw();


	// セッター

	// モデル設定
	void SetModel(Model* model) { this->model = model; }
	
	// モデル指定
	void SetModel(const std::string& filePath);
	
	// カメラ設定
	void SetCamera(Camera* camera) { this->camera = camera; }
	
	// 名前設定
	void SetName(const std::string& name) { this->name = name; }

	// プリミティブ形状
	void SetPrimitive(Primitive* primitive) { primitive_ = primitive; }

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
	Mesh* GetMesh(int index) { return model->modelData.mesh[index].get(); }

	// マテリアル取得
	Material* GetMaterial(int index) { return model->modelData.material[index].get(); }

	// モデル取得
	Model* GetModel() const { return model; }

	// プリミティブ取得
	Primitive* GetPrimitive() { return primitive_; }

private:
	// 各コマンドリスト
	void DrawSetting();

	// スキニング設定
	void DrawSettingSkin();

	//
	void ObjectTypeDiscrimination(ObjectRasterizerType type);
	void ObjectSkinTypeDiscrimination(ObjectRasterizerType type);


private:
	// カメラ
	Camera* camera = nullptr;
	// トランスフォームデータ
	std::unique_ptr<Transfomation> transfomation = nullptr;
	//

	// アニメーションするかのフラグ
	bool flag = true;

	bool imguiFlag_ = false;

	// オブジェクトのタイプ
	ObjectType objectType_ = ObjectType::kNormal;

	// オブジェクトの映り方タイプ
	ObjectRasterizerType rasterizerType_ = ObjectRasterizerType::NoUvInterpolation_MODE_SOLID_BACK;

public:
	// モデル
	Model* model = nullptr;
	//
	Primitive* primitive_ = nullptr;

	std::unique_ptr<Entity3D> entity3D_;

	// 位置
	WorldTransform worldtransform_;

	// オブジェクト名前
	std::string name = "";

private:
	Object3dCommon* object3dCommon_;
	SkinningConmmon* skinningConmmon_;
	ImGuiManager* imGuiManager_;
	Entity3DManager* entity3DManager_;
};


