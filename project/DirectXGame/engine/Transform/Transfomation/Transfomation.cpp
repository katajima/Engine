#include "Transfomation.h"

#include"DirectXGame/engine/Effect/Primitive/Primitive.h"

void Engine::Transfomation::Initialize(DirectXCommon* dxCommon)
{
	// ダイレクトX共通クラス
	dxCommon_ = dxCommon;

	// リソース生成
	cbResource_.CreateBuffer(dxCommon_);

	//単位行列を書き込んでおく
	cbResource_.Data()->WVP = MakeIdentity4x4();
	cbResource_.Data()->World = MakeIdentity4x4();
	cbResource_.Data()->worldInverseTranspose = MakeIdentity4x4();
}

void Engine::Transfomation::Update(Model* model, Camera* camera, Matrix4x4& local, Matrix4x4& mat)
{
	Matrix4x4 worldViewProjectionMatrix{};

	// カメラがあるなら
	if (camera) {
		// WVP計算
		Matrix4x4 worldViewProjectionMatrix{};

		worldViewProjectionMatrix = Multiply(local, mat); // ワールド変換

		worldViewProjectionMatrix = Multiply(worldViewProjectionMatrix, camera->GetViewMatrix()); // ビュー変換
		worldViewProjectionMatrix = Multiply(worldViewProjectionMatrix, camera->GetProjectionMatrix()); // 射影変換



		if (model) {

			cbResource_.Data()->WVP = worldViewProjectionMatrix;
			cbResource_.Data()->World = Multiply(local, mat);

		}
		else {
			cbResource_.Data()->WVP = worldViewProjectionMatrix;
			cbResource_.Data()->World = mat;
		}
	}
	else {
		worldViewProjectionMatrix = mat;
		cbResource_.Data()->WVP = worldViewProjectionMatrix;
		cbResource_.Data()->World = mat;
	}

	cbResource_.Data()->worldInverseTranspose = Transpose(Inverse(mat));

}

void Engine::Transfomation::Update(BasePrimitive* primitive, Camera* camera, Matrix4x4& local, Matrix4x4& mat)
{
	Matrix4x4 worldViewProjectionMatrix{};

	// カメラがあるなら
	if (camera) {
		// WVP計算
		Matrix4x4 worldViewProjectionMatrix{};

		worldViewProjectionMatrix = Multiply(local, mat); // ワールド変換

		worldViewProjectionMatrix = Multiply(worldViewProjectionMatrix, camera->GetViewMatrix()); // ビュー変換
		worldViewProjectionMatrix = Multiply(worldViewProjectionMatrix, camera->GetProjectionMatrix()); // 射影変換



		if (primitive) {

			cbResource_.Data()->WVP = worldViewProjectionMatrix;
			cbResource_.Data()->World = Multiply(local, mat);

		}
		else {
			cbResource_.Data()->WVP = worldViewProjectionMatrix;
			cbResource_.Data()->World = mat;
		}
	}
	else {
		worldViewProjectionMatrix = mat;
		cbResource_.Data()->WVP = worldViewProjectionMatrix;
		cbResource_.Data()->World = mat;
	}

	cbResource_.Data()->worldInverseTranspose = Transpose(Inverse(mat));
}

void Engine::Transfomation::Update(SkyBox* skyBox, Camera* camera, Matrix4x4& local, Matrix4x4& mat)
{
	Matrix4x4 worldViewProjectionMatrix{};

	// カメラがあるなら
	if (camera) {
		// WVP計算
		Matrix4x4 worldViewProjectionMatrix{};

		worldViewProjectionMatrix = Multiply(local, mat); // ワールド変換

		worldViewProjectionMatrix = Multiply(worldViewProjectionMatrix, camera->GetViewMatrix()); // ビュー変換
		worldViewProjectionMatrix = Multiply(worldViewProjectionMatrix, camera->GetProjectionMatrix()); // 射影変換



		if (skyBox) {

			cbResource_.Data()->WVP = worldViewProjectionMatrix;
			cbResource_.Data()->World = Multiply(local, mat);

		}
		else {
			cbResource_.Data()->WVP = worldViewProjectionMatrix;
			cbResource_.Data()->World = mat;
		}
	}
	else {
		worldViewProjectionMatrix = mat;
		cbResource_.Data()->WVP = worldViewProjectionMatrix;
		cbResource_.Data()->World = mat;
	}

	cbResource_.Data()->worldInverseTranspose = Transpose(Inverse(mat));
}

void Engine::Transfomation::Update(Ocean* ocean, Camera* camera, Matrix4x4& local, Matrix4x4& mat)
{
	Matrix4x4 worldViewProjectionMatrix{};

	// カメラがあるなら
	if (camera) {
		// WVP計算
		Matrix4x4 worldViewProjectionMatrix{};

		worldViewProjectionMatrix = Multiply(local, mat); // ワールド変換

		worldViewProjectionMatrix = Multiply(worldViewProjectionMatrix, camera->GetViewMatrix()); // ビュー変換
		worldViewProjectionMatrix = Multiply(worldViewProjectionMatrix, camera->GetProjectionMatrix()); // 射影変換



		if (ocean) {

			cbResource_.Data()->WVP = worldViewProjectionMatrix;
			cbResource_.Data()->World = Multiply(local, mat);

		}
		else {
			cbResource_.Data()->WVP = worldViewProjectionMatrix;
			cbResource_.Data()->World = mat;
		}
	}
	else {
		worldViewProjectionMatrix = mat;
		cbResource_.Data()->WVP = worldViewProjectionMatrix;
		cbResource_.Data()->World = mat;
	}

	cbResource_.Data()->worldInverseTranspose = Transpose(Inverse(mat));
}

void Engine::Transfomation::Update(Camera* camera, Matrix4x4& mat)
{
	Matrix4x4 worldViewProjectionMatrix{};

	// あるなら
	if (camera) {

		worldViewProjectionMatrix = mat;

		worldViewProjectionMatrix = Multiply(worldViewProjectionMatrix, camera->GetViewMatrix()); // ビュー変換
		worldViewProjectionMatrix = Multiply(worldViewProjectionMatrix, camera->GetProjectionMatrix()); // 射影変換

		cbResource_.Data()->WVP = worldViewProjectionMatrix;
		cbResource_.Data()->World = mat;
	}
	else {
		worldViewProjectionMatrix = mat;
		cbResource_.Data()->WVP = worldViewProjectionMatrix;
		cbResource_.Data()->World = mat;
	}

	cbResource_.Data()->worldInverseTranspose = Transpose(Inverse(mat));
}

void Engine::Transfomation::UpdateSkinning(Model* model, Camera* camera, Matrix4x4& local, Matrix4x4& mat)
{
	Matrix4x4 worldViewProjectionMatrix{};

	// カメラがあるなら
	if (camera) {

		worldViewProjectionMatrix = mat;

		worldViewProjectionMatrix = Multiply(worldViewProjectionMatrix, camera->GetViewMatrix()); // ビュー変換
		worldViewProjectionMatrix = Multiply(worldViewProjectionMatrix, camera->GetProjectionMatrix()); // 射影変換



		if (model) {

			cbResource_.Data()->WVP = worldViewProjectionMatrix;
			cbResource_.Data()->World = Multiply(local, mat);

		}
		else {
			cbResource_.Data()->WVP = worldViewProjectionMatrix;
			cbResource_.Data()->World = mat;
		}
	}
	else {
		worldViewProjectionMatrix = mat;
		cbResource_.Data()->WVP = worldViewProjectionMatrix;
		cbResource_.Data()->World = mat;
	}

	cbResource_.Data()->worldInverseTranspose = Transpose(Inverse(mat));

}

void Engine::Transfomation::UpdateSprite(Matrix4x4& mat)
{
	cbResource_.Data()->World = mat;
	cbResource_.Data()->WVP = mat;
}

void Engine::Transfomation::GetCommandList(int index)
{
	cbResource_.SetGraphicsRootConstantBufferView(index);
}

