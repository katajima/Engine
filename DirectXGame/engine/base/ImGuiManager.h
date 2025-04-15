#pragma once
#include"externals/imgui/imgui_impl_dx12.h"
#include"externals/imgui/imgui_impl_win32.h"
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
#include<wrl.h>

#include"DirectXGame/engine/Manager/SRV/SrvManager.h"
#include "ImGuizmo.h"
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include "DirectXGame/engine/WorldTransform/WorldTransform.h"
#include "DirectXGame/engine/input/Input.h"
#include "DirectXGame/engine/Camera/Camera.h"

// 前方宣言
class WinApp;
class DirectXCommon;
// ImGuiの管理
class ImGuiManager
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* dxCommon);

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// ImGui受付開始
	/// </summary>
	void Begin();

	/// <summary>
	/// ImGui受付終了
	/// </summary>
	void End();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	// ギズモの使用例
	void RenderGizmo2(WorldTransform& obj, const Camera& camera, const char* name);

	void SetCustomColorScheme();

	void SetInput(Input* input) { input_ = input; }

private:

	// DirectX基盤インスタンス（借りてくる）
	DirectXCommon* dxCommon_ = nullptr;
	// SRV用ヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;

	Input* input_;

	Input::FlagXYZ flag;

};



