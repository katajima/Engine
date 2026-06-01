#include "LoadLevelData.h"
#include "DirectXGame/engine/3d/Model/ModelManager.h"
#include "DirectXGame/engine/manager/entity/EntityManager.h"

#include "LoadDataFanction.h"

#include"DirectXGame/application/base/Camera/Base/CameraManeger.h"
#include "DirectXGame/application/base/Camera/FixedCamera/FixedCamera.h"

void LoadLevelData::Initialize(Engine::EntityManager* entityManager, Engine::ModelManager* modelManager, Engine::Camera* camera,
	const std::string extensionName, const std::string fileName)
{
	this->entityManager = entityManager;	// エンティティ
	this->modelManager = modelManager;		// モデル管理クラス

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
	const auto& models = modelManager->GetModel();


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
		Engine::Object3d* newObject = entityManager->CreateObject3D(objectData.fileName, Engine::ObjectModelType::kNormal,{},{});
		//newObject->Initialize(entity3DManager_);
		newObject->SetModel(model);
		newObject->SetIsDraw(true);
		newObject->SetNameTag("LevelObject");
		if (objectData.isCollider) {
			newObject->InitColliderComponent();

			// SphereColliderを追加
			auto aabb = std::make_unique<Engine::AABBCollider>();
			aabb->SetTag(CollisionTag::Wall);
			//sphere->layer = CollisionLayer::None;
			aabb->SetCollisionMask(0xFFFFFFFF);
			//aabb->radius = objectData.size.x;
			aabb->aabb.min = -((objectData.size) / 2)* objectData.scale;
			aabb->aabb.max = ((objectData.size) / 2)* objectData.scale;
			aabb->SetIsStatic(true);
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
		camera->Initialize(nullptr, entityManager, nullptr, {});
		camera->GetUniqueCamera()->SetTransform({ cameraData.scale,
			DegreesToRadians(cameraData.rotation),cameraData.position });
		
		camera->SetName(cameraData.fileName);

		cameras_.push_back(std::move(camera));
	}


	
	// カメラマネージャーに追加
	for (auto& camera : cameras_) {
		cameraManager->AddCamera({ camera.get() ,false}, camera->GetName());
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
		entityManager->GetLightManager()->AddLight(light);
	}

	
	
}

void LoadLevelData::ClearData()  
{  
	// "LevelObject" タグを持つオブジェクトを全削除  
	entityManager->EraseObject3DByTag("LevelObject");  
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
	entityManager->GetLightManager()->ClearLights();
	lights_.clear();


	// カメラの削除処理を修正  
	for (auto& camera : cameras_) {  
		cameraManager->DeleteCamera(camera->GetName());
		camera.release();
	}  
	cameras_.clear();  
}


