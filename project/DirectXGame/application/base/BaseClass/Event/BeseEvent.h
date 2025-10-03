#pragma once
#include"DirectXGame/application/base/BaseClass/Object/BaseObject.h"

// イベント種類
enum class EventType
{
	None,			// 無し
	Scene,			// シーン変更
	Camera,			// カメラ関係
	Effect,			// エフェクト関係
};

// 基底イベントクラス
class BaseEvent : public BaseObject 
{
public:
	///< summary>
	/// 初期化
	///</summary>
	void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) override;

	///< summary>
	/// 更新
	///</summary>
	void Update() override;

	/// <summary>
	/// エフェクトの描画
	/// </summary>
	void DrawEffect() override;


	void Draw2D() override;
protected:
	EventType eventType;
	bool isRepeatable = false; // 一度きり

};

