#pragma once

#include "BaseParticleEmitter.h"

// 点エミッター
class PointParticleEmitter : public BaseParticleEmitter {
public:
	// 初期化
	void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;
private:
	// パーティクル発生
	void EmitUniqe() override;
	// エミッターライン描画
	void DrawEmitterLine() override;
};

// AABBエミッター
class AABBParticleEmitter : public BaseParticleEmitter {
public:
	// 初期化
	void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;
private:
	// パーティクル発生
	void EmitUniqe() override;
	// デバックImGui
	void DebugImGui() override;
	// エミッターライン描画
	void DrawEmitterLine() override;
	// グローバルバリアブル適応
	void ApplyGlobalVariablesUniqe() override;
public:
	// 範囲設定
	void SetRange(Vector3 min, Vector3 max);
private:
	Range<Vector3> range_;	// 出現範囲
};

// 球エミッター
class SphereParticleEmitter : public BaseParticleEmitter {
public:
	// 初期化
	void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;

private:
	// パーティクル発生
	void EmitUniqe() override;
	// デバックImGui
	void DebugImGui() override;
	// エミッターライン描画
	void DrawEmitterLine() override;
	// グローバルバリアブル適応
	void ApplyGlobalVariablesUniqe() override;
public:
	// 半径設定
	void SetRadius(float radius) { radius_ = radius; }
private:
	float radius_ = 1.0f; // 半径
};

// 円エミッター
class CornerParticleEmitter : public BaseParticleEmitter {
public:
	// 初期化
	void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;
private:
	// パーティクル発生
	void EmitUniqe() override;
	// 固有更新
	void UpdateUniqe() override;
	// デバックImGui
	void DebugImGui() override;
	// エミッターライン描画
	void DrawEmitterLine() override;
	// グローバルバリアブル適応
	void ApplyGlobalVariablesUniqe() override;
public:
	// 半径設定
	void SetRadius(float radius) { corner.radius = radius; } 
	// セグメント設定
	void SetRadius(int segment) { corner.segment = segment; } 
private:
	CornerSegment corner;
};

//	ラインエミッター
class LineParticleEmitter : public BaseParticleEmitter {
public:
	// 初期化
	void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;
private:
	// パーティクル発生
	void EmitUniqe() override;
	// デバックImGui
	void DebugImGui() override;
	// エミッターライン描画
	void DrawEmitterLine() override;
	// グローバルバリアブル適応
	void ApplyGlobalVariablesUniqe() override;
public:
	// セグメント設定
	void SetSegment(Vector3 origin, Vector3 end);

private:
	Segment segment_ = Segment(Vector3{}, Vector3{});

};

// スプライン
class SplineParticleEmitter : public BaseParticleEmitter {
public:
	// 初期化
	void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;
private:
	// パーティクル発生
	void EmitUniqe() override;
	// デバックImGui
	void DebugImGui() override;
	// エミッターライン描画
	void DrawEmitterLine() override;
	// グローバルバリアブル適応
	void ApplyGlobalVariablesUniqe() override;
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
class TriangleParticleEmitter : public BaseParticleEmitter {
public:
	// 初期化
	void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;
private:
	// パーティクル発生
	void EmitUniqe() override;
	// デバックImGui
	void DebugImGui() override;
	// エミッターライン描画
	void DrawEmitterLine() override;
	// グローバルバリアブル適応
	void ApplyGlobalVariablesUniqe() override;
public:

private:
	Triangle triangle_ = Triangle({}, {}, {});
};

// メッシュ
class MeshParticleEmitter : public BaseParticleEmitter {
public:
	// 初期化
	void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) override;
	// メッシュ設定
	void SetMesh(ModelMesh* modelMesh) { modelMesh_ = modelMesh; };

private:
	// パーティクル発生
	void EmitUniqe() override;
	// デバックImGui
	void DebugImGui() override;
	// エミッターライン描画
	void DrawEmitterLine() override;
	// グローバルバリアブル適応
	void ApplyGlobalVariablesUniqe() override;
public:

private:
	ModelMesh* modelMesh_;
};