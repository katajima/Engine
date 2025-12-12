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
		// 初期化
		void Init(Material* mateirial);
		// 更新
		void Update(float deltaTime);



	public:
		// UVアニメーションデータ取得
		UVAnimetionData& GetUVAnimetionData() { return uvAnimationData_; }
		// マテリアルセット
		void Setmaterial(Material* mateiria) { mateirial_ = mateiria; };
	private:
		// 
		UVAnimetionData uvAnimationData_;
	private:
		Material* mateirial_;
	};
}
