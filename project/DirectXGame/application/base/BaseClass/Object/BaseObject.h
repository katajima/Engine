//#pragma once
//#include"DirectXGame/engine/Camera/Camera.h"
//#include"DirectXGame/engine/3d/Object/Object3d.h"
//#include"DirectXGame/engine/2d/Sprite.h"
//#include"DirectXGame/engine/base/Imgui/ImGuiManager.h"
//#include"DirectXGame/engine/math/MathFanctions.h"
//#include"DirectXGame/engine/input/Input.h"
//#include"DirectXGame/engine/audio/Audio.h"
//#include<DirectXGame/engine/Effect/EffectComponent.h>
//#include"DirectXGame/engine/Animation/AnimationData.h"
//#include"DirectXGame/application/base/Component/MoveComponent.h"
//
//
//
//
//
//
//
//class Entity3DManager;
//class Entity2DManager;
//class BaseObject : public IHitReceiver
//{
//public:
//	///< summary>
//	/// 初期化a
//	///</summary>
//	virtual void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) = 0;
//
//	///< summary>
//	/// 更新
//	///</summary>
//	virtual void Update() = 0;
//
//	/// <summary>
//	/// エフェクトの描画
//	/// </summary>
//	virtual void DrawEffect() = 0;
//
//	/// <summary>
//	/// 2d描画
//	/// </summary>
//	virtual void Draw2D() = 0;
//
//public: // オブジェクト
//	// オブジェクトタイプ取得
//	ObjectType GetObjectType() const { return objectType_; }
//	// オブジェクトの状態フラグ取得
//	ObjectStateFlags GetFlags() const { return flags_; }
//	// オブジェクト3d取得
//	Object3d* GetObject3D() { return objectBase_; }
//	// ワールド変換取得
//	WorldTransform& GetWorldTransform() { return objectBase_->GetWorldTransform(); }
//
//
//
//
//	// オブジェクト時間取得
//	float GetTime() const;
//	// カメラのビュープロジェクション
//	void SetCamera(Camera* camera) { camera_ = camera; };
//
//
//	void SetName(std::string name) { this->name_ = name; };
//	
//	std::string GetName() const { return name_; }
//
//	// 削除フラグ取得
//	bool  GetDelete() const { return flags_.isDeleted; };
//	// 削除する
//	void Delete() { flags_.isDeleted = true; };
//
//	// ロックオン状態の取得
//	bool GetLockOn() const { return flags_.isLockonTarget; }
//	// ロックオン状態を設定
//	void SetLockOn(bool lock) { flags_.isLockonTarget = lock; }
//
//	// 入力
//	Input* GetInput() { return input_; }
//
//protected:
//	// オブジェクトの状態フラグ取得
//	ObjectStateFlags& GetFlags() { return flags_; }
//
//	
//
//protected:
//	Object3d* objectBase_ = nullptr;// オブジェクト3d
//	ObjectType objectType_ = ObjectType::None; // オブジェクトの種類
//	std::string name_ = "";		// オブジェクト名
//	
//	
//
//	
//	
//	ObjectStateFlags flags_;
//
//
//	float timeSpeed_ = 1.0f;	// 時間の進む速さ(1.0fが通常、0.0fで停止、2.0fで2倍速など)
//protected: // 貰ってくるもの
//	Entity3DManager* entity3DManager_ = nullptr;	// 3Dエンティティマネージャー
//	Entity2DManager* entity2DManager_ = nullptr;	// 2Dエンティティマネージャー
//	GlobalVariables* globalVariables_ = nullptr;	// グローバル変数
//	Camera* camera_ = nullptr;						// カメラ
//	Input* input_ = nullptr;						// 入力(使わないならnullptr)
//	Audio* audio_ = nullptr;
//};
