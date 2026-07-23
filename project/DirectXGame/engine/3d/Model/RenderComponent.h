#pragma once
#include "DirectXGame/engine/Mesh/ModelMesh.h"
#include"DirectXGame/engine/3d/Model/Model.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"


namespace Engine {
	// オブジェクトのタイプを指定する
	enum class ObjectModelType { // オブジェクト種類
		kNormal,	// モデルを描画するオブジェクト
		kAnimation,	// モデルをアニメーション描画するオブジェクト
		kSkinning,	// モデルをスキニング描画するオブジェクト
		kPrimitive, // プリミティブを描画するオブジェクト
		kSkyBox,	// スカイボックスを描画するオブジェクト
		kOcean,		// 波を描画するオブジェクト
	};

	// 描画属性
	enum class ObjectDrawType {
		kOpaque,		// 不透明。alpha が 1 未満なら EntityManager 側で自動的に半透明扱いにする
		kTranslucent01,	// 半透明指定(互換用)。実際の描画順はカメラ奥行きで自動ソートする
		kTranslucent02,	// 半透明指定(互換用)
		kTranslucent03,	// 半透明指定(互換用)
	};

	// 前方宣言
	class BasePrimitive;
	class SkyBox;
	class Ocean;
	class Transformation;
	class EntityManager;
	class ShadowMap;

	/// <summary>
	/// 描画に関するコンポーネントクラス
	/// </summary>
	class RenderComponent
	{
	public:
		~RenderComponent();
		RenderComponent();

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="entityManager">オブジェクトに関係するマネージャー</param>
		/// <param name="objectType">オブジェクトのモデルタイプ(スキニング、アニメーション、プリミティブ、スカイボックスなど)</param>
		/// <param name="rasterizerType">モデルのうつり方を設定</param>
		void Init(EntityManager* entityManager, ObjectModelType objectType, PSOType rasterizerType);

		/// <summary>
		/// 更新
		/// </summary>
		void Update();

		/// <summary>
		/// 描画
		/// </summary>
		void Draw();
		/// <summary>
		/// シャドウマップ用の深度描画
		/// </summary>
		void DrawShadowMap(ShadowMap* shadowMap);

		/// <summary>
		/// カメラ設定
		/// </summary>
		void SetCamera(Camera* camera) { this->camera = camera; }
		/// <summary>
		/// トランスフォーム設定
		/// </summary>
		void SetTransformation(Transformation* transformation) { this->transformation = transformation; }

		/// <summary>
		/// モデル設定
		/// </summary>
		void SetModel(Model* model);
		/// <summary>
		/// スカイボックス設定
		/// </summary>
		void SetSkyBox(SkyBox* skyBox);
		/// <summary>
		/// 波セット設定
		/// </summary>
		void SetOcean(Ocean* ocean);
		/// <summary>
		/// プリミティブ設定
		/// </summary>
		void SetPrimitive(std::unique_ptr<BasePrimitive> primitive);

		

	public:
		/// <summary>
		/// モデル取得
		/// </summary>
		Model* GetModel() const;
		/// <summary>
		/// 波取得
		/// </summary>
		Ocean* GetOcean();
		/// <summary>
		/// スカイボックス取得
		/// </summary>
		SkyBox* GetSkyBox();
		/// <summary>
		/// プリミティブ形状取得
		/// </summary>
		BasePrimitive* GetPrimitive();
	public:
		/// <summary>
		/// 何かしらの見た目があるか
		/// </summary>
		bool GetIsSkin() const { return isSkin_; }
		/// <summary>
		/// 描画するかの設定
		/// </summary>
		void SetIsDraw(bool is) { isDraw = is; }
		/// <summary>
		/// オブジェクトタイプ取得
		/// </summary>
		ObjectModelType GetObjectType() const { return objectType_; }
		/// <summary>
		/// オブジェクトの描画順取得
		/// </summary>
		ObjectDrawType GetObjectDrawType() const { return objectDrawType_; }
		/// <summary>
		/// 描画順の設定
		/// </summary>
		void SetObjectDrawType(ObjectDrawType type) { objectDrawType_ = type; };
		/// <summary>
		/// 映り方タイプ設定
		/// </summary>
		void SetObjectRasterizerType(PSOType type) { rasterizerType_ = type; }
		/// <summary>
		/// オブジェクト型名前取得
		/// </summary>
		std::string GetObjectTypeName() const { return objectTypeName; }
		/// <summary>
		/// マテリアルインスタンス
		/// </summary>
		std::vector<MaterialInstance>& GetMaterialInstance() { return materialInstances_; }

	public:
		/// <summary>
		/// 透明度取得
		/// </summary>
		float GetAlpha();


	private:
		/// <summary>
		/// 通常オブジェクトのうつり方設定
		/// </summary>
		void ObjectNormalTypeDiscrimination(PSOType type);
		/// <summary>
		/// スキニングオブジェクトのうつり方設定
		/// </summary>
		void ObjectSkinningTypeDiscrimination(PSOType type);

		/// <summary>
		/// 描画セッティング(デフォルト)
		/// </summary>
		void DrawSetting();
		/// <summary>
		/// 描画セッティング(スキニング)
		/// </summary>
		void DrawSettingSkin();
		/// <summary>
		/// 描画セッティング(海)
		/// </summary>
		void DrawSettingOcean();


	private:
		// オブジェクトの描画順
		ObjectDrawType objectDrawType_ = ObjectDrawType::kOpaque;
		// オブジェクトの映り方タイプ
		PSOType rasterizerType_ = PSOType::NoUvInterpolation_MODE_SOLID_BACK;
		// オブジェクトのタイプ
		ObjectModelType objectType_ = ObjectModelType::kNormal;
		// 何かしらの見た目があるか
		bool isSkin_ = false;
		// 描画するかのフラグ
		bool isDraw = true;
		// オブジェクトタイプ名前
		std::string objectTypeName = "";

		// マテリアルインスタンス
		std::vector<MaterialInstance> materialInstances_{};
		// マテリアル用定数バッファ
		std::vector<std::unique_ptr<ConstantBuffer<Material::DataGPU>>> cbResources_;
	private: // 貰いもの
		// 3Dエンティティマネージャー
		EntityManager* entityManager = nullptr;
		// モデル
		Model* model = nullptr;
		// プリミティブ
		std::unique_ptr<BasePrimitive> primitive = nullptr;
		// スカイボックス
		SkyBox* skyBox = nullptr;
		// 波
		Ocean* ocean = nullptr;
		// トランスフォーム
		Transformation* transformation = nullptr;
		// カメラ
		Camera* camera = nullptr;
	};
}
