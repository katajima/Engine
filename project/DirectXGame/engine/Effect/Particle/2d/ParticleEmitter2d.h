#pragma once
// C;;
#include<random>
#include<numbers>

// engine
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/Effect/Particle/2d/ParticleManager2d.h"
#include"DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"


namespace Engine {
	/// <summary>
	/// パーティクル2dエミッター
	/// </summary>
	class ParticleEmitter2d
	{
	public:
		// 初期化
		void Initialize(ParticleManager2d* particleManager, std::string emitName, std::string particleName);
		// 更新
		void Update();
		// 出現
		void Emit();

		// 範囲設定
		void SetRange(Vector2 min, Vector2 max);// 範囲設定
		// エミッターデータ取得
		EffectEmitData2d& GetEmitData() { return emitData_; };
		// 発生頻度取得
		float& GetFrequency() { return frequency_; };
		// 出現数取得
		int& GetCount() { return emitData_.count.median; };
		// 親子付け設定
		void SetParent(WorldTransform2d& parent) { transform_.parent_ = &parent; };
		// 位置設定
		void SetPos(Vector2 pos) { transform_.translate_ = pos; }
		// 出現数設定
		void SetCount(int count, int range);
		// 発生頻度設定
		void SetFrequency(float frequency) { frequency_ = frequency_; }
		// αクリッピング設定
		void SetAlphaClipping(float alpha) { alphaClipping_ = alpha; }

	public:
		// ライティングするか設定
		void SetEnableLighting(int lighting) { enableLighting_ = lighting; }
		// 出現させるか設定
		void SetIsEmit(bool is) { isEmit = is; } // 出すか
	public: // 
		// カラー設定
		void SetColorMinMax(Vector4 min, Vector4 max) { emitData_.colorRange.min = min; emitData_.colorRange.max = max; } // カラー
		// サイズ設定
		void SetSize(Vector2 size, Vector2 range);
		// 回転設定
		void SetRotate(float rotate, float range);
		// 生存時間設定
		void SetLifeTime(float life, float range);
		// 速度設定
		void SetVelocity(Vector2 velocity, Vector2 range);
		// 回転速度設定
		void SetRotateVelocity(float rotateVelocity, float range);
		// 加速度設定
		void SetAcceleration(Vector2 acceleration, Vector2 range);
	private:
		Range<Vector2> range_;	// 出現範囲
	private:
		// 固有の更新
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
}
