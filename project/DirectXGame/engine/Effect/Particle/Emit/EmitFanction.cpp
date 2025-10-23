#include "EmitFanction.h"
#include "DirectXGame/engine/Math/Random.h"

void EmitFanction::CreateParticle(ParticleGroup& group, EffectEmitData emitData, std::mt19937& randomEngine, Vector3 emitPos)
{
	Particle newParticle;

	// パーティクルの初期化 (必要に応じて詳細を設定)
	// 位置
	newParticle.transform.translate = emitPos;

	// 大きさ
	if (emitData.isUniformSize) {
		Vector3 size = Random::RandVector3(emitData.size, randomEngine);

		newParticle.transform.scale = { size.x,size.x ,size.x };
	}
	else {
		newParticle.transform.scale = Random::RandVector3(emitData.size, randomEngine);
	}
	
	//速度
	newParticle.velocity = Random::RandVector3(emitData.velocity, randomEngine);

	
	// 回転
	Vector3 rotate{};

	if (emitData.isDirectionRotate) {
		rotate = Math::DirectionToRotate(newParticle.velocity.Normalize(),Dire::X);
	}
	else {
		rotate = Random::RandVector3(emitData.rotate, randomEngine);
	}

	newParticle.transform.rotate = rotate;

	// サイズ変化量
	newParticle.sizeAmount = Random::RandVector3(emitData.sizeAmount, randomEngine);

	// 色
	newParticle.color = Random::RandVector4(emitData.colorRange, randomEngine);

	// 回転速度
	newParticle.rotateVelocity = Random::RandVector3(emitData.rotateVelocity, randomEngine);

	
	// ライフタイム
	newParticle.lifeTime = Random::RandFloat(emitData.lifeTime, randomEngine);

	// 加速度
	newParticle.acceleration = Random::RandVector3(emitData.acceleration, randomEngine);

	// タイム
	newParticle.currentTime = 0;

	// 初期値
	newParticle.strtTransform = newParticle.transform;


	//

	newParticle.isNoise = emitData.isNoise;

	newParticle.direction = emitData.direction;


	group.particle.push_back(newParticle);
}

void EmitFanction::CreateParticle(ParticleGroup& group, EffectEmitData emitData, std::mt19937& randomEngine, Vector3 emitPos, Vector3 direction)
{
	Particle newParticle;

	// パーティクルの初期化 (必要に応じて詳細を設定)
	// 位置
	newParticle.transform.translate = emitPos;

	// 大きさ
	if (emitData.isUniformSize) {
		Vector3 size = Random::RandVector3(emitData.size, randomEngine);

		newParticle.transform.scale = { size.x,size.x ,size.x };
	}
	else {
		newParticle.transform.scale = Random::RandVector3(emitData.size, randomEngine);
	}

	// 回転
	newParticle.transform.rotate = Random::RandVector3(emitData.rotate, randomEngine);

	// 色
	newParticle.color = Random::RandVector4(emitData.colorRange, randomEngine);

	// サイズ変化量
	newParticle.sizeAmount = Random::RandVector3(emitData.sizeAmount, randomEngine);


	// 回転速度
	newParticle.rotateVelocity = Random::RandVector3(emitData.rotateVelocity, randomEngine);

	//速度
	newParticle.velocity = direction;

	// ライフタイム
	newParticle.lifeTime = Random::RandFloat(emitData.lifeTime, randomEngine);

	// 加速度
	newParticle.acceleration = Random::RandVector3(emitData.acceleration, randomEngine);

	// タイム
	newParticle.currentTime = 0;

	// 初期値
	newParticle.strtTransform = newParticle.transform;

	//

	newParticle.isNoise = emitData.isNoise;

	newParticle.direction = emitData.direction;

	group.particle.push_back(newParticle);
}

void EmitFanction::CreateParticle(ParticleGroup2d& group, EffectEmitData2d emitData, std::mt19937& randomEngine, Vector3 emitPos)
{
	Particle newParticle;

	// パーティクルの初期化 (必要に応じて詳細を設定)
	// 位置
	newParticle.transform.translate = emitPos;
	Vector2 scale = Random::RandVector2(emitData.size, randomEngine);

	// 大きさ
	newParticle.transform.scale.x = scale.x;
	newParticle.transform.scale.y = scale.y;
	newParticle.transform.scale.z = 0.0f;
	// 回転
	newParticle.transform.rotate = Random::RandFloat(emitData.rotate, randomEngine);

	// 色
	newParticle.color = Random::RandVector4(emitData.colorRange, randomEngine);

	// 回転速度
	newParticle.rotateVelocity = Random::RandFloat(emitData.rotateVelocity, randomEngine);


	Vector2 velo = Random::RandVector2(emitData.velocity, randomEngine);
	//速度
	newParticle.velocity.x = velo.x;
	newParticle.velocity.y = velo.y;


	// ライフタイム
	newParticle.lifeTime = Random::RandFloat(emitData.lifeTime, randomEngine);

	Vector2 acc = Random::RandVector2(emitData.acceleration, randomEngine);

	// 加速度
	newParticle.acceleration.x = acc.x;
	newParticle.acceleration.y = acc.y;
	newParticle.acceleration.z = 0.0f;

	// タイム
	newParticle.currentTime = 0;

	// 初期値
	newParticle.strtTransform = newParticle.transform;

	group.particle.push_back(newParticle);
}




