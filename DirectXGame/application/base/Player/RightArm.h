#pragma once
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object3d.h"
#include"DirectXGame/engine/2d/Sprite.h"
#include"DirectXGame/engine/base/ImGuiManager.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/input/Input.h"



class RightArm
{
public:

	///< summary>
	/// 初期化
	///</summary>
	void Initialize(Camera* camera);

	///< summary>
	/// 更新
	///</summary>
	void Update();

	///< summary>
	/// 描画
	///</summary>
	void Draw();
public:
	// 3Dオブジェクトの参照を返す（変更可能）
	WorldTransform& GetObject3D() { return objectBase_->worldtransform_; }

	Vector3 GetVeloctiy() const { return velo_; }

	// 放っているか
	bool GetIsShot() const { return isShot_; }

	float GetVeloctiyTime_t() const { return (maxVeloTime_ - veloTime_) / maxVeloTime_; }


private:

	void LevelParameters();

	void Levelup();

	void ChargeAttack();



private:
	// 入力初期化
	Input* input_;

	// オブジェクト3D

	std::unique_ptr<Object3d> objectBase_;
	// 本体
	Object3d objectBody_;

	Camera* camera_ = nullptr;


	bool isWallFloor_ = false; // 壁や床に刺さっているか
	bool isTake_ = false; // 使っているか
	bool isShot_ = false; // 放ったか

	bool isObject_ = false; // オブジェクトに刺さっているか

	float  knockback_ = 0;


	// レベル

	int level_ = 1; // 溜めレベル
	float levelUpTime_ = 0; // レベルアップ時間
	float maxLevelUpTime_ = 0.5f; // レベルアップ時間	(最大)


	// 速度
	Vector3 velo_ = {}; // 速度
	Vector3 maxVelo_ = {}; // 速度(最大)
	float  veloTime_ = 0; // 速度時間
	float maxVeloTime_ = 2; // 速度時間(最大)

	


};

