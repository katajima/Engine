#pragma once
// C;;
#include<random>
#include<numbers>

// engine
#include"DirectXGame/engine/math/MathFunctions.h"
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/Effect/Particle/CPU/ParticleManager.h"
#include"DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"


namespace Engine {
	// 前方宣言
	class LineCommon;
	class GlobalVariables;
	/// <summary>
	/// パーティクルエミッター基底クラス
	/// </summary>
	class BaseParticleEmitter
	{
	public:
		virtual ~BaseParticleEmitter() {}
		/// <summary>
		/// 初期化
		/// </summary>
		virtual void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) = 0;
		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// 発生
		/// </summary>
		void Emit();


	protected:
		/// <summary>
		/// 共通初期化
		/// </summary>
		void CommonParticleInit(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName);
		/// <summary>
		/// 固有の出現処理
		/// </summary>
		virtual void EmitUnique() = 0;
		/// <summary>
		/// 固有の更新
		/// </summary>
		virtual void UpdateUnique() {}; //
		/// <summary>
		/// ImGuiのデバック処理
		/// </summary>
		virtual void DebugImGui() {};
		/// <summary>
		/// グローバルバリアブル適応(固有)
		/// </summary>
		virtual void ApplyGlobalVariablesUnique() {}; // グローバル変数適用
		/// <summary>
		/// グローバルバリアブル適応(共通)
		/// </summary>
		void ApplyGlobalVariables(); // グローバル変数適用

	public: // Getter Setter

		/// <summary>
		/// エミットデータ取得
		/// </summary>
		EffectEmitData& GetEmitData() { return emitData_; };
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
		void SetParent(WorldTransform& parent) { transform_.parent_ = &parent; };
		/// <summary>
		/// 位置設定
		/// </summary>
		void SetPos(Vector3 pos) { transform_.translate_ = pos; }
		/// <summary>
		/// 出現数設定
		/// </summary>
		void SetCount(int count, int range);
		/// <summary>
		/// 頻度設定
		/// </summary>
		void SetFrequency(float frequency) { frequency_ = frequency; }
		/// <summary>
		/// αクリッピング設定
		/// </summary>
		void SetAlphaClipping(float alpha) { alphaClipping_ = alpha; }
		/// <summary>
		/// ワールドトランスフォーム取得
		/// </summary>
		WorldTransform& GetWorldTransform() { return transform_; }
		/// <summary>
		/// 方向によって回転させるか設定
		/// </summary>
		void SetIsDirectionRotate(const bool& is) { emitData_.isDirectionRotate = is; }

	public:
		/// <summary>
		/// パーティクル名前設定
		/// </summary>
		void SetParticleName(std::string name) { particleName_ = name; }
		/// <summary>
		/// ライティングするか設定
		/// </summary>
		void SetEnableLighting(int lighting) { enableLighting_ = lighting; }
		/// <summary>
		/// 出現させるか設定
		/// </summary>
		void SetIsEmit(bool is) { isEmit = is; }

		/// <summary>
		/// フラグ設定
		/// </summary>
		void SetIsFlag(const EmitData::IsFlag& is) { isFlag = is; isLine = is.isLine; }
		/// <summary>
		/// 線形補間設定
		/// </summary>
		void SetIsLineInterpolation(bool is) { isFlag.isLineInterpolation = is; }
		/// <summary>
		/// 重力設定
		/// </summary>
		void SetIsGravity(bool is) { isFlag.isGravity = is; }
		/// <summary>
		/// ビルボード設定
		/// </summary>
		void SetUsebillboard(bool is) { isFlag.usebillboard = is; }
		/// <summary>
		/// ビルボードZ設定
		/// </summary>
		void SetUsebillboardRotZ(bool is) { isFlag.billboardRotZ = is; }
		/// <summary>
		/// 透明度あるか設定
		/// </summary>
		void SetIsAlpha(bool is) { isFlag.isAlpha = is; }
		/// <summary>
		/// 加速させるか設定
		/// </summary>
		void SetIsAcceleration(bool is) { isFlag.isAcceleration = is; } //
		/// <summary>
		/// スケール変更設定
		/// </summary>
		void SetIsLifeTimeScale(bool is) { isFlag.isLifeTimeScale_ = is; }
		/// <summary>
		/// スケール拡縮法設定
		/// </summary>
		void SetLifeTimeScaleTopBottom(EmitData::TopBottom topBottom) { topBottom_ = topBottom; }
		/// <summary>
		/// 回転速度させるか設定
		/// </summary>
		void SetIsRotateVelocity(bool is) { isFlag.isRotateVelocity = is; }
		/// <summary>
		/// 跳ねるか設定
		/// </summary>
		void SetIsBounce(bool is) { isFlag.isBounce = is; }
		/// <summary>
		/// サイズを変化させるか設定
		/// </summary>
		void SetIsScaling(bool is) { isFlag.isScaling_ = is; }

	public: //
		/// <summary>
		/// カラー設定
		/// </summary>
		void SetColorMinMax(Vector4 min, Vector4 max) { emitData_.colorRange.min = min; emitData_.colorRange.max = max; }
		/// <summary>
		/// サイズ設定
		/// </summary>
		void SetSize(Vector3 size, Vector3 range);
		/// <summary>
		/// 回転設定
		/// </summary>
		void SetRotate(Vector3 rotate, Vector3 range);
		/// <summary>
		/// 生存時間設定
		/// </summary>
		void SetLifeTime(float life, float range);
		/// <summary>
		/// 速度設定
		/// </summary>
		void SetVelocity(Vector3 velocity, Vector3 range);
		/// <summary>
		/// 回転速度設定
		/// </summary>
		void SetRotateVelocity(Vector3 rotateVelocity, Vector3 range);
		/// <summary>
		/// 加速度設定
		/// </summary>
		void SetAcceleration(Vector3 acceleration, Vector3 range);
		/// <summary>
		/// 増加量設定
		/// </summary>
		void SetSizeAmount(Vector3 size, Vector3 range);
		/// <summary>
		/// フィールド名設定
		/// </summary>
		void SetUseFieldName(std::vector<std::string> fieldName) {
			for (auto& name : fieldName) {
				fieldName.push_back(name);
			}
		}

		/// <summary>
		/// 出現方法設定
		/// </summary>
		void SetEmitType(EmitData::EmitType emitType) { emitType_ = emitType; };
		/// <summary>
		/// 出現方向設定
		/// </summary>
		void SetEmitDirectionType(EmitData::DirectionType directionType) { directionType_ = directionType; };



	public:
		/// <summary>
		/// UVトランスフォーム回転速度設定
		/// </summary>
		void SetUvRotateVeloctiy(Vector3 rotate) { uvTransformVeloctiy_.rotate = rotate; };
		/// <summary>
		/// UVトランスフォーム拡縮速度設定
		/// </summary>
		void SetUvScaleVeloctiy(Vector3 scale) { uvTransformVeloctiy_.scale = scale; };
		/// <summary>
		/// UVトランスフォーム移動速度設定
		/// </summary>
		void SetUvTranslateVeloctiy(Vector3 translate) { uvTransformVeloctiy_.translate = translate; };
		/// <summary>
		/// UVトランスフォームトランスフォーム設定
		/// </summary>
		void SetUvTransformVeloctiy(Transform uvTransform) { uvTransformVeloctiy_ = uvTransform; }
	protected:

		/// <summary>
		/// エミッタ可視化
		/// </summary>
		virtual void DrawEmitterLine() = 0;

		/// <summary>
		/// 最小最大値算出
		/// </summary>
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

	protected:
		GlobalVariables* globalVariables = nullptr;
		ParticleManager* particleManager = nullptr;
		LineCommon* lineCommon = nullptr;
	};
}