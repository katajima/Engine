#pragma once
#include"DirectXGame/engine/Line/LineCommon.h"
#include"DirectXGame/engine/collider/CollisionTypeIdDef.h"
#include "ColliderFunction3D.h"

constexpr float kFloatMax = 3.4028235e+38f;


namespace Engine {
	class ColliderComponent;

	// SRTリザルト
	struct SATResult {
		bool hit = false;
		float minOverlap = FLT_MAX; // 初期化は最大値
		Vector3 pushDir{}; // 押し出し軸（world space）
	};
	// コライダー基底クラス
/// <summary>
/// Colliderを管理・実装するクラス。
/// </summary>
	class Collider
	{
	public:
		/// <summary>
		/// 判定有効
		/// </summary>
		void Enable() { enabled_ = true; }
		/// <summary>
		/// 判定無効
		/// </summary>
		void Disable() { enabled_ = false; }
		/// <summary>
		/// 判定効力取得
		/// </summary>
		bool IsEnabled() const { return enabled_; }
		void SetEnabled(bool enabled) { enabled_ = enabled; }

		/// <summary>
		/// 通知先コンポーネント
		/// </summary>
		void SetOwner(ColliderComponent* owner) { owner_ = owner; }
		ColliderComponent* GetOwner() const { return owner_; }

		/// <summary>
		/// 動かないコライダーか
		/// </summary>
		void SetIsStatic(bool isStatic) { isStatic_ = isStatic; }
		bool IsStatic() const { return isStatic_; }

		/// <summary>
		/// デバッグライン
		/// </summary>
		void SetIsDebugLine(bool isDebugLine) { isDebugLine_ = isDebugLine; }
		bool IsDebugLine() const { return isDebugLine_; }
		void SetLineColor(const Vector4& color) { lineColor_ = color; }
		const Vector4& GetLineColor() const { return lineColor_; }

		/// <summary>
		/// 衝突分類
		/// </summary>
		void SetLayer(CollisionLayer layer) { layer_ = layer; }
		CollisionLayer GetLayer() const { return layer_; }
		void SetTag(CollisionTag tag) { tag_ = tag; }
		CollisionTag GetTag() const { return tag_; }
		void SetCollisionMask(uint32_t mask) { collisionMask_ = mask; }
		uint32_t GetCollisionMask() const { return collisionMask_; }

		/// <summary>
		/// ID
		/// </summary>
		void SetId(uint32_t id) { id_ = id; }
		uint32_t GetId() const { return id_; }

		/// <summary>
		/// ワールド中心
		/// </summary>
		void SetCenterWorld(const Vector3& centerWorld) { centerWorld_ = centerWorld; }
		const Vector3& GetCenterWorld() const { return centerWorld_; }

		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update(const WorldTransform& worldTransform, LineCommon* lineCommon) = 0;
		/// <summary>
		/// 判定
		/// </summary>
		virtual bool CheckHit(const Collider& other) const = 0;
		/// <summary>
		/// 押し戻し
		/// </summary>
		virtual bool ResolveCollision(const Collider& other, Vector3& outPushVec) const = 0;
		/// <summary>
		/// コライダータイプ取得
		/// </summary>
		virtual ColliderShapeType GetType() const = 0;
		virtual ~Collider() = default;

		/// <summary>
		/// 追加: このコライダーのワールド空間AABBを返す（Broad Phase用）
		/// </summary>
		virtual AABB GetAABB() const {
			// 基底は点AABB（派生でオーバーライド推奨）
			return AABB{ centerWorld_, centerWorld_ };
		}
	protected:
		ColliderComponent* owner_ = nullptr; // 通知先ポインタ
		bool enabled_ = true;
		bool isStatic_ = false;  // 動かさない
		bool isDebugLine_ = false;// ライン描画するか
		Vector4 lineColor_ = { 1,1,1,1 };	// ライン色

		Vector3 centerWorld_ = { 0,0,0 };
		CollisionLayer layer_ = CollisionLayer::ALL;
		CollisionTag tag_ = CollisionTag::None; // タグ
		uint32_t collisionMask_ = 0xFFFFFFFF; // ビットで衝突対象を指定（全部と当たる）
		uint32_t id_ = 0; // コライダーID（ユニーク）
	};

	// 球コライダークラス
/// <summary>
/// SphereColliderを管理・実装するクラス。
/// </summary>
	class SphereCollider : public Collider
	{
	public:
		float radius = 1.0f;

		/// <summary>
		/// 更新
		/// </summary>
		void Update(const WorldTransform& worldTransform, LineCommon* lineCommon) override;
		/// <summary>
		/// 判定
		/// </summary>
		bool CheckHit(const Collider& other) const override;
		/// <summary>
		/// 押し出し
		/// </summary>
		bool ResolveCollision(const Collider& other, Vector3& outPushVec) const override;
		/// <summary>
		/// コライダタイプ取得
		/// </summary>
		ColliderShapeType GetType() const override {
			return ColliderShapeType::Sphere;
		}
		/// <summary>
		/// AABB取得
		/// </summary>
		AABB GetAABB() const override {
			Vector3 r{ radius, radius, radius };
			return AABB{ centerWorld_ - r, centerWorld_ + r };
		}

	};

	// 三角形コライダークラス
/// <summary>
/// TriangleColliderを管理・実装するクラス。
/// </summary>
	class TriangleCollider : public Collider {
	public:
		Vector3 triangle01{};
		Vector3 triangle02{};
		Vector3 triangle03{};
		bool isNormal = false;

		/// <summary>
		/// 更新
		/// </summary>
		void Update(const WorldTransform& worldTransform, LineCommon* lineCommon) override;
		/// <summary>
		/// 判定
		/// </summary>
		bool CheckHit(const Collider& other) const override;
		/// <summary>
		/// 押し出し
		/// </summary>
		bool ResolveCollision(const Collider& other, Vector3& outPushVec) const override;
		/// <summary>
		/// コライダタイプ取得
		/// </summary>
		ColliderShapeType GetType() const override {
			return ColliderShapeType::Triangle;
		}

		/// <summary>
		/// 三角面取得
		/// </summary>
		Triangle GetWorldTriangle() const {
			return Triangle{ triangle01 + centerWorld_,triangle02 + centerWorld_, triangle03 + centerWorld_};
		}

		AABB GetAABB() const override {
			/// <summary>
			/// 基底は点AABB（派生でオーバーライド推奨）
			/// </summary>
			return GetWorldTriangle().GetAABB();
		}
	private:
	};

	// AABBコライダークラス
/// <summary>
/// AABBColliderを管理・実装するクラス。
/// </summary>
	class AABBCollider : public Collider
	{
	public:
		AABB aabb{ {-0.5f,-0.5f,-0.5f} ,{0.5f,0.5f,0.5f} };
		Vector3 minWorld;
		Vector3 maxWorld;

		/// <summary>
		/// 更新
		/// </summary>
		void Update(const WorldTransform& worldTransform, LineCommon* lineCommon) override;
		/// <summary>
		/// 判定
		/// </summary>
		bool CheckHit(const Collider& other) const override;
		/// <summary>
		/// 押し出し
		/// </summary>
		bool ResolveCollision(const Collider& other, Vector3& outPushVec) const override;
		/// <summary>
		/// コライダタイプ取得
		/// </summary>
		ColliderShapeType GetType() const override {
			return ColliderShapeType::AABB;
		}
		/// <summary>
		/// AABB取得
		/// </summary>
		AABB GetAABB() const override {
			return AABB{ minWorld, maxWorld };
		}
	};

	// カプセルコライダークラス
/// <summary>
/// CapsuleColliderを管理・実装するクラス。
/// </summary>
	class CapsuleCollider : public Collider
	{
	public:
		Capsule capsule{ Vector3{0.0f,1.0f,0.0f},Vector3{0.0f,-1.0f,0.0f},{1.0f} };
		Capsule capWorld_{ Vector3{0.0f,1.0f,0.0f},Vector3{0.0f,-1.0f,0.0f},{1.0f} };


		/// <summary>
		/// 更新
		/// </summary>
		void Update(const WorldTransform& worldTransform, LineCommon* lineCommon) override;
		/// <summary>
		/// 判定
		/// </summary>
		bool CheckHit(const Collider& other) const override;
		/// <summary>
		/// 押し出し
		/// </summary>
		bool ResolveCollision(const Collider& other, Vector3& outPushVec) const override;
		/// <summary>
		/// コライダタイプ取得
		/// </summary>
		ColliderShapeType GetType() const override {
			return ColliderShapeType::Capsule;
		}
		/// <summary>
		/// AABB取得
		/// </summary>
		AABB GetAABB() const override {
			/// <summary>
			/// Capsule 型に computeAABB() がある前提
			/// </summary>
			return capWorld_.computeAABB();
		}
	};

	// OBBコライダークラス
/// <summary>
/// OBBColliderを管理・実装するクラス。
/// </summary>
	class OBBCollider : public Collider
	{
	public:
		OBB obb{ {0,0,0},{0,0,0},{0.5f,0.5f,0.5f} };


		/// <summary>
		/// 更新
		/// </summary>
		void Update(const WorldTransform& worldTransform, LineCommon* lineCommon) override;
		/// <summary>
		/// 判定
		/// </summary>
		bool CheckHit(const Collider& other) const override;
		/// <summary>
		/// 押し出し
		/// </summary>
		bool ResolveCollision(const Collider& other, Vector3& outPushVec) const override;
		/// <summary>
		/// コライダタイプ取得
		/// </summary>
		ColliderShapeType GetType() const override {
			return ColliderShapeType::OBB;
		}
		/// <summary>
		/// AABB取得
		/// </summary>
		AABB GetAABB() const override {
			// OBB の 8 頂点から AABB を作る
			Vector3 mins{ FLT_MAX, FLT_MAX, FLT_MAX };
			Vector3 maxs{ -FLT_MAX, -FLT_MAX, -FLT_MAX };
			Vector3 c = obb.center;
			Vector3 u0 = obb.orientations[0] * (obb.size[0] * 0.5f);
			Vector3 u1 = obb.orientations[1] * (obb.size[1] * 0.5f);
			Vector3 u2 = obb.orientations[2] * (obb.size[2] * 0.5f);
			for (int i = 0; i < 8; ++i) {
				Vector3 corner = c;
				corner += (i & 1) ? u0 : -u0;
				corner += (i & 2) ? u1 : -u1;
				corner += (i & 4) ? u2 : -u2;
				mins = Min(mins, corner);
				maxs = Max(maxs, corner);
			}
			return AABB{ mins, maxs };
		}
	private:

	};


	// Rayコライダークラス
/// <summary>
/// RayColliderを管理・実装するクラス。
/// </summary>
	class RayCollider : public Collider
	{
	public:
		Ray ray_;

		/// <summary>
		/// 更新
		/// </summary>
		void Update(const WorldTransform& worldTransform, LineCommon* lineCommon) override;
		/// <summary>
		/// 判定
		/// </summary>
		bool CheckHit(const Collider& other) const override;
		/// <summary>
		/// 押し出し
		/// </summary>
		bool ResolveCollision(const Collider& other, Vector3& outPushVec) const override;
		/// <summary>
		/// コライダタイプ取得
		/// </summary>
		ColliderShapeType GetType() const override {
			return ColliderShapeType::Ray;
		}
		/// <summary>
		/// AABB取得
		/// </summary>
		AABB GetAABB() const override {
			// Ray が origin, direction, length を持っている前提
			Vector3 p0 = ray_.origin;
			Vector3 p1 = ray_.origin + ray_.Direction() * ray_.diff; // length が無ければ direction を想定
			Vector3 mins = Min(p0, p1);
			Vector3 maxs = Max(p0, p1);
			return AABB{ mins, maxs };
		}

	private:

	};

	// OBB当たり判定
	static SATResult CheckOBBCollisionSAT(const OBB& obb0, const OBB& obb1)
	{
		SATResult result;
		const Vector3* A = obb0.orientations;
		const Vector3* B = obb1.orientations;

		Vector3 T = obb1.center - obb0.center;

		float R[3][3], AbsR[3][3];
		constexpr float EPSILON = 1e-6f;

		/// <summary>
		/// 回転行列の生成
		/// </summary>
		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j) {
				R[i][j] = A[i].Dot(B[j]);
				AbsR[i][j] = std::abs(R[i][j]) + EPSILON;
			}

		Vector3 tLocal{
			T.Dot(A[0]),
			T.Dot(A[1]),
			T.Dot(A[2])
		};

		float ra, rb;// , overlap;
		/// <summary>
		/// Vector3 axis;
		/// </summary>

		auto testAxis = [&](const Vector3& testAxis, float ra, float rb, float tProj) {
			float overlap = ra + rb - std::abs(tProj);
			if (overlap < 0.0f) {
				result.hit = false;
				return false;
			}
			if (overlap < result.minOverlap) {
				result.minOverlap = overlap;
				result.pushDir = testAxis;
			}
			return true;
			};

		result.hit = true;

		/// <summary>
		/// 各軸のテスト（A[0~2], B[0~2], A×Bの9軸）
		/// </summary>
		for (int i = 0; i < 3; ++i) {
			ra = obb0.size[i] * 0.5f;
			rb = obb1.size[0] * AbsR[i][0] + obb1.size[1] * AbsR[i][1] + obb1.size[2] * AbsR[i][2];
			if (!testAxis(A[i], ra, rb, tLocal[i])) return result;
		}

		for (int i = 0; i < 3; ++i) {
			ra = obb0.size[0] * AbsR[0][i] + obb0.size[1] * AbsR[1][i] + obb0.size[2] * AbsR[2][i];
			rb = obb1.size[i] * 0.5f;
			if (!testAxis(B[i], ra, rb, T.Dot(B[i]))) return result;
		}

		// 交差軸は省略（まずはここまでで試す）

		// pushDir の向き補正
		if (T.Dot(result.pushDir) < 0.0f)
			result.pushDir = -result.pushDir;

		return result;
	}
}


