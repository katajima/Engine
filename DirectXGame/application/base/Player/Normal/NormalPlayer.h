#pragma once
#include "DirectXGame/application/base/Player/Base/BasePlayer.h"

#include "DirectXGame/application/base/Player/Weapon/playerWeapon.h"
#include "DirectXGame/application/base/Player/Effect/PlayerEffect.h"
#include "DirectXGame/application/base/Player/UI/PlayerUI.h"
#include "DirectXGame/application/base/Player/Special/BulletSpecial.h"

class NormalPlayer :public BasePlayer
{
public:

	///< summary>
	/// 初期化
	///</summary>
	void Initialize(Input* input,DirectXCommon* dxcommon, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Vector3 position, Camera* camera)override;

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


private:
	// スペシャル攻撃
	std::unique_ptr<BulletSpecial> bulletSpecial_;
	//  プレイヤー用UI
	std::unique_ptr<PlayerUI> ui_ = std::make_unique<PlayerUI>();
	// エフェクト 
	std::unique_ptr<PlayerEffect> effect_ = std::make_unique<PlayerEffect>();
	// 武器
	std::unique_ptr<playerWeapon> weapon_;

	// 本体
	Object3d objectBody_;

	//　レティクル
	Object3d objectReticle_;
};

