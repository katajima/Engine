#pragma once
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include "CameraController.h"

// 前方宣言
class CameraManager;

/// <summary>
/// カメラ基底クラス
/// </summary>
class BaseCamera
{
public:
	virtual ~BaseCamera() = default; 
	///< summary>
	/// 初期化
	///</summary>
	virtual void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables, Vector3 position) = 0;

	///< summary>
	/// 更新
	///</summary>
	virtual void Update() = 0;



	/// <summary>
	/// カメラ取得
	/// </summary>
	/// <returns></returns>
	Engine::Camera* GetUniqueCamera() { return uniqueCamera_.get(); }


	/// <summary>
	/// ロックオン対象を設定
	/// </summary>
	/// <param name="object"></param>
	void LockOn(Engine::Object3d* object) { lockOnObject = object; };

	/// <summary>
	/// カメラ管理を設定
	/// </summary>
	/// <param name="cameraManeger"></param>
	void SetCameraManeger(CameraManager* cameraManeger) { this->cameraManeger = cameraManeger; };


	/// <summary>
	/// 時間取得
	/// </summary>
	/// <returns></returns>
	float GetTime() const;

	//名前設定
	void SetName(const std::string& name) { name_ = name; };
	// 名前取得
	std::string GetName() const { return name_; }
	// カメラ使用フラグ
	bool GetUseCamera() const { return useCamera; }
	// カメラ使用フラグ設定
	void SetUseCamera(bool is) { useCamera = is; }
protected:
	bool useCamera = false;

	std::unique_ptr<Engine::Camera> uniqueCamera_;	// 固有カメラ
	
	float timeSpeed_ = 1.0f;	// 時間の進む速さ(1.0fが通常、0.0fで停止、2.0fで2倍速など)
	std::string name_;
protected:
	Engine::Object3d* lockOnObject = nullptr;		// ロックオン対象
	CameraManager* cameraManeger = nullptr;	// カメラ管理
protected:
	Engine::EntityManager* entityManager = nullptr;	// エンティティマネージャー
	Engine::GlobalVariables* globalVariables = nullptr;	// グローバル変数
	InputSystem* inputSystem = nullptr;						// 入力(使わないならnullptr)

};

