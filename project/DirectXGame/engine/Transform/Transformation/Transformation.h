#pragma once
#include"DirectXGame/engine/math/MathFunctions.h"
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Model/Model.h"
#include "DirectXGame/engine/DirectX/Resource/ConstantBuffer.h"


namespace Engine {
	// 前方宣言
	class BasePrimitive;
	class SkyBox;
	class Ocean;

	/// <summary>
	/// トランスフォームクラス
	/// </summary>
	class Transformation
	{
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DirectXCommon* dxCommon);
		//// 更新
		void Update(void* entity, Camera* camera, Matrix4x4& local, Matrix4x4& mat);

		/// <summary>
		/// 更新
		/// </summary>
		void Update(Camera* camera, Matrix4x4& mat);
		/// <summary>
		/// 更新
		/// </summary>
		void UpdateSprite(Matrix4x4& mat);

		/// <summary>
		/// データ
		/// </summary>
		void GetCommandList(int index);

		// トランスフォーム行列
		struct TransformationMatrix
		{
			Matrix4x4 WVP;
			Matrix4x4 World;
			Matrix4x4 worldInverseTranspose;
		};

	private:
		DirectXCommon* dxCommon = nullptr;

		ConstantBuffer<TransformationMatrix> cbResource_;

	};
}
