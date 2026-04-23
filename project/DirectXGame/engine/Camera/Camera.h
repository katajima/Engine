#pragma once
#include "DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFunctions.h"

#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
using namespace Microsoft::WRL;
#include<d3d12.h>
#include<dxgi1_6.h>
#include "DirectXGame/engine/Offscreen/PostEffectData.h"
#include "DirectXGame/engine/Offscreen/PostEffect.h"

// 前方宣言
namespace Engine {
	class PostEffectManager;
	class PostEffectBlock;
	class DirectXCommon;
	class Input;
	class CameraCommon;
	// カメラ
	class Camera {
	public: // メンバ関数
		// コンストラクタ
		Camera();

		// 初期化
		void Initialize(CameraCommon* cameraCommon);

		// コマンドバインド設定
		void GetCommandList(int index);

		// 更新
		void UpdateMatrix();
		// 更新
		void UpdateMatrix(const Vector3& targetPosition);
		// トランスファー更新
		void TransferMatrix();

		// 向いている方向
		void LookAt(const Vector3& cameraPosition, const Vector3& targetPosition, const Vector3& upVector);
		
		
	public: // 取得　設定
		// Fov設定
		void SetFovY(const float fovY) { fovY_ = fovY; }
		// アスペクト比設定
		void SetAspectRatio(const float aspect) { aspect_ = aspect; }
		// Nearクリップ設定
		void SetNearClip(const float nearC) { nearClip_ = nearC; }
		// Farクリップ設定
		void SetFarClip(const float farC) { farClip_ = farC; }
		// getter
		// ワールド行列取得
		const Matrix4x4& GetWorldMatrix() const { return worldMatrix_; }
		// ビュー行列取得
		const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
		// プロジェクション行列取得
		const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }
		// ビュープロジェクション行列取得
		const Matrix4x4& GetViewProjectionMatrix() const { return viewProjectionMatrix_; }
		// 回転取得
		const Vector3& GetRotate() const { return transform_.rotate; }
		// 位置取得
		const Vector3& GetTranslate() const { return transform_.translate; }
		// 位置取得
		const Vector3& GetScale() const { return transform_.scale; }
		// トランスフォーム取得
		const Transform& GetTransform() const { return transform_; }
		// トランスフォーム設定
		void SetTransform(const Transform& transform) { transform_ = transform; }
		// 位置設定
		void SetTranslate(const Vector3& translate) { transform_.translate = translate; }
		// 回転設定
		void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }
		// スケール設定
		void SetScale(const Vector3& scale) { transform_.scale = scale; }
		// Nearクリップ取得
		const float& GetNearZ() const { return nearClip_; }
		// Farクリップ取得
		const float& GetFarZ() const { return farClip_; }
		// FovY取得
		const float& GetFovY() const { return fovY_; }
		// aspect取得
		const float& GetAspect() const { return aspect_; }


		// 向いている方向取得
		Vector3 GetForward() const;
		// プロジェクションしているか取得
		bool GetIsProjection() const { return isProjection_; }
		// プロジェクション設定
		void SetIsProjection(bool isProjection) { isProjection_ = isProjection; }
	public: // ポストエフェクト
		// レンダーテクスチャ追加
		void AddEffectBlock(const std::string name, PostEffectBlockType type, bool use = true);
		// ポストエフェクトをクリア
		void Clear() { effectBlocks_.clear(); }
		// ポストエフェクトのブロック取得
		std::vector<Engine::PostEffectBlock*> GetPostEffectBlocks();
		// ポストエフェクトマネージャー取得
		PostEffectManager* GetPostEffectManager() { return postEffectManager; }
	private: // デバッグ
		// ImGui更新
		void UpdateImGui();
	private:
		bool isProjection_ = true;
		float fovY_ = 0.45f;
		float nearClip_ = 0.1f;
		float farClip_ = 10000.0f;
		float aspect_ = float(1280) / float(720);
		Transform transform_;
	private:
		Matrix4x4 worldMatrix_;
		Matrix4x4 viewMatrix_;
		Matrix4x4 projectionMatrix_;
		Matrix4x4 viewProjectionMatrix_;
	private:// ポストエフェクト
		std::vector<std::unique_ptr<PostEffectBlock>> effectBlocks_;
	private: // GPU
		// GPUデータ
		struct DataGPU {
			Vector3 worldPosition;
			float padding[1];
			Vector3 normal;
			float padding2[1];
		};
		// GPUに送るデータ
		DataGPU* data;
		// リソース
		Microsoft::WRL::ComPtr < ID3D12Resource> resource;
	private: // 貰いもの
		// ダイレクトX共通クラス
		DirectXCommon* dxCommon = nullptr;
		// ポストエフェクト管理
		PostEffectManager* postEffectManager = nullptr;
		// 入力
		Input* input = nullptr;
	};
}

