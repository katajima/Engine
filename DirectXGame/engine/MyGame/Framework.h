#pragma once
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/input/Input.h"
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include"DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"

#include"DirectXGame/engine/2d/SpriteCommon.h"
#include"DirectXGame/engine/3d/Model/ModelCommon.h"


#include"DirectXGame/engine/Camera/Camera.h"


#include"DirectXGame/engine/effect/Particle/ParticleManager.h"


#include"DirectXGame/engine/Effect/Primitive/PrimitiveCommon.h"


#include"DirectXGame/engine/base/ImGuiManager.h"
#include"DirectXGame/engine/Camera/CameraCommon.h"


#include"DirectXGame/engine/audio/Audio.h"

#include"DirectXGame/engine/Line/LineCommon.h"
#include"DirectXGame/engine/scene/SceneManager.h"
#include"DirectXGame/engine/scene/AbstractSceneFactory.h"
#include "DirectXGame/engine/base/RenderingCommon.h"
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
	Input* input = nullptr;

	// ダイレクトX
	std::unique_ptr <DirectXCommon> dxCommon = nullptr;

	// 3D全般
	std::unique_ptr<Entity3DManager> entity3DManager_;


	// シーンマネージャー
	std::unique_ptr<SceneManager> sceneManager_;


	// ImGuiマネージャー
	ImGuiManager* imguiManager;
	// スプライトこもん
	SpriteCommon* spriteCommon = nullptr;
	
	// モデルコモン
	CameraCommon* cameraCommon = nullptr;
	// ラインコモン
	LineCommon* lineCommon = nullptr;
	

	ParticleManager* particleManager_ = nullptr;
	
	RenderingCommon* renderingCommon = nullptr;

	
	PrimitiveCommon* primitiveCommon = nullptr;

	SkyBoxCommon* skyBoxCommon = nullptr;

	// シーンファクトリー
	std::unique_ptr<AbstractSceneFactory> sceneFactory_;
};

