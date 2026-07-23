#pragma once
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include<wrl.h>
using namespace Microsoft::WRL;
#include"DirectXGame/engine/PSO/PSOManager.h"
#include"DirectXGame/engine/Manager/SRV/SrvManager.h"
#include "DirectXGame/engine/Material/Material.h"

#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/struct/Material.h"

#include "ObjectInstance.h"

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
/// <summary>
/// Object3dInstanceManagerを管理・実装するクラス。
/// </summary>
	class Object3dInstanceManager
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
			std::deque<ObjectInstance> object; // ✅ ここをdequeに
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
		Object3dInstanceManager();
		~Object3dInstanceManager();
		Object3dInstanceManager(Object3dInstanceManager&) = delete;
		Object3dInstanceManager& operator=(Object3dInstanceManager&) = delete;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DirectXCommon* dxCommon);
		/// <summary>
		/// 終了処理
		/// </summary>
		void Finalize();
		/// <summary>
		/// エンティティ3dの設定
		/// </summary>
		void SetEntity3D(EntityManager* entity3DManager) { this->entity3DManager = entity3DManager; };

		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// 描画
		/// </summary>
		void Draw();

		/// <summary>
		/// 描画
		/// </summary>
		void DrawTransparency();
		/// <summary>
		/// シャドウマップ用の深度描画
		/// </summary>
		void DrawShadowMap(ShadowMap* shadowMap);


		/// <summary>
		/// 描画準備
		/// </summary>
		void DrawCommonSetting(RasterizerType rasteType, BlendType blendType);

		// オブジェクトグループ作り(モデル)
		void CreateObject3dGroup(const std::string& name, const std::string& textureFilePath, Model* model,
			RasterizerType rasteType = RasterizerType::MODE_SOLID_BACK, BlendType blendType = BlendType::MODE_ADD, ObjectInstance::TransparencyType transparencyType = ObjectInstance::TransparencyType::kNo);
		// オブジェクトグループ作り(モデル)
		void CreateObject3dGroup(const std::string& name, const std::string& textureFilePath, ModelMesh* mesh,
			RasterizerType rasteType = RasterizerType::MODE_SOLID_BACK, BlendType blendType = BlendType::MODE_ADD, ObjectInstance::TransparencyType transparencyType = ObjectInstance::TransparencyType::kNo);

		/// <summary>
		/// カメラセット
		/// </summary>
		void SetCamera(Camera* camera) { this->camera_ = camera; }
		/// <summary>
		/// オブジェクトの追加
		/// </summary>
		void AddObject(const std::string& name, const std::string& texName, ObjectInstance&& object, int& id, MeshType type = MeshType::kModel, ObjectInstance::TransparencyType transparencyType = ObjectInstance::TransparencyType::kNo);
		/// <summary>
		/// オブジェクトのグループ数取得
		/// </summary>
		int GetSize() { return static_cast<int>(objectGroups.size()); };
		/// <summary>
		/// オブジェクトグループ名前でオブジェクトクリーン
		/// </summary>
		void Clear(const std::string& name);
		/// <summary>
		/// 全てのオブジェクトグループのオブジェクトのクリーン
		/// </summary>
		void ClearObject() {
			for (auto& obj : objectGroups) {
				obj.second.object.clear();
			}

			for (auto& obj : objectTranslucentGroups) {
				obj.second.object.clear();
			}

		}
		/// <summary>
		/// 全てクリーン
		/// </summary>
		void AllClear() {
			for (auto& obj : objectGroups) {
				ReleaseGroupResource(obj.second);
			}
			for (auto& obj : objectTranslucentGroups) {
				ReleaseGroupResource(obj.second);
			}
			objectTranslucentGroups.clear();
			objectGroups.clear();
		}


	public: //取得
		/// <summary>
		/// オブジェクトインスタンスをIDで取得
		/// </summary>
		ObjectInstance* GetObjectById(const std::string& groupName, int id, ObjectInstance::TransparencyType transparencyType);
		/// <summary>
		/// 全てのオブジェクトインスタンス取得
		/// </summary>
		std::deque<ObjectInstance>& GetObjects(const std::string& groupName, ObjectInstance::TransparencyType transparencyType);
		/// <summary>
		/// オブジェクトグループ取得
		/// </summary>
		ObjectGroup& GetObjectGroup(const std::string& groupName, ObjectInstance::TransparencyType transparencyType);

	private:

		ObjectGroup& GroupContains(const std::string& groupName, ObjectInstance::TransparencyType transparencyType, bool& isReturn);
		/// <summary>
		/// インスタンシング用GPUリソースを解放する
		/// </summary>
		void ReleaseGroupResource(ObjectGroup& group);

	private:
		/// <summary>
		/// ルートシグネチャの作成
		/// </summary>
		void CreateRootSignature();
		/// <summary>
		/// グラフィックスパイプラインの作成
		/// </summary>
		void CreateGraphicsPipeline();
		/// <summary>
		/// シャドウマップ用パイプラインの作成
		/// </summary>
		void CreateShadowMapPipeline();
		/// <summary>
		/// ブレンドモード設定(加算)
		/// </summary>
		void BlendAdd();
		/// <summary>
		/// ブレンドモード設定(減算)
		/// </summary>
		void BlendSubtract();
		/// <summary>
		/// ブレンドモード設定(乗算)
		/// </summary>
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








