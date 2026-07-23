#pragma once
#include<fstream>
#include<sstream>
#include<vector>
#include<string>
#include<memory>

#include <DirectXGame/engine/Transform/WorldTransform/WorldTransform.h>

namespace Engine {
	// 前方宣言
	class ColliderComponent;
	class Camera;
	class Model;
	class RigidBodyComponent;
	class Object3dCommon;
	class ModelManager;
	class LineCommon;
	class BasePrimitive;
	class ModelMesh;
	class ContactRecord;
	class EntityManager;

	

	// オブジェクトのインスタスクラス
/// <summary>
/// ObjectInstanceを管理・実装するクラス。
/// </summary>
	class ObjectInstance {
	public:
		ObjectInstance();
		~ObjectInstance();
		/// <summary>
		/// ムーブ許可
		/// </summary>
		ObjectInstance(ObjectInstance&&) noexcept;
		ObjectInstance& operator=(ObjectInstance&&) noexcept;
		
		// 透明度タイプ
		enum class TransparencyType {
			kYes,
			kNo,
		};
		
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(EntityManager* entity3DManager, bool useCollider = false, bool rigidUpdate = true, Transform transform = { {1,1,1},{},{} });
		/// <summary>
		/// 更新
		/// </summary>
		void Update();

		/// <summary>
		/// Object3d内でコライダーコンポーネントを更新するか
		/// </summary>
		void SetIsUpdateColliderComponent(bool is);

		/// <summary>
		/// コライダーコンポーネントを取得
		/// </summary>
		ColliderComponent* GetColliderComponent();
		/// <summary>
		/// コライダーコンポーネントの接触情報を取得
		/// </summary>
		ContactRecord& GetContactRecord();
		/// <summary>
		/// リジットボディー取得
		/// </summary>
		RigidBodyComponent* GetRigidBodyComponent();


		/// <summary>
		/// 削除する
		/// </summary>
		void IsDelete() { isDelete_ = true; }

		/// <summary>
		/// 削除されているか取得
		/// </summary>
		bool GetIsDelete() const { return isDelete_; }
	private:
		// コライダーコンポーネント
		std::unique_ptr<ColliderComponent> colliderComponent_ = nullptr;
		// コライダーコンポーネントをObject3d内で更新するかのフラグ
		bool isColliderComponenyUpdate_ = false;
		bool useCollider_ = false;
		bool rigidUpdate_ = true;
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
}

