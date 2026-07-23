#pragma once
#include "DirectXGame/engine/Utility/ConvertUtility.h"
#include "DirectXGame/application/base/Special/Base/SpecialData.h"


// 前方宣言
namespace Engine {
	class GlobalVariables;
};
class BaseCharacter;


// 必殺技用のエディター
/// <summary>
/// SpecialEditorを管理・実装するクラス。
/// </summary>
class SpecialEditor {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Engine::GlobalVariables* globalVariable,BaseCharacter* owner);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt);

private:
	Engine::GlobalVariables* globalVariable = nullptr;
	BaseCharacter* owner = nullptr;
};