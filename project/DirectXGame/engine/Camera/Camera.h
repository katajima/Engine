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
#include "DirectXGame/engine/Offscreen/PostEffectBlock.h"
#include "DirectXGame/engine/Base/WinApp/WinApp.h"

// 前方宣言
namespace Engine {
	class PostEffectManager;
	class PostEffectPipeline;
	class PostEffectPass;
	class DirectXCommon;
	class Input;
	class CameraCommon;
	// カメラ
/// <summary>
/// Cameraを管理・実装するクラス。
/// </summary>
	class Camera {
	public: // メンバ関数
		// カメラ共通の既定Farクリップ距離
		static constexpr float kDefaultFarClip = 10000.0f;
		/// <summary>
		/// ウィンドウ既定サイズから求める投影用アスペクト比
		/// </summary>
		static constexpr float kDefaultAspectRatio = static_cast<float>(WinApp::kClientWidth) / static_cast<float>(WinApp::kClientHeight);

		/// <summary>
		/// コンストラクタ
		/// </summary>
		Camera();
		/// <summary>
		/// デストラクタ
		/// </summary>
		~Camera();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(CameraCommon* cameraCommon);
		/// <summary>
		/// 終了処理
		/// </summary>
		void Finalize();

		/// <summary>
		/// コマンドバインド設定
		/// </summary>
		void GetCommandList(int index);

		/// <summary>
		/// 更新
		/// </summary>
		void UpdateMatrix();
		/// <summary>
		/// 更新
		/// </summary>
		void UpdateMatrix(const Vector3& targetPosition);
		/// <summary>
		/// トランスファー更新
		/// </summary>
		void TransferMatrix();

		/// <summary>
		/// 向いている方向
		/// </summary>
		void LookAt(const Vector3& cameraPosition, const Vector3& targetPosition, const Vector3& upVector);


	public: // 取得　設定
		/// <summary>
		/// Fov設定
		/// </summary>
		void SetFovY(const float fovY) { fovY_ = fovY; }
		/// <summary>
		/// アスペクト比設定
		/// </summary>
		void SetAspectRatio(const float aspect) { aspect_ = aspect; }
		/// <summary>
		/// Nearクリップ設定
		/// </summary>
		void SetNearClip(const float nearC) { nearClip_ = nearC; }
		/// <summary>
		/// Farクリップ設定
		/// </summary>
		void SetFarClip(const float farC) { farClip_ = farC; }
		// getter
		/// <summary>
		/// ワールド行列取得
		/// </summary>
		const Matrix4x4& GetWorldMatrix() const { return worldMatrix_; }
		/// <summary>
		/// ビュー行列取得
		/// </summary>
		const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
		/// <summary>
		/// プロジェクション行列取得
		/// </summary>
		const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }
		/// <summary>
		/// ビュープロジェクション行列取得
		/// </summary>
		const Matrix4x4& GetViewProjectionMatrix() const { return viewProjectionMatrix_; }
		/// <summary>
		/// 回転取得
		/// </summary>
		const Vector3& GetRotate() const { return transform_.rotate; }
		/// <summary>
		/// 位置取得
		/// </summary>
		const Vector3& GetTranslate() const { return transform_.translate; }
		/// <summary>
		/// 位置取得
		/// </summary>
		const Vector3& GetScale() const { return transform_.scale; }
		/// <summary>
		/// トランスフォーム取得
		/// </summary>
		const Transform& GetTransform() const { return transform_; }
		/// <summary>
		/// トランスフォーム設定
		/// </summary>
		void SetTransform(const Transform& transform) { transform_ = transform; }
		/// <summary>
		/// 位置設定
		/// </summary>
		void SetTranslate(const Vector3& translate) { transform_.translate = translate; }
		/// <summary>
		/// 回転設定
		/// </summary>
		void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }
		/// <summary>
		/// スケール設定
		/// </summary>
		void SetScale(const Vector3& scale) { transform_.scale = scale; }
		/// <summary>
		/// Nearクリップ取得
		/// </summary>
		const float& GetNearZ() const { return nearClip_; }
		/// <summary>
		/// Farクリップ取得
		/// </summary>
		const float& GetFarZ() const { return farClip_; }
		/// <summary>
		/// FovY取得
		/// </summary>
		const float& GetFovY() const { return fovY_; }
		/// <summary>
		/// aspect取得
		/// </summary>
		const float& GetAspect() const { return aspect_; }


		/// <summary>
		/// 向いている方向取得
		/// </summary>
		Vector3 GetForward() const;
		/// <summary>
		/// プロジェクションしているか取得
		/// </summary>
		bool GetIsProjection() const { return isProjection_; }
		/// <summary>
		/// プロジェクション設定
		/// </summary>
		void SetIsProjection(bool isProjection) { isProjection_ = isProjection; }
	public: // ポストエフェクト
		/// <summary>
		/// レンダーテクスチャ追加
		/// </summary>
		void AddEffectBlock(const std::string name, PostEffectBlockType type, bool use = true);
		/// <summary>
		/// ポストエフェクトをクリア
		/// </summary>
		void Clear();
		/// <summary>
		/// ポストエフェクトのパス取得
		/// </summary>
		PostEffectPass* GetPostEffectPass(size_t index);
		/// <summary>
		/// ポストエフェクトパイプライン取得
		/// </summary>
		PostEffectPipeline* GetPostEffectPipeline() { return postEffectPipeline_.get(); }
		/// <summary>
		/// ポストエフェクトマネージャー取得
		/// </summary>
		PostEffectManager* GetPostEffectManager() { return postEffectManager; }
	private: // デバッグ
		/// <summary>
		/// ImGui更新
		/// </summary>
		void UpdateImGui();
	private:
		bool isProjection_ = true;
		float fovY_ = 0.45f;
		float nearClip_ = 0.1f;
		float farClip_ = kDefaultFarClip;
		float aspect_ = kDefaultAspectRatio;
		Transform transform_;
	private:
		Matrix4x4 worldMatrix_;
		Matrix4x4 viewMatrix_;
		Matrix4x4 projectionMatrix_;
		Matrix4x4 viewProjectionMatrix_;
	private:// ポストエフェクト
		std::unique_ptr<PostEffectPipeline> postEffectPipeline_;
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

