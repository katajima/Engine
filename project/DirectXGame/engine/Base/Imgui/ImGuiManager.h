#pragma once
#include"externals/imgui/imgui_impl_dx12.h"
#include"externals/imgui/imgui_impl_win32.h"
#include "imgui_internal.h"
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
#include<wrl.h>

#include"DirectXGame/engine/Manager/SRV/SrvManager.h"

#define IMOGIZMO_LEFT_HANDED
//#define IMOGIZMO_Z_UP;

#include "ImGuizmo.h"
#include "externals/imgui/ImSequencer.h"


#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFunctions.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"
#include "DirectXGame/engine/input/Input.h"



#define ICON_MD_HOME u8"\uE88A"

namespace Engine {
	// 前方宣言
	class Camera;
	class WinApp;
	class DirectXCommon;
	class SrvManager;

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
		void RenderGizmo2(WorldTransform& obj, const Camera* camera, const char* name);

		// ImGuiスタイル設定
		void SetCustomColorScheme();
		// インプット設定
		void SetInput(Input* input) { this->input = input; }

	public:

	private:
		// ImGuiスタイル
		void InitImGuiStyle();
		// AddFont
		void AddFont();


	private:

		// DirectX基盤インスタンス（借りてくる）
		DirectXCommon* dxCommon = nullptr;
		// SRV管理クラス
		SrvManager* srvManager = nullptr;
		// 入力クラス
		Input* input = nullptr;

		Input::FlagXYZ flag;

		uint32_t srvIndex = 0;

	};

	/// <summary>
	/// ImGuiウィンドウのスコープを管理するRAIIクラス。
	/// </summary>
	class ImGuiWindowScope {
	public:
		// コンストラクタ
		ImGuiWindowScope(const char* name, ImGuiWindowFlags flags = 0) {
			ImGui::Begin(name, nullptr, flags);
		}
		// デストラクタ
		~ImGuiWindowScope() {
			ImGui::End();
		}
	};
}


