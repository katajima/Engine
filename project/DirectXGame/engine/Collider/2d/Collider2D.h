#pragma once

#pragma once
#include"DirectXGame/engine/math/MathFunctions.h"
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/Line/LineCommon.h"
#include"DirectXGame/engine/collider/CollisionTypeIdDef.h"
#include "ColliderFunction2D.h"
#include"DirectXGame/engine/Transform/WorldTransform/WorldTransform2d.h"

namespace Engine {
	// コライダー2D基底クラス
/// <summary>
/// Collider2Dを管理・実装するクラス。
/// </summary>
	class Collider2D
	{
	public:
		void* owner = nullptr; // 通知先ポインタ
		bool enabled = true;
		bool isStatic = false;  // 動かさない
		Vector2 centerWorld;
		CollisionLayer layer = CollisionLayer::ALL;
		CollisionTag tag = CollisionTag::None; // タグ
		uint32_t collisionMask = 0xFFFFFFFF; // ビットで衝突対象を指定（全部と当たる）
		uint32_t id = 0; // コライダーID（ユニーク）

		/// <summary>
		/// 判定有効
		/// </summary>
		void Enable() { enabled = true; }
		/// <summary>
		/// 判定無効
		/// </summary>
		void Disable() { enabled = false; }
		/// <summary>
		/// 判定効力取得
		/// </summary>
		bool IsEnabled() const { return enabled; }
		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update(const WorldTransform2d& worldTransform) = 0;
		/// <summary>
		/// 判定
		/// </summary>
		virtual bool CheckHit(const Collider2D& other) const = 0;
		/// <summary>
		/// 押し戻し
		/// </summary>
		virtual bool ResolveCollision(const Collider2D& other, Vector2& outPushVec) const = 0;
		/// <summary>
		/// コライダータイプ取得
		/// </summary>
		virtual ColliderShapeType GetType() const = 0;
		virtual ~Collider2D() = default;
	};

	// 球コライダークラス
/// <summary>
/// SphereCollider2Dを管理・実装するクラス。
/// </summary>
	class SphereCollider2D : public Collider2D
	{
	public:
		float radius = 1.0f;
		/// <summary>
		/// 更新
		/// </summary>
		void Update(const WorldTransform2d& worldTransform) override;
		/// <summary>
		/// 判定
		/// </summary>
		bool CheckHit(const Collider2D& other) const override;
		/// <summary>
		/// 押し出し
		/// </summary>
		bool ResolveCollision(const Collider2D& other, Vector2& outPushVec) const override;
		/// <summary>
		/// コライダタイプ取得
		/// </summary>
		ColliderShapeType GetType() const override {
			return ColliderShapeType::Sphere;
		}

	};

	// AABBコライダークラス
/// <summary>
/// AABBCollider2Dを管理・実装するクラス。
/// </summary>
	class AABBCollider2D : public Collider2D
	{
	public:
		Box box{ {-0.5f,-0.5f} ,{0.5f,0.5f} };
		Vector2 minWorld;
		Vector2 maxWorld;

		/// <summary>
		/// 更新
		/// </summary>
		void Update(const WorldTransform2d& worldTransform) override;
		/// <summary>
		/// 判定
		/// </summary>
		bool CheckHit(const Collider2D& other) const override;
		/// <summary>
		/// 押し出し
		/// </summary>
		bool ResolveCollision(const Collider2D& other, Vector2& outPushVec) const override;
		/// <summary>
		/// コライダタイプ取得
		/// </summary>
		ColliderShapeType GetType() const override {
			return ColliderShapeType::AABB;
		}

	};
}



