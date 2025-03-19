#pragma once
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object3d.h"
#include"DirectXGame/engine/2d/Sprite.h"
#include"DirectXGame/engine/base/ImGuiManager.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/input/Input.h"

class Player;
class LeftArm
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

	// 飛糸の位置
	Vector3 GetThreadPosition() { return objectThread_->worldtransform_.worldMat_.GetWorldPosition(); }
	Vector3 GetAttracPosition() const { return attracPos_; }
	Vector3 GetVeloctiy() const { 
		Vector3 result{
			veloTotal_.x,
			veloTotal_.y,
			veloTotal_.z
		};
		
		
		return result;
	}
	

	void SetPlayer(Player* player) { player_ = player; }

	// 引き寄せるか
	bool& GetIsAttract() { return isAttract_; } 
	float GetAttracTime_t() const { return (maxAttracTime_ - attracTime_) / maxAttracTime_; }

	// 途中引き戻し
	bool& GetIsInTheMiddle() { return isInTheMiddle_;}

private:
	void Shot();

private:
	Player* player_;

	// 入力初期化
	Input* input_;

	// オブジェクト3D

	std::unique_ptr<Object3d> objectBase_;


	// 糸
	std::unique_ptr<Object3d> objectThread_;

	Camera* camera_ = nullptr;


	bool isWallFloor_ = false; // 床や壁に刺さっているのか
	bool isTake_ = false; // 使っているのか
	bool isEnemy_ = false; // 敵に刺さっているのか

	

	// 発射系
	bool isShot_ = false; // 発射するか
	float shotTime_ = 0; // 発射時間
	float maxShotTime_ = 0.25f; // 発射時間(最大)
	float shotSpeed = 12.0f; // 撃つスピード
	Vector3 shotVelo_ = {}; // 発射速度

	// 引き寄せ系
	bool isAttract_ = false; // 引き寄せているか
	float attracTime_ = 0; // 引き寄せ時間
	float maxAttracTime_ = 1; // 引き寄せ時間(最大)
	Vector3 attracPos_{}; // 引き寄せた場所

	// 戻し系
	bool isRemove_ = false; // 戻っているか
	Vector3 removeVelo_ = {}; // 戻る速度
	float removeTime_ = 0; // 引き寄せ時間
	float maxRemoveTime_ = 0.5f; // 引き寄せ時間(最大)
	Vector3 removePos_{}; // 引き戻す場所(始点)

	// 途中戻し
	bool isInTheMiddle_ = false;

	// 糸残量
	int  threadAmount = 100;
	int  maxThreadAmount = 100; // 最大値
	
	// 速度
	Vector3 velo_ = {}; // 速度
	Vector3 veloAll_ = {}; // 速度
	Vector3 veloTotal_ = {}; // 速度
	
	Vector3 playerPos_;
	Vector3 playerOldPos_;
	
	float length_{};
	float maxlength_{250};
	float lengthTime_{};


};