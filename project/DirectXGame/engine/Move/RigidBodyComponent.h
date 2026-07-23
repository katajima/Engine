#pragma once
#include"DirectXGame/engine/math/MathFunctions.h"
#include "DirectXGame/engine/Transform/TransformComponent.h"


/// <summary>
/// リジットボディー
/// </summary>
namespace Engine {
/// <summary>
/// RigidBodyComponentを管理・実装するクラス。
/// </summary>
	class RigidBodyComponent
	{
	public:
		/// <summary>
		/// 力加算
		/// </summary>
		void AddForce(const Vector3& f) {
			force += f;
		}

		/// <summary>
		/// トルク加算
		/// </summary>
		void AddTorque(const Vector3& t) {
			torque += t;
		}
		/// <summary>
		/// 統合
		/// </summary>
		void Integrate(float deltaTime, WorldTransform& transform);

		/// <summary>
		/// 重力倍率設定
		/// </summary>
		void SetGravityScale(float scale) { gravityScale = scale; }
		/// <summary>
		/// 重力あるか取得
		/// </summary>
		bool IsGravity() const { return useGravity; }
		/// <summary>
		/// 重力あるか設定
		/// </summary>
		void SetIsGravity(bool is) { useGravity = is; }
		/// <summary>
		/// 速度取得
		/// </summary>
		Vector3 GetVelocity() const { return velocity_; }
		/// <summary>
		/// 速度取得
		/// </summary>
		Vector3& Velocity() { return velocity_; }
		/// <summary>
		/// 質量設定
		/// </summary>
		void SetMass(float m) {
			mass_ = m;
			inverseMass = (m != 0.0f) ? 1.0f / m : 0.0f;
		}
		/// <summary>
		/// 加速度リセット
		/// </summary>
		void ResetAcceleration() { acceleration_ = {0.0f,0.0f,0.0f}; }
		/// <summary>
		/// 速度リセット
		/// </summary>
		void ResetVelocity() { velocity_ = { 0.0f,0.0f ,0.0f }; };
	private:
		/// <summary>
		/// 移動処理
		/// </summary>
		void ProcessTranslation(float deltaTime, WorldTransform& transform);

		/// <summary>
		/// 回転処理
		/// </summary>
		void ProcessRotation(float deltaTime, WorldTransform& transform);
	private:
		Vector3 velocity_ = { 0,0,0 };				// 速度
		Vector3 acceleration_ = { 0, 0, 0 };		// 加速度
		Vector3 angularVelocity = { 0, 0, 0 };		// 角速度（回転）
		float mass_ = 1.0f;							// 質量
		float inverseMass = 1.0f;					// 計算用の逆質量（質量0の除算対策）

		Vector3 force = { 0, 0, 0 };				// 現在加えられている合力（フレーム毎に加算してリセット）
		Vector3 torque = { 0, 0, 0 };				// 回転方向の力（トルク）


		float gravity = 9.8f;						// 重力
		float gravityScale = 1.0f;					// 重力の強さの倍率

		float friction = 0.3f;						// 摩擦係数（床とのすべり）
		float restitution = 0.1f;					// 反発係数（跳ね返り）

		bool useGravity = true;						// 重力の影響を受けるか
		bool isKinematic = false;					// 外部力を受けない（スクリプト制御用）

		bool isSleeping = false;					// 物体が静止していて物理更新をスキップしてよいか
	};
}
