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

#include "DirectXGame/engine/Utility/MapUtility.h"

#define ICON_MD_HOME u8"\uE88A"

namespace Engine {
	// 前方宣言
	class Camera;
	class WinApp;
	class DirectXCommon;
	class SrvManager;

	// ImGuiの管理
/// <summary>
/// ImGuiManagerを管理・実装するクラス。
/// </summary>
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

		/// <summary>
		/// ギズモの使用例
		/// </summary>
		void RenderGizmo2(WorldTransform& obj, const Camera* camera, const char* name);

		/// <summary>
		/// ImGuiスタイル設定
		/// </summary>
		void SetCustomColorScheme();
		/// <summary>
		/// インプット設定
		/// </summary>
		void SetInput(Input* input) { this->input = input; }

	public:
		// BeginComboを使用した選択ImGui
		template<typename Type, size_t N>
		static void Select(const std::string& name, const char* const (&label)[N], Type& type);

		// BeginComboでの選択(map<string,Type>)
		template<typename Type>
		static void Select(const std::string& name, std::string& animationName, const std::map<std::string, Type>& map);
		// BeginComboでの選択(unordered_map<string,Type>)
		template<typename Type>
		static void Select(const std::string& name, std::string& animationName, const std::unordered_map<std::string, Type>& map);
		// BeginComboでの選択(unordered_map<string,Type>)
		template<typename Type>
		static void Select(const std::string& name, std::string& animationName, const UnorderedMapContainer<std::string, Type>& map);


	private:
		/// <summary>
		/// ImGuiスタイル
		/// </summary>
		void InitImGuiStyle();
		/// <summary>
		/// AddFont
		/// </summary>
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
		/// <summary>
		/// コンストラクタ
		/// </summary>
		ImGuiWindowScope(const char* name, ImGuiWindowFlags flags = 0) {
			ImGui::Begin(name, nullptr, flags);
		}
		/// <summary>
		/// デストラクタ
		/// </summary>
		~ImGuiWindowScope() {
			ImGui::End();
		}
	};


	// BeginComboを使用した選択ImGui
	template<typename Type, size_t N>
	inline void ImGuiManager::Select(const std::string& name, const char* const (&label)[N], Type& type) {
		int current = static_cast<int>(type);

		/// <summary>
		/// 範囲外アクセス防止
		/// </summary>
		if (current < 0 || current >= static_cast<int>(N)) {
			current = 0;
			type = static_cast<Type>(0);
		}

		if (ImGui::BeginCombo(name.c_str(), label[current])) {
			for (int i = 0; i < static_cast<int>(N); ++i) {
				const bool isSelected = (current == i);

				if (ImGui::Selectable(label[i], isSelected)) {
					type = static_cast<Type>(i);
					current = i;
				}

				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}

	// BeginComboでの選択(map<string,Type>)
	template<typename Type>
	inline void ImGuiManager::Select(const std::string& beginComboName, std::string& animationName, const std::map<std::string, Type>& map) {

		/// <summary>
		/// 初回：未選択なら先頭を選択
		/// </summary>
		if (animationName.empty()) {
			animationName = map.begin()->first;
		}
		/// <summary>
		/// --- 選択UI（コンボボックス） ---
		/// </summary>
		const char* preview = animationName.c_str();
		if (ImGui::BeginCombo(beginComboName.c_str(), preview)) {

			for (auto& it : map) {
				const std::string& name = it.first;

				const bool isSelected = (name == animationName);
				if (ImGui::Selectable(name.c_str(), isSelected)) {
					animationName = name;
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}
	}
	template<typename Type>
	inline void ImGuiManager::Select(const std::string& beginComboName, std::string& animationName, const std::unordered_map<std::string, Type>& map){
		/// <summary>
		/// 初回：未選択なら先頭を選択
		/// </summary>
		if (animationName.empty()) {
			animationName = map.begin()->first;
		}
		/// <summary>
		/// --- 選択UI（コンボボックス） ---
		/// </summary>
		const char* preview = animationName.c_str();
		if (ImGui::BeginCombo(beginComboName.c_str(), preview)) {

			for (auto& it : map) {
				const std::string& name = it.first;

				const bool isSelected = (name == animationName);
				if (ImGui::Selectable(name.c_str(), isSelected)) {
					animationName = name;
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}
	}
	template<typename Type>
	inline void ImGuiManager::Select(const std::string& beginComboName, std::string& animationName, const UnorderedMapContainer<std::string, Type>& map) {
		/// <summary>
		/// 初回：未選択なら先頭を選択
		/// </summary>
		if (animationName.empty()) {
			animationName = map.begin()->first;
		}
		/// <summary>
		/// --- 選択UI（コンボボックス） ---
		/// </summary>
		const char* preview = animationName.c_str();
		if (ImGui::BeginCombo(beginComboName.c_str(), preview)) {

			for (auto& it : map) {
				const std::string& name = it.first;

				const bool isSelected = (name == animationName);
				if (ImGui::Selectable(name.c_str(), isSelected)) {
					animationName = name;
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}
	}
}


