#include "LoadLevelData.h"
#include "DirectXGame/engine/3d/Model/ModelManager.h"
#include "DirectXGame/engine/manager/entity3d/Entity3DManager.h"

#include "LoadDataFanction.h"

#include"DirectXGame/application/base/Camera/Base/CameraManeger.h"
#include "DirectXGame/application/base/Camera/FixedCamera/FixedCamera.h"

void LoadLevelData::Initialize(Engine::Entity3DManager* entity3DManager, Engine::ModelManager* modelManager, Engine::Camera* camera,
	const std::string extensionName, const std::string fileName)
{
	entity3DManager_ = entity3DManager;	// エンティティ3d
	modelManager_ = modelManager;		// モデル管理クラス

	kFileName = fileName;
	kExtension = extensionName;
	
	// 凍結してフルパスを得る 
	nlohmann::json deserialized = LoadDataFanc::FileData(kDefaultBaseDirectory + kFileName + kExtension);

	levelData_ = std::make_unique<LevelData>();
	// "objects"の全オブジェクトを走査 
	for (nlohmann::json& object : deserialized["objects"]) {
		// 有効無効フラグ
		if (object.contains("object_disabled")) {
			auto& objectDisabled = object["object_disabled"];
			if (objectDisabled["object_disabled"]) {
				// 配置しない
				continue;
			}
		}
		
		// JSONファイルからトランスフォーム情報をレベルデータに書き出す(オブジェクト)
		LoadDataFanc::ModelTransfom(object, levelData_.get());
		// JSONファイルからトランスフォーム情報をレベルデータに書き出す(出現位置)
		LoadDataFanc::SpawwnPoint(object, levelData_.get());
		// JSONファイルからトランスフォーム情報をレベルデータに書き出す(カメラ位置)
		LoadDataFanc::CameraTransform(object, levelData_.get());
		// JSONファイルからトランスフォーム情報をレベルデータに書き出す(ライト位置)
		LoadDataFanc::LightTransform(object, levelData_.get());
	}
	// レベルデータよりオブジェクトを生成する
	CreateObject3d(levelData_.get());
	// レベルデータよりカメラを生成する
	CreateCamera(levelData_.get());
	// レベルデータよりライトを生成する
	CreateLight(levelData_.get());
}

void LoadLevelData::ReLoad()
{
	ClearData();

	nlohmann::json deserialized = LoadDataFanc::FileData(kDefaultBaseDirectory + kFileName + kExtension);

	// "objects"の全オブジェクトを走査 
	for (nlohmann::json& object : deserialized["objects"]) {
		// 有効無効フラグ
		if (object.contains("object_disabled")) {
			auto& objectDisabled = object["object_disabled"];
			if (objectDisabled["object_disabled"]) {
				// 配置しない
				continue;
			}
		}

		// JSONファイルからトランスフォーム情報をレベルデータに書き出す(オブジェクト)
		LoadDataFanc::ModelTransfom(object, levelData_.get());
		// JSONファイルからトランスフォーム情報をレベルデータに書き出す(出現位置)
		LoadDataFanc::SpawwnPoint(object, levelData_.get());
		// JSONファイルからトランスフォーム情報をレベルデータに書き出す(カメラ位置)
		LoadDataFanc::CameraTransform(object, levelData_.get());
		// JSONファイルからトランスフォーム情報をレベルデータに書き出す(ライト位置)
		LoadDataFanc::LightTransform(object, levelData_.get());
	}
	// レベルデータよりオブジェクトを生成する
	CreateObject3d(levelData_.get());
	// レベルデータよりカメラを生成する
	CreateCamera(levelData_.get());
	// レベルデータよりライトを生成する
	CreateLight(levelData_.get());


}


void LoadLevelData::Update()
{
#ifdef _DEBUG
	ImGui::Begin("sceneData");
	if (ImGui::Button("ReLoad")) {
		ReLoad();
	}
	ImGui::End();
#endif // _DEBUG

}

void LoadLevelData::Draw3D()
{
	
}

void LoadLevelData::CreateObject3d(LevelData* levelData)
{
	const auto& models = modelManager_->GetModel();


	for (auto& objectData : levelData->objects) {
		// ファイル名から登録済みモデルを検索
		Engine::Model* model = nullptr;

		// obj
		if (model == nullptr) {
			auto it = models.find(objectData.fileName + ".obj");
			if (it != models.end()) { model = it->second.get(); }
		}

		// gltf
		if (model == nullptr) {
			auto it = models.find(objectData.fileName + ".gltf");
			if (it != models.end()) { model = it->second.get(); }
		}

		// モデルを指定して3Dオブジェクトを生成 
		Engine::Object3d* newObject = entity3DManager_->CreateObject3D(objectData.fileName, Engine::ObjectModelType::kNormal,{},{});
		//newObject->Initialize(entity3DManager_);
		newObject->SetModel(model);
		newObject->SetIsDraw(true);
		newObject->SetNameTag("LevelObject");
		if (objectData.isCollider) {
			newObject->InitColliderComponent();

			// SphereColliderを追加
			auto aabb = std::make_unique<Engine::AABBCollider>();
			aabb->tag = CollisionTag::Wall;
			//sphere->layer = CollisionLayer::None;
			aabb->collisionMask = 0xFFFFFFFF;
			//aabb->radius = objectData.size.x;
			aabb->aabb.min_ = -((objectData.size) / 2)* objectData.scale;
			aabb->aabb.max_ = ((objectData.size) / 2)* objectData.scale;
			aabb->isStatic = true;
			newObject->GetColliderComponent()->AddCollider(std::move(aabb));
		}
		// 座標 
		newObject->GetWorldTransform().translate_ = objectData.position;
		// 回転角 
		newObject->GetWorldTransform().rotate_ = objectData.rotation;
		// 大きさ
		newObject->GetWorldTransform().scale_ = objectData.scale;

		objects_.push_back(newObject);
	}
}

void LoadLevelData::CreateCamera(LevelData* levelData)
{


	for (auto& cameraData : levelData->cameras) {
		std::unique_ptr<BaseCamera> camera = std::make_unique<FixedCamera>();
		camera->Initialize(nullptr, entity3DManager_, nullptr, {});
		camera->GetUniqueCamera()->transform_.translate = cameraData.position;
		camera->GetUniqueCamera()->transform_.rotate = DegreesToRadians(cameraData.rotation);
		camera->GetUniqueCamera()->transform_.scale = cameraData.scale;

		camera->SetName(cameraData.fileName);

		cameras_.push_back(std::move(camera));
	}


	
	// カメラマネージャーに追加
	for (auto& camera : cameras_) {
		cameraManager_->AddCamera({ camera.get() ,false}, camera->GetName());
	}

}

void LoadLevelData::CreateLight(LevelData* levelData)
{
	for (auto& directionalData : levelData->directionalDatas) {
		std::shared_ptr<Engine::DirectionalLight> directional = std::make_shared<Engine::DirectionalLight>();
		directionalData.intensity = 0.3f;
		directional->directional = directionalData;

		lights_.push_back(directional);
	}
	for (auto& pointData : levelData->pointDatas) {
		std::shared_ptr<Engine::PointLight> point = std::make_shared<Engine::PointLight>();
		point->point = pointData;
		
		lights_.push_back(point);
	}
	for (auto& spotData : levelData->spotDatas) {
		std::shared_ptr<Engine::SpotLight> spot = std::make_shared<Engine::SpotLight>();
		spot->spot = spotData;
		
		lights_.push_back(spot);
	}
	

	for (auto& light : lights_) {
		entity3DManager_->GetLightManager()->AddLight(light);
	}

	
	
}

void LoadLevelData::ClearData()  
{  
	// "LevelObject" タグを持つオブジェクトを全削除  
	entity3DManager_->EraseObject3DByTag("LevelObject");  
	levelData_->objects.clear();  
	levelData_->players.clear();  
	levelData_->enemys.clear();  
	levelData_->cameras.clear();  
	objects_.clear();  
	levelData_->directionalDatas.clear();
	levelData_->pointDatas.clear();
	levelData_->spotDatas.clear();


	
	for (auto& light : lights_) {
		light.reset();
	}
	entity3DManager_->GetLightManager()->ClearLights();
	lights_.clear();


	// カメラの削除処理を修正  
	for (auto& camera : cameras_) {  
		cameraManager_->DeleteCamera(camera->GetName());
		camera.release();
	}  
	cameras_.clear();  
}


