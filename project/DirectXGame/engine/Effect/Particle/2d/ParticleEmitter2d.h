#pragma once
// C;;
#include<random>
#include<numbers>

// engine
#include"DirectXGame/engine/math/MathFunctions.h"
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
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(ParticleManager2d* particleManager, std::string emitName, std::string particleName);
		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// 出現
		/// </summary>
		void Emit();

		/// <summary>
		/// 範囲設定
		/// </summary>
		void SetRange(Vector2 min, Vector2 max);// 範囲設定
		/// <summary>
		/// エミッターデータ取得
		/// </summary>
		EffectEmitData2d& GetEmitData() { return emitData_; };
		/// <summary>
		/// 発生頻度取得
		/// </summary>
		float& GetFrequency() { return frequency_; };
		/// <summary>
		/// 出現数取得
		/// </summary>
		int& GetCount() { return emitData_.count.median; };
		/// <summary>
		/// 親子付け設定
		/// </summary>
		void SetParent(WorldTransform2d& parent) { transform_.parent_ = &parent; };
		/// <summary>
		/// 位置設定
		/// </summary>
		void SetPos(Vector2 pos) { transform_.translate_ = pos; }
		/// <summary>
		/// 出現数設定
		/// </summary>
		void SetCount(int count, int range);
		/// <summary>
		/// 発生頻度設定
		/// </summary>
		void SetFrequency(float frequency) { frequency_ = frequency_; }
		/// <summary>
		/// αクリッピング設定
		/// </summary>
		void SetAlphaClipping(float alpha) { alphaClipping_ = alpha; }

	public:
		/// <summary>
		/// ライティングするか設定
		/// </summary>
		void SetEnableLighting(int lighting) { enableLighting_ = lighting; }
		/// <summary>
		/// 出現させるか設定
		/// </summary>
		void SetIsEmit(bool is) { isEmit = is; } // 出すか
	public: //
		/// <summary>
		/// カラー設定
		/// </summary>
		void SetColorMinMax(Vector4 min, Vector4 max) { emitData_.colorRange.min = min; emitData_.colorRange.max = max; } // カラー
		/// <summary>
		/// サイズ設定
		/// </summary>
		void SetSize(Vector2 size, Vector2 range);
		/// <summary>
		/// 回転設定
		/// </summary>
		void SetRotate(float rotate, float range);
		/// <summary>
		/// 生存時間設定
		/// </summary>
		void SetLifeTime(float life, float range);
		/// <summary>
		/// 速度設定
		/// </summary>
		void SetVelocity(Vector2 velocity, Vector2 range);
		/// <summary>
		/// 回転速度設定
		/// </summary>
		void SetRotateVelocity(float rotateVelocity, float range);
		/// <summary>
		/// 加速度設定
		/// </summary>
		void SetAcceleration(Vector2 acceleration, Vector2 range);
	private:
		Range<Vector2> range_;	// 出現範囲
	private:
		/// <summary>
		/// 固有の更新
		/// </summary>
		void EmitUnique();

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

		ParticleManager2d* particleManager;
	};
}
