#include"ImGuiManager.h"
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "DirectXGame/engine/Manager/SRV/SrvManager.h"
#include "DirectXGame/engine/input/Input.h"
#include "DirectXGame/engine/Base/WinApp/WinApp.h"
#include "DirectXGame/engine/Camera/Camera.h"
#include <iostream>//用いるヘッダファイルが変わります。

// 日本語対応用
#include "ImGuiJapaneseFont.h"

#include "imgui.h"

// ギズモの操作モード
static ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE; // 初期値は移動


void Engine::ImGuiManager::Initialize(DirectXCommon* dxCommon)
{
#ifdef _DEBUG
	this->dxCommon = dxCommon;						// DX共通クラス
	this->srvManager = dxCommon->GetSrvManager();	// SRV管理クラス

	// コンテキストの生成
	ImGui::CreateContext();
	// Imguiのスタイルを設定 黒
	ImGui::StyleColorsDark();

	// ImGuiスタイル設定
	InitImGuiStyle();

	// プラットフォームとレンダラーのバックエンドを設定する
	ImGui_ImplWin32_Init(WinApp::GetHwnd());
	ImGui_ImplDX12_Init(
		dxCommon->GetDevice().Get(), static_cast<int>(dxCommon->GetBackBufferCount()),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, srvManager->GetDescriptorHeap(),
		srvManager->GetCPUDescriptorHandle(0),
		srvManager->GetGPUDescriptorHandle(0));


	
	// フォント追加
	AddFont();
#endif // _DEBUG

}

void Engine::ImGuiManager::Finalize()
{
#ifdef _DEBUG
	// 後始末
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

#endif // _DEBUG
}

void Engine::ImGuiManager::Begin()
{
#ifdef _DEBUG
	// ImGuiフレーム開始
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// 
	ImGuiViewport* vi = ImGui::GetMainViewport();
	ImGui::DockSpaceOverViewport(0, vi);

	ImGuizmo::BeginFrame();

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)WinApp::GetClientWidth(),
		(float)WinApp::GetClientHeight());
#endif // _DEBUG
}

void Engine::ImGuiManager::End()
{
#ifdef _DEBUG
	// 描画前準備
	ImGui::Render();
#endif // _DEBUG
}

void Engine::ImGuiManager::Draw()
{
#ifdef _DEBUG

	// 描画コマンドを発行
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList().Get());
#endif // _DEBUG
}



void Engine::ImGuiManager::RenderGizmo2(WorldTransform& obj, const Camera* camera, const char* name)
{
	if (ImGui::BeginTabBar("Gizmo"))
	{
		if (ImGui::BeginTabItem(name))
		{

			

			static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
			static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

			// ギズモを操作
			bool isManipulated = ImGuizmo::Manipulate(
				&camera->GetViewMatrix().m[0][0],
				&camera->GetProjectionMatrix().m[0][0],
				mCurrentGizmoOperation,
				mCurrentGizmoMode,
				&obj.worldMat_.m[0][0]);


			if (isManipulated) {
				std::cout << "Manipulate succeeded" << std::endl; // オブジェクトの行列を取得 
				float translation[3], rotation[3], scale[3];
				ImGuizmo::DecomposeMatrixToComponents(&obj.worldMat_.m[0][0], translation, rotation, scale);
				obj.translate_ = Vector3(translation[0], translation[1], translation[2]);
				obj.rotate_ = Vector3(Math::DegreesToRadians(rotation[0]), Math::DegreesToRadians(rotation[1]), Math::DegreesToRadians(rotation[2]));
				obj.scale_ = Vector3(scale[0], scale[1], scale[2]);
				std::cout << "Translation: " << translation[0] << ", " << translation[1] << ", " << translation[2] << std::endl;
				std::cout << "Rotation: " << rotation[0] << ", " << rotation[1] << ", " << rotation[2] << std::endl;
				std::cout << "Scale: " << scale[0] << ", " << scale[1] << ", " << scale[2] << std::endl;
			}
			else {
				std::cout << "Manipulate failed" << std::endl;
			}
			if (ImGuizmo::IsUsing()) {
				std::cout << "ImGuizmo is using" << std::endl;
			}
			else {
				std::cout << "ImGuizmo is not using" << std::endl;
			}

			if (ImGui::Button("TRANSLATE")) mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
			if (ImGui::Button("ROTATE")) mCurrentGizmoOperation = ImGuizmo::ROTATE;
			if (ImGui::Button("SCALE")) mCurrentGizmoOperation = ImGuizmo::SCALE;

			if (input->IsTriggerKey(DIK_G)) {
				mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
			}
			if (input->IsTriggerKey(DIK_R)) {
				mCurrentGizmoOperation = ImGuizmo::ROTATE;
			}
			if (input->IsTriggerKey(DIK_S)) {
				mCurrentGizmoOperation = ImGuizmo::SCALE;
			}

			if (mCurrentGizmoOperation != ImGuizmo::SCALE) {
				if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
					mCurrentGizmoMode = ImGuizmo::LOCAL;
				ImGui::SameLine();
				if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
					mCurrentGizmoMode = ImGuizmo::WORLD;
			}

			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
			ImGuizmo::Enable(true);




			bool over = ImGuizmo::IsOver();
			ImGui::Checkbox("IsOver", &over);
			bool isUse = ImGuizmo::IsUsing();
			ImGui::Checkbox("IsUsing", &isUse);

			if (ImGuizmo::IsUsing()) {
				std::cout << "ImGuizmo is using" << std::endl;
				// オブジェクトの行列を取得
				float translation[3], rotation[3], scale[3];
				ImGuizmo::DecomposeMatrixToComponents(&obj.worldMat_.m[0][0], translation, rotation, scale);
				Vector3 matrixTranslation = { translation[0], translation[1], translation[2] };
				Vector3 matrixRotation = { rotation[0], rotation[1], rotation[2] };
				Vector3 matrixScale = { scale[0], scale[1], scale[2] };
				ImGui::InputFloat3("Tr", &matrixTranslation.x);
				ImGui::InputFloat3("Rt", &matrixRotation.x);
				ImGui::InputFloat3("Sc", &matrixScale.x);
				ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, &obj.worldMat_.m[0][0]);
			}
			else {
				std::cout << "ImGuizmo is not using" << std::endl;
			}

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

void Engine::ImGuiManager::SetCustomColorScheme()
{
	ImGuiStyle& style = ImGui::GetStyle();

	// 背景色
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);  // ダークグレー

	// ボタンの色
	style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.7f, 0.2f, 1.0f);  // 緑
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.4f, 1.0f, 0.4f, 1.0f);  // 明るい緑
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.1f, 0.5f, 0.1f, 1.0f);  // ダーク緑

	// ヘッダーの色
	style.Colors[ImGuiCol_Header] = ImVec4(0.2f, 0.3f, 0.8f, 1.0f);  // 青
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.4f, 0.5f, 1.0f, 1.0f);  // 明るい青
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.1f, 0.2f, 0.6f, 1.0f);  // ダーク青
}

void Engine::ImGuiManager::InitImGuiStyle()
{
	

	ImGuiStyle& style = ImGui::GetStyle();

	// ウィンドウの背景色（通常のウィンドウ部分全体の背景）
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

	// 折りたたみ可能なヘッダー（ツリー、カテゴリ、CollapsingHeaderなど）の背景色（通常時）
	style.Colors[ImGuiCol_Header] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);

	// 折りたたみ可能なヘッダーにマウスホバーしているときの背景色
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.5f, 0.8f, 1.0f);

	// 折りたたみ可能なヘッダーを押してアクティブになったときの背景色
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.45f, 0.75f, 1.0f);

	// ボタンの背景色（通常時）
	style.Colors[ImGuiCol_Button] = ImVec4(1.0f, 0.271f, 0.0f, 1.0f);

	// ボタンにマウスホバーしているときの背景色
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.5f, 0.8f, 1.0f);

	// ボタンを押しているとき（アクティブ）の背景色
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 0.45f, 0.75f, 1.0f);

	// 入力欄、チェックボックス、スライダーなどのフレーム背景色（通常時）
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
}

void Engine::ImGuiManager::AddFont()
{
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/msgothic.ttc", 10.0f, nullptr, glyphRangesJapanese);

	
	// アイコンフォントの範囲（Material Icons のコードポイント範囲）
	static const ImWchar icon_ranges[] = { 0xe000, 0xf8ff + 1, 0 };  // Private Use Area



	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.GlyphMinAdvanceX = 13.0f; // お好みで調整
	icons_config.OversampleH = 1;
	icons_config.OversampleV = 1;

	io.Fonts->AddFontFromFileTTF("./externals/resources/GoogleMaterialFont/MaterialIcons-Regular.ttf", 10.0f, &icons_config, icon_ranges);
	io.ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable;



}






