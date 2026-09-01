#pragma once
#include "BaseCamera.h"

/// <summary>CameraManagerへ登録する非所有カメラ情報。</summary>
struct CameraInfo {
	BaseCamera* camera = nullptr; // 呼び出し側が所有し、登録中は有効であること
	bool useCamera = false;       // 登録直後から使用カメラにする場合はtrue
};

/// <summary>
/// カメラマネージャークラス
/// </summary>
class CameraManager
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~CameraManager();

	/// <summary>シーン共通の出力カメラを生成し、描画システムへ設定する。</summary>
	/// <param name="inputSystem">カメラ操作に使用する入力。自動カメラのみの場合はnullptrを許容する。</param>
	/// <param name="entityManager">カメラを設定する描画・エフェクト管理元。利用中は有効であること。</param>
	/// <param name="globalVariables">カメラ調整値の登録・保存先。</param>
	void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables);

	/// <summary>登録カメラ、切り替え補間、共通出力カメラを1フレーム更新する。</summary>
	void Update();
	/// <summary>登録解除と所有カメラの解放を行う。</summary>
	void Finalize();

	/// <summary>呼び出し側が所有するカメラを名前付きで登録する。</summary>
	/// <param name="cameraInfo">登録する非所有カメラと初期使用状態。cameraはnullptr不可。</param>
	/// <param name="name">検索と切り替えに使用する一意な名前。同名登録時は追加されない。</param>
	void AddCamera(const CameraInfo& cameraInfo, const std::string& name);

	/// <summary>指定名のカメラへ切り替える。</summary>
	/// <param name="name">AddCameraで登録したカメラ名。未登録名の場合は何もしない。</param>
	/// <param name="time">切り替え補間時間（秒）。0以下の場合は即時切り替え。</param>
	void SetUseCamera(const std::string& name, float time);
	/// <summary>デバッグ用カメラ編集UIを更新する。</summary>
	void UpdateImGui();
	/// <summary>各描画システムへ渡す共通出力カメラを取得する。</summary>
	/// <returns>CameraManagerが所有するカメラへの非所有ポインター。</returns>
	Engine::Camera* GetCamera() { return camera.get(); }
	/// <summary>現在使用中の登録カメラを取得する。</summary>
	/// <returns>登録元が所有するBaseCameraへの非所有ポインター。使用中カメラがなければnullptr。</returns>
	BaseCamera* GetBaseCamera();
	/// <summary>指定したカメラを登録対象から外す。</summary>
	/// <param name="name">削除する登録名。未登録名の場合は何もしない。</param>
	/// <remarks>カメラの所有権は呼び出し側にあるため、オブジェクト自体は破棄しない。</remarks>
	void DeleteCamera(const std::string& name);
	/// <summary>全カメラを登録対象から外す。登録元が所有するカメラは破棄しない。</summary>
	void Clear() {
		cameras.clear();
	};

private:
	// 呼び出し側が所有する全カメラへの非所有ポインター
	std::map<std::string, BaseCamera*> cameras;
	// カメラ
	std::unique_ptr < Engine::Camera> camera;
	// ゲームでの画面か
	bool isGameCamera = true;

	// カメラ補間状態
	bool isInterpolating = false;
	float interpolationTime = 0.0f;
	float currentTime = 0.0f;


	float chengeTime = 0.1f;

	QuaternionTransform startTransform;
	QuaternionTransform targetTransform;



private:
	Engine::EntityManager* entityManager = nullptr;	// エンティティマネージャー
	Engine::GlobalVariables* globalVariables = nullptr;	// グローバル変数
	InputSystem* inputSystem = nullptr;						// 入力
};
