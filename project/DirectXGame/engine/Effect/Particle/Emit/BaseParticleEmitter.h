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
		// 初期化
		virtual void Initialize(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName) = 0;
		// 更新
		void Update();
		// 発生
		void Emit();


	protected:
		// 共通初期化
		void CommonParticleInit(ParticleManager* particleManager, GlobalVariables* globalVariables, std::string emitName, std::string particleName);
		// 固有の出現処理
		virtual void EmitUniqe() = 0;
		// 固有の更新
		virtual void UpdateUniqe() {}; //
		// ImGuiのデバック処理
		virtual void DebugImGui() {};
		// グローバルバリアブル適応(固有)
		virtual void ApplyGlobalVariablesUniqe() {}; // グローバル変数適用
		// グローバルバリアブル適応(共通)
		void ApplyGlobalVariables(); // グローバル変数適用

	public: // Getter Setter

		// エミットデータ取得
		EffectEmitData& GetEmitData() { return emitData_; };
		//発生頻度取得
		float& GetFrequency() { return frequency_; };
		// 出現数取得
		int& GetCount() { return emitData_.count.median; };
		// 親子付け設定
		void SetParent(WorldTransform& parent) { transform_.parent_ = &parent; };
		// 位置設定
		void SetPos(Vector3 pos) { transform_.translate_ = pos; }
		// 出現数設定
		void SetCount(int count, int range);
		// 頻度設定
		void SetFrequency(float frequency) { frequency_ = frequency; }
		// αクリッピング設定
		void SetAlphaClipping(float alpha) { alphaClipping_ = alpha; }
		// ワールドトランスフォーム取得
		WorldTransform& GetWorldTransform() { return transform_; }
		// 方向によって回転させるか設定
		void SetIsDirectionRotate(const bool& is) { emitData_.isDirectionRotate = is; }

	public:
		// ライティングするか設定
		void SetEnableLighting(int lighting) { enableLighting_ = lighting; }
		// 出現させるか設定
		void SetIsEmit(bool is) { isEmit = is; }
		// 線形補間設定
		void SetIsLineInterpolation(bool is) { isFlag.isLineInterpolation = is; }
		// 重力設定
		void SetIsGravity(bool is) { isFlag.isGravity = is; }
		// ビルボード設定
		void SetUsebillboard(bool is) { isFlag.usebillboard = is; }
		// ビルボードZ設定
		void SetUsebillboardRotZ(bool is) { isFlag.billboardRotZ = is; }
		// 透明度あるか設定
		void SetIsAlpha(bool is) { isFlag.isAlpha = is; }
		// 加速させるか設定
		void SetIsAcceleration(bool is) { isFlag.isAcceleration = is; } // 
		// スケール変更設定
		void SetIsLifeTimeScale(bool is) { isFlag.isLifeTimeScale_ = is; }
		// スケール拡縮法設定
		void SetLifeTimeScaleTopBottom(EmitData::TopBottom topBottom) { topBottom_ = topBottom; }
		// 回転速度させるか設定
		void SetIsRotateVelocity(bool is) { isFlag.isRotateVelocity = is; }
		// 跳ねるか設定
		void SetIsBounce(bool is) { isFlag.isBounce = is; }
		// サイズを変化させるか設定
		void SetIsScaling(bool is) { isFlag.isScaling_ = is; }

	public: // 
		// カラー設定
		void SetColorMinMax(Vector4 min, Vector4 max) { emitData_.colorRange.min = min; emitData_.colorRange.max = max; }
		// サイズ設定
		void SetSize(Vector3 size, Vector3 range);
		// 回転設定
		void SetRotate(Vector3 rotate, Vector3 range);
		// 生存時間設定
		void SetLifeTime(float life, float range);
		// 速度設定
		void SetVelocity(Vector3 velocity, Vector3 range);
		// 回転速度設定
		void SetRotateVelocity(Vector3 rotateVelocity, Vector3 range);
		// 加速度設定
		void SetAcceleration(Vector3 acceleration, Vector3 range);
		// 増加量設定
		void SetSizeAmount(Vector3 size, Vector3 range);
		// フィールド名設定
		void SetUseFieldName(std::vector<std::string> fieldName) {
			for (auto& name : fieldName) {
				fieldName.push_back(name);
			}
		}

		// 出現方法設定
		void SetEmitType(EmitData::EmitType emitType) { emitType_ = emitType; };
		// 出現方向設定
		void SetEmitDirectionType(EmitData::DirectionType directionType) { directionType_ = directionType; };



	public:
		// UVトランスフォーム回転速度設定
		void SetUvRotateVeloctiy(Vector3 rotate) { uvTransformVeloctiy_.rotate = rotate; };
		// UVトランスフォーム拡縮速度設定
		void SetUvScaleVeloctiy(Vector3 scale) { uvTransformVeloctiy_.scale = scale; };
		// UVトランスフォーム移動速度設定
		void SetUvTranslateVeloctiy(Vector3 translate) { uvTransformVeloctiy_.translate = translate; };
		// UVトランスフォームトランスフォーム設定
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

	protected:
		GlobalVariables* globalVariables = nullptr;
		ParticleManager* particleManager = nullptr;
		LineCommon* lineCommon = nullptr;
	};
}