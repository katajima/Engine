#pragma once
#include"DirectXGame/engine/scene/BaseScene.h"
#include"DirectXGame/engine/scene/SceneManager.h"
#include "DirectXGame/engine/input/Input.h"
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/Camera/DebugCamera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include "DirectXGame/engine/base/Texture/TextureManager.h"
#include "DirectXGame/engine/input/Input.h"
#include "DirectXGame/engine/audio/Audio.h"
#include"DirectXGame/engine/Light/LightCommon.h"
#include"DirectXGame/engine/2d/Sprite.h"
#include"DirectXGame/engine/base/Imgui/ImGuiManager.h"

#include<DirectXGame/engine/Effect/EffectComponent.h>


#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "DirectXGame/engine/base/Load/LoadLevelData.h"



class ResultScene : public Engine::BaseScene {
public:

	// 初期化
	void Initialize() override;

	// 終了
	void Finalize() override;

	// 毎フレーム更新
	void Update() override;

	// 描画3d
	void Draw3D() override;

	// 描画2d
	void Draw2D() override;

private:

};
