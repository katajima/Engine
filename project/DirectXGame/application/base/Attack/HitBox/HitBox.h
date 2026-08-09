#pragma once
#include "HitBoxFunction.h"
#include <DirectXGame/engine/Transform/WorldTransform/WorldTransform.h>
#include <DirectXGame/engine/Collider/3d/ColliderComponent.h>
#include <map>



namespace Engine {
	class EntityManager; // 前方宣言
	class LineCommon; // 軌道デバッグライン描画
	class LineMeshData; // デバッグラインメッシュ
}
namespace Character {
	class BaseCharacter;
}

namespace HitBox {

	/// <summary>
	/// ヒットボックスクラス
	/// </summary>
	class HitBoxInstance : public IHitReceiver {
	public:
		~HitBoxInstance() {
			colliders_.clear();
		}

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(Engine::EntityManager* entityManager, Character::BaseCharacter* character, UseType type, bool useContactRecord);

		/// <summary>
		/// 更新
		/// </summary>
		// ヒットボックスを更新し、必要なら独立軌道上へ移動させる。
		void Update(float dt);
		// 生成時の親Transformを基準に、以後独立して軌道を評価する。
		void SetTrajectory(const GlobalHitBox& data, Engine::WorldTransform* anchor);
		// 生成前の軌道設定を、選択中コンボのデバッグ表示用に描画する。
		static void DrawTrajectoryDebug(const GlobalHitBox& data, Engine::WorldTransform* anchor, Engine::LineCommon* lineCommon);
		uint32_t GetAttackInstanceId() const { return attackInstanceId_; }

	public:

		/// <summary>
		/// コライダー追加
		/// </summary>
		void AddCollider(std::unique_ptr<Engine::Collider> collider, const Vector3& offset, const HitReactionData& reaction);
		/// <summary>
		/// 有効化
		/// </summary>
		void Enable(Vector4 color = {1,1,1,1});
		/// <summary>
		/// 無効化
		/// </summary>
		void Disable(Vector4 color = { 1,1,1,1 });
		/// <summary>
		/// 履歴削除
		/// </summary>
		void ClearContactRecord() { GetContactRecord().Clear(); }
		void SetRecordPerCollider(bool enabled);
	public:
		/// <summary>
		/// ワールドトランスフォーム取得
		/// </summary>
		Engine::WorldTransform& GetWorldTransform() { return worldTransform_; }
		/// <summary>
		/// コンポーネント取得
		/// </summary>
		Engine::ColliderComponent* GetColliderComponent() { return colliderComponent_.get(); }
	private:
		/// <summary>
		/// 衝突履歴取得
		/// </summary>
		Engine::ContactRecord& GetContactRecord() { return colliderComponent_->contactRecord_; }
	private:
		// コライダーコンポーネント
		std::unique_ptr<Engine::ColliderComponent> colliderComponent_;
		// ヒットボックス衝突応答クラス
		std::unique_ptr<HitBoxFunction> hitBoxFunction_;

		// ワールドトランスフォーム
		Engine::WorldTransform worldTransform_;
		// 使用者タイプ
		UseType type_;
		// ヒット記録を使用するか（使用した場合連続ヒットしない）
		bool useContactRecord = true;
		uint32_t attackInstanceId_ = 0;


		// コライダーデータ構造体
		struct ColliderData {
			Engine::WorldTransform worldTransform;
			Engine::Collider* collider = nullptr;

			float damage = 0.0f;
			HitReactionData reactionData;

			int colliderID = -1;
		};

		// コライダーのワールドトランスフォームマップ
		std::map<int, ColliderData> colliders_;
	private:
		// コライダー数
		int colliderCount = 0;
		Character::BaseCharacter* character = nullptr;

		// 軌道を使う場合にだけ保持する、生成時のワールド基準情報。
		bool trajectoryEnabled_ = false;
		TrajectoryType trajectoryType_ = TrajectoryType::kNone;
		float trajectoryElapsed_ = 0.0f;
		float trajectoryDuration_ = 0.0f;
		Vector3 trajectoryAnchorPosition_{};
		Vector3 trajectoryRight_{ 1.0f, 0.0f, 0.0f };
		Vector3 trajectoryUp_{ 0.0f, 1.0f, 0.0f };
		Vector3 trajectoryForward_{ 0.0f, 0.0f, 1.0f };
		Vector3 trajectoryPoint0_{};
		Vector3 trajectoryPoint1_{};
		Vector3 trajectoryPoint2_{};
		Vector3 trajectoryPoint3_{};
		Vector3 trajectoryOrbitCenter_{};
		float trajectoryOrbitRadius_ = 0.0f;
		float trajectoryOrbitHeight_ = 0.0f;
		float trajectoryOrbitStartAngle_ = 0.0f;
		float trajectoryOrbitEndAngle_ = 0.0f;

		// 現在の軌道パラメータから、アンカー基準のローカル位置を計算する。
		Vector3 EvaluateTrajectory(float normalizedTime) const;

		// ローカル軌道座標を生成時アンカー基準のワールド座標へ変換する。
		Vector3 ToTrajectoryWorldPosition(const Vector3& localPosition) const;
		// デバッグビルド時だけ、軌道全体を3Dラインとして描画する。
		void DrawTrajectory() const;
		void DrawTrajectory(Engine::LineMeshData& lineMeshData) const;
		Engine::EntityManager* entityManager = nullptr;
	};
};
