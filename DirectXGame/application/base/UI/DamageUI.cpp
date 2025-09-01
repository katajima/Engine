#include "DamageUI.h"


void DamageUI::Initialize(Input* input, Entity2DManager* entity2DManager, GlobalVariables* globalVariables) {
	entity2DManager_ = entity2DManager;
	globalVariables_ = globalVariables;
	input_ = input;

	worldTransform_.Initialize();


};

void DamageUI::Update()
{
	
	if (deleteTime < time_) {}
	else {
		worldTransform_.translate_ = pos;

		GetUICount(name)->SetPos(GetScreenPos(worldTransform_, camera_));
		//GetUICount(name)->SetColor({1,1,1,Lerp(1,0,time_ / deleteTime)});
		time_ += 1.0f/ 60.0f;
	}
	worldTransform_.Update();
	UpdateUIElement(0);
}

void DamageUI::Draw() 
{
	if (deleteTime > time_) {
		DrawUIElement();
	}
}

void DamageUI::AddDamegeUI(std::string name, float time, float damage,Vector3 pos)
{
	this->pos = pos;
	this->name = name;
	worldTransform_.translate_ = pos;
	worldTransform_.Update();
	InitUICount(name, GetScreenPos(worldTransform_, camera_), 3);
	deleteTime = time;
	GetUICount(name)->SetMaxSize({ 64 , 96  }, { 20.0f,0.0f });
	GetUICount(name)->SetTextuerSize({ 64,96 });
	GetUICount(name)->SetCountMax(999);
	GetUICount(name)->SetCount(damage);
	size = { 64 , 96  };
}
