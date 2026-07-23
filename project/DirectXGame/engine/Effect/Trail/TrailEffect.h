#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>
#include<list>
#include<string>
#include<vector>
#include<deque>
#include<format>
#include<cstdint>
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFunctions.h"
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include"DirectXGame/engine/Camera/Camera.h"
#include "DirectXGame/engine/Entity/Entity.h"

#include"DirectXGame/engine/Mesh/ModelMesh.h"
#include"DirectXGame/engine/Material/Material.h"
#include "DirectXGame/engine/Transform/Transformation/Transformation.h"

#include"TrailEffectManager.h"


namespace Engine {
	// 前方宣言
	class EffectManager;

	enum class TrailFeature : uint32_t {
		None = 0,
		Ribbon = 1 << 0,
		AfterImage = 1 << 1,
		Mesh = 1 << 2,
		Particle = 1 << 3,
		UVScroll = 1 << 4,
		Distortion = 1 << 5,
		Dissolve = 1 << 6,
	};

	inline TrailFeature operator|(TrailFeature lhs, TrailFeature rhs)
	{
		return static_cast<TrailFeature>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
	}

	inline TrailFeature operator&(TrailFeature lhs, TrailFeature rhs)
	{
		return static_cast<TrailFeature>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
	}

	inline TrailFeature& operator|=(TrailFeature& lhs, TrailFeature rhs)
	{
		lhs = lhs | rhs;
		return lhs;
	}

	struct TrailSettings {
		TrailFeature features = TrailFeature::Ribbon;
		float minEmitDistance = 0.03f;
		size_t maxSegmentCount = 128;
		bool useSpline = true;
		int splineSubdivision = 4;

		Vector2 uvScrollSpeed = { 0.0f, 0.0f };
		float dissolveSpeed = 1.0f;
		float dissolveAlphaClipMin = 0.0f;
		float dissolveAlphaClipMax = 0.85f;

		int afterImageCount = 4;
		float meshTrailInterval = 0.03f;
		float particleEmitInterval = 0.02f;
		float distortionStrength = 0.0f;
	};

	/// <summary>
	/// トレイルエフェクトクラス
	/// </summary>
	class TrailEffect : public Entity
	{
	public:

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(EffectManager* effectManager, const std::string& tex, float maxtime, const Color color = { 1,1,1,1 });
		/// <summary>
		/// 更新
		/// </summary>
		void Update() override;
		/// <summary>
		/// 描画
		/// </summary>
		void Draw() override;
		/// <summary>
		/// デバッグ表示。保存はせず、実行中の調整と状態確認だけを行う。
		/// </summary>
		void UpdateImgui() override;
		/// <summary>
		/// カメラ設定
		/// </summary>
		void SetCamera(Camera* camera) { camera_ = camera; };
		/// <summary>
		/// トレイルのオフセット設定
		/// </summary>
		void SetOffset(Vector3 offsetStr, Vector3 offsetEnd, WorldTransform& world) {
			worldtransformTstr_.Initialize();
			worldtransformTstr_.parent_ = &world;
			worldtransformTstr_.translate_ = offsetStr;

			worldtransformTend_.Initialize();
			worldtransformTend_.parent_ = &world;
			worldtransformTend_.translate_ = offsetEnd;
		}

		/// <summary>
		/// 発生フラグをセット
		/// </summary>
		void SetIsEmit(bool is) { flag_ = is; }
		/// <summary>
		/// 発生フラグを取得
		/// </summary>
		bool GetIsEmit() const { return flag_; }
		/// <summary>
		/// 時間をセット
		/// </summary>
		void SetTimer(float t) { timer = t; }
		/// <summary>
		/// 発生時間を取得
		/// </summary>
		float GetTimer() const { return timer; }
		/// <summary>
		/// トレイルの品質設定。細かい動きを捨てる距離と、保持する最大セグメント数を指定する。
		/// </summary>
		void SetQuality(float minEmitDistance, size_t maxSegmentCount);
		/// <summary>
		/// 複数のトレイル機能をまとめて設定する
		/// </summary>
		void SetSettings(const TrailSettings& settings);
		const TrailSettings& GetSettings() const { return settings_; }
		/// <summary>
		/// 既存トレイルへ機能を足し引きする
		/// </summary>
		void AddFeature(TrailFeature feature);
		void RemoveFeature(TrailFeature feature);
		bool HasFeature(TrailFeature feature) const;


		/// <summary>
		/// 行列設定
		/// </summary>
		void SetMatrix(Matrix4x4& mat) { mat_ = mat; }
		/// <summary>
		/// メッシュ取得
		/// </summary>
		ModelMesh* GetMesh() const { return mesh.get(); }
		size_t GetSegmentCount() const { return segments_.size(); }

		std::unique_ptr<ModelMesh> mesh;

	private:
		struct TrailSegment {
			Vector3 start;
			Vector3 end;
			Vector3 prevStart;
			Vector3 prevEnd;
			float age = 0.0f;
		};

		void EmitSegment(const Vector3& start, const Vector3& end);
		void RebuildMesh();
		void RebuildLinearMesh();
		void RebuildSplineMesh();
		void DrawDebugLines();
		void UpdateUvScroll(float deltaTime);
		void UpdateDissolve(float deltaTime);
		void UpdateComposableModules(float deltaTime);

		// 頂点データ
		struct VertexData {
			Vector4 position;
			Vector2 texcoord;
			Vector3 normal;
		};
		// カメラ
		Camera* camera_ = nullptr;

		std::unique_ptr<Material> material;
		std::unique_ptr<Transformation> transformation = nullptr;

		Matrix4x4 mat_;
		Matrix4x4 parentTransform_;

		bool flag_ = false;
		Vector3 velocity_; // 速度

		float timer = 0;
		float lifeTime_ = 0.2f;
		float minEmitDistance_ = 0.03f;
		size_t maxSegmentCount_ = 128;
		TrailSettings settings_{};
		float dissolveTime_ = 0.0f;
		Vector2 uvScrollOffset_ = {};
		bool hasLastSample_ = false;
		bool meshDirty_ = true;
		Vector3 lastStart_{};
		Vector3 lastEnd_{};
		Color baseColor_ = { 1,1,1,1 };
		std::deque<TrailSegment> segments_;
		bool debugDraw_ = false;
		bool debugDrawRails_ = true;
		bool debugDrawMesh_ = true;
		bool debugDrawPoints_ = false;

	private:
		EffectManager* effectManager = nullptr;

		WorldTransform worldtransformTstr_;
		WorldTransform worldtransformTend_;
	};
}
