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

	// パーティクル生成
	void CreateParticle();

	//
	void LoadModel();

public:
	static float const kDeltaTime_;
	static float kTimeSpeed_;
	static float GameTime() { return kDeltaTime_ * kTimeSpeed_; };
	static float NowTime() { return nowTime; };
	static float nowTime;
private:
	std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();

	float fps = 0.0f;
	
	DebugTimer debugTimer_;

	std::unique_ptr<TorusPrimitive> primi = nullptr;
	std::unique_ptr<PlanePrimitive> primiPlane = nullptr;
	std::unique_ptr<StarPrimitive> primiStar = nullptr;
	std::unique_ptr<TrianglePrimitive> primiTrai = nullptr;
	
private:
	// ゲーム終了フラグ
	bool endRequst_ = false;
};

