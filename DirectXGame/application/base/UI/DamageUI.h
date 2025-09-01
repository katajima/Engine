#pragma once
#include "DirectXGame/application/base/BaseClass/UI/BaseUI.h"


class DamageUI : public BaseUI {
public:
	void Initialize(Input* input, Entity2DManager* entity2DManager, GlobalVariables* globalVariables) override;

	void Update() override;


	void Draw() override;


	void SetDeleteTime(float time) { deleteTime = time; }

	bool IsDelete() const { return time_ > deleteTime; }

	void AddDamegeUI(std::string name, float time, float damage,Vector3 pos);

	void SetCamera(Camera* camera) { camera_ = camera; };

private:
	float deleteTime = 0.0f;
	float time_ = 0.0f;
	std::string name = "";
	WorldTransform worldTransform_;
	Camera* camera_;
	Vector3 pos;
	Vector2 size = { 1.0f,1.0f };
};