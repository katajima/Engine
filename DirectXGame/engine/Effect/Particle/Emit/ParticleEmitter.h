#pragma once

// C;;
#include<random>
#include<numbers>

// engine
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/Effect/Particle/ParticleManager.h"
#include"DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"

class LineCommon;
class ParticleEmitter
{
public:
	~ParticleEmitter() {

	}

	void Initialize(ParticleManager* particleManager, std::string emitName, std::string particleName, EmitData::SpawnType spawnType = EmitData::SpawnType::kAABB);

	void Update();

	void Emit();
public: // ゲッター兼セッター


	void AddControlPoints(const Vector3& pos) { emitter_.controlPoints.push_back(pos); }

	float& GetFrequency() { return frequency_; };//発生数

	int& GetCount() { return emitter_.count; };// 発生頻度



	void SetParent(WorldTransform& parent);// 親子付け

	void SetCount(int count) { emitter_.count = count; }; // 出現数

	void SetFrequency(float frequency) { frequency_ = frequency_; } // 頻度

	void SetPos(Vector3 pos) { transform_.translate_ = pos; } // 位置

	void SetAlphaClipping(float alpha) { alphaClipping_ = alpha; }

	void SetEnableLighting(int lighting) { enableLighting_ = lighting; }

	void SetIsEmit(bool is) { isEmit = is; } // 出すか

	void SetIsLineInterpolation(bool is) { isFlag.isLineInterpolation = is; } // 線形補間

	void SetIsGravity(bool is) { isFlag.isGravity = is; } // 重力

	void SetUsebillboard(bool is) { isFlag.usebillboard = is; } // ビルボード
	void SetUsebillboardRotZ(bool is) { isFlag.billboardRotZ = is; } // ビルボードZ

	void SetIsAlpha(bool is) { isFlag.isAlpha = is; } // 透明度

	void SetIsAcceleration(bool is) { isFlag.isAcceleration = is; } // 

	void SetIsLifeTimeScale(bool is) { isFlag.isLifeTimeScale_ = is; } // スケール変更

	void SetLifeTimeScaleTopBottom(EmitData::TopBottom topBottom) { topBottom_ = topBottom; }

	void SetIsRotateVelocity(bool is) { isFlag.isRotateVelocity = is; } // 回転速度

	void SetIsBounce(bool is) { isFlag.isBounce = is; } // 跳ねるか

	void SetIsAll(bool billboard, bool alpha, bool gravity, bool isLifeTimeScale, bool rotateVelocity);

	void SetCorner(int segment, float rad) { emitter_.corner.radius = rad; emitter_.corner.segment = segment; }

	void SetUvRotateVeloctiy(Vector3 rotate) { uvTransformVeloctiy_.rotate = rotate; };

	void SetUvScaleVeloctiy(Vector3 scale) { uvTransformVeloctiy_.scale = scale; };

	void SetUvTranslateVeloctiy(Vector3 translate) { uvTransformVeloctiy_.translate = translate; };

	void SetUvTransformVeloctiy(Transform uvTransform) { uvTransformVeloctiy_ = uvTransform; }

	// ランダム用
	void SetRengeMinMax(Vector3 min, Vector3 max) { emitter_.renge.min = min; emitter_.renge.max = max; }; // 範囲

	void SetColorMinMax(Vector4 min, Vector4 max) { emitter_.color.min = min; emitter_.color.max = max; } // カラー

	void SetSizeMinMax(Vector3 min, Vector3 max) { emitter_.size.min = min; emitter_.size.max = max; } // サイズ

	void SetRotateMinMax(Vector3 min, Vector3 max) { emitter_.rotate.min = min; emitter_.rotate.max = max; } // 回転

	void SetLifeTimeMinMax(float min, float max) { emitter_.lifeTime.min = min; emitter_.lifeTime.max = max; } // 生存時間

	void SetVelocityMinMax(Vector3 min, Vector3 max) { emitter_.velocity.min = min; emitter_.velocity.max = max; } // 速度

	void SetRotateVelocityMinMax(Vector3 min, Vector3 max) { emitter_.rotateVelocity.min = min; emitter_.rotateVelocity.max = max; } // 速度

	void SetAccelerationMinMax(Vector3 min, Vector3 max) { emitter_.acceleration.min = min; emitter_.acceleration.max = max; } // 速度

	void SetUseFieldName(std::vector<std::string> fieldName) { // フィールド名
		for (auto& name : fieldName) {
			emitter_.fieldName.push_back(name);
		}
	} 

private: // エミッタ可視化

	void DrawEmitterLine();

	void EmitMinMax();

private:
	EmitData::EmitType emitType_ = EmitData::EmitType::kRandom; // 出現方法
	EmitData::TopBottom topBottom_ = EmitData::TopBottom::kBottom; // 拡縮
	EmitData::SpawnType spawnShapeType_ = EmitData::SpawnType::kAABB; // 出現形状

private:
	// パーティクル制御に必要なフラグ
	EmitData::IsFlag isFlag;

	// 出すか
	bool isEmit = true; // 出すか

	float alphaClipping_ = 0.5f; // αクリッピング
	int enableLighting_ = false; // ライティング
private:
	Transform uvTransformVeloctiy_{};	// UVトランスフォーム
public:
	WorldTransform transform_;			// ワールドトランスフォーム
private:
	float frequency_;		// < 発生頻度
	float frequencyTime_;	// < 頻度用時刻



	std::string emitName_; // エミッター名
	std::string particleName_; // パーティクル名


	Emiter emitter_{};

	ParticleManager* particleManager_;
	LineCommon* lineCommon_ = nullptr;
};



