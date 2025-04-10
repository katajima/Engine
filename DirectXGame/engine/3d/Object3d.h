#pragma once
#include"DirectXGame/engine/math/MathFanctions.h"
#include "DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/struct/Light.h"
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>
#include<string>
#include<vector>
#include<format>
#include"Model.h"
#include"ModelManager.h"
#include"DirectXGame/engine/Transfomation/Transfomation.h"
#include "DirectXGame/engine/WorldTransform/WorldTransform.h"

using namespace Microsoft::WRL;

class Object3dCommon;
class SkinningConmmon;
class ImGuiManager;

class Object3d
{
public:
	enum class ObjectType {
		UvInterpolation_MODE_SOLID_BACK,
		NoUvInterpolation_MODE_SOLID_BACK,
		UvInterpolation_MODE_WIREFRAME_BACK,
		NoUvInterpolation_MODE_WIREFRAME_BACK,

		UvInterpolation_MODE_SOLID_NONE,
		NoUvInterpolation_MODE_SOLID_NONE,
		UvInterpolation_MODE_WIREFRAME_NONE,
		NoUvInterpolation_MODE_WIREFRAME_NONE,
	};


	// 初期化
	void Initialize();
	// 更新(アニメーション無し)
	void Update();
	// 更新(スキニング有り)
	void UpdateSkinning();
	// 更新(アニメーション有り)
	void UpdateAnimation();
	
	// メッシュライン描画
	void LineMesh();

	// 描画通常
	void Draw(ObjectType type = {});
	// 描画スキニング用
	void DrawSkinning(ObjectType type = {});
	// 描画ライン
	void DrawLine();
	
	// setter
	 void SetModel(Model* model) { this->model = model; }
	 Model* GetModel() const { return model; }
	
	 void SetModel(const std::string& filePath);
	void SetCamera(Camera* camera) { this->camera = camera; }
	void SetName(const std::string& name) { this->name = name; }


	Vector3 GetWorldPosition() const {
		// ワールド座標を入れる
		Vector3 worldPos;
		worldPos.x = worldtransform_.worldMat_.m[3][0];
		worldPos.y = worldtransform_.worldMat_.m[3][1];
		worldPos.z = worldtransform_.worldMat_.m[3][2];
		return worldPos;
	};
	Vector3 GetPreWorldPosition() const {
		// ワールド座標を入れる
		Vector3 worldPos;
		worldPos.x = worldtransform_.worldPreMat_.m[3][0];
		worldPos.y = worldtransform_.worldPreMat_.m[3][1];
		worldPos.z = worldtransform_.worldPreMat_.m[3][2];
		return worldPos;
	};

	Vector2 GetScreenPosition();
	
	bool IsInFrustum(const Matrix4x4& viewProjectionMatrix, const Vector3& position);

	Mesh* GetMesh(int index) { return model->modelData.mesh[index].get(); }
	Material* GetMaterial(int index) { return model->modelData.material[index].get(); }

private:
	// 各コマンドリスト
	void DrawSetting();

	void DrawSettingSkin();
	//
	void ObjectTypeDiscrimination(ObjectType type);
	void ObjectSkinTypeDiscrimination(ObjectType type);


private:
	// カメラ
	Camera* camera = nullptr;
	// トランスフォームデータ
	std::unique_ptr<Transfomation> transfomation = nullptr;
	//
	// 
	bool flag = true;
	
	Object3dCommon* object3dCommon_;
	SkinningConmmon* skinningConmmon_;
	ImGuiManager* imGuiManager_;

public:
	// モデル
	Model* model = nullptr;
	
	WorldTransform worldtransform_;

	std::string name = "";
};


