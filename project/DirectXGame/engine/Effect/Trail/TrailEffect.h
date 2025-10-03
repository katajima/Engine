#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>
#include<list>
#include<string>
#include<vector>
#include<format>
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include"DirectXGame/engine/Camera/Camera.h"

#include"DirectXGame/engine/Mesh/ModelMesh.h"
#include"DirectXGame/engine/Material/Material.h"
#include "DirectXGame/engine/Transform/Transfomation/Transfomation.h"

#include"TrailEffectManager.h"


class EffectManager;
class TrailEffect
{
public:
	

	void Initialize(EffectManager* effectManager, const std::string& tex, float maxtime, const Color color = { 1,1,1,1 });

	void Update();
	
	void Draw();

	void SetCamera(Camera* camera) { camera_ = camera; };

	void SetOffset(Vector3 offsetStr, Vector3 offsetEnd,WorldTransform& world){
		worldtransformTstr_.Initialize();
		worldtransformTstr_.parent_ = &world;
		worldtransformTstr_.translate_ = offsetStr;

		worldtransformTend_.Initialize();
		worldtransformTend_.parent_ = &world;
		worldtransformTend_.translate_ = offsetEnd;
	}
	
	// 発生フラグをセット
	void SetIsEmit(bool is) { flag_ = is;}
	// 発生フラグを取得
	bool GetIsEmit() const { return flag_; }
	// 時間をセット
	void SetTimer(float t) { timer = t; }
	// 発生時間を取得
	float GetTimer() const { return timer; }



	void SetMatrix(Matrix4x4& mat) { mat_ = mat; }

	ModelMesh* GetMesh() const { return mesh.get(); }

	std::unique_ptr<ModelMesh> mesh;

private:
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};
	// カメラ
	Camera* camera_ = nullptr;

	std::unique_ptr<Material> material;
	std::unique_ptr<Transfomation> transfomation = nullptr;

	Matrix4x4 mat_;
	Matrix4x4 parentTransform_;

	bool flag_ = false;
	Vector3 velocity_; // 速度
	
	float timer = 0;

private:
	EffectManager* effectManager_;

	WorldTransform worldtransformTstr_;
	WorldTransform worldtransformTend_;
};

