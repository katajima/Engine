#pragma once

#include "DirectXGame/application/base/Bullet/Base/BulletSpawn.h"

#include <memory>
#include <string>

class BulletManager;
class EffectSystem;

namespace Engine {
	class EntityManager;
	class GlobalVariables;
}

namespace Projectile {

	/// <summary>
	/// ProjectileEditorで作成した弾定義を使って、デバッグ用に試射するクラス。
	/// CharacterDebugSceneから弾の生成処理を分離するために持つ。
	/// </summary>
	class ProjectileDebug {
	public:
		// 試射に必要な生成器とターゲット情報を設定する
		void Initialize(Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables,
			EffectSystem* effectSystem, BulletManager* bulletManager, Character::BaseCharacter* target);
		/// <summary>
		/// ImGui上で発射位置や定義を編集し、任意タイミングで試射する
		/// </summary>
		void Update();
		/// <summary>
		/// ターゲット設定
		/// </summary>
		void SetTarget(Character::BaseCharacter* target) { spawnInfo_.target = target; };

	private:
		// デバッグ試射用の発射処理。通常のキャラクター所有のBulletSpawnとは分けて扱う。
		std::unique_ptr<BulletSpawn> bulletSpawn_;
		// ProjectileEditorが持つ定義一覧を参照するための管理クラス
		BulletManager* bulletManager_ = nullptr;
		// デバッグ試射時の出現情報
		ProjectileSpawnInfo spawnInfo_{};
		// ImGuiで現在選択している弾定義名
		std::string selectedDefinitionName_;
	};

}
