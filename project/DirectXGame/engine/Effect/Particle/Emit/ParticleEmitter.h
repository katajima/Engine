#pragma once

#include "BaseParticleEmitter.h"

// 点エミッター
class PointParticleEmitter : public BaseParticleEmitter {
public:
	void Initialize(ParticleManager* particleManager, std::string emitName, std::string particleName) override;
private:
	void EmitUniqe() override;

	void DrawEmitterLine() override;
};

// AABBエミッター
class AABBParticleEmitter : public BaseParticleEmitter {
public:
	void Initialize(ParticleManager* particleManager, std::string emitName, std::string particleName) override;
private:
	void EmitUniqe() override;

	void DebugImGui() override;

	void DrawEmitterLine() override;
public:
	void SetRange(Vector3 min, Vector3 max);// 範囲設定
private:
	Range<Vector3> range_;	// 出現範囲
};

// 球エミッター
class SphereParticleEmitter : public BaseParticleEmitter {
public:
	void Initialize(ParticleManager* particleManager, std::string emitName, std::string particleName) override;

private:
	void EmitUniqe() override;

	void DebugImGui() override;

	void DrawEmitterLine() override;

public:

	void SetRadius(float radius) { radius_ = radius; } // 半径
private:
	float radius_ = 1.0f; // 半径
};

// 円エミッター
class CornerParticleEmitter : public BaseParticleEmitter {
public:
	void Initialize(ParticleManager* particleManager, std::string emitName, std::string particleName) override;
private:
	void EmitUniqe() override;

	void UpdateUniqe() override;

	void DebugImGui() override;

	void DrawEmitterLine() override;

public:

	void SetRadius(float radius) { corner.radius = radius; } // 半径
	void SetRadius(int segment) { corner.segment = segment; } // セグメント
private:
	CornerSegment corner;
};

//	ラインエミッター
class LineParticleEmitter : public BaseParticleEmitter {
public:
	void Initialize(ParticleManager* particleManager, std::string emitName, std::string particleName) override;


private:
	void EmitUniqe() override;

	void DebugImGui() override;

	void DrawEmitterLine() override;
public:
	void SetSegment(Vector3 origin, Vector3 end);

private:
	Segment segment_ = Segment(Vector3{}, Vector3{});

};

// スプライン
class SplineParticleEmitter : public BaseParticleEmitter {
public:
	void Initialize(ParticleManager* particleManager, std::string emitName, std::string particleName) override;



private:
	void EmitUniqe() override;

	void DebugImGui() override;

	void DrawEmitterLine() override;
public:

	void AddControlPoints(const Vector3& pos) { controlPoints.push_back(pos); }

	void Clear() { controlPoints.clear(); };

	void SetControlPos(int index, Vector3 pos);

private:
	std::vector<Vector3> controlPoints; // 各ポジション
};

// 三角
class TriangleParticleEmitter : public BaseParticleEmitter {
public:
	void Initialize(ParticleManager* particleManager, std::string emitName, std::string particleName) override;

private:
	void EmitUniqe() override;

	void DebugImGui() override;

	void DrawEmitterLine() override;
public:

private:
	Triangle triangle_ = Triangle({}, {}, {});
};

// メッシュ
class MeshParticleEmitter : public BaseParticleEmitter {
public:
	void Initialize(ParticleManager* particleManager, std::string emitName, std::string particleName) override;

	void SetMesh(ModelMesh* modelMesh) { modelMesh_ = modelMesh; };

private:
	void EmitUniqe() override;

	void DebugImGui() override;

	void DrawEmitterLine() override;
public:

private:
	ModelMesh* modelMesh_;
};