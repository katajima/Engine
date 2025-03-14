#pragma once
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object3d.h"
#include"DirectXGame/engine/2d/Sprite.h"
#include"DirectXGame/engine/base/ImGuiManager.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/input/Input.h"
#include <imgui.h>
#include <list>



#include "DirectXGame/engine/effect/Particle/ParticleManager.h"
#include "DirectXGame/engine/effect/Particle/ParticleEmitter.h"

#include "DirectXGame/engine/collider/3d/Collider.h"


#include "DirectXGame/engine/effect/Trail/TrailEffect.h"


#include "DirectXGame/engine/MyGame/MyGame.h"

///< summary>
/// 自キャラ
///</summary>

class Enemy;
class FollowCamera;

class Player : public Collider {
public:


	///< summary>
	/// 初期化
	///</summary>
	void Initialize(Vector3 position, Camera* camera);

	///< summary>
	/// 更新
	///</summary>
	void Update();

	///< summary>
	/// 描画
	///</summary>
	void Draw();

	void DrawP();
	
	void Draw2D();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision([[maybe_unused]] Collider* other) override;

	virtual Vector3 GetCenterPosition() const;

	

private: //Behavior




public: // 攻撃関係
	
private: // 攻撃関係
	

	



public:

	

private: // 移動
	 
	void Move();

	void Gravity();

	void AddMove();

	float graVelo;

	float groundY = 2;

	bool isJamp = false;

	float ty = 0;

	bool isMove = false;

	
public:
	std::string strin;

	Object3d& GetObject3D() { return objectBase_; }
	//Object3d& GetObjectWeapon3D() { return ; }

	

	// カメラのビュープロジェクション
	void SetCamera(Camera* camera) { camera_ = camera; };
	
	
	//void SetPlayer(Player* play) { weapon_->SetPlayer(this); }

	uint32_t GetSerialNumber() const { return serialNumber; }

	bool GetAlive() const { return isAlive; };
	
	void AddDamege(float da) { hp -= int(da); };

	


	void SetFollowCamera(FollowCamera* followCamera) { followCamera_ = followCamera; }


	// 弾リストを取得
	
private:
	FollowCamera* followCamera_;
	
	Camera* camera_ = nullptr;

	std::vector<Enemy*> lockedOnEnemies;
private:  // パラメータ
	
	uint32_t maxHp = 100;
	int hp = 100;
	bool isAlive = true;
	float damage_ = 0;
	bool isInvincible = false;
private:
	

	// オブジェクト3D

	Object3d objectBase_;
	// 本体
	Object3d objectBody_;
	
	//　レティクル
	Object3d objectReticle_;
	
	// ミサイル発射位置
	Object3d injectionLeftObj_;
	Object3d injectionRightObj_;

	Vector3 injectionLeftPos_{ -2.5f,1.0f,-1.5f };
	Vector3 injectionRightPos_{ 2.5,1.0f,-1.5f };




	// 影
	Object3d objectSha_;



	
	// シリアルナンバー
	uint32_t serialNumber = 0;


	
	// スプライト

	std::unique_ptr<Sprite> HpBer_;
	std::unique_ptr<Sprite> SpecailBer_;
	std::unique_ptr<Sprite> textMax_;
	std::unique_ptr<Sprite> textRB_;
	bool isTextRB_ = false;




	// 速度
	Vector3 velocity_ = {};
	
	float moveLimit = 200;
	
	// 浮遊ギミック媒介変数
	float floatingParameter_ = 0.0f;
	float attackParameter_;

	
	float speed;

	


	// エフェクト

	std::unique_ptr<TrailEffect> trailEffect_;
	bool flag33;


	std::unique_ptr<ParticleEmitter> dashEmitter_ = nullptr;


};


