#pragma once

// 前方宣言
namespace Engine {
	class DirectXCommon;
	class EntityManager;
	class GlobalVariables;
	class SceneManager;
}
class EffectSystem;
class InputSystem;

/// <summary>
/// CustomSystemを管理・実装するクラス。
/// </summary>
class CustomSystem {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="entity3DManager"></param>
	/// <param name="entity2DManager"></param>
	/// <param name="globalVariables"></param>
	void Initialize(Engine::SceneManager* sceneManager, InputSystem* input, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables);
	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>
	void Update(float dt);

private:
	Engine::EntityManager* entityManager = nullptr;
	Engine::GlobalVariables* globalVariables = nullptr;
	Engine::SceneManager* sceneManager = nullptr;
	InputSystem* input = nullptr;
};
