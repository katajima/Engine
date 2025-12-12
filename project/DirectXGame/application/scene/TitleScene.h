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

struct AttackEvent {
	int startFrame;
	int endFrame;
	unsigned int color;
	std::string name;
};

class AttackSequence : public ImSequencer::SequenceInterface {
public:
	std::vector<AttackEvent> events;

	int GetFrameMin() const override { return 0; }
	int GetFrameMax() const override { return 300; }   // 全体フレーム幅
	int GetItemCount() const override { return (int)events.size(); }

	// 開始フレーム、終了フレーム、タイプ、色を返す
	void Get(int index, int** start, int** end, int* type, unsigned int* color) override {
		if (start) *start = &events[index].startFrame;
		if (end)   *end = &events[index].endFrame;
		if (type)  *type = 0;
		if (color) *color = events[index].color;
	}

	const char* GetItemLabel(int index) const override {
		return events[index].name.c_str();
	}

	// [+] 追加ボタンで呼ばれる処理
	void Add(int /*type*/) override {
		events.push_back({ 0, 10, 0xFF00FF00, "NewEvent" });
	}

	void Del(int index) override {
		events.erase(events.begin() + index);
	}

	void Duplicate(int index) override {
		events.push_back(events[index]);
	}
};


/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene : public Engine::BaseScene
{
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
private:
	// リソース初期化
	void InitializeResources();
	// カメラ初期化
	void InitializeCamera();
private:
	Engine::Input* input_ = nullptr;
	Engine::Audio* audio_ = nullptr;


	// カメラ
	std::unique_ptr <Engine::Camera> camera;
	std::unique_ptr < Engine::DebugCamera> debugCamera;


	std::unique_ptr<Engine::Sprite> icon_B;
	std::unique_ptr<Engine::Sprite> title;


	Engine::Object3d tail;

	// ライト
	std::shared_ptr<Engine::DirectionalLight> directional;


	std::unique_ptr<LoadLevelData> loadData_;


	
	AttackSequence attackSeq;
	bool initialized = false;
	int currentFrame = 0;
	int selected = -1;
	bool expanded = true;
	int firstFrame = 0;
};


