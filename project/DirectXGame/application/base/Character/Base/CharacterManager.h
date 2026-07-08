#pragma once

// engine
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/application/GlobalVariables/GlobalVariables.h"

// ベースキャラクター
#include"BaseCharacter.h"
// アプリケーション(敵)
#include"DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"
// アプリケーション(プレイヤー)
#include "DirectXGame/application/base/Character/Player/Base/BasePlayer.h"

#include <DirectXGame/application/base/Character/Enemy/Base/AI/EnemyAiSystem.h>
#include <DirectXGame/application/base/Character/Enemy/Base/AI/EnemyAiDebugSystem.h>
#include <DirectXGame/application/base/Character/Enemy/Base/AI/EnemyCrowdSpawnDebugSystem.h>

#include "CharacterGlobalData.h"

//前方宣言
class EffectSystem;
class FollowCamera;
class BulletManager;
class CameraManager;
class SpecialPointManager;

namespace Engine {
	class EntityManager;
}

namespace Character {
	/// <summary>
	/// キャラクターマネージャークラス
	/// </summary>
	class CharacterManager {
	public:
		/// <summary>キャラクター生成・AI・衝突連携に必要な依存を設定して初期化する。</summary>
		/// <param name="inputSystem">プレイヤー操作に使用する入力。非所有ポインター。</param>
		/// <param name="hitBoxSystem">キャラクターの攻撃判定を登録するシステム。非所有ポインター。</param>
		/// <param name="entity3DManager">キャラクターオブジェクトを生成するエンティティ管理。</param>
		/// <param name="globalVariables">キャラクター調整値の登録・保存先。</param>
		/// <param name="camera">キャラクター描画に使用するカメラ。後からCameraManagerで切り替え可能。</param>
		void Initialize(InputSystem* inputSystem,HitBox::System* hitBoxSystem,Engine::EntityManager* entity3DManager,
			Engine::GlobalVariables* globalVariables, Engine::Camera* camera);

		/// <summary>全キャラクターと敵AIを更新し、死亡済みキャラクターを整理する。</summary>
		/// <param name="dt">秒単位のフレーム時間。</param>
		/// <param name="isMove">ゲーム進行上キャラクター移動を許可する場合はtrue。</param>
		void Update(float dt,bool isMove);
		// 描画2d
		void Draw2D();
	public:
		/// <summary>キャラクター演出が操作するカメラ管理を設定する。</summary>
		/// <param name="cameraManager">非所有ポインター。CharacterManagerより長く生存すること。</param>
		void SetCameraManager(CameraManager* cameraManager) { this->cameraManager = cameraManager; };
		/// <summary>プレイヤー追従に使用するカメラを設定する。</summary>
		/// <param name="followCamera">非所有ポインター。未使用の場合はnullptrを許容する。</param>
		void SetFollowCamera(FollowCamera* followCamera) { this->followCamera = followCamera; }
		/// <summary>キャラクターが生成する弾の登録先を設定する。</summary>
		/// <param name="bulletManager">非所有ポインター。弾を生成するキャラクターがいる間は有効であること。</param>
		void SetBulletManager(BulletManager* bulletManager) { this->bulletManager = bulletManager; }
		/// <summary>キャラクター演出の出力先を設定する。</summary>
		/// <param name="effect">非所有ポインター。演出不要の場合はnullptrを許容する。</param>
		void SetEffect(EffectSystem* effect) { this->effect = effect; }
		/// <summary>敵撃破時などに使用するスペシャルポイント管理を設定する。</summary>
		/// <param name="specialPointManager">非所有ポインター。</param>
		void SetSpecialPointManager(SpecialPointManager* specialPointManager) { this->specialPointManager = specialPointManager; };
	public: // 取得系

		/// <summary>指定種別の生存管理中キャラクター数を取得する。</summary>
		/// <param name="type">集計するプレイヤーまたは敵の種別。</param>
		/// <returns>現在管理している該当キャラクター数。</returns>
		int GetCharacterCount(Type type) const
		{
			int result = 0;
			for (const auto& character : character_)
			{
				if (character->GetCharacterType() == type)
				{
					result++;
				}
			}

			return result;
		}

		/// <summary>管理中の全キャラクターを取得する。</summary>
		/// <returns>所有権を持たないポインターの配列。次回の削除処理後は無効になる可能性がある。</returns>
		std::vector<BaseCharacter*> GetCharacters() {
			std::vector<BaseCharacter*> result;
			for (auto& enemy : character_) {
				result.push_back(enemy.get());
			}
			return result;
		}

		/// <summary>現在管理中のプレイヤーを取得する。</summary>
		/// <returns>プレイヤーへの非所有ポインター。存在しない場合はnullptr。</returns>
		BasePlayer* GetPlayer() {
			for (auto& character : character_) {
				if (character->GetCharacterType() == Type::Player) {
					return static_cast<BasePlayer*>(character.get());
				}
			}
			return nullptr;
		}
		/// <summary>タグ番号が一致する敵を検索する。</summary>
		/// <param name="tagNumber">生成時に割り当てられたタグ番号。</param>
		/// <returns>一致する敵への非所有ポインター。存在しない場合はnullptr。</returns>
		BaseEnemy* GetEnemy(uint32_t tagNumber) {
			for (auto& character : character_) {
				if (character->GetCharacterType() == Type::Enemy && character->GetTagNumber() == tagNumber) {
					return static_cast<BaseEnemy*>(character.get());
				}
			}
			return nullptr;
		}
		/// <summary>種別を問わずタグ番号が一致するキャラクターを検索する。</summary>
		/// <param name="tagNumber">生成時に割り当てられたタグ番号。</param>
		/// <returns>一致するキャラクターへの非所有ポインター。存在しない場合はnullptr。</returns>
		BaseCharacter* GetCharacter(uint32_t tagNumber) {
			for (auto& character : character_) {
				if (character->GetTagNumber() == tagNumber) {
					return static_cast<BaseCharacter*>(character.get());
				}
			}
			return nullptr;
		}
	public: // 生成系
		/// <summary>指定設定で敵キャラクターを1体生成する。</summary>
		/// <param name="enemyType">生成する敵種別。</param>
		/// <param name="characterName">調整値とオブジェクト識別に使用する名前。空文字の場合は種別の既定名を使用する。</param>
		/// <param name="groupId">AI群衆グループの識別番号。</param>
		/// <param name="transform">初期スケール・回転・座標。</param>
		/// <param name="crowdBehavior">群衆AIの振る舞い設定。</param>
		/// <returns>生成した敵へ割り当てたタグ番号。</returns>
		uint32_t CreateCharacter(EnemyType enemyType, const std::string& characterName, int groupId, Transform transform,
			const CrowdBehaviorSettings& crowdBehavior = CrowdBehaviorSettings::Flocking());
		/// <summary>指定設定でプレイヤーキャラクターを生成する。</summary>
		/// <param name="playerType">生成するプレイヤー種別。</param>
		/// <param name="characterName">調整値とオブジェクト識別に使用する名前。</param>
		/// <param name="transform">初期スケール・回転・座標。</param>
		/// <returns>生成したプレイヤーへ割り当てたタグ番号。</returns>
		uint32_t CreateCharacter(PlayerType playerType, const std::string& characterName, Transform transform);

		/// <summary>指定範囲へ複数の敵グループをまとめて生成する。</summary>
		/// <param name="enemyType">生成する敵種別。</param>
		/// <param name="groupCount">生成するAIグループ数。</param>
		/// <param name="perGroup">1グループ当たりの敵数。</param>
		/// <param name="origin">生成範囲の基準座標。</param>
		/// <param name="aabb">基準座標からの生成範囲。</param>
		/// <param name="crowdBehavior">各グループへ設定する群衆AIの振る舞い。</param>
		void CreateEnemyGroup(EnemyType enemyType, int groupCount, int perGroup, Vector3 origin, AABB aabb,
			const CrowdBehaviorSettings& crowdBehavior = CrowdBehaviorSettings::Flocking());
	public:
		/// <summary>指定種別のキャラクターを管理対象から削除する。</summary>
		/// <param name="type">削除するプレイヤーまたは敵の種別。</param>
		void Clear(Type type);
		// ウェーブ切り替え用に、敵を即削除せず退場演出へ移行させる
		/// <param name="duration">退場演出に使用する秒数。0以下なら即時終了相当。</param>
		void BeginEnemyWaveExit(float duration = 1.2f);
		// 指定した群衆グループだけを、退場演出を伴って削除する
		/// <param name="crowdGroupId">退場させる群衆グループID。</param>
		/// <param name="duration">退場演出に使用する秒数。</param>
		void BeginEnemyCrowdExit(int crowdGroupId, float duration = 1.2f);

		/// <summary>敵撃破などで蓄積した現在のスコアを取得する。</summary>
		/// <returns>現在のスコア。</returns>
		float GetScore() { return score; }
	private:
		// キャラクター
		std::vector<std::unique_ptr<BaseCharacter>> character_;	
		// キャラクターの数
		uint32_t characterCount_ = 0;	
		// 敵カウンター
		uint32_t enemyCount_ = 0;	
		// 敵AIシステム
		std::unique_ptr<EnemyAiSystem> enemyAiSystem_;	
		// 敵AIの判断内容を可視化するデバッグシステム
		std::unique_ptr<EnemyAiDebugSystem> enemyAiDebugSystem_;
		// 任意の群衆をImGuiから生成するデバッグシステム
		std::unique_ptr<EnemyCrowdSpawnDebugSystem> enemyCrowdSpawnDebugSystem_;

		float score = 0.0f;

		// キャラクター用保存項目データ
		std::map<std::string, GlobalData> globalDatas_;
	private: // 貰いもの
		Engine::Camera* camera = nullptr;
		// 入力システム
		InputSystem* inputSystem = nullptr;
		Engine::EntityManager* entityManager = nullptr;
		Engine::GlobalVariables* globalVariables = nullptr;	// グローバル変数
		FollowCamera* followCamera = nullptr;		// フォローカメラ 
		BulletManager* bulletManager = nullptr;	// 弾をマネジャー
		CameraManager* cameraManager = nullptr;	// カメラ管理
		EffectSystem* effect = nullptr;
		SpecialPointManager* specialPointManager = nullptr;
		HitBox::System* hitBoxSystem = nullptr;		// ヒットボックスシステム
	};
}
