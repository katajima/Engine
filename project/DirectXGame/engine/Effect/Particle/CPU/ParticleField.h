#pragma once
#include "DirectXGame/engine/math/MathFunctions.h"
#include "DirectXGame/engine/struct/Structs3D.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"

#include "ParticleData.h"
#include "DirectXGame/engine/Effect/Particle/Emit/EmitFunction.h"

#include "DirectXGame/engine/Math/Noise.h"

#include <DirectXGame/engine/struct/Structs.h>


namespace Engine {
	// 前方宣言
	class LineCommon;

	// フィールド
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


		/// <summary>
		/// パーティクルにかかるフィールドエフェクトクラス
		/// </summary>
		class FieldEffect {
		public:
			/// <summary>
			/// 初期化
			/// </summary>
			void Initialize(const std::string& name, ShapeType shapeType, EffectType type, LineCommon* lineCommon);
			/// <summary>
			/// 更新
			/// </summary>
			void Update();
			/// <summary>
			/// デバック確認ImGui
			/// </summary>
			void DebugImgui();
			/// <summary>
			/// パーティクルとの当たり判定
			/// </summary>
			bool IsCollisionAABB(const Vector3& point);


		public:
			/// <summary>
			/// 名前取得
			/// </summary>
			std::string GetName() const { return name_; }
			/// <summary>
			/// エフェクトがかかっているか
			/// </summary>
			bool GetIsEffect() const { return isEffect; }
			// 親子関係設定
			void SetParent(WorldTransform& parent)
			{
				transform_.parent_ = &parent;
			}

		public:	// 各パラメータ
			EffectType effectType_ = EffectType::kAcceleration;					// エフェクト種類選択
			Vector3 acceleration_{};											// 加速
			AABB range_{ -Vector3{1.0f,1.0f,1.0f},Vector3{1.0f,1.0f,1.0f} };	// 範囲
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

			Range <Vector3> randomRange{};										// 乱数範囲 (Vector3の範囲)

		private:
			bool isEffect = true;												// 効果を出すか
		private:
			ShapeType shapeType_ = ShapeType::kAABB;							// 形状選択
			std::string name_ = "EffectFildeAABB";								// 名前
			std::string nameType = "Acceleration";								// タイプ名前
			LineCommon* lineCommon_;
		};

		/// <summary>
		/// パーティクルにかかるフィールドエフェクトの処理
		/// </summary>
		void Effect(ParticleGroup& grop, std::list<Particle>::iterator& particleIterator, std::vector<Field::FieldEffect*> fieldEffect, float deltaTime);


	}
}
