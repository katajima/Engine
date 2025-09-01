#pragma once
#include "DamageUI.h"
#include "DirectXGame/engine/Utility/VectorUtility.h"


class DamageUIManager {
public:
	void Initialize(Input* input, Entity2DManager* entity2DManager, GlobalVariables* globalVariables);

	void Update();


	void Draw();

	void AddUI(std::string name, float damage, float deleteTime = 1.0f, Vector3 pos = {});

	void SetCamera(Camera* camera) { camera_ = camera; };
private:
	Entity2DManager* entity2DManager_;
	GlobalVariables* globalVariables_;
	Input* input_;
	std::map<std::string,std::unique_ptr<DamageUI>>	damageUi_;
	int count = 0;
	Camera* camera_;
};
