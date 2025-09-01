#include "EmitFanction.h"
#include "DirectXGame/engine/Math/Random.h"
Vector3 EmitFanction::EmitPos(ParticleGroup& particleGroup, WorldTransform& transform, EmitData::SpawnType emitType, std::mt19937& randomEngine, int index)
{
	Vector3 result{};

	std::vector<Vector3> vertices;
	float angleStep;
	Segment segment_{ Vector3{},Vector3{} };
	switch (emitType)
	{
	case EmitData::SpawnType::kPoint:
		result = transform.worldMat_.GetWorldPosition();
		break;
	case EmitData::SpawnType::kAABB:
		result = transform.worldMat_.GetWorldPosition() + Random::RandVector3(particleGroup.emiter.renge, randomEngine);
		break;
	case EmitData::SpawnType::kSphere:
		break;
	case EmitData::SpawnType::kSegmentLine:
		// 発生セグメントの設定
		
		if (particleGroup.isFlag.isLineInterpolation) {
			segment_.origin = particleGroup.emiter.renge.min;  // 始点
			segment_.end = particleGroup.emiter.renge.max;    // 終点

			// 線分の始点と終点を補間して位置を決定
			float t = static_cast<float>(index) / static_cast<float>(particleGroup.emiter.count);

			Vector3 lerpPosition = Lerp(segment_.origin, segment_.end, t);

			result = transform.worldMat_.GetWorldPosition() + lerpPosition;
		}
		else {
			segment_.origin = particleGroup.emiter.renge.min;  // 始点
			segment_.end = particleGroup.emiter.renge.max;    // 終点
			result = transform.worldMat_.GetWorldPosition() +
				Lerp(segment_.origin, segment_.end, Random::RandFloat({ 0.0f,1.0f }, randomEngine));
		}

		
		break;
	case EmitData::SpawnType::kCornerLine:
		// 頂点を計算
		
		angleStep = DirectX::XM_2PI / particleGroup.emiter.corner.segment; // 360° を segment 分割

		// 回転行列を作成
		Matrix4x4 rotationMatrix = MakeRotateXYZ(transform.rotate_);
		for (int i = 0; i < particleGroup.emiter.corner.segment; ++i)
		{
			float angle = i * angleStep; // 各頂点の角度
			Vector3 localVertex;
			localVertex.x = cos(angle) * particleGroup.emiter.corner.radius;
			localVertex.y = 0.0f;
			localVertex.z = sin(angle) * particleGroup.emiter.corner.radius;

			// 回転を適用
			Vector3 rotatedVertex = rotationMatrix.Transform(localVertex);

			// ワールド座標へ変換
			Vector3 worldVertex = rotatedVertex;
			vertices.push_back(worldVertex);
		}


		result = Lerp(vertices[index], vertices[(index + 1) % particleGroup.emiter.corner.segment], Random::RandFloat({ 0.0f,1.0f }, randomEngine))
			+ transform.worldMat_.GetWorldPosition();
		break;
	case EmitData::SpawnType::kSpline:
		if (particleGroup.emiter.controlPoints.size() < 4) {
			return {0,0,0};
		}

		result = CatmullRom(particleGroup.emiter.controlPoints, Random::RandFloat({ 0.0f,1.0f }, randomEngine))
			+ transform.worldMat_.GetWorldPosition();
		break;
	default:
		result = { 0,0,0 };
		break;
	}


	return result;
}



void EmitFanction::CreateParticle(ParticleGroup& group,WorldTransform& transform ,std::mt19937& randomEngine, EmitData::SpawnType spawnType, int index)
{
	Particle newParticle;

	// パーティクルの初期化 (必要に応じて詳細を設定)
	// 位置
	newParticle.transform.translate = EmitFanction::EmitPos(group, transform, spawnType, randomEngine, index);

	// 大きさ
	newParticle.transform.scale = Random::RandVector3(group.emiter.size, randomEngine);

	// 回転
	newParticle.transform.rotate = Random::RandVector3(group.emiter.rotate, randomEngine);

	// 色
	newParticle.color = Random::RandVector4(group.emiter.color, randomEngine);

	// 回転速度
	newParticle.rotateVelocity = Random::RandVector3(group.emiter.rotateVelocity, randomEngine);

	//速度
	newParticle.velocity = Random::RandVector3(group.emiter.velocity, randomEngine);

	// ライフタイム
	newParticle.lifeTime = Random::RandFloat(group.emiter.lifeTime, randomEngine);

	// 加速度
	newParticle.acceleration = Random::RandVector3(group.emiter.acceleration, randomEngine);

	// タイム
	newParticle.currentTime = 0;

	// 初期値
	newParticle.strtTransform = newParticle.transform;

	newParticle.fieldEffect = group.emiter.fieldName;
	

	group.particle.push_back(newParticle);
}




