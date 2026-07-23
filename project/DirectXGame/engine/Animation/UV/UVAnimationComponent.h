#pragma once
#include"DirectXGame/engine/Material/Material.h"


namespace Engine {
	// UVアニメーションデータ構造体
	struct UVAnimetionData {
		bool isScaleX = false;
		bool isScaleY = false;
		Vector2 maxCount{ 10,10 };
		Vector2 speed{ 0.2f,0.2f };

		bool isRotateX = false;
		bool isRotateY = false;
		Vector2 maxRotate{ 10,10 };
		Vector2 rotateSpeed{ 0.2f,0.2f };

	};

	/// <summary>
	/// UVアニメーションコンポーネント
	/// </summary>
	class UVAnimationComponent
	{
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Init(Material* mateirial);
		/// <summary>
		/// 更新
		/// </summary>
		void Update(float deltaTime);



	public:
		/// <summary>
		/// UVアニメーションデータ取得
		/// </summary>
		UVAnimetionData& GetUVAnimetionData() { return uvAnimationData_; }
		/// <summary>
		/// マテリアルセット
		/// </summary>
		void Setmaterial(Material* mateiria) { this->mateirial = mateiria; };
	private:
		// 
		UVAnimetionData uvAnimationData_;
	private:
		Material* mateirial;
	};
}
