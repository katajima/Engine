#include "HitBoxEditor.h"
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"

namespace HitBox {

#pragma region HItBoxColl

	// 初期化
	void HitBoxColl::Initialize(Engine::GlobalVariables* globalVariables, BaseCharacter* owner) {
		this->globalVariables = globalVariables;
		this->owner = owner;
		hitBoxSystem_ = owner->GetAttackController()->GetHitBoxSystem();
	};

	// ImGui更新
	void HitBoxColl::ImGuiUpdate(float dt) {
#ifdef _DEBUG
		// ヒットボックス名前
		ImGuiHitBoxName();
		// ヒットボックス使用タイプ
		ImGuiHitBoxUseType();
		// ヒットボックス形状
		ImGuiHitBoxShape();
		// ヒットボックスオフセット
		ImGuiHitBoxOffset();
		// ヒットボックスサイズ
		ImGuiHitBoxSize();
#endif // _DEBUG
	}
	void HitBoxColl::ImGuiHitBoxName() {
#ifdef _DEBUG
		ImGui::Separator();
		ImGui::Text("HitBox : % s", hitBoxName.c_str());
		ImGui::Separator();
#endif // _DEBUG
	}
	void HitBoxColl::ImGuiHitBoxUseType() {
#ifdef _DEBUG
		// enum -> コンボ表示
		static const char* kUseTypeLabels[] = {
			"Player",
			"Enemy",
			"Other",
		};

		int current = static_cast<int>(useType);
		if (ImGui::Combo("UseType", &current, kUseTypeLabels, IM_ARRAYSIZE(kUseTypeLabels))) {
			// 範囲安全（念のため）
			if (0 <= current && current < static_cast<int>(IM_ARRAYSIZE(kUseTypeLabels))) {
				useType = static_cast<UseType>(current);
			}
		}
#endif // _DEBUG
	}
	void HitBoxColl::ImGuiHitBoxShape() {
#ifdef _DEBUG
		static const char* kShapeLabels[] = {
		"OBB",
		"AABB",
		"Sphere",
		};

		int current = static_cast<int>(shape);
		if (ImGui::Combo("Shape", &current, kShapeLabels, IM_ARRAYSIZE(kShapeLabels))) {
			if (0 <= current && current < static_cast<int>(IM_ARRAYSIZE(kShapeLabels))) {
				shape = static_cast<Shape>(current);
			}
		}
#endif // _DEBUG
	}
	void HitBoxColl::ImGuiHitBoxOffset() {
#ifdef _DEBUG
		// X=Right, Y=Up, Z=Forward 前提でそのまま表示
		float v[3] = { offset.x, offset.y, offset.z };
		if (ImGui::DragFloat3("Offset (X,Y,Z)", v, 0.01f)) {
			offset.x = v[0];
			offset.y = v[1];
			offset.z = v[2];
		}
#endif // _DEBUG
	}
	void HitBoxColl::ImGuiHitBoxSize() {
#ifdef _DEBUG
		// Sphere のときは Radius、それ以外は Size を出す
		if (shape == Shape::kSphere) {
			ImGui::DragFloat("Radius", &radius, 0.01f, 0.0f, 10000.0f);
			if (radius < 0.0f) { radius = 0.0f; }
		}
		else {
			float v[3] = { size.x, size.y, size.z };
			if (ImGui::DragFloat3("Size (X,Y,Z)", v, 0.01f, 0.0f, 10000.0f)) {
				size.x = (v[0] < 0.0f) ? 0.0f : v[0];
				size.y = (v[1] < 0.0f) ? 0.0f : v[1];
				size.z = (v[2] < 0.0f) ? 0.0f : v[2];
			}
		}
#endif // _DEBUG
	}


#pragma endregion // ヒットボックスコライダーブロック

#pragma region HitBoxEditor

	void HitBoxEditor::Initialize(Engine::GlobalVariables* globalVariables, BaseCharacter* owner) {
		this->globalVariables = globalVariables;
		this->owner = owner;
		hitBoxSystem_ = owner->GetAttackController()->GetHitBoxSystem();
	}

	void HitBoxEditor::ImGuiUpdate(float dt) {
		ImGui::Begin("Combo Editor");

		// 初回：未選択なら先頭を選択
		if (hitBoxColls_.empty()) {
			selectedHitBoxCollName_ = hitBoxColls_.begin()->first;
		}

		// --- 選択UI（コンボボックス） ---
		{
			const char* preview = selectedHitBoxCollName_.c_str();
			if (ImGui::BeginCombo("Selected Combo", preview)) {

				for (auto& it : hitBoxColls_) {
					const std::string& name = it.first;

					const bool isSelected = (name == selectedHitBoxCollName_);
					if (ImGui::Selectable(name.c_str(), isSelected)) {
						selectedHitBoxCollName_ = name;
					}
					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}
		}

		ImGui::Separator();
		ImGui::Text("Editing: %s", selectedHitBoxCollName_.c_str());

		ImGui::End();
	}

	void HitBoxEditor::CreateHitBoxEditorBlock(const std::string& hitBoxName, const GlobalData& data) {
		// 既に存在する場合は追加しない
		if (hitBoxColls_.find(hitBoxName) != hitBoxColls_.end()) {
			return;
		}

		// コンボエディターブロック作成
		HitBoxColl block;
		block.Initialize(globalVariables, owner);

		// マップに追加
		hitBoxColls_[hitBoxName] = block;
	}

#pragma endregion // ヒットボックスエディター

};
