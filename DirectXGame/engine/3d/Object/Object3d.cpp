#include"Object3dCommon.h"
#include"Object3d.h"
#include"DirectXGame/engine/Skinning/Skinning.h"

#include"DirectXGame/engine/base/Texture/TextureManager.h"
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/MyGame/MyGame.h"



#include"DirectXGame/engine/Animation/Animation.h"
#include"DirectXGame/engine/Light/LightCommon.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Effect/Ocean/Ocean.h"

void Object3d::Initialize(Entity3DManager* entity3DManager, ObjectModelType objectType, ObjectRasterizerType rasterizerType)
{
	entity3DManager_ = entity3DManager;
	object3dCommon_ = entity3DManager_->GetObject3dCommon();
	skinningConmmon_ = entity3DManager_->GetSkinningConmmon();
	imGuiManager_ = entity3DManager_->GetObject3dCommon()->GetDxCommon()->GetImGuiManager();
	skyBoxCommon_ = entity3DManager_->GetSkyBoxCommon();
	oceanManager_ = entity3DManager_->GetOceanManager();


	transformComponent_ = std::make_unique<TransformComponent>();
	transformComponent_->Init();

	
	name = "object" + std::to_string(object3dCommon_->count);

	transformation = std::make_unique<Transfomation>();
	transformation->Initialize(object3dCommon_->GetDxCommon());

	defaltCamera = entity3DManager_->GetObject3dCommon()->GetDefaltCamera();
	

	isColliderComponenyUpdate_ = true;
	isSkin_ = false;

	// オブジェクトタイプ
	objectType_ = objectType;

	switch (objectType)
	{
	case Object3d::ObjectModelType::kNormal:
		objectTypeName = "NormalModelObject";
		break;
	case Object3d::ObjectModelType::kAnimation:
		objectTypeName = "AnimationModelObject";
		break;
	case Object3d::ObjectModelType::kSkinning:
		objectTypeName = "SkinningModelObject";
		break;
	case Object3d::ObjectModelType::kPrimitive:
		objectTypeName = "PrimitiveObject";
		break;
	case Object3d::ObjectModelType::kSkyBox:
		objectTypeName = "SkyBoxObject";
		break;
	case Object3d::ObjectModelType::kOcean:
		objectTypeName = "OceanObject";
		break;
	default:
		objectTypeName = "NoObject";
		break;
	}
	
	// 映り方
	rasterizerType_ = rasterizerType;

	// オブジェクト数
	object3dCommon_->count++;
}

#pragma region Update

void Object3d::Update()
{
	if (isDelete) return;

	Matrix4x4 localMatrix = MakeIdentity4x4();

	// 物理
	if (rigidBodyComponent_) {
		rigidBodyComponent_->Integrate(MyGame::GameTime(), *transformComponent_.get());
	}

	// トランスフォームコンポーネント
	transformComponent_->Update();


	if (model || primitive_ || skyBox_ || ocean_) {
		isSkin_ = true;
	}
	


	Camera* cameraPtr;
	if (isIndividualCamera_) {
		cameraPtr = individualCamera_;
	}
	else {
		cameraPtr = defaltCamera;
	}



	switch (objectType_)
	{
	case Object3d::ObjectModelType::kNormal:
		// モデルが存在する場合
		if (model) {
			localMatrix = model->modelData.rootNode.localMatrix;

			for (auto& mesh : model->modelData.mesh) {
				mesh->material->GPUData();
			}
		}

		// トランスフォームデータ
		transformation->Update(model, cameraPtr, localMatrix, transformComponent_->GetWorldTransform().worldMat_);
		break;
	case Object3d::ObjectModelType::kAnimation:
		// モデルが存在する場合
		if (model) {
			// アニメーションの更新
			//if (model->modelData.animation.flag) {
			//	if (flag) {
			//		model->modelData.animationTime += MyGame::GameTime(); // フレームごとの時間経過を反映
			//	}
			//	model->modelData.animationTime = std::fmod(model->modelData.animationTime, model->modelData.animation.duration);

			//	// 単一のジョイントの場合
			//	const NodeAnimation& rootNodeAnimation = model->modelData.animation.nodeAnimations[model->modelData.rootNode.name];
			//	Vector3 translate = Animetion::CalculateValue(rootNodeAnimation.translate.keyframes, model->modelData.animationTime);
			//	Quaternion rotate = Animetion::CalculateValue(rootNodeAnimation.rotate.keyframes, model->modelData.animationTime);
			//	Vector3 scale = Animetion::CalculateValue(rootNodeAnimation.scale.keyframes, model->modelData.animationTime);
			//	localMatrix = MakeAffineMatrix(scale, rotate, translate);
			//}
			/*else {
				localMatrix = model->modelData.rootNode.localMatrix;
			}*/
			for (auto& mesh : model->modelData.mesh) {
				mesh->material->GPUData();
			}
		}

		// トランスフォームデータ
		transformation->Update(model, cameraPtr, localMatrix, transformComponent_->GetWorldTransform().worldMat_);
		break;
	case Object3d::ObjectModelType::kSkinning:

		// モデルが存在する場合
		if (model) {
			const auto& animations = model->modelData.animations;
			auto& modelData = model->modelData;

			const std::string& currentName = modelData.currentAnimName;
			auto itCurrent = animations.find(currentName);

			if (itCurrent != animations.end()) {
				// アニメーション時間更新（毎フレーム）
				float deltaTime = MyGame::GameTime() * animationSpeed;
				if (isPlaying) {
					if (isReversePlayback) { // 逆再生なら
						modelData.animationTime -= deltaTime;
					}
					else {
						modelData.animationTime += deltaTime;
					}	
				}

				// ループするなら
				if (isLoop) {
					modelData.animationTime = std::fmod(modelData.animationTime, itCurrent->second.duration);
					
					// 負の値を返す可能性があるので
					if (modelData.animationTime < 0.0f) {
						modelData.animationTime += itCurrent->second.duration;
					}
				}
				else { // しないなら
					if (isReversePlayback) {
						if (modelData.animationTime <= 0) {
							modelData.animationTime = 0; // 最終フレームで止める
							isPlaying = false; // 自動停止
						}
					}
					else {
						if (modelData.animationTime >= itCurrent->second.duration) {
							modelData.animationTime = itCurrent->second.duration; // 最終フレームで止める
							isPlaying = false; // 自動停止
						}
					}		
				}


				// アニメーションブレンド中か？
				if (modelData.isBlending && modelData.previousAnimName != "") {
					auto itPrev = animations.find(modelData.previousAnimName);
					if (itPrev != animations.end()) {
						const Animation& prevAnim = itPrev->second;
						const Animation& currAnim = itCurrent->second;

						auto WrapTime = [](float time, float duration) {
							float wrapped = std::fmod(time, duration);
							if (wrapped < 0.0f) wrapped += duration;
							return wrapped;
							};

						float prevTime = WrapTime(modelData.animationTime, prevAnim.duration);
						float currTime = WrapTime(modelData.animationTime, currAnim.duration);

						
						// ① 各スケルトン姿勢を取得
						Skeleton prevSkeleton = modelData.skeleton;
						Skeleton currSkeleton = modelData.skeleton;
						Animetion::ApplyAnimation(prevSkeleton, prevAnim, prevTime);
						Animetion::ApplyAnimation(currSkeleton, currAnim, currTime);

						// ② 補間割合を更新（EaseInOutでなめらかに）
						modelData.blendTime += deltaTime;
						float t = modelData.blendTime / modelData.blendDuration;
						t = std::clamp(t, 0.0f, 1.0f);
						t = t * t * (3.0f - 2.0f * t); // Hermite補間（EaseInOut）

						// ③ スケルトン補間
						Animetion::BlendSkeletons(modelData.skeleton, prevSkeleton, currSkeleton, t);

						// ブレンド完了判定
						if (modelData.blendTime >= modelData.blendDuration) {
							modelData.isBlending = false;
							modelData.previousAnimName.clear();
						}
					}
					else {
						// 前アニメーションが見つからなければ通常再生
						Animetion::ApplyAnimation(modelData.skeleton, itCurrent->second, modelData.animationTime);
					}
				}
				else {
					// ブレンドしていない通常の再生
					Animetion::ApplyAnimation(modelData.skeleton, itCurrent->second, modelData.animationTime);
				}

				// スケルトン姿勢更新
				Animetion::UpdateSkeleton(modelData.skeleton);

				std::vector<Matrix4x4> cachedSkeletonMatrices;
				for (auto& mesh : modelData.mesh) {
					Animetion::UpdateSkinCluster(*mesh->skinCluster, modelData.skeleton, cachedSkeletonMatrices);
				}
				
				// ルートの変換行列反映
				localMatrix = modelData.skeleton.joints[0].skeletonSpaceMatrix;
				// デバッグ用：スケルトン描画
				Animetion::DrawSkeleton(
					entity3DManager_->Get3DLineCommon(),
					modelData.skeleton.joints,
					transformComponent_->GetWorldTransform().worldMat_.GetWorldPosition(),
					transformComponent_->GetWorldTransform().scale_,
					MakeRotateXYZ(transformComponent_->GetWorldTransform().rotate_)
				);
			}
			else {
				// アニメーションが見つからない場合のフォールバック
				localMatrix = model->modelData.rootNode.localMatrix;

				std::vector<Matrix4x4> cachedSkeletonMatrices;
				for (auto& mesh : modelData.mesh) {
					Animetion::UpdateSkinCluster(*mesh->skinCluster, modelData.skeleton, cachedSkeletonMatrices);
				}

				// デバッグ用：スケルトン描画
				Animetion::DrawSkeleton(
					entity3DManager_->Get3DLineCommon(),
					modelData.skeleton.joints,
					transformComponent_->GetWorldTransform().worldMat_.GetWorldPosition(),
					transformComponent_->GetWorldTransform().scale_,
					MakeRotateXYZ(transformComponent_->GetWorldTransform().rotate_)
				);
			}



			std::vector<std::future<void>> futures;
			for (auto& mesh : model->modelData.mesh) {
				futures.push_back(std::async(std::launch::async, [&mesh]() {
				mesh->material->GPUData();
					}));
			}

			// 全スレッドの終了を待つ
			for (auto& f : futures) {
				f.get();
			}

		}

		// トランスフォームデータ
		transformation->UpdateSkinning(model, cameraPtr, localMatrix, transformComponent_->GetWorldTransform().worldMat_);
		break;
	case ObjectModelType::kPrimitive:
		if (primitive_) {
			primitive_->Update();

			transformation->Update(primitive_.get(), cameraPtr, localMatrix, transformComponent_->GetWorldTransform().worldMat_);
		}
		break;
	case ObjectModelType::kSkyBox:
		if (skyBox_) {
			skyBox_->Update();

			transformation->Update(primitive_.get(), cameraPtr, localMatrix, transformComponent_->GetWorldTransform().worldMat_);
		}
		break;
	case ObjectModelType::kOcean:
		if (ocean_) {
			ocean_->Update();

			transformation->Update(ocean_, cameraPtr, localMatrix, transformComponent_->GetWorldTransform().worldMat_);
		}
		break;
	default:
		break;
	}


	// トレイル
	if (trailEffect_) {
		trailEffect_->Update(isEmitTrailEffect);
	}

	// コライダー
	if (isColliderComponent_) {
		if (isColliderComponenyUpdate_) {
			colliderComponent_->UpdateAll(transformComponent_->GetWorldTransform());
		}
	}
}

#pragma endregion //更新系

#pragma region Draw

void Object3d::Draw()
{
	if (!isDraw) return;
	if (isDelete) return;

	switch (objectType_)
	{
	case Object3d::ObjectModelType::kNormal:
		ObjectTypeDiscrimination(rasterizerType_);

		DrawSetting();


		// 3Dモデルが割り当てれていれば描画する
		if (model) {
			model->Draw();
		}
		break;
	case Object3d::ObjectModelType::kAnimation:
		ObjectTypeDiscrimination(rasterizerType_);

		DrawSetting();


		// 3Dモデルが割り当てれていれば描画する
		if (model) {
			model->Draw();
		}
		break;
	case Object3d::ObjectModelType::kSkinning:
		ObjectSkinTypeDiscrimination(rasterizerType_);

		DrawSettingSkin();

		// 3Dモデルが割り当てれていれば描画する
		if (model) {
			model->DrawSkinning();
		}
		break;
	case ObjectModelType::kPrimitive:


		if (primitive_) {

			primitive_->DrawSetting(primitive_->GetPsoType());

			transformation->GetCommandList(1);

			primitive_->Draw();
		}
		break;

	case ObjectModelType::kSkyBox:

		if (skyBox_) {
			skyBoxCommon_->DrawCommonSetting();

			transformation->GetCommandList(1);

			skyBox_->Draw();
		}
		break;
	case ObjectModelType::kOcean:

		if (ocean_) {
			oceanManager_->DrawCommonSetting();

			DrawSettingOcean();

			ocean_->Draw();
		}
		break;
	}




}

void Object3d::DrawTrailEffect()
{
	if (trailEffect_) {
		// トレイルエフェクトの描画
		trailEffect_->Draw();
	}
}

void Object3d::DebugImguiSkin()
{
	DebugModel::ImguiSkin(model->modelData);
}

float Object3d::GetAlpha()
{
	float a;
	switch (objectType_)
	{
	case Object3d::ObjectModelType::kNormal:
		a = model->GetMaterialAlfa();
		break;
	case Object3d::ObjectModelType::kAnimation:
		a = model->GetMaterialAlfa();
		break;
	case Object3d::ObjectModelType::kSkinning:
		a = model->GetMaterialAlfa();
		break;
	case Object3d::ObjectModelType::kPrimitive:
		a = primitive_->GetMaterial()->color.a;
		break;
	case Object3d::ObjectModelType::kSkyBox:
		a = skyBox_->GetMaterial()->color.a;
		break;
	case Object3d::ObjectModelType::kOcean:
		a = ocean_->GetMaterial()->color.a;
		break;
	default:
		a = 1.0f;
		break;
	}

	return a;
}

void Object3d::InitColliderComponent()
{
	// コライダーコンポーネントの初期化
	colliderComponent_ = std::make_unique<ColliderComponent>();
	colliderComponent_->SetOwner(colliderComponent_.get());
	// ラインコモンをセット
	colliderComponent_->SetLineCommon(entity3DManager_->Get3DLineCommon());
	// 登録（IDを取得したければ変数で受ける）
	colliderComponent_->SetUniqueId(UniqueIdGenerator::Generate());
	isColliderComponent_ = true;
	isColliderComponenyUpdate_ = true;
}

bool Object3d::IsAnimationFinished()
{
	if (!model) return false;

	const auto& animations = model->modelData.animations;
	auto& modelData = model->modelData;

	const std::string& currentName = modelData.currentAnimName;
	auto itCurrent = animations.find(currentName);

	// 現在のアニメーション名が見つからなかった場合は再生終了扱い
	if (itCurrent == animations.end()) {
		return true;
	}

	// アニメ再生中かつアニメ時間が duration に達していれば終了
	if (isReversePlayback) {
		
		return !isPlaying && modelData.animationTime <= 0;
	}
	else {
		return !isPlaying && modelData.animationTime >= itCurrent->second.duration;
	}
}

void Object3d::DrawSetting()
{
	entity3DManager_->GetLightManager()->DrawLight();

	transformation->GetCommandList(1);



	transformation->GetCommandList(10);

	if (isIndividualCamera_) {
		individualCamera_->GetCommandList(4);
	}
	else {
		defaltCamera->GetCommandList(4);
	}

	
}

void Object3d::DrawSettingSkin()
{

	entity3DManager_->GetLightManager()->DrawLight();

	transformation->GetCommandList(1);

	if (isIndividualCamera_) {
		individualCamera_->GetCommandList(4);
	}
	else {
		defaltCamera->GetCommandList(4);
	}
}

void Object3d::DrawSettingOcean()
{
	entity3DManager_->GetOceanManager()->DrawCommonSetting();


	entity3DManager_->GetLightManager()->DrawLight();

	transformation->GetCommandList(1);
	transformation->GetCommandList(9);

	if (isIndividualCamera_) {
		individualCamera_->GetCommandList(4);
	}
	else {
		defaltCamera->GetCommandList(4);
	}
}

void Object3d::ObjectTypeDiscrimination(ObjectRasterizerType type)
{
	switch (type)
	{
	case Object3d::ObjectRasterizerType::UvInterpolation_MODE_SOLID_BACK:
		object3dCommon_->DrawCommonSetting(Object3dCommon::PSOType::UvInterpolation_MODE_SOLID_BACK);
		break;
	case Object3d::ObjectRasterizerType::NoUvInterpolation_MODE_SOLID_BACK:
		object3dCommon_->DrawCommonSetting(Object3dCommon::PSOType::NoUvInterpolation_MODE_SOLID_BACK);
		break;
	case Object3d::ObjectRasterizerType::UvInterpolation_MODE_WIREFRAME_BACK:
		object3dCommon_->DrawCommonSetting(Object3dCommon::PSOType::UvInterpolation_MODE_WIREFRAME_BACK);
		break;
	case Object3d::ObjectRasterizerType::NoUvInterpolation_MODE_WIREFRAME_BACK:
		object3dCommon_->DrawCommonSetting(Object3dCommon::PSOType::NoUvInterpolation_MODE_WIREFRAME_BACK);
		break;
	case Object3d::ObjectRasterizerType::UvInterpolation_MODE_SOLID_NONE:
		object3dCommon_->DrawCommonSetting(Object3dCommon::PSOType::UvInterpolation_MODE_SOLID_NONE);
		break;
	case Object3d::ObjectRasterizerType::NoUvInterpolation_MODE_SOLID_NONE:
		object3dCommon_->DrawCommonSetting(Object3dCommon::PSOType::NoUvInterpolation_MODE_SOLID_NONE);
		break;
	case Object3d::ObjectRasterizerType::UvInterpolation_MODE_WIREFRAME_NONE:
		object3dCommon_->DrawCommonSetting(Object3dCommon::PSOType::UvInterpolation_MODE_WIREFRAME_NONE);
		break;
	case Object3d::ObjectRasterizerType::NoUvInterpolation_MODE_WIREFRAME_NONE:
		object3dCommon_->DrawCommonSetting(Object3dCommon::PSOType::NoUvInterpolation_MODE_WIREFRAME_NONE);
		break;
	default:
		break;
	}


}

void Object3d::ObjectSkinTypeDiscrimination(ObjectRasterizerType type)
{
	skinningConmmon_->DrawComputeSetting();

	for (auto& mesh : model->modelData.mesh) {

		skinningConmmon_->GetDxCommon()->GetCommandList()->SetComputeRootDescriptorTable(1, mesh->skinCluster->paletteSrvHandle.second);
		skinningConmmon_->GetDxCommon()->GetCommandList()->SetComputeRootDescriptorTable(2, mesh->skinCluster->inputVertexSrvHandle.second);
		skinningConmmon_->GetDxCommon()->GetCommandList()->SetComputeRootDescriptorTable(3, mesh->skinCluster->influenceSrvHandle.second);
		skinningConmmon_->GetDxCommon()->GetCommandList()->SetComputeRootDescriptorTable(4, mesh->skinCluster->outputVertexUavHandle.second);
		skinningConmmon_->GetDxCommon()->GetCommandList()->SetComputeRootConstantBufferView(0, mesh->skinCluster->skinningInfomation->GetGPUVirtualAddress());


		skinningConmmon_->GetDxCommon()->GetCommandList()->Dispatch(UINT(mesh->vertices.size() + 1023) / 1024, 1, 1);

		// 初期状態を UAV 用に遷移させる
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = mesh->skinCluster->outputVertexResource.Get();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		skinningConmmon_->GetDxCommon()->GetCommandList()->ResourceBarrier(1, &barrier);
	}

	switch (type)
	{
	case Object3d::ObjectRasterizerType::UvInterpolation_MODE_SOLID_BACK:
		skinningConmmon_->DrawCommonSetting(SkinningConmmon::PSOType::UvInterpolation_MODE_SOLID_BACK);
		break;
	case Object3d::ObjectRasterizerType::NoUvInterpolation_MODE_SOLID_BACK:
		skinningConmmon_->DrawCommonSetting(SkinningConmmon::PSOType::NoUvInterpolation_MODE_SOLID_BACK);
		break;
	case Object3d::ObjectRasterizerType::UvInterpolation_MODE_WIREFRAME_BACK:
		skinningConmmon_->DrawCommonSetting(SkinningConmmon::PSOType::UvInterpolation_MODE_WIREFRAME_BACK);
		break;
	case Object3d::ObjectRasterizerType::NoUvInterpolation_MODE_WIREFRAME_BACK:
		skinningConmmon_->DrawCommonSetting(SkinningConmmon::PSOType::NoUvInterpolation_MODE_WIREFRAME_BACK);
		break;
	case Object3d::ObjectRasterizerType::UvInterpolation_MODE_SOLID_NONE:
		skinningConmmon_->DrawCommonSetting(SkinningConmmon::PSOType::UvInterpolation_MODE_SOLID_NONE);
		break;
	case Object3d::ObjectRasterizerType::NoUvInterpolation_MODE_SOLID_NONE:
		skinningConmmon_->DrawCommonSetting(SkinningConmmon::PSOType::NoUvInterpolation_MODE_SOLID_NONE);
		break;
	case Object3d::ObjectRasterizerType::UvInterpolation_MODE_WIREFRAME_NONE:
		skinningConmmon_->DrawCommonSetting(SkinningConmmon::PSOType::UvInterpolation_MODE_WIREFRAME_NONE);
		break;
	case Object3d::ObjectRasterizerType::NoUvInterpolation_MODE_WIREFRAME_NONE:
		skinningConmmon_->DrawCommonSetting(SkinningConmmon::PSOType::NoUvInterpolation_MODE_WIREFRAME_NONE);
		break;
	default:
		break;
	}
	

}

#pragma endregion // 描画系

#pragma region Other

Vector2 Object3d::GetScreenPosition()
{
	Vector3 wPos = transformComponent_->GetWorldTransform().worldMat_.GetWorldPosition();

	// カメラのビュープロジェクション行列を取得
	Matrix4x4 matViewProjection;
	if (isIndividualCamera_) {
		matViewProjection = Multiply(individualCamera_->GetViewMatrix(), individualCamera_->GetProjectionMatrix()); 
	}
	else {
		matViewProjection = Multiply(defaltCamera->GetViewMatrix(), defaltCamera->GetProjectionMatrix());;
	}

	// ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, 1280, 720, 0, 1);

	// 視錐台内にオブジェクトがあるかチェック (matViewProjection を渡す)
	if (!IsInFrustum(matViewProjection, wPos)) {
		return Vector2{ -100, -100 }; // 視錐台外にある場合、無効なスクリーン座標を返す
	}

	// ワールド座標をクリップ空間座標へ変換
	Vector4 clipSpacePos = Transforms(Vector4(wPos.x, wPos.y, wPos.z, 1.0f), matViewProjection);

	// 透視除算 (NDC へ変換)
	if (clipSpacePos.w == 0.0f) {
		return Vector2{ -100, -100 }; // 透視除算エラー
	}
	Vector3 ndcPos = {
		clipSpacePos.x / clipSpacePos.w,
		clipSpacePos.y / clipSpacePos.w,
		clipSpacePos.z / clipSpacePos.w
	};

	// NDC → スクリーン座標変換
	Vector4 screenPos = Transforms(Vector4(ndcPos.x, ndcPos.y, ndcPos.z, 1.0f), matViewport);

	return Vector2{ screenPos.x, screenPos.y };
}

bool Object3d::IsInFrustum(const Matrix4x4& viewProjectionMatrix, const Vector3& position)
{
	// クリップスペース座標を取得
	Vector4 clipSpacePosition = Transforms(Vector4(position.x, position.y, position.z, 1.0f), viewProjectionMatrix);

	// w が負の場合、カメラの後ろにあるため視錐台外
	if (clipSpacePosition.w <= 0.0f) {
		return false;
	}

	// 視錐台内にあるかチェック
	if (clipSpacePosition.x < -clipSpacePosition.w || clipSpacePosition.x > clipSpacePosition.w ||
		clipSpacePosition.y < -clipSpacePosition.w || clipSpacePosition.y > clipSpacePosition.w ||
		clipSpacePosition.z < 0 || clipSpacePosition.z > clipSpacePosition.w)
	{
		return false;
	}

	return true;
}

Primitive* Object3d::GetPrimitive() const
{
	return primitive_.get();
}

void Object3d::DebugImguiModel()
{
	DebugModel::ImguiModel(model->modelData);
}

void Object3d::SetModel(const std::string& filePath)
{
	//モデルを検索してセット

	model = object3dCommon_->GetDxCommon()->GetModelManager()->FindModel(filePath);
}

void Object3d::SetPrimitive(std::unique_ptr<Primitive> primitive)
{
	primitive_ = std::move(primitive);
	//primitive_ = primitive_.get();
}

void Object3d::UseTrailEffect(const std::string tex, float maxTime, Color color,Vector3 offsetStr,Vector3 offsetEnd)
{
	trailEffect_ = std::make_unique<TrailEffect>();
	trailEffect_->Initialize(entity3DManager_->GetEffectManager(), tex, maxTime, color);
	trailEffect_->SetCamera(defaltCamera);
	trailEffect_->SetOffset(offsetStr,offsetEnd,transformComponent_->GetWorldTransform());
	
}

#pragma endregion // その他


