#pragma once
#include <windows.h>
#include<cstdint>
#include<string>
#include<fstream>
#include<sstream>
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
#include<memory>
#include<deque> 
using namespace Microsoft::WRL;
#include<vector>
#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"
#include"DirectXGame/engine/math/MathFunctions.h"
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/struct/Material.h"
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"

#include"DirectXGame/engine/Manager/SRV/SrvManager.h"

#include "DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3dCommon.h"
#include "DirectXGame/engine/3d/Model/Model.h"
#include"DirectXGame/engine/Line/LineCommon.h"
#include "DirectXGame/engine/Material/Material.h"
#include "DirectXGame/engine/Effect/Primitive/Primitive.h"
#include"DirectXGame/engine/PSO/PSOManager.h"

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
	// 前方宣言
	class RigidBodyComponent;
	class ColliderComponent;
	class ContactRecord;

	// オブジェクトのインスタスクラス
	class ObjectInstans
	{
	public:
		// 初期化
		void Initialize(Entity3DManager* entity3DManager, bool useCollider = false, Transform transform = { {1,1,1},{},{} });
		// 更新
		void Update();

		// Object3d内でコライダーコンポーネントを更新するか
		void SetIsUpdateColliderComponent(bool is) { isColliderComponenyUpdate_ = is; };

		// コライダーコンポーネントを取得
		ColliderComponent* GetColliderComponent() { return colliderComponent_.get(); };
		// コライダーコンポーネントの接触情報を取得
		ContactRecord& GetContactRecord();
		// リジットボディー取得
		RigidBodyComponent* GetRigidBodyComponent() { return rigidBodyComponent_.get(); };


		// 削除する
		void IsDelete() { isDelete_ = true; }

		// 削除されているか取得
		bool GetIsDelete() const { return isDelete_; }
	private:
		// コライダーコンポーネント
		std::unique_ptr<ColliderComponent> colliderComponent_;
		// コライダーコンポーネントをObject3d内で更新するかのフラグ
		bool isColliderComponenyUpdate_ = false;
		bool useCollider_ = false;

	private:
		std::unique_ptr<RigidBodyComponent> rigidBodyComponent_ = nullptr;
		bool isDelete_ = false;
	public:
		WorldTransform transform{};
		Vector4 color = { 1,1,1,1 };
		bool is_ = false;
		bool isDraw_ = true;
		uint32_t texIndex;
		int id = -1;   // ← 固有ID（負なら未使用）
	};

	// 前方宣言
	class Entity3DManager;

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
		// オブジェクト構造体
		struct Object
		{
			WorldTransform transform;
			Vector4 color;
			bool is;
			uint32_t texIndex;

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
		Object3dInstansManager() = default;
		~Object3dInstansManager() = default;
		Object3dInstansManager(Object3dInstansManager&) = delete;
		Object3dInstansManager& operator=(Object3dInstansManager&) = delete;

		// 初期化
		void Initialize(DirectXCommon* dxCommon);
		// エンティティ3dの設定
		void SetEntity3D(Entity3DManager* entity3DManager) { entity3DManager_ = entity3DManager; };

		// 更新
		void Update();
		// 描画
		void Draw();
		// 描画準備
		void DrawCommonSetting(RasterizerType rasteType, BlendType blendType);

		// オブジェクトグループ作り(モデル)
		void CreateObject3dGroup(const std::string& name, const std::string& textureFilePath, Model* model, RasterizerType rasteType = RasterizerType::MODE_SOLID_BACK, BlendType blendType = BlendType::MODE_ADD);
		// オブジェクトグループ作り(モデル)
		void CreateObject3dGroup(const std::string& name, const std::string& textureFilePath, ModelMesh* mesh, RasterizerType rasteType = RasterizerType::MODE_SOLID_BACK, BlendType blendType = BlendType::MODE_ADD);

		// カメラセット
		void SetCamera(Camera* camera) { this->camera_ = camera; }
		// オブジェクトの追加
		void AddObject(const std::string& name, const std::string& texName, ObjectInstans&& object, int& id, MeshType type = MeshType::kModel);
		// オブジェクトのグループ数取得
		int GetSize() { return static_cast<int>(objectGroups.size()); };
		// オブジェクトグループ名前でオブジェクトクリーン
		void Clear(const std::string& name);
		// 全てのオブジェクトグループのオブジェクトのクリーン
		void ClearObject() {
			for (auto& obj : objectGroups) {
				obj.second.object.clear();
			}
		}
		// 全てクリーン
		void AllClear() { objectGroups.clear(); }

		// タイルマップ作成
		void CreateTileMap(const std::string& groupName,
			const std::string& textureFilePath,
			Model* tileModel,
			int mapWidth,
			int mapHeight,
			Vector3 tileSize,
			Vector2 tileInterval,
			const std::vector<int>& tileIndices,
			const std::vector<MapId>& mapIds,
			MapAxis axis = MapAxis::ZX); // テクスチャIndexや種類を表すID

	public: //取得
		// オブジェクトインスタンスをIDで取得
		ObjectInstans* GetObjectById(const std::string& groupName, int id);
		// 全てのオブジェクトインスタンス取得
		std::deque<ObjectInstans>& GetObjects(const std::string& groupName);
		// オブジェクトグループ取得
		ObjectGroup& GetObjectGroup(const std::string& groupName);

	private:

		// ルートシグネチャの作成
		void CreateRootSignature();
		// グラフィックスパイプラインの作成
		void CreateGraphicsPipeline();
		// ブレンドモード設定(加算)
		void BlendAdd();
		// ブレンドモード設定(減算)
		void BlendSubtract();
		// ブレンドモード設定(乗算)
		void BlendMuliply();





		std::unordered_map<std::string, ObjectGroup> objectGroups;


		const uint32_t kNumMaxInstance = 10000;

		uint32_t numInstance{};


		Camera* camera_ = nullptr;


		Transform transform{};

		int id_;

	private:
		DirectXCommon* dxCommon_ = nullptr;
		SrvManager* srvManager_ = nullptr;
		std::unique_ptr<PSOManager> psoManager_ = nullptr;
		ModelManager* modelManager_;
		Entity3DManager* entity3DManager_;
	private:
		const float kGravitationalAcceleration = 9.8f;
		////ルートシグネチャデスク
		D3D12_ROOT_SIGNATURE_DESC descriptionSignature{};
		////ルートシグネチャ
		Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature;
		//// グラフィックスパイプラインステート
		Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState[6];



		D3D12_BLEND_DESC blendDesc{};
		D3D12_RASTERIZER_DESC rasterizerDesc{};
	};
}








