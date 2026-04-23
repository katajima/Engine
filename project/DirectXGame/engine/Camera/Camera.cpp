#include "Camera.h"
#include "DirectXGame/engine/base/WinApp/WinApp.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/engine/Camera/CameraCommon.h"
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "DirectXGame/engine/input/Input.h"

#include "DirectXGame/engine/Offscreen/PostEffectBlock.h"
#include "DirectXGame/engine/Offscreen/PostEffect.h"

Engine::Camera::Camera()

	:transform_({ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} })
	, fovY_(0.45f)
	, aspect_(static_cast<float>(WinApp::GetClientWidth()) / static_cast<float>(WinApp::GetClientHeight()))
	, nearClip_(0.1f)
	, farClip_(1000.0f)
	, worldMatrix_(MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate))
	, viewMatrix_(Inverse(worldMatrix_))
	, projectionMatrix_(MakePerspectiveFovMatrix(fovY_, aspect_, nearClip_, farClip_))
	, viewProjectionMatrix_(Multiply(viewMatrix_, projectionMatrix_))

{



}

void Engine::Camera::Initialize(CameraCommon* cameraCommon) {
	dxCommon = cameraCommon->GetDxCommon();				// DX共通クラス
	postEffectManager = dxCommon->GetPostEffectManager();	// ポストエフェクト管理クラス
	input = cameraCommon->GetInput();		// インプット

	// リソース生成
	resource = dxCommon->GetDXGIDevice()->CreateBufferResource(sizeof(DataGPU));
	//書き込むためのアドレスを取得
	resource->Map(0, nullptr, reinterpret_cast<void**>(&data));

	data->worldPosition = Vector3{ 1.0f,1.0f,1.0f };
	data->normal = { 0,0,0 };
	isProjection_ = true;
}

void Engine::Camera::GetCommandList(int index) {
	// Cameraのバインド
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(index, resource->GetGPUVirtualAddress());
}


#pragma region Update

void Engine::Camera::UpdateMatrix() {
	float winWidth = static_cast<float>(WinApp::GetClientWidth(false));
	float winHeight = static_cast<float>(WinApp::GetClientHeight(false));
#ifndef _DEBUG
	winWidth = static_cast<float>(WinApp::GetClientWidth());
	winHeight = static_cast<float>(WinApp::GetClientHeight());
#endif // _DEBUG
	aspect_ = winWidth / winHeight;

	UpdateImGui();

	// カメラのワールド行列を計算
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	// ビュー行列をカメラのワールド行列の逆行列として計算
	viewMatrix_ = Inverse(worldMatrix_);
	// 射影行列を計算
	if (isProjection_) {
		projectionMatrix_ = MakePerspectiveFovMatrix(fovY_, aspect_, nearClip_, farClip_);
	}
	else {
		projectionMatrix_ = MakeOrthographicMatrix2(
			-winWidth * 0.5f,
			winWidth * 0.5f,
			-winHeight * 0.5f,
			winHeight * 0.5f,
			nearClip_, farClip_);
	}
	// ビュー・プロジェクション行列を更新
	viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
	// カメラデータの更新
	Vector3 cameraFront(viewMatrix_.m[0][2], viewMatrix_.m[1][2], viewMatrix_.m[2][2]);
	data->normal = Normalize(cameraFront); // 必要なら正規化
	data->worldPosition = worldMatrix_.GetWorldPosition();
}

void Engine::Camera::UpdateMatrix(const Vector3& targetPosition) {
	// カメラとターゲットの距離を設定
	float distanceFromTarget = 2.0f; // 適切な距離に調整
	Vector3 directionToTarget = Normalize(Subtract(targetPosition, transform_.translate));

	// カメラの位置をターゲットから一定距離後ろに配置
	transform_.translate = Subtract(targetPosition, Multiply(directionToTarget, distanceFromTarget));

	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);




	// 射影行列の再計算
	projectionMatrix_ = MakePerspectiveFovMatrix(fovY_, aspect_, nearClip_, farClip_);

	// ビュー・プロジェクション行列を更新
	viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
}

void Engine::Camera::UpdateImGui(){}

void Engine::Camera::TransferMatrix() {
	// ビュー行列の逆行列を計算してカメラの位置を抽出
	Matrix4x4 iView = Inverse(worldMatrix_);
	transform_.translate = { iView.m[3][0], iView.m[3][1], iView.m[3][2] };

	// ビュー・プロジェクション行列を計算
	viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
}

void Engine::Camera::LookAt(const Vector3& cameraPosition, const Vector3& targetPosition, const Vector3& upVector) {
	Vector3 forward = Normalize(Subtract(targetPosition, cameraPosition)); // 前方向ベクトル
	Vector3 right = Normalize(Cross(upVector, forward)); // 右方向ベクトル
	Vector3 up = Cross(forward, right); // 上方向ベクトル

	// カメラの回転行列を設定
	transform_.rotate.x = std::atan2(forward.y, std::sqrt(forward.x * forward.x + forward.z * forward.z));
	transform_.rotate.y = std::atan2(forward.x, forward.z);

	// ワールド行列の更新
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, cameraPosition);

	// ビュー行列を更新
	viewMatrix_ = Inverse(worldMatrix_);
	projectionMatrix_ = MakePerspectiveFovMatrix(fovY_, aspect_, nearClip_, farClip_);
	viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
}

#pragma endregion // 更新

Vector3 Engine::Camera::GetForward() const {
	float cp = std::cos(transform_.rotate.x);
	float sp = std::sin(transform_.rotate.x);
	float cy = std::cos(transform_.rotate.y);
	float sy = std::sin(transform_.rotate.y);

	return Vector3(
		sy * cp, // X
		sp,      // Y
		cy * cp  // Z
	);
}

#pragma region MyRegion

void Engine::Camera::AddEffectBlock(const std::string name, PostEffectBlockType type, bool use)
{
	auto effectBlock = std::make_unique<Engine::PostEffectBlock>();
	effectBlock->Intialize(dxCommon->GetDXGIDevice(), dxCommon->GetCommand(), dxCommon->GetSrvManager(), dxCommon->GetRtvManager(),
		dxCommon->GetRenderingCommon(), dxCommon->GetDepthStencil(), dxCommon->GetBarrier(), dxCommon->GetScissorRect(), dxCommon->GetViewPort(), name, type);
	effectBlock->SetUse(use);			// 使うか
	effectBlock->SetIndex(0); // 順番
	effectBlocks_.push_back(std::move(effectBlock));
	//indexCount_++; // 加算
}

std::vector<Engine::PostEffectBlock*> Engine::Camera::GetPostEffectBlocks()
{
	std::vector<PostEffectBlock*> rawPtrs;
	rawPtrs.reserve(effectBlocks_.size());
	for (auto& block : effectBlocks_) {
		rawPtrs.push_back(block.get());
	}
	return rawPtrs;
}

#pragma endregion // ポストエフェクト

