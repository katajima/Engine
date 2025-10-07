#pragma once
// C;;
#include<random>
#include<numbers>

// engine
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/Effect/Particle/CPU/ParticleManager.h"
#include"DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"

class GlobalVariables;
class LineCommon;
class BaseParticleEmitter
{
public:
	virtual ~BaseParticleEmitter() {}
	// 初期化
	virtual void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) = 0;
	// 更新
	void Update();
	// 発生
	void Emit(); 
	

protected:
	// 共通初期化
	void CommonParticleInit(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName);

	virtual void EmitUniqe() = 0;

	virtual void UpdateUniqe() {}; //

	virtual void DebugImGui() {};

	virtual void ApplyGlobalVariablesUniqe() {}; // グローバル変数適用

	void ApplyGlobalVariables(); // グローバル変数適用

public: // Getter Setter
	EffectEmitData& GetEmitData() { return emitData_; };	// エミットデータ

	float& GetFrequency() { return frequency_; };			//発生頻度

	int& GetCount() { return emitData_.count.median; };		// 出現数

	void SetParent(WorldTransform& parent) { transform_.parent_ = &parent;};// 親子付け

	void SetPos(Vector3 pos) { transform_.translate_ = pos; } // 位置

	void SetCount(int count, int range);					// 出現数

	void SetFrequency(float frequency) { frequency_ = frequency; } // 頻度

	void SetAlphaClipping(float alpha) { alphaClipping_ = alpha; }

	WorldTransform& GetWorldTransform() { return transform_; }

	void SetIsDirectionRotate(const bool& is) { emitData_.isDirectionRotate = is; }

public:

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
	
	void SetIsScaling(bool is) { isFlag.isScaling_ = is; } // サイズを変化させるか

public: // 

	void SetColorMinMax(Vector4 min, Vector4 max) { emitData_.colorRange.min = min; emitData_.colorRange.max = max; } // カラー

	void SetSize(Vector3 size, Vector3 range);	// サイズ

	void SetRotate(Vector3 rotate, Vector3 range); // 回転

	void SetLifeTime(float life, float range); // 生存時間

	void SetVelocity(Vector3 velocity, Vector3 range);  // 速度

	void SetRotateVelocity(Vector3 rotateVelocity, Vector3 range); // 回転速度

	void SetAcceleration(Vector3 acceleration, Vector3 range); // 加速度

	void SetSizeAmount(Vector3 size, Vector3 range);		// 増加量()

	void SetUseFieldName(std::vector<std::string> fieldName) { // フィールド名
		for (auto& name : fieldName) {
			fieldName.push_back(name);
		}
	}


	void SetEmitType(EmitData::EmitType emitType) { emitType_ = emitType; };
	void SetEmitDirectionType(EmitData::DirectionType directionType) { directionType_ = directionType; };



public: // UVトランスフォーム
	void SetUvRotateVeloctiy(Vector3 rotate) { uvTransformVeloctiy_.rotate = rotate; };

	void SetUvScaleVeloctiy(Vector3 scale) { uvTransformVeloctiy_.scale = scale; };

	void SetUvTranslateVeloctiy(Vector3 translate) { uvTransformVeloctiy_.translate = translate; };

	void SetUvTransformVeloctiy(Transform uvTransform) { uvTransformVeloctiy_ = uvTransform; }
protected:

	// エミッタ可視化
	virtual void DrawEmitterLine() = 0;

	// 最小最大値算出
	void EmitMinMax();
protected:
	EmitData::EmitType emitType_ = EmitData::EmitType::kRandom;					// 出現方法
	EmitData::DirectionType directionType_ = EmitData::DirectionType::kRandom;	// 方向
	EmitData::TopBottom topBottom_ = EmitData::TopBottom::kBottom;				// 拡縮
protected:
	// パーティクル制御に必要なフラグ
	EmitData::IsFlag isFlag;

	// 出すか
	bool isEmit = true; // 出すか
	bool isLine = true;

	float alphaClipping_ = 0.5f; // αクリッピング
	int enableLighting_ = false; // ライティング
protected:
	Transform uvTransformVeloctiy_{};	// UVトランスフォーム
public:
	WorldTransform transform_;			// ワールドトランスフォーム
protected:
	float frequency_;		// < 発生頻度
	float frequencyTime_;	// < 頻度用時刻



	std::string emitName_; // エミッター名
	std::string particleName_; // パーティクル名
	std::vector<std::string> fieldName; // 名前(フィールド)


	EffectEmitData emitData_{}; // エミットデータ
	GlobalVariables* globalVariables_ = nullptr;
	ParticleManager* particleManager_ = nullptr;
	LineCommon* lineCommon_ = nullptr;
};