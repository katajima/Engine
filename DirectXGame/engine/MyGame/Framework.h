#pragma once
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/input/Input.h"
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include"DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include"DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"



#include"DirectXGame/engine/3d/Model/ModelCommon.h"

#include"DirectXGame/engine/effect/Particle/ParticleManager.h"



#include"DirectXGame/engine/Camera/CameraCommon.h"


#include"DirectXGame/engine/audio/Audio.h"

#include"DirectXGame/engine/Line/LineCommon.h"
#include"DirectXGame/engine/scene/SceneManager.h"
#include"DirectXGame/engine/scene/AbstractSceneFactory.h"
#include"DirectXGame/application/GlobalVariables/GlobalVariables.h"
#include"memory"



// ゲーム全体
class Framework
{
public:
	// 初期化
	virtual void Initialize();

	// 終了
	virtual void Finalize();

	// 毎フレーム更新
	virtual void Update();

	// 描画
	virtual void Draw() = 0;

	// 終了フラグのチェック
	virtual bool IsEndRequst() { return endRequst_; }
	
	virtual ~Framework() = default;

	// 実行
	void Run();

protected:
	// ゲーム終了フラグ
	bool endRequst_ = false;
	// ウィンアップ
	
	std::unique_ptr <WinApp> winApp;
	
	//
	std::unique_ptr<Input> input_ = nullptr;

	// ダイレクトX
	std::unique_ptr <DirectXCommon> dxCommon = nullptr;

	// 3D全般
	std::unique_ptr<Entity3DManager> entity3DManager_;

	// 2D全般
	std::unique_ptr<Entity2DManager> entity2DManager_;

	// シーンマネージャー
	std::unique_ptr<SceneManager> sceneManager_;

	// シーンファクトリー
	std::unique_ptr<AbstractSceneFactory> sceneFactory_;

	

	

	ParticleManager* particleManager_ = nullptr;
		
};

