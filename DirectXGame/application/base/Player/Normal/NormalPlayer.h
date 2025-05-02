#pragma once
#include "DirectXGame/application/base/Player/Base/BasePlayer.h"

class NormalPlayer :public BasePlayer
{
public:

	///< summary>
	/// 初期化
	///</summary>
	void Initialize(DirectXCommon* dxcommon, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Vector3 position, Camera* camera)override;

	///< summary>
	/// 更新
	///</summary>
	void Update()override;

	///< summary>
	/// 描画
	///</summary>
	void Draw()override;

	void DrawP()override;

	void Draw2D()override;
public:
	// 移動
	void Move();

};

