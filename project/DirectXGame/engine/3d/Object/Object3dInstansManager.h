#pragma once
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include<wrl.h>
using namespace Microsoft::WRL;
#include"DirectXGame/engine/PSO/PSOManager.h"
#include"DirectXGame/engine/Manager/SRV/SrvManager.h"
#include "DirectXGame/engine/Material/Material.h"

#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/struct/Material.h"

#include "ObjectInstans.h"

#include<fstream>
#include<sstream>
#include<vector>
#include<string>
#include<memory>
#include<deque> 
#include<memory>
#include <thread>
#include <mutex>

// 方向
enum class MapAxis {
	XY,
	ZX,
	YZ
};

// マップID
struct MapId {
	int id; // テクスチャIndexや種類を表すID
	std::string tex; // テクスチャname
};


namespace Engine {
	class ShadowMap;

	// オブジェクトインスタンシングクラス(大量描画用)
	class Object3dInstansManager
	{
	public:
		// GPU転送用
		struct ObjectGPU
		{
			Matrix4x4 WVP;
			Matrix4x4 World;
			Matrix4x4 worldInverseTranspose;
			Vector4 color;
			UINT textureIndex;
			Vector3 pad;
		};
		// ラスタライザタイプ
		enum class RasterizerType
		{
			MODE_SOLID_BACK,
			MODE_SOLID_NONE,
		};
		// メッシュタイプ
		enum class MeshType
		{
			kPrimitiv,
			kModel,
		};


		// オブジェクトグループ
		struct ObjectGroup
		{
			std::string name; // 名前
			std::deque<ObjectInstans> object; // ✅ ここをdequeに
			std::unordered_map<int, size_t> idMap;
			Model* model;
			bool flag;
			uint32_t srvIndex;
			Microsoft::WRL::ComPtr<ID3D12Resource> resource;
			uint32_t instanceCount; // インスタンス数
			ObjectGPU* instanceData; // インスタンシングデータを書き込むためのポインタ
			D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU;
			D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU;
			ModelMesh* mesh;
			RasterizerType rasteType;
			BlendType blendType;
		};

	public:
		Object3dInstansManager();
		~Object3dInstansManager();
		Object3dInstansManager(Object3dInstansManager&) = delete;
		Object3dInstansManager& operator=(Object3dInstansManager&) = delete;

		// 初期化
		void Initialize(DirectXCommon* dxCommon);
		// エンティティ3dの設定
		void SetEntity3D(EntityManager* entity3DManager) { this->entity3DManager = entity3DManager; };

		// 更新
		void Update();
		// 描画
		void Draw();

		// 描画
		void DrawTransparency();
		// シャドウマップ用の深度描画
		void DrawShadowMap(ShadowMap* shadowMap);


		// 描画準備
		void DrawCommonSetting(RasterizerType rasteType, BlendType blendType);

		// オブジェクトグループ作り(モデル)
		void CreateObject3dGroup(const std::string& name, const std::string& textureFilePath, Model* model,
			RasterizerType rasteType = RasterizerType::MODE_SOLID_BACK, BlendType blendType = BlendType::MODE_ADD, ObjectInstans::TransparencyType transparencyType = ObjectInstans::TransparencyType::kNo);
		// オブジェクトグループ作り(モデル)
		void CreateObject3dGroup(const std::string& name, const std::string& textureFilePath, ModelMesh* mesh,
			RasterizerType rasteType = RasterizerType::MODE_SOLID_BACK, BlendType blendType = BlendType::MODE_ADD, ObjectInstans::TransparencyType transparencyType = ObjectInstans::TransparencyType::kNo);

		// カメラセット
		void SetCamera(Camera* camera) { this->camera_ = camera; }
		// オブジェクトの追加
		void AddObject(const std::string& name, const std::string& texName, ObjectInstans&& object, int& id, MeshType type = MeshType::kModel, ObjectInstans::TransparencyType transparencyType = ObjectInstans::TransparencyType::kNo);
		// オブジェクトのグループ数取得
		int GetSize() { return static_cast<int>(objectGroups.size()); };
		// オブジェクトグループ名前でオブジェクトクリーン
		void Clear(const std::string& name);
		// 全てのオブジェクトグループのオブジェクトのクリーン
		void ClearObject() {
			for (auto& obj : objectGroups) {
				obj.second.object.clear();
			}

			for (auto& obj : objectTranslucentGroups) {
				obj.second.object.clear();
			}

		}
		// 全てクリーン
		void AllClear() {
			objectTranslucentGroups.clear();
			objectGroups.clear();
		}


	public: //取得
		// オブジェクトインスタンスをIDで取得
		ObjectInstans* GetObjectById(const std::string& groupName, int id, ObjectInstans::TransparencyType transparencyType);
		// 全てのオブジェクトインスタンス取得
		std::deque<ObjectInstans>& GetObjects(const std::string& groupName, ObjectInstans::TransparencyType transparencyType);
		// オブジェクトグループ取得
		ObjectGroup& GetObjectGroup(const std::string& groupName, ObjectInstans::TransparencyType transparencyType);

	private:

		ObjectGroup& GroupContains(const std::string& groupName, ObjectInstans::TransparencyType transparencyType, bool& isReturn);

	private:
		// ルートシグネチャの作成
		void CreateRootSignature();
		// グラフィックスパイプラインの作成
		void CreateGraphicsPipeline();
		// シャドウマップ用パイプラインの作成
		void CreateShadowMapPipeline();
		// ブレンドモード設定(加算)
		void BlendAdd();
		// ブレンドモード設定(減算)
		void BlendSubtract();
		// ブレンドモード設定(乗算)
		void BlendMuliply();




		// オブジェクトグループ
		std::unordered_map<std::string, ObjectGroup> objectGroups;

		// 半透明オブジェクトグループ
		std::unordered_map<std::string, ObjectGroup> objectTranslucentGroups;


		const uint32_t kNumMaxInstance = 10000;

		uint32_t numInstance{};


		Camera* camera_ = nullptr;


		Transform transform{};

		int id_;

	private:
		DirectXCommon* dxCommon = nullptr;
		SrvManager* srvManager = nullptr;
		ModelManager* modelManager = nullptr;
		EntityManager* entity3DManager = nullptr;

		std::unique_ptr<PSOManager> psoManager_ = nullptr;
	private:
		const float kGravitationalAcceleration = 9.8f;
		////ルートシグネチャデスク
		D3D12_ROOT_SIGNATURE_DESC descriptionSignature{};
		////ルートシグネチャ
		Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature;
		Microsoft::WRL::ComPtr < ID3D12RootSignature> shadowRootSignature;
		//// グラフィックスパイプラインステート
		Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState[6];
		Microsoft::WRL::ComPtr < ID3D12PipelineState> shadowGraphicsPipelineState;



		D3D12_BLEND_DESC blendDesc{};
		D3D12_RASTERIZER_DESC rasterizerDesc{};
	};
}








