#pragma once
// C;;
#include<random>
#include<numbers>

// engine
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/Effect/Particle/2d/ParticleManager2d.h"
#include"DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"

class ParticleEmitter2d
{
public:
	void Initialize(ParticleManager2d* particleManager, std::string emitName, std::string particleName);

	void Update();

	void Emit();


	void SetRange(Vector2 min, Vector2 max);// 範囲設定

	EffectEmitData2d& GetEmitData() { return emitData_; };	// エミットデータ

	float& GetFrequency() { return frequency_; };			//発生頻度

	int& GetCount() { return emitData_.count.median; };		// 出現数

	void SetParent(WorldTransform2d& parent) { transform_.parent_ = &parent; };// 親子付け

	void SetPos(Vector2 pos) { transform_.translate_ = pos; } // 位置

	void SetCount(int count, int range);					// 出現数

	void SetFrequency(float frequency) { frequency_ = frequency_; } // 頻度

	void SetAlphaClipping(float alpha) { alphaClipping_ = alpha; }

public:

	void SetEnableLighting(int lighting) { enableLighting_ = lighting; }

	void SetIsEmit(bool is) { isEmit = is; } // 出すか
public: // 

	void SetColorMinMax(Vector4 min, Vector4 max) { emitData_.colorRange.min = min; emitData_.colorRange.max = max; } // カラー

	void SetSize(Vector2 size, Vector2 range);	// サイズ

	void SetRotate(float rotate, float range); // 回転

	void SetLifeTime(float life, float range); // 生存時間

	void SetVelocity(Vector2 velocity, Vector2 range);  // 速度

	void SetRotateVelocity(float rotateVelocity, float range); // 回転速度

	void SetAcceleration(Vector2 acceleration, Vector2 range); // 加速度
private:
	Range<Vector2> range_;	// 出現範囲
private:

	void EmitUniqe();

private:
	// 出すか
	bool isEmit = true; // 出すか
	bool isLine = true;

	float alphaClipping_ = 0.5f; // αクリッピング
	int enableLighting_ = false; // ライティング
protected:

public:
	WorldTransform2d transform_;			// ワールドトランスフォーム
protected:
	float frequency_;		// < 発生頻度
	float frequencyTime_;	// < 頻度用時刻



	std::string emitName_; // エミッター名
	std::string particleName_; // パーティクル名


	EffectEmitData2d emitData_{}; // エミットデータ

	ParticleManager2d* particleManager_;


};

