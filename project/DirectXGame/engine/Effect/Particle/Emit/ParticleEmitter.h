#pragma once

#include "BaseParticleEmitter.h"


namespace Engine {
	// 点エミッター
/// <summary>
/// PointParticleEmitterを管理・実装するクラス。
/// </summary>
	class PointParticleEmitter : public BaseParticleEmitter {
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;
	private:
		/// <summary>
		/// パーティクル発生
		/// </summary>
		void EmitUnique() override;
		/// <summary>
		/// エミッターライン描画
		/// </summary>
		void DrawEmitterLine() override;
	};

	// AABBエミッター
/// <summary>
/// AABBParticleEmitterを管理・実装するクラス。
/// </summary>
	class AABBParticleEmitter : public BaseParticleEmitter {
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;
	private:
		/// <summary>
		/// パーティクル発生
		/// </summary>
		void EmitUnique() override;
		/// <summary>
		/// デバックImGui
		/// </summary>
		void DebugImGui() override;
		/// <summary>
		/// エミッターライン描画
		/// </summary>
		void DrawEmitterLine() override;
		/// <summary>
		/// グローバルバリアブル適応
		/// </summary>
		void ApplyGlobalVariablesUnique() override;
	public:
		/// <summary>
		/// 範囲設定
		/// </summary>
		void SetRange(Vector3 min, Vector3 max);
	private:
		Range<Vector3> range_;	// 出現範囲
	};

	// 球エミッター
/// <summary>
/// SphereParticleEmitterを管理・実装するクラス。
/// </summary>
	class SphereParticleEmitter : public BaseParticleEmitter {
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;

	private:
		/// <summary>
		/// パーティクル発生
		/// </summary>
		void EmitUnique() override;
		/// <summary>
		/// デバックImGui
		/// </summary>
		void DebugImGui() override;
		/// <summary>
		/// エミッターライン描画
		/// </summary>
		void DrawEmitterLine() override;
		/// <summary>
		/// グローバルバリアブル適応
		/// </summary>
		void ApplyGlobalVariablesUnique() override;
	public:
		/// <summary>
		/// 半径設定
		/// </summary>
		void SetRadius(float radius) { radius_ = radius; }
	private:
		float radius_ = 1.0f; // 半径
	};

	// 円エミッター
/// <summary>
/// CornerParticleEmitterを管理・実装するクラス。
/// </summary>
	class CornerParticleEmitter : public BaseParticleEmitter {
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;
	private:
		/// <summary>
		/// パーティクル発生
		/// </summary>
		void EmitUnique() override;
		/// <summary>
		/// 固有更新
		/// </summary>
		void UpdateUnique() override;
		/// <summary>
		/// デバックImGui
		/// </summary>
		void DebugImGui() override;
		/// <summary>
		/// エミッターライン描画
		/// </summary>
		void DrawEmitterLine() override;
		/// <summary>
		/// グローバルバリアブル適応
		/// </summary>
		void ApplyGlobalVariablesUnique() override;
	public:
		/// <summary>
		/// 半径設定
		/// </summary>
		void SetRadius(float radius) { corner.radius = radius; }
		/// <summary>
		/// セグメント設定
		/// </summary>
		void SetSegment(int segment) { corner.segment = segment; }
	private:
		CornerSegment corner;
	};

	//	ラインエミッター
/// <summary>
/// LineParticleEmitterを管理・実装するクラス。
/// </summary>
	class LineParticleEmitter : public BaseParticleEmitter {
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;
	private:
		/// <summary>
		/// パーティクル発生
		/// </summary>
		void EmitUnique() override;
		/// <summary>
		/// デバックImGui
		/// </summary>
		void DebugImGui() override;
		/// <summary>
		/// エミッターライン描画
		/// </summary>
		void DrawEmitterLine() override;
		/// <summary>
		/// グローバルバリアブル適応
		/// </summary>
		void ApplyGlobalVariablesUnique() override;
	public:
		/// <summary>
		/// セグメント設定
		/// </summary>
		void SetSegment(Vector3 origin, Vector3 end);

	private:
		Segment segment_ = Segment(Vector3{}, Vector3{});

	};

	// スプライン
/// <summary>
/// SplineParticleEmitterを管理・実装するクラス。
/// </summary>
	class SplineParticleEmitter : public BaseParticleEmitter {
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;
	private:
		/// <summary>
		/// パーティクル発生
		/// </summary>
		void EmitUnique() override;
		/// <summary>
		/// デバックImGui
		/// </summary>
		void DebugImGui() override;
		/// <summary>
		/// エミッターライン描画
		/// </summary>
		void DrawEmitterLine() override;
		/// <summary>
		/// グローバルバリアブル適応
		/// </summary>
		void ApplyGlobalVariablesUnique() override;
	public:
		/// <summary>
		/// コントロールポイント追加
		/// </summary>
		void AddControlPoints(const Vector3& pos) { controlPoints.push_back(pos); }
		/// <summary>
		/// コントロールポイント削除
		/// </summary>
		void Clear() { controlPoints.clear(); };
		/// <summary>
		/// コントロールポイントに位置設定
		/// </summary>
		void SetControlPos(int index, Vector3 pos);

	private:
		std::vector<Vector3> controlPoints; // 各ポジション
	};

	// 三角
/// <summary>
/// TriangleParticleEmitterを管理・実装するクラス。
/// </summary>
	class TriangleParticleEmitter : public BaseParticleEmitter {
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;
	private:
		/// <summary>
		/// パーティクル発生
		/// </summary>
		void EmitUnique() override;
		/// <summary>
		/// デバックImGui
		/// </summary>
		void DebugImGui() override;
		/// <summary>
		/// エミッターライン描画
		/// </summary>
		void DrawEmitterLine() override;
		/// <summary>
		/// グローバルバリアブル適応
		/// </summary>
		void ApplyGlobalVariablesUnique() override;
	public:
		/// <summary>
		/// 頂点設定
		/// </summary>
		void SetTriangle(Triangle triangle) { triangle_ = triangle; }
	private:
		Triangle triangle_;
	};

	// メッシュ
/// <summary>
/// MeshParticleEmitterを管理・実装するクラス。
/// </summary>
	class MeshParticleEmitter : public BaseParticleEmitter {
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;
		/// <summary>
		/// メッシュ設定
		/// </summary>
		void SetMesh(ModelMesh* modelMesh) { modelMesh_ = modelMesh; };

	private:
		/// <summary>
		/// パーティクル発生
		/// </summary>
		void EmitUnique() override;
		/// <summary>
		/// デバックImGui
		/// </summary>
		void DebugImGui() override;
		/// <summary>
		/// エミッターライン描画
		/// </summary>
		void DrawEmitterLine() override;
		/// <summary>
		/// グローバルバリアブル適応
		/// </summary>
		void ApplyGlobalVariablesUnique() override;
	public:

	private:
		ModelMesh* modelMesh_ = nullptr; // メッシュ未設定時にエミット処理を安全に止めるためnullptrで初期化
	};
}
