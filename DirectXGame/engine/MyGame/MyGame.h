#pragma once
#include<cstdint>
#include<string>
#include<format>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cassert>
#include<dxgidebug.h>
#include<dxcapi.h>
#include<fstream>
#include<sstream>
#include<wrl.h>
#include<vector>


#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")


#include"externals/imgui/imgui.h"

#include"Framework.h"

#include "DirectXGame/application/scene/SceneFactory.h"



class  MyGame : public Framework
{
public:
	
	// 初期化
	void Initialize() override;

	// 終了
	void Finalize() override;

	// 毎フレーム更新
	void Update() override;

	// 描画
	void Draw() override;

	void InitializeResource();

public:
	static float const kDeltaTime_;
	static float kTimeSpeed_;
	static float GameTime() { return kDeltaTime_ * kTimeSpeed_; };
	static void hitStop(float time) { hitStopTimer = time; };
	static float hitStopTimer;
private:
	void HitStpoTime();

private:
	std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();

	float fps = 0.0f;
	
	
	std::unique_ptr<Primitive> primi = nullptr;
	std::unique_ptr<Primitive> primiPlane = nullptr;
	std::unique_ptr<Primitive> primiStar = nullptr;
	std::unique_ptr<Primitive> primiTrai = nullptr;
	

private:
	// ゲーム終了フラグ
	bool endRequst_ = false;
};

