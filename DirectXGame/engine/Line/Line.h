#pragma once
#include"DirectXGame/engine/struct/Structs.h"
#include"LineCommon.h"
#include"DirectXGame/engine/Camera/Camera.h"
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>

#include"DirectXGame/engine/Mesh/Mesh.h"

class LineCommon;



class LineDraw
{
public:
	void Initialize();

	void AddLine(Vector2 pos, Vector2 pos2);

	void SetCamera(Camera* camera) { this->camera = camera; }

	//void SetMatrix(const Matrix4x4& mat) { mat_ = mat; };
	void SetTransform(const Transform tra) { transform = tra; };

	

private:
	Camera* camera = nullptr;

	LineCommon* lineCommon_ = nullptr;

	std::unique_ptr<Mesh> mesh_;
	//std::vector<>
	
	////マテリアルデータ
	//struct Material {
	//	Vector4 color;
	//};
	//// 座標変換行列データ
	//struct TransfomationMatrix
	//{
	//	Matrix4x4 WVP;
	//	Matrix4x4 World;
	//};
	//Matrix4x4 mat_;

	//
	//Microsoft::WRL::ComPtr < ID3D12Resource> materialResource;
	//Material* materialData;

	//

	//TransfomationMatrix* transfomationMatrixData;
	//Microsoft::WRL::ComPtr < ID3D12Resource> transformationMatrixResource;

	Transform cameraTransform;

public:
	Transform transform;
};


