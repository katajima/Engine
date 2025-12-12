#pragma once
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/struct/Light.h"
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>
#include<string>
#include<vector>
#include<format>
#include<DirectXGame/engine/Material/Material.h>
#include<DirectXGame/engine/Mesh/SkyBoxMesh.h>


// 前方宣言
namespace Engine {
	class Entity3DManager;

	/// <summary>
	/// スカイボックスクラス
	/// </summary>
	class SkyBox
	{
	public:
		// 初期化
		void Initialize(Entity3DManager* entity3DManager, std::string txtueName);
		// 更新
		void Update();
		// 描画
		void Draw();
		// マテリアル取得
		Material* GetMaterial() { return material.get(); }
		// メッシュ取得
		SkyBoxMesh* GetMesh() { return mesh_.get(); }
	private:

		std::unique_ptr<SkyBoxMesh> mesh_;
		std::unique_ptr<Material> material;

	private:
		Entity3DManager* entity3DManager_;

	};
}
