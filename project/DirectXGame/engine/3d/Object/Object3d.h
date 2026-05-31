#pragma once
#include"DirectXGame/engine/struct/Light.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"
#include "DirectXGame/engine/3d/Model/RenderComponent.h"
#include <DirectXGame/engine/Collider/ColliderData.h>
#include <future>

// 前方宣言

namespace Engine {
	class EntityManager;
	class Object3dCommon;
	class SkinningConmmon;
	class ImGuiManager;
	class SkyBoxCommon;
	class OceanManager;
	class RigidBodyComponent;
	class AnimationComponent;
	class ColliderComponent;
	class ContactRecord;
	class PrimitiveCommon;
	class EffectManager;
	class TransformComponent;
	class RenderComponent;
	class ShadowMap;

	class TrailEffect;
	class Camera;
	class Ocean;
	class SkyBox;
	class BasePrimitive;

	/// <summary>
	/// オブジェクトクラス
	/// </summary>
	class Object3d
	{
	public:
		Object3d();
		~Object3d();

		// 初期化
		void Initialize(EntityManager* entity3DManager, ObjectModelType objectType = ObjectModelType::kNormal, PSOType rasterizerType = PSOType::NoUvInterpolation_MODE_SOLID_BACK);
		// 更新
		void Update();

		// 物理更新
		void RigidBodyUpdate();

		// 描画通常
		void Draw();
		// シャドウマップ用の深度描画
		void DrawShadowMap(ShadowMap* shadowMap);

		// トレイルエフェクト描画
		void DrawTrailEffect();

		/// <summary>
		/// トレイルエフェクトを使うときの設定
		/// </summary>
		/// <param name="tex">tテクスチャ設定</param>
		/// <param name="maxTime">トレイルの生存時間</param>
		/// <param name="color">色</param>
		/// <param name="offsetStr">トレイルのオフセットの位置始点</param>
		/// <param name="offsetEnd">トレイルのオフセットの位置終点</param>
		void UseTrailEffect(const std::string tex, float maxTime, Color color = { 1,1,1,1 }, Vector3 offsetStr = { 0,0.5f,0 }, Vector3 offsetEnd = { 0,-0.5f,0 });

		// トレイルを出すかの設定
		void SetIsEmitTrailEffect(bool isTrailEffect) { isEmitTrailEffect = isTrailEffect; }

	public:// セッター
		// モデル設定(モデル)
		void SetModel(Model* model);
		// モデル設定(モデル名での)
		void SetModel(const std::string& filePath);
		// カメラ設定
		void SetCamera(Camera* camera);
		// 名前設定
		void SetName(const std::string& name);
		// タグ設定
		void SetNameTag(const std::string& name);
		// プリミティブ形状
		void SetPrimitive(std::unique_ptr<BasePrimitive> primitive);
		// スカイボックス
		void SetSkyBox(SkyBox* skyBox);
		// 波セット
		void SetOcean(Ocean* ocean);
		// オブジェクト固有に映すカメラを使用するか設定
		void SetIsIndividualCamera(bool isIndividualCamera);
		// メッシュ取得
		ModelMesh* GetMesh(int index);
		// マテリアル取得
		Material* GetMaterial(int index);
		// モデル取得
		Model* GetModel() const;
		// プリミティブ取得
		BasePrimitive* GetPrimitive() const;
		// 波取得
		Ocean* GetOcean() const;
		// スカイボックス取得
		SkyBox* GetSkyBox() const;
		// マテリアルインスタンス取得
		std::vector<MaterialInstance>& GetMaterialInstance();

		// タグ取得
		std::string GetNameTag() const;
		// 描画するか設定
		void SetIsDraw(bool is);
		// モデルのデバッグ用ImGui
		void DebugImguiModel();
		// スキンモデルのデバッグ用
		void DebugImguiSkin();
		// 削除する
		void IsDelete();
		// 削除されているか取得
		bool GetIsDelete() const;
		// オブジェクトに使われているモデルの透明度取得
		float GetAlpha();

	private:
		// カメラ
		Camera* defaltCamera = nullptr;
		Camera* individualCamera = nullptr;
		// 個人的にカメラを使用するか
		bool isIndividualCamera_ = false;

		// トランスフォームデータ
		std::unique_ptr<Transfomation> transformation = nullptr;



		// ImGuiを表示するか
		bool imguiFlag_ = false;

		// 削除フラグ
		bool isDelete = false;
	private: // コンポネント

		/// <summary>
		/// コライダー
		/// </summary>	

		// コライダーコンポーネント
		std::unique_ptr<ColliderComponent> colliderComponent_;
		// コライダーコンポーネントをObject3d内で更新するかのフラグ
		bool isColliderComponenyUpdate_ = false;

		/// <summary>
		///  トランスフォーム
		/// </summary>

		std::unique_ptr<TransformComponent> transformComponent_ = nullptr;

		/// <summary>
		///  物理
		/// </summary>

		std::unique_ptr<RigidBodyComponent> rigidBodyComponent_ = nullptr;

		/// <summary>
		/// アニメーション
		/// </summary>

		std::unique_ptr<AnimationComponent> animationComponent_ = nullptr;

		/// <summary>
		/// 描画
		/// </summary>

		std::unique_ptr<RenderComponent> renderComponent_ = nullptr;

	public:
		// コライダーコンポーネントを初期化
		void InitColliderComponent();
		// Object3d内でコライダーコンポーネントを更新するか
		void SetIsUpdateColliderComponent(bool is);
		// コライダーコンポーネントを取得
		ColliderComponent* GetColliderComponent();
		// コライダーコンポーネントの接触情報を取得
		ContactRecord& GetContactRecord();

		// トランスフォームコンポーネント
		TransformComponent* GetTransformComponent();
		// ワールド座標
		Vector3 GetWorldPosition() const;
		// １フレーム前のワールド座標
		Vector3 GetPreWorldPosition() const;
		// ワールド座標
		WorldTransform& GetWorldTransform();
		// 座標更新
		void UpdateWorldTransform();
		// 向いている方向
		Vector3 ObjectDirection() const;
		// スクリーン位置取得
		Vector2 GetScreenPosition();

		/// <summary>
		/// 物理
		/// </summary>

		// 初期化
		void InitRigidBodyComponent();
		// 物理取得
		RigidBodyComponent* GetRigidBodyComponent();
		//
		void SetIsRigidUpdate(bool isRigidUpdate);

		/// <summary>
		/// アニメーション
		/// </summary>

		// アニメーションコンポーネント初期化

		void InitAnimationComponent();
		// アニメーションコンポーネント取得
		AnimationComponent* GetAnimationComponent();

		/// <summary>
		/// 描画
		/// </summary>
		/// <returns></returns>
		// レンダーコンポーネント取得
		RenderComponent* GetRenderComponent();

	public:
		//
		std::unique_ptr<TrailEffect> trailEffect_ = nullptr;
		// trailエフェクトを使用するかのフラグ
		bool isEmitTrailEffect = false;
		//
		bool isRigidUpdate_ = true;

		// オブジェクト名前
		std::string name = "";
		// オブジェクトタグ
		std::string nameTag = "";
		// ID
		uint32_t id = 0;


		WorldTransform direWorldTransform_;
		Vector3 direction_ = {};
	private:
		Object3dCommon*		object3dCommon = nullptr;
		SkinningConmmon*	skinningConmmon = nullptr;
		ImGuiManager*		imGuiManager = nullptr;
		EntityManager*		entityManager = nullptr;
		LineCommon*			lineCommon = nullptr;
		SkyBoxCommon*		skyBoxCommon = nullptr;
		OceanManager *		oceanManager = nullptr;
	};

	// スクリーン座標計算取得
	Vector2 ScreenPosition(const Engine::WorldTransform world, Engine::Camera* camera);
}

