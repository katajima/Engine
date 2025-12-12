#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>
#include<list>
#include<string>
#include<vector>
#include<format>
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include"DirectXGame/engine/Camera/Camera.h"

#include"DirectXGame/engine/Mesh/ModelMesh.h"
#include"DirectXGame/engine/Material/Material.h"
#include "DirectXGame/engine/Transform/Transfomation/Transfomation.h"

#include"TrailEffectManager.h"


namespace Engine {
	// 前方宣言
	class EffectManager;

	/// <summary>
	/// トレイルエフェクトクラス
	/// </summary>
	class TrailEffect
	{
	public:

		// 初期化
		void Initialize(EffectManager* effectManager, const std::string& tex, float maxtime, const Color color = { 1,1,1,1 });
		// 更新
		void Update();
		// 描画
		void Draw();
		// カメラ設定
		void SetCamera(Camera* camera) { camera_ = camera; };
		// トレイルのオフセット設定
		void SetOffset(Vector3 offsetStr, Vector3 offsetEnd, WorldTransform& world) {
			worldtransformTstr_.Initialize();
			worldtransformTstr_.parent_ = &world;
			worldtransformTstr_.translate_ = offsetStr;

			worldtransformTend_.Initialize();
			worldtransformTend_.parent_ = &world;
			worldtransformTend_.translate_ = offsetEnd;
		}

		// 発生フラグをセット
		void SetIsEmit(bool is) { flag_ = is; }
		// 発生フラグを取得
		bool GetIsEmit() const { return flag_; }
		// 時間をセット
		void SetTimer(float t) { timer = t; }
		// 発生時間を取得
		float GetTimer() const { return timer; }


		// 行列設定
		void SetMatrix(Matrix4x4& mat) { mat_ = mat; }
		// メッシュ取得
		ModelMesh* GetMesh() const { return mesh.get(); }

		std::unique_ptr<ModelMesh> mesh;

	private:
		// 頂点データ
		struct VertexData {
			Vector4 position;
			Vector2 texcoord;
			Vector3 normal;
		};
		// カメラ
		Camera* camera_ = nullptr;

		std::unique_ptr<Material> material;
		std::unique_ptr<Transfomation> transfomation = nullptr;

		Matrix4x4 mat_;
		Matrix4x4 parentTransform_;

		bool flag_ = false;
		Vector3 velocity_; // 速度

		float timer = 0;

	private:
		EffectManager* effectManager_;

		WorldTransform worldtransformTstr_;
		WorldTransform worldtransformTend_;
	};
}
