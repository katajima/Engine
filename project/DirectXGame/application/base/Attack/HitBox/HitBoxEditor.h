#pragma once
#include "HitBox.h"
#include "DirectXGame/engine/Utility/ConvertUtility.h"

namespace Engine {
	class GlobalVariables;
}
// 前方宣言
class BaseCharacter;
class HitBoxSystem;


/// ヒットボックスコリジョンエディター
class HitBoxColl {
public:

	// 初期化
	void Initialize(Engine::GlobalVariables* globalVariables, BaseCharacter* owner);

	// ImGui更新
	void ImGuiUpdate(float dt);
private:
	// 名前
	void ImGuiHitBoxName();
	// ヒットボックス使用タイプImGui設定
	void ImGuiHitBoxUseType();
	// ヒットボックス形状ImGui設定
	void ImGuiHitBoxShape();
	// ヒットボックスオフセットImGui設定
	void ImGuiHitBoxOffset();
	// ヒットボックスサイズImGui設定
	void ImGuiHitBoxSize();

private:
	std::string hitBoxName = "HitBox";
	HitBoxUseType useType = HitBoxUseType::kPlayer;
	HitBoxShape shape = HitBoxShape::kOBB;
	Vector3 offset{};
	Vector3 size = { 1.0f,1.0f,1.0f };
	float radius = 1.0f;


private:
	Engine::GlobalVariables* globalVariables = nullptr;
	BaseCharacter* owner = nullptr;
	HitBoxSystem* hitBoxSystem_ = nullptr;
};




// ヒットボックスエディター
class HitBoxEditor {
public:

	// 初期化
	void Initialize(Engine::GlobalVariables* globalVariables,BaseCharacter* owner);

	// ImGui更新
	void ImGuiUpdate(float dt);


private:
	// ヒットボックスエディターブロック作成
	void CreateHitBoxEditorBlock(const std::string& hitBoxName,const GlobalHitBoxdata& data);

private:
	std::map<std::string, HitBoxColl> hitBoxColls_;
	std::string selectedHitBoxCollName_ = "";
private: // もらいもの
	Engine::GlobalVariables* globalVariables = nullptr;
	BaseCharacter* owner = nullptr;
	HitBoxSystem* hitBoxSystem_ = nullptr;
};
