#include "EmitFanction.h"


float EmitFanction::RandFloat(const MaxMin<float>& vec3, std::mt19937& randomEngine)
{
	std::uniform_real_distribution<float> vec(vec3.min, vec3.max);

	return vec(randomEngine);
}

Vector2 EmitFanction::RandVector2(const MaxMin<Vector2>& vec3, std::mt19937& randomEngine)
{
	std::uniform_real_distribution<float> vecX(vec3.min.x, vec3.max.x);
	std::uniform_real_distribution<float> vecY(vec3.min.y, vec3.max.y);


	return Vector2{ vecX(randomEngine),vecY(randomEngine) };
}

Vector3 EmitFanction::RandVector3(const MaxMin<Vector3>& vec3, std::mt19937& randomEngine)
{
	std::uniform_real_distribution<float> vecX(vec3.min.x, vec3.max.x);
	std::uniform_real_distribution<float> vecY(vec3.min.y, vec3.max.y);
	std::uniform_real_distribution<float> vecZ(vec3.min.z, vec3.max.z);


	return Vector3{ vecX(randomEngine),vecY(randomEngine),vecZ(randomEngine) };
}

Vector4 EmitFanction::RandVector4(const MaxMin<Vector4>& vec3, std::mt19937& randomEngine)
{
	std::uniform_real_distribution<float> vecX(vec3.min.x, vec3.max.x);
	std::uniform_real_distribution<float> vecY(vec3.min.y, vec3.max.y);
	std::uniform_real_distribution<float> vecZ(vec3.min.z, vec3.max.z);
	std::uniform_real_distribution<float> vecW(vec3.min.w, vec3.max.w);


	return Vector4{ vecX(randomEngine),vecY(randomEngine),vecZ(randomEngine),vecW(randomEngine)};
}

Vector3 EmitFanction::EmitPos(ParticleGroup& particleGroup, ParticleData::SpawnType emitType, std::mt19937& randomEngine, int index)
{
	Vector3 result{};

	std::vector<Vector3> vertices;
	float angleStep;
	Segment segment_{ Vector3{},Vector3{} };
	switch (emitType)
	{
	case ParticleData::SpawnType::kPoint:
		result = particleGroup.emiter.worldtransform.worldMat_.GetWorldPosition();
		break;
	case ParticleData::SpawnType::kAABB:
		result = particleGroup.emiter.worldtransform.worldMat_.GetWorldPosition() + EmitFanction::RandVector3(particleGroup.emiter.renge, randomEngine);
		break;
	case ParticleData::SpawnType::kOBB:
		break;
	case ParticleData::SpawnType::kSphere:
		break;
	case ParticleData::SpawnType::kSegmentLine:
		// 発生セグメントの設定
		
		segment_.origin = particleGroup.emiter.renge.min;  // 始点
		segment_.end = particleGroup.emiter.renge.max;    // 終点
		result = particleGroup.emiter.worldtransform.worldMat_.GetWorldPosition() +
			Lerp(segment_.origin, segment_.end, EmitFanction::RandFloat({ 0.0f,1.0f }, randomEngine));
		break;
	case ParticleData::SpawnType::kCornerLine:
		// 頂点を計算
		
		angleStep = DirectX::XM_2PI / particleGroup.emiter.corner.segment; // 360° を segment 分割

		// 回転行列を作成
		Matrix4x4 rotationMatrix = MakeRotateXYZ(particleGroup.emiter.worldtransform.rotate_);
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


		result = Lerp(vertices[index], vertices[(index + 1) % particleGroup.emiter.corner.segment], EmitFanction::RandFloat({ 0.0f,1.0f }, randomEngine))
			+ particleGroup.emiter.worldtransform.worldMat_.GetWorldPosition();
		break;
	case ParticleData::SpawnType::kSpline:
		if (particleGroup.emiter.controlPoints.size() < 4) {
			return {0,0,0};
		}

		result = CatmullRom(particleGroup.emiter.controlPoints, EmitFanction::RandFloat({ 0.0f,1.0f }, randomEngine)) 
			+ particleGroup.emiter.worldtransform.worldMat_.GetWorldPosition();
		break;
	default:
		result = { 0,0,0 };
		break;
	}


	return result;
}

void EmitFanction::ConversionMinMaxFloat(MaxMin<float>& renge)
{
	renge.min = (std::min)(renge.min, renge.max);
	renge.max = (std::max)(renge.min, renge.max);
}

void EmitFanction::ConversionMinMaxV2(MaxMin<Vector2>& renge)
{
	renge.min.x = (std::min)(renge.min.x, renge.max.x);
	renge.max.x = (std::max)(renge.min.x, renge.max.x);
	renge.min.y = (std::min)(renge.min.y, renge.max.y);
	renge.max.y = (std::max)(renge.min.y, renge.max.y);
}

void EmitFanction::ConversionMinMaxV3(MaxMin<Vector3>& renge)
{
	renge.min.x = (std::min)(renge.min.x, renge.max.x);
	renge.max.x = (std::max)(renge.min.x, renge.max.x);
	renge.min.y = (std::min)(renge.min.y, renge.max.y);
	renge.max.y = (std::max)(renge.min.y, renge.max.y);
	renge.min.z = (std::min)(renge.min.z, renge.max.z);
	renge.max.z = (std::max)(renge.min.z, renge.max.z);
}

void EmitFanction::ConversionMinMaxV4(MaxMin<Vector4>& renge)
{
	renge.min.x = (std::min)(renge.min.x, renge.max.x);
	renge.max.x = (std::max)(renge.min.x, renge.max.x);
	renge.min.y = (std::min)(renge.min.y, renge.max.y);
	renge.max.y = (std::max)(renge.min.y, renge.max.y);
	renge.min.z = (std::min)(renge.min.z, renge.max.z);
	renge.max.z = (std::max)(renge.min.z, renge.max.z);
	renge.min.w = (std::min)(renge.min.w, renge.max.w);
	renge.max.w = (std::max)(renge.min.w, renge.max.w);
}

void EmitFanction::CreateParticle(ParticleGroup& group, std::mt19937& randomEngine, ParticleData::SpawnType spawnType, int index)
{
	Particle newParticle;

	// パーティクルの初期化 (必要に応じて詳細を設定)
	// 位置
	newParticle.transform.translate = EmitFanction::EmitPos(group, spawnType, randomEngine, index);

	// 大きさ
	newParticle.transform.scale = EmitFanction::RandVector3(group.emiter.size, randomEngine);

	// 回転
	newParticle.transform.rotate = EmitFanction::RandVector3(group.emiter.rotate, randomEngine);

	// 色
	newParticle.color = EmitFanction::RandVector4(group.emiter.color, randomEngine);

	// 回転速度
	newParticle.rotateVelocity = EmitFanction::RandVector3(group.emiter.rotateVelocity, randomEngine);

	//速度
	newParticle.velocity = EmitFanction::RandVector3(group.emiter.velocity, randomEngine);

	// ライフタイム
	newParticle.lifeTime = EmitFanction::RandFloat(group.emiter.lifeTime, randomEngine);

	// 加速度
	newParticle.acceleration = EmitFanction::RandVector3(group.emiter.acceleration, randomEngine);

	// タイム
	newParticle.currentTime = 0;

	// 初期値
	newParticle.strtTransform = newParticle.transform;

	// 
	newParticle.isEvent = group.emiter.isEvent;

	newParticle.fieldEffect = group.emiter.fieldName;
	

	group.particle.push_back(newParticle);
}




