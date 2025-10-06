#pragma once
#include "DirectXGame/engine/math/MathFanctions.h"
#include "DirectXGame/engine/struct/Structs3D.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"

#include "ParticleData.h"
#include "DirectXGame/engine/Effect/Particle/Emit/EmitFanction.h"

#include "DirectXGame/engine/Math/Noise.h"

#include <DirectXGame/engine/struct/Structs.h>

class LineCommon;
namespace Field {

	// 形状
	enum class ShapeType {
		kAABB,			// AABB
		kSphere,		// 球
		kCapsule,		// カプセル
		kCapsuleSpline, // カプセルスプライン

	};

	// 効果
	enum class EffectType {
		kGravity,		// 重力
		kAcceleration,	// 加速
		kDestruction,	// 破棄
		kColor,			// 色
		kDeceleration,  // 減速
		kNoise,			// ノイズ
		kTornado,		// トルネード
		kEvent,			// イベント
	};

	class FieldEffect {
	public:
		void Initialize(const std::string& name, ShapeType shapeType, EffectType type, LineCommon* lineCommon);

		void Update();

		void DebugImgui();

		bool IsCollisionAABB(const Vector3& point);


	public:

		std::string GetName() const { return name_; }

		bool GetIsEffect() const { return isEffect; }

		void SetParent(WorldTransform& parent)
		{
			transform_.parent_ = &parent;
		}

	public:	// 各パラメータ
		EffectType effectType_ = EffectType::kAcceleration;					// エフェクト種類選択
		Vector3 acceleration_{};											// 加速
		AABB renge_{ -Vector3{1.0f,1.0f,1.0f},Vector3{1.0f,1.0f,1.0f} };	// 範囲
		WorldTransform transform_;											// ワールドトランスフォーム
		float rad;															// 半径
		int segmentPerCurve = 3;											// カーブのセグメント数
		std::vector<Vector3> controlPoints;									// 各ポジション
		float force_{};														// 力
		float lift_{};														// 上昇力
		float deceleration_{};												// 減速
		Vector4 color_{ 1,1,1,1 };											// 色
		Noise noise_;														// ノイズ
		float noiseScale_ = 1.0f;											// ノイズスケール

		Range <Vector3> rondomRenge{};										// 乱数範囲 (Vector3の範囲)

	private:
		bool isEffect = true;												// 効果を出すか
	private:
		ShapeType shapeType_ = ShapeType::kAABB;							// 形状選択
		std::string name_ = "EffectFildeAABB";								// 名前
		std::string nameType = "Acceleration";								// タイプ名前
		LineCommon* lineCommon_;
	};


	void Effect(ParticleGroup& grop, std::list<Particle>::iterator& particleIterator, std::vector<Field::FieldEffect*> fieldEffect, float deltaTime);


}

















//// フィールド影響
//for (auto& acc : accelerationFieldSpline_) {
//
//	// 入っているなら
//	bool isCollided = false;
//
//	int totalSegments = static_cast<int>(acc.controlPoints.size() - 3) * acc.segmentPerCurve;
//	if (totalSegments <= 0) return;
//
//	// 最も近い位置を探すための情報
//	float minDistSq = (std::numeric_limits<float>::max());
//	Vector3 closestDir = {};
//	bool found = false;
//
//	// スプライン終端から始めて、最も近いセグメントを探す
//	for (int i = totalSegments - 1; i >= 0; --i) {
//		float t0 = static_cast<float>(i + 1) / totalSegments;
//		float t1 = static_cast<float>(i) / totalSegments;
//
//		Vector3 pA = CatmullRom(acc.controlPoints, t0);
//		Vector3 pB = CatmullRom(acc.controlPoints, t1);
//
//		if (PointInCapsule(particleIterator->transform.translate, pA, pB, acc.rad)) {
//			// 点からカプセル線分への最近点を計算
//			Vector3 ab = pB - pA;
//			Vector3 ap = particleIterator->transform.translate - pA;
//			float abLengthSq = Dot(ab, ab);
//			float t = abLengthSq > 0.0f ? std::clamp(Dot(ap, ab) / abLengthSq, 0.0f, 1.0f) : 0.0f;
//			Vector3 closestPoint = pA + ab * t;
//
//			// 距離を比較して最も近い方向を取得
//			float distSq = LengthSquared(particleIterator->transform.translate - closestPoint);
//			if (distSq < minDistSq) {
//				minDistSq = distSq;
//				closestDir = Normalize(ab); // スプライン方向
//				found = true;
//			}
//		}
//	}
//
//	// 最も近いセグメント方向に滑らかに加速
//	if (found) {
//		float accelAmount = Length(acc.force);
//		particleIterator->velocity = -closestDir * accelAmount; // += で滑るように
//		isCollided = true;
//	}
//}