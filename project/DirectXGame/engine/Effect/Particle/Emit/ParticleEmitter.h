#pragma once

#include "BaseParticleEmitter.h"


namespace Engine {
	// 点エミッター
/// <summary>
/// PointParticleEmitterを管理・実装するクラス。
/// </summary>
	class PointParticleEmitter : public BaseParticleEmitter {
	public:
		// 初期化
		void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;
	private:
		// パーティクル発生
		void EmitUnique() override;
		// エミッターライン描画
		void DrawEmitterLine() override;
	};

	// AABBエミッター
/// <summary>
/// AABBParticleEmitterを管理・実装するクラス。
/// </summary>
	class AABBParticleEmitter : public BaseParticleEmitter {
	public:
		// 初期化
		void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;
	private:
		// パーティクル発生
		void EmitUnique() override;
		// デバックImGui
		void DebugImGui() override;
		// エミッターライン描画
		void DrawEmitterLine() override;
		// グローバルバリアブル適応
		void ApplyGlobalVariablesUnique() override;
	public:
		// 範囲設定
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
		// 初期化
		void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;

	private:
		// パーティクル発生
		void EmitUnique() override;
		// デバックImGui
		void DebugImGui() override;
		// エミッターライン描画
		void DrawEmitterLine() override;
		// グローバルバリアブル適応
		void ApplyGlobalVariablesUnique() override;
	public:
		// 半径設定
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
		// 初期化
		void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;
	private:
		// パーティクル発生
		void EmitUnique() override;
		// 固有更新
		void UpdateUnique() override;
		// デバックImGui
		void DebugImGui() override;
		// エミッターライン描画
		void DrawEmitterLine() override;
		// グローバルバリアブル適応
		void ApplyGlobalVariablesUnique() override;
	public:
		// 半径設定
		void SetRadius(float radius) { corner.radius = radius; }
		// セグメント設定
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
		// 初期化
		void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;
	private:
		// パーティクル発生
		void EmitUnique() override;
		// デバックImGui
		void DebugImGui() override;
		// エミッターライン描画
		void DrawEmitterLine() override;
		// グローバルバリアブル適応
		void ApplyGlobalVariablesUnique() override;
	public:
		// セグメント設定
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
		// 初期化
		void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;
	private:
		// パーティクル発生
		void EmitUnique() override;
		// デバックImGui
		void DebugImGui() override;
		// エミッターライン描画
		void DrawEmitterLine() override;
		// グローバルバリアブル適応
		void ApplyGlobalVariablesUnique() override;
	public:
		// コントロールポイント追加
		void AddControlPoints(const Vector3& pos) { controlPoints.push_back(pos); }
		// コントロールポイント削除
		void Clear() { controlPoints.clear(); };
		// コントロールポイントに位置設定
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
		// 初期化
		void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;
	private:
		// パーティクル発生
		void EmitUnique() override;
		// デバックImGui
		void DebugImGui() override;
		// エミッターライン描画
		void DrawEmitterLine() override;
		// グローバルバリアブル適応
		void ApplyGlobalVariablesUnique() override;
	public:
		// 頂点設定
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
		// 初期化
		void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;
		// メッシュ設定
		void SetMesh(ModelMesh* modelMesh) { modelMesh_ = modelMesh; };

	private:
		// パーティクル発生
		void EmitUnique() override;
		// デバックImGui
		void DebugImGui() override;
		// エミッターライン描画
		void DrawEmitterLine() override;
		// グローバルバリアブル適応
		void ApplyGlobalVariablesUnique() override;
	public:

	private:
		ModelMesh* modelMesh_ = nullptr; // メッシュ未設定時にエミット処理を安全に止めるためnullptrで初期化
	};
}
