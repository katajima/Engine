#pragma once

namespace Engine {
	class DirectXCommon;
	class EntityManager;
	class GlobalVariables;
	class SceneManager;
}

namespace Character {
	class BaseCharacter;
	struct CharacterContext;
}
class InputSystem;

class PoseSystem {
public:
	PoseSystem() {}
	~PoseSystem() {}
	// 初期化
	void Initialize(Engine::SceneManager* sceneManager, InputSystem* input, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables);
	// 更新
	void Update(float dt);

private:
	Engine::EntityManager* entityManager = nullptr;
	Engine::GlobalVariables* globalVariables = nullptr;
	Engine::SceneManager* sceneManager = nullptr;
	InputSystem* input = nullptr;
};
