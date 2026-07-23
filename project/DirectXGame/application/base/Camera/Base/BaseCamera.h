#pragma once
#include "CameraController.h"
#include <DirectXGame/engine/Camera/Camera.h>
#include <DirectXGame/engine/Transform/WorldTransform/WorldTransform.h>
// 前方宣言
class CameraManager;
class InputSystem;

namespace Engine {
	class EntityManager;
	class GlobalVariables;
}

/// <summary>
/// カメラ基底クラス
/// </summary>
class BaseCamera
{
public:
	virtual ~BaseCamera()
	{
		/// <summary>
		/// 派生カメラが所有する描画カメラのD3Dリソースを、シーン破棄時に確実に解放する
		/// </summary>
		if (uniqueCamera_) {
			uniqueCamera_->Finalize();
		}
	}
	/// <summary>カメラ固有の制御器と描画カメラを初期化する。</summary>
	/// <param name="inputSystem">カメラ操作に使用する入力。自動カメラではnullptrを許容する。</param>
	/// <param name="entityManager">カメラが参照するオブジェクトの管理元。</param>
	/// <param name="globalVariables">カメラ調整値の登録・保存先。</param>
	/// <param name="position">カメラの初期ワールド座標。</param>
	virtual void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables, Vector3 position) = 0;

	///< summary>
	/// 更新
	///</summary>
	virtual void Update() = 0;



	/// <summary>
	/// カメラ取得
	/// </summary>
	/// <returns>このBaseCameraが所有する描画カメラへの非所有ポインター。</returns>
	Engine::Camera* GetUniqueCamera() { return uniqueCamera_.get(); }

	/// <summary>
	///	カメラ統括クラス取得
	/// </summary>
	/// <returns>このBaseCameraが所有する制御器への非所有ポインター。</returns>
	CameraController* GetCameraController() { return controller.get(); }

	/// <summary>
	/// ロックオン対象を設定
	/// </summary>
	/// <param name="object">注視対象のワールド変換。解除する場合はnullptr。</param>
	void LockOn(const Engine::WorldTransform* object) { target = object; };

	/// <summary>
	/// カメラ管理を設定
	/// </summary>
	/// <param name="cameraManager">カメラ切り替えを管理する非所有ポインター。</param>
	void SetCameraManager(CameraManager* cameraManager) { this->cameraManager = cameraManager; };


	/// <summary>
	/// 時間取得
	/// </summary>
	/// <returns>シーンのタイムスケールを反映したフレーム時間。</returns>
	float GetTime() const;

	/// <summary>
	/// 名前設定
	/// </summary>
	void SetName(const std::string& name) { name_ = name; };
	/// <summary>
	/// 名前取得
	/// </summary>
	std::string GetName() const { return name_; }
	/// <summary>
	/// カメラ使用フラグ
	/// </summary>
	bool GetUseCamera() const { return useCamera; }
	/// <summary>
	/// カメラ使用フラグ設定
	/// </summary>
	void SetUseCamera(bool is) { useCamera = is; }

protected:
	void CreateFixedCamera(Engine::EntityManager* entityManager,const Transform& transform, float farClip);

protected:
	// カメラ使用フラグ
	bool useCamera = false;
	// 固有カメラ
	std::unique_ptr<Engine::Camera> uniqueCamera_;	
	// 統括
	std::unique_ptr<CameraController> controller = nullptr;
	// 時間の進む速さ(1.0fが通常、0.0fで停止、2.0fで2倍速など)
	float timeSpeed_ = 1.0f;	
	// 名前
	std::string name_;
protected:
	const Engine::WorldTransform* target = nullptr;		// ロックオン対象
	CameraManager* cameraManager = nullptr;			// カメラ管理
protected:
	Engine::EntityManager* entityManager = nullptr;	// エンティティマネージャー
	Engine::GlobalVariables* globalVariables = nullptr;	// グローバル変数
	InputSystem* inputSystem = nullptr;						// 入力(使わないならnullptr)

};

