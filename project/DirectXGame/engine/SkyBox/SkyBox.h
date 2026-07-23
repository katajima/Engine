#pragma once
#include"DirectXGame/engine/math/MathFunctions.h"
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
	class EntityManager;

	/// <summary>
	/// スカイボックスクラス
	/// </summary>
	class SkyBox
	{
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(EntityManager* entity3DManager, std::string txtueName);
		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// 描画
		/// </summary>
		void Draw();
		/// <summary>
		/// マテリアル取得
		/// </summary>
		Material* GetMaterial() { return material.get(); }
		/// <summary>
		/// メッシュ取得
		/// </summary>
		SkyBoxMesh* GetMesh() { return mesh_.get(); }
	private:

		std::unique_ptr<SkyBoxMesh> mesh_;
		std::unique_ptr<Material> material;

	private:
		EntityManager* entityManager = nullptr;

	};
}
