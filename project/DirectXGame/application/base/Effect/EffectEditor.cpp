#include "EffectEditor.h"
#include "DirectXGame/engine/Base/Imgui/ImGuiUtility.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/engine/3d/Model/ModelManager.h"
#include "DirectXGame/engine/Base/Texture/TextureManager.h"
#include "DirectXGame/application/GlobalVariables/GlobalVariables.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <set>

namespace {
	const char* kEffectRegistryGroup = "EffectEditor";
	const char* kDeletedEffectRegistryGroup = "EffectEditorDeleted";
	const char* kEffectEditorSettingsGroup = "EffectEditorSettings";
	const char* kParticleRegistryGroup = "ParticleEditor";
	const char* kDeletedParticleRegistryGroup = "ParticleEditorDeleted";
	const char* kParticleDataPrefix = "ParticleGroup_";
	const char* kPrimitiveRegistryGroup = "EffectPrimitiveEditor";
	const char* kPrimitiveDataPrefix = "EffectPrimitive_";
	const char* kPrimitivePreviewParticleName = "__EffectPrimitivePreviewParticle";
	const char* kPrimitivePreviewEmitterName = "__EffectPrimitivePreviewEmitter";
	const char* kParticleGroupPreviewEmitterName = "__ParticleGroupPreviewEmitter";
	const char* kFallbackParticleTexturePath = "resources/Texture/Image.dds";

	bool DrawPrimitiveShapeParameters(Engine::ParticleGroupEditorData& data) {
		// 選択中プリミティブに対応した形状パラメータだけを表示し、変更があれば保存対象にする。
		bool changed = false;
		if (!ImGui::CollapsingHeader("Primitive Shape")) {
			return changed;
		}

		switch (data.shapeType)
		{
		case Engine::ShapeParameter::ShapeType::Plane:
			changed |= ImGui::DragFloat("Plane Width", &data.plane.width, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragFloat("Plane Height", &data.plane.height, 0.01f, 0.01f, 1000.0f);
			break;
		case Engine::ShapeParameter::ShapeType::Triangle:
			changed |= ImGui::DragFloat2("Triangle Up", &data.triangle.upPos.x, 0.01f);
			changed |= ImGui::DragFloat2("Triangle Left", &data.triangle.leftPos.x, 0.01f);
			changed |= ImGui::DragFloat2("Triangle Right", &data.triangle.rightPos.x, 0.01f);
			break;
		case Engine::ShapeParameter::ShapeType::Cross:
			changed |= ImGui::DragFloat("Cross Arm Length", &data.cross.armLength, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragFloat("Cross Arm Width", &data.cross.armWidth, 0.01f, 0.01f, 1000.0f);
			break;
		case Engine::ShapeParameter::ShapeType::Cube:
			changed |= ImGui::DragFloat3("Cube Size", &data.cube.size.x, 0.01f, 0.01f, 1000.0f);
			break;
		case Engine::ShapeParameter::ShapeType::Circle:
			changed |= ImGui::DragFloat("Circle Radius", &data.circle.radius, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragInt("Circle Segments", &data.circle.segments, 1, 3, 256);
			break;
		case Engine::ShapeParameter::ShapeType::Star:
			changed |= ImGui::DragFloat("Star Inner Radius", &data.star.innerRadius, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragFloat("Star Outer Radius", &data.star.outerRadius, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragInt("Star Segments", &data.star.segments, 1, 3, 256);
			break;
		case Engine::ShapeParameter::ShapeType::Crescent:
			changed |= ImGui::DragFloat("Crescent Inner Radius", &data.crescent.innerRadius, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragFloat("Crescent Outer Radius", &data.crescent.outerRadius, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragFloat("Crescent Distance", &data.crescent.distance, 0.01f, 0.0001f, 1000.0f);
			changed |= ImGui::DragInt("Crescent Segments", &data.crescent.segments, 1, 8, 256);
			break;
		case Engine::ShapeParameter::ShapeType::Ring:
			changed |= ImGui::DragFloat("Ring Inner Radius", &data.ring.innerRadius, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragFloat("Ring Outer Radius", &data.ring.outerRadius, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragInt("Ring Segments", &data.ring.segments, 1, 5, 256);
			break;
		case Engine::ShapeParameter::ShapeType::Sphere:
			changed |= ImGui::DragFloat("Sphere Radius", &data.sphere.radius, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragInt("Sphere Latitude", &data.sphere.latitudeSegments, 1, 3, 256);
			changed |= ImGui::DragInt("Sphere Longitude", &data.sphere.longitudeSegments, 1, 3, 256);
			changed |= ImGui::Checkbox("Sphere Top Based", &data.sphere.isTopBased);
			break;
		case Engine::ShapeParameter::ShapeType::Arrow:
			changed |= ImGui::DragFloat("Arrow Shaft Length", &data.arrow.shaftLength, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragFloat("Arrow Shaft Radius", &data.arrow.shaftRadius, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragFloat("Arrow Head Length", &data.arrow.headLength, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragFloat("Arrow Head Radius", &data.arrow.headRadius, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragInt("Arrow Segments", &data.arrow.segments, 1, 3, 256);
			break;
		case Engine::ShapeParameter::ShapeType::Cylinder:
			changed |= ImGui::DragFloat("Cylinder Inner Radius", &data.cylinder.innerRadius, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragFloat("Cylinder Outer Radius", &data.cylinder.outerRadius, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragFloat("Cylinder Height", &data.cylinder.height, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragInt("Cylinder Segments", &data.cylinder.segments, 1, 3, 256);
			changed |= ImGui::Checkbox("Cylinder Cover", &data.cylinder.isCover);
			break;
		case Engine::ShapeParameter::ShapeType::Tube:
			changed |= ImGui::DragFloat("Tube Radius", &data.tube.radius, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragFloat("Tube Inner Radius", &data.tube.innerRadius, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragFloat("Tube Height", &data.tube.height, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragInt("Tube Segments", &data.tube.segments, 1, 3, 256);
			break;
		case Engine::ShapeParameter::ShapeType::Pyramid:
			changed |= ImGui::DragFloat("Pyramid Radius", &data.pyramid.radius, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragFloat("Pyramid Height", &data.pyramid.height, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragInt("Pyramid Segments", &data.pyramid.segments, 1, 3, 256);
			break;
		case Engine::ShapeParameter::ShapeType::Torus:
			changed |= ImGui::DragFloat("Torus Inner Radius", &data.torus.innerRadius, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragFloat("Torus Outer Radius", &data.torus.outerRadius, 0.01f, 0.01f, 1000.0f);
			changed |= ImGui::DragInt("Torus Segments", &data.torus.segments, 1, 3, 256);
			changed |= ImGui::DragInt("Torus Tube Segments", &data.torus.tubeSegments, 1, 3, 256);
			break;
		default:
			ImGui::TextDisabled("No primitive shape parameters.");
			break;
		}
		return changed;
	}

	bool DrawShapeTypeCombo(const char* label, Engine::ShapeParameter::ShapeType& shapeType) {
		// ShapeTypeはNoneが-1なので、単純な配列index変換では1つずれる。明示対応で安全に選択する。
		struct ShapeLabel {
			const char* label;
			Engine::ShapeParameter::ShapeType value;
		};
		static constexpr ShapeLabel kShapeLabels[] = {
			{ "None", Engine::ShapeParameter::ShapeType::None },
			{ "Plane", Engine::ShapeParameter::ShapeType::Plane },
			{ "Triangle", Engine::ShapeParameter::ShapeType::Triangle },
			{ "Cross", Engine::ShapeParameter::ShapeType::Cross },
			{ "Cube", Engine::ShapeParameter::ShapeType::Cube },
			{ "Circle", Engine::ShapeParameter::ShapeType::Circle },
			{ "Star", Engine::ShapeParameter::ShapeType::Star },
			{ "Crescent", Engine::ShapeParameter::ShapeType::Crescent },
			{ "Ring", Engine::ShapeParameter::ShapeType::Ring },
			{ "Sphere", Engine::ShapeParameter::ShapeType::Sphere },
			{ "Arrow", Engine::ShapeParameter::ShapeType::Arrow },
			{ "Cylinder", Engine::ShapeParameter::ShapeType::Cylinder },
			{ "Tube", Engine::ShapeParameter::ShapeType::Tube },
			{ "Pyramid", Engine::ShapeParameter::ShapeType::Pyramid },
			{ "Torus", Engine::ShapeParameter::ShapeType::Torus },
		};

		const char* preview = "Unknown";
		for (const ShapeLabel& item : kShapeLabels) {
			if (item.value == shapeType) {
				preview = item.label;
				break;
			}
		}

		bool changed = false;
		if (ImGui::BeginCombo(label, preview)) {
			for (const ShapeLabel& item : kShapeLabels) {
				const bool selected = (item.value == shapeType);
				if (ImGui::Selectable(item.label, selected)) {
					shapeType = item.value;
					changed = true;
				}
				if (selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		return changed;
	}

	bool DrawParticleMeshSourceCombo(const char* label, Engine::ParticleMeshSourceType& meshSourceType) {
		// パーティクル群が使うメッシュを、エディタ生成プリミティブかロード済みモデルから選ばせる。
		static const char* kMeshSourceLabels[] = {
			"Primitive",
			"Model",
		};
		return Engine::ImGuiUtility::SelectEnum(label, kMeshSourceLabels, meshSourceType);
	}

	bool DrawModelNameCombo(const char* label, std::string& modelName, Engine::ModelManager* modelManager) {
		if (modelManager == nullptr || modelManager->GetModel().empty()) {
			ImGui::TextDisabled("No loaded models.");
			return false;
		}

		// ModelManagerにロード済みのモデル名だけを選択肢にして、未ロード参照を避ける。
		bool changed = false;
		const char* preview = modelName.empty() ? "No Model" : modelName.c_str();
		if (modelName.empty()) {
			// 生成元をModelへ切り替えた直後でも再作成できるよう、先頭モデルを仮選択する。
			modelName = modelManager->GetModel().begin()->first;
			preview = modelName.c_str();
			changed = true;
		}

		if (ImGui::BeginCombo(label, preview)) {
			for (const auto& [loadedModelName, model] : modelManager->GetModel()) {
				const bool selected = modelName == loadedModelName;
				if (ImGui::Selectable(loadedModelName.c_str(), selected)) {
					modelName = loadedModelName;
					changed = true;
				}
				if (selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		return changed;
	}

	bool DrawParticleTextureSelector(const char* comboLabel, const char* inputLabel,
		std::array<char, 256>& texturePathBuffer, Engine::TextureManager* textureManager) {
		// ロード済みテクスチャを選択できるようにし、必要な場合だけ手入力も許可する。
		bool changed = false;
		const std::string currentPath = texturePathBuffer.data();
		const char* preview = currentPath.empty() ? "Select loaded texture" : currentPath.c_str();
		if (ImGui::BeginCombo(comboLabel, preview)) {
			if (textureManager != nullptr) {
				for (const std::string& filePath : textureManager->GetTextureFilePaths()) {
					const bool selected = currentPath == filePath;
					if (ImGui::Selectable(filePath.c_str(), selected)) {
						Engine::ImGuiUtility::SetInputTextBuffer(texturePathBuffer, filePath);
						changed = true;
					}
					if (selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::EndCombo();
		}
		if (Engine::ImGuiUtility::InputText(inputLabel, texturePathBuffer)) {
			changed = true;
		}
		return changed;
	}

	std::string ResolveParticleTexturePath(Engine::TextureManager* textureManager, const std::string& requestedPath) {
		// 無効なパスは描画処理へ渡さず、常時ロード済みの仮テクスチャへ置き換える。
		if (textureManager != nullptr && textureManager->HasTexture(requestedPath)) {
			return requestedPath;
		}
		return kFallbackParticleTexturePath;
	}
	void CopyPrimitiveShapeData(const Engine::ParticleGroupEditorData& source, Engine::ParticleGroupEditorData& destination) {
		// プリミティブ形状に関わる保存項目だけをパーティクル群へ反映する。
		destination.meshSourceType = Engine::ParticleMeshSourceType::Primitive;
		destination.modelName.clear();
		destination.shapeType = source.shapeType;
		destination.plane = source.plane;
		destination.triangle = source.triangle;
		destination.cross = source.cross;
		destination.cube = source.cube;
		destination.circle = source.circle;
		destination.star = source.star;
		destination.crescent = source.crescent;
		destination.ring = source.ring;
		destination.sphere = source.sphere;
		destination.arrow = source.arrow;
		destination.cylinder = source.cylinder;
		destination.tube = source.tube;
		destination.pyramid = source.pyramid;
		destination.torus = source.torus;
		destination.isEditorPrimitive = true;
	}
}

void EffectEditor::Initialize(Engine::EffectComponent* effectComponent,
	Engine::GlobalVariables* globalVariables) {

	this->effectComponent = effectComponent;
	this->globalVariables = globalVariables;
	serializer_.Initialize(globalVariables);
	Engine::ImGuiUtility::SetInputTextBuffer(newParticleTexturePathBuffer_, newParticleGroupData_.texturePath);
	LoadRegisteredPrimitiveDefinitions();
	LoadEditorUiSettings();

}

// 更新
void EffectEditor::Update(float dt) {
#ifdef _DEBUG
	ImGui::Begin("Effect");

	if (ImGui::BeginTabBar("EffectEditorTabs")) {
		if (ImGui::BeginTabItem("Effect Select")) {
			DrawEffectManagement();
			if (!effectGlobalDatas_.empty()) {
				// 編集対象のエフェクトを選択する。
				Engine::ImGuiUtility::SelectMapKey("Selected Effect", selectedBlockName_, effectGlobalDatas_);
				ImGui::Text("Editing: %s", selectedBlockName_.c_str());
			}
			else {
				ImGui::TextDisabled("エフェクトを追加してください。");
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Emitter Settings")) {
			if (effectGlobalDatas_.empty()) {
				ImGui::TextDisabled("エフェクトを追加してください。");
			}
			else {
				// 選択中エフェクトの発火確認とエミッター設定をまとめる。
				Engine::ImGuiUtility::SelectMapKey("Selected Effect", selectedBlockName_, effectGlobalDatas_);
				ImGui::Separator();
				ImGui::Text("Editing: %s", selectedBlockName_.c_str());
				ImGui::Checkbox("出現", &isSpawnEmit);
				ImGui::DragFloat3("位置", &spawnEmitPos.x, 0.1f);
				ImGui::DragFloat("頻度", &frequency, 0.01f);

				if (isSpawnEmit) {
					timer += dt;
					if (frequency < timer) {
						Emit(selectedBlockName_, spawnEmitPos);
						timer = 0.0f;
					}
				}

				for (auto& combo : effectGlobalDatas_) {
					const std::string& name = combo.first;
					const bool nowChoice = (name == selectedBlockName_);
					if (!nowChoice) continue;

					// エディターでの調整
					DrawEffectDetail(name, combo.second);
					// データの保存
					serializer_.SaveEffectData(name, combo.second);
				}
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Particle Groups")) {
			DrawParticleGroupEditor();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Primitive Library")) {
			DrawPrimitiveLibraryEditor();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Save Settings")) {
			// 手動で全設定をまとめて保存する。
			if (ImGui::Button("Save All Effect Settings")) {
				SaveAllEditorData();
			}
			ImGui::TextWrapped("Effect, particle group, primitive definition, and registry data are saved together.");
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
#endif // _DEBUG

#ifdef _DEBUG
	for (auto& [name, data] : effectGlobalDatas_) {
		serializer_.LoadEffectData(name, data);
		SetEffectGlobalData(name, data.shapeType, data);
	}
#endif // _DEBUG
}

void EffectEditor::SetEffectGlobalData(const std::string& name, EmitterShapeType shapeType, const EffectGlobalData& data) {
	Engine::BaseParticleEmitter* emit = effectComponent->GetBaseEmitter(name);
	emit->SetParticleName(data.particleName);		// パーティクル名
	emit->GetFrequency() = data.frequency;			// 出現頻度
	emit->GetEmitData() = data.emitData;			// 出現データ
	emit->SetIsFlag(data.isFlag);
	// UVスクロールなどのパーティクルマテリアル変化をエミッターへ反映する。
	emit->SetUvTransformVeloctiy(data.uvTransformVeloctiy);
	emit->SetAlphaClipping(data.alphaClipping);		// アルファクリッピング
	emit->SetEnableLighting(data.enableLighting);	// ライティングの有無
	emit->SetLifeTimeScaleTopBottom(data.topBottom);// ライフタイムスケールの基準
	emit->SetEmitType(data.emitType);				// 形状内の出現場所
	emit->SetEmitDirectionType(data.directionType);	// 形状に対する初速方向


	switch (shapeType)
	{
	case EmitterShapeType::AABB:
	{
		Engine::AABBParticleEmitter* aabbEmit = effectComponent->GetEmitterAs<Engine::AABBParticleEmitter>(name);
		aabbEmit->SetRange(data.rangeMin, data.rangeMax);
	}
	break;
	case EmitterShapeType::SPHERE:
	{
		Engine::SphereParticleEmitter* sphereEmit = effectComponent->GetEmitterAs<Engine::SphereParticleEmitter>(name);
		sphereEmit->SetRadius(data.radius);
	}
	break;
	case EmitterShapeType::CIRCLE:
	{
		Engine::CornerParticleEmitter* circleEmit = effectComponent->GetEmitterAs<Engine::CornerParticleEmitter>(name);
		circleEmit->SetRadius(data.radius);
		circleEmit->SetSegment(data.segment);
	}
	break;
	case EmitterShapeType::POINT:
	{

	}
	break;
	case EmitterShapeType::LINE:
	{
		Engine::LineParticleEmitter* lineEmit = effectComponent->GetEmitterAs<Engine::LineParticleEmitter>(name);
		lineEmit->SetSegment(data.lineStart, data.lineEnd);
	}
	break;
	case EmitterShapeType::SPLINE:
	{
		Engine::SplineParticleEmitter* splineEmit = effectComponent->GetEmitterAs<Engine::SplineParticleEmitter>(name);
		for (const auto& point : data.splinePoints) {
			splineEmit->AddControlPoints(point);
		}
	}
	break;
	case EmitterShapeType::TRIANGLE:
	{
		Engine::TriangleParticleEmitter* triangleEmit = effectComponent->GetEmitterAs<Engine::TriangleParticleEmitter>(name);
		triangleEmit->SetTriangle(data.triangle);
	}
	break;
	case EmitterShapeType::MESH:
		break;
	default:
		break;
	}
}

void EffectEditor::AddEffectGlobalData(const std::string& name, const std::string& particleName) {
	if (effectGlobalDatas_.find(name) != effectGlobalDatas_.end()) {
		// すでに存在する場合
		return;
	}
	if (IsDeletedEffectName(name)) {
		// UIで削除した初期エフェクトは、次回起動時の固定登録で復活させない。
		return;
	}
	EffectGlobalData data;
	data.particleName = particleName;
	globalVariables->SetGroupCategory(name, "Effect");
	// 保存項目に追加
	serializer_.RegisterEffectData(name, data);
	// データを取得
	serializer_.LoadEffectData(name, data);
	// データを保存
	effectGlobalDatas_[name] = data;
	// エフェクト一覧として保存し、次回起動時にUI追加分も復元できるようにする。
	RegisterEffectName(name);
}

void EffectEditor::LoadRegisteredEffectGlobalDatas() {
	// レジストリグループがなければ作成し、以降の追加保存に備える。
	globalVariables->CreateGroup(kEffectRegistryGroup);
	globalVariables->CreateGroup(kDeletedEffectRegistryGroup);
	globalVariables->SetGroupCategory(kEffectRegistryGroup, "Effect");
	globalVariables->SetGroupCategory(kDeletedEffectRegistryGroup, "Effect");

	for (const std::string& effectName : globalVariables->GetKeys(kEffectRegistryGroup)) {
		if (effectGlobalDatas_.find(effectName) != effectGlobalDatas_.end() ||
			IsDeletedEffectName(effectName) ||
			!globalVariables->HasGroup(effectName)) {
			continue;
		}

		// 保存済みエフェクト本体のグループからデータを読み戻す。
		EffectGlobalData data;
		globalVariables->SetGroupCategory(effectName, "Effect");
		serializer_.LoadEffectData(effectName, data);
		effectGlobalDatas_[effectName] = data;
	}
}

void EffectEditor::LoadRegisteredParticleGroups() {
	Engine::ParticleManager* particleManager = effectComponent->GetParticleManager();

	// レジストリグループがなければ作成し、以降の追加保存に備える。
	globalVariables->CreateGroup(kParticleRegistryGroup);
	globalVariables->CreateGroup(kDeletedParticleRegistryGroup);
	globalVariables->SetGroupCategory(kParticleRegistryGroup, "Effect/Particle");
	globalVariables->SetGroupCategory(kDeletedParticleRegistryGroup, "Effect/Particle");

	// 削除済みとして保存されている既存パーティクル群を、ハードコード生成後に取り除く。
	for (const std::string& particleName : globalVariables->GetKeys(kDeletedParticleRegistryGroup)) {
		particleManager->RemoveParticleGroup(particleName);
	}

	std::set<std::string> particleNames;
	for (const std::string& particleName : globalVariables->GetKeys(kParticleRegistryGroup)) {
		// レジストリに登録されている名前は、従来通り復元対象にする。
		particleNames.insert(particleName);
	}
	for (const std::string& groupName : globalVariables->GetGroupNames()) {
		if (groupName.rfind(kParticleDataPrefix, 0) != 0) {
			continue;
		}

		// ParticleGroup_<名前> の保存ファイルがある場合は、レジストリ漏れでも復元できるようにする。
		particleNames.insert(groupName.substr(std::strlen(kParticleDataPrefix)));
	}

	for (const std::string& particleName : particleNames) {
		if (IsDeletedParticleGroupName(particleName)) {
			continue;
		}

		const std::string dataGroupName = kParticleDataPrefix + particleName;
		if (!globalVariables->HasGroup(dataGroupName)) {
			continue;
		}

		// 保存済みメタデータから、エディタ所有プリミティブのパーティクル群を復元する。
		Engine::ParticleGroupEditorData data;
		globalVariables->SetGroupCategory(dataGroupName, "Effect/Particle");
		serializer_.LoadParticleGroupData(particleName, data);
		if (!particleManager->GetParticleGroups().Contains(particleName)) {
			// モデルがまだロードされていなくても、保存メタデータだけは失わない。
			if (!particleManager->CreateEditorParticleGroup(particleName, data)) {
				particleManager->SetEditorParticleGroupData(particleName, data);
			}
		}
		else {
			particleManager->SetEditorParticleGroupData(particleName, data);
			particleManager->ApplyEditorParticleGroupData(particleName, data);
		}
		RegisterParticleGroupName(particleName);
	}
}

void EffectEditor::DrawEffectManagement() {
	ImGui::SeparatorText("Effect Management");

	// 新規追加名とパーティクルを選択してエフェクトを作成する。
	Engine::ImGuiUtility::InputText("New Effect Name", newEffectNameBuffer_);
	Engine::ImGuiUtility::SelectMapKey("New Particle", newEffectParticleName_,
		effectComponent->GetParticleManager()->GetParticleGroups(), "No Particle");
	ImGui::SameLine();
	if (ImGui::Button("Add Effect")) {
		AddEffectFromEditor();
	}

	if (!selectedBlockName_.empty() &&
		effectGlobalDatas_.find(selectedBlockName_) != effectGlobalDatas_.end()) {
		ImGui::Text("Selected: %s", selectedBlockName_.c_str());
		if (ImGui::Button("Rename Selected Effect")) {
			pendingRenameEffectName_ = selectedBlockName_;
			strncpy_s(renameEffectNameBuffer_.data(), renameEffectNameBuffer_.size(),
				selectedBlockName_.c_str(), _TRUNCATE);
			ImGui::OpenPopup("Confirm Effect Rename");
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete Selected Effect")) {
			pendingDeleteEffectName_ = selectedBlockName_;
			ImGui::OpenPopup("Confirm Effect Delete");
		}
	}

	if (!managementMessage_.empty()) {
		ImGui::TextWrapped("%s", managementMessage_.c_str());
	}

	if (ImGui::BeginPopupModal("Confirm Effect Delete", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Delete effect '%s'?", pendingDeleteEffectName_.c_str());
		ImGui::TextWrapped("This removes the emitter, saved data, and effect registry entry.");
		ImGui::Separator();
		if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f))) {
			pendingDeleteEffectName_.clear();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete Permanently", ImVec2(160.0f, 0.0f))) {
			DeleteEffect(pendingDeleteEffectName_);
			pendingDeleteEffectName_.clear();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("Confirm Effect Rename", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Rename effect '%s'", pendingRenameEffectName_.c_str());
		Engine::ImGuiUtility::InputText("New Effect Name", renameEffectNameBuffer_);
		ImGui::TextWrapped("This renames the saved data group and recreates the emitter with the new name.");
		ImGui::Separator();
		if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f))) {
			pendingRenameEffectName_.clear();
			renameEffectNameBuffer_.fill('\0');
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Apply Rename", ImVec2(160.0f, 0.0f))) {
			RenameEffect(pendingRenameEffectName_, renameEffectNameBuffer_.data());
			if (managementMessage_.rfind("Renamed effect:", 0) == 0) {
				pendingRenameEffectName_.clear();
				renameEffectNameBuffer_.fill('\0');
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::EndPopup();
	}

	ImGui::Separator();
}

void EffectEditor::DrawPrimitiveLibraryEditor() {
	ImGui::SeparatorText("Primitive Definition Management");

	// 新しいプリミティブ定義を追加するための入力欄です。
	Engine::ImGuiUtility::InputText("New Primitive Name", newPrimitiveDefinitionNameBuffer_);
	DrawShapeTypeCombo("New Primitive Shape", newPrimitiveDefinitionData_.shapeType);
	ImGui::PushID("NewPrimitiveShape");
	DrawPrimitiveShapeParameters(newPrimitiveDefinitionData_);
	ImGui::PopID();
	if (ImGui::Button("Add Primitive Definition")) {
		AddPrimitiveDefinitionFromEditor();
	}

	if (!primitiveManagementMessage_.empty()) {
		ImGui::TextWrapped("%s", primitiveManagementMessage_.c_str());
	}

	ImGui::SeparatorText("Primitive Definition Detail");
	if (primitiveDefinitionDatas_.empty()) {
		ImGui::TextDisabled("プリミティブ定義を追加してください。");
		return;
	}

	Engine::ImGuiUtility::SelectMapKey("Selected Primitive Definition", selectedPrimitiveDefinitionName_,
		primitiveDefinitionDatas_, "No Primitive Definition");
	if (selectedPrimitiveDefinitionName_.empty() ||
		primitiveDefinitionDatas_.find(selectedPrimitiveDefinitionName_) == primitiveDefinitionDatas_.end()) {
		return;
	}

	// 選択中の定義を編集し、変更があれば即保存する。
	Engine::ParticleGroupEditorData& data = primitiveDefinitionDatas_[selectedPrimitiveDefinitionName_];
	bool saveData = false;
	if (DrawShapeTypeCombo("Shape", data.shapeType)) {
		saveData = true;
	}
	ImGui::PushID("EditPrimitiveShape");
	if (DrawPrimitiveShapeParameters(data)) {
		saveData = true;
	}
	ImGui::PopID();

	if (ImGui::Button("Save Primitive Definition")) {
		saveData = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Primitive Definition")) {
		DeletePrimitiveDefinition(selectedPrimitiveDefinitionName_);
		ClearPrimitivePreview();
		return;
	}

	if (saveData) {
		SavePrimitiveDefinitionData(selectedPrimitiveDefinitionName_, data);
		globalVariables->SaveFile(kPrimitiveDataPrefix + selectedPrimitiveDefinitionName_);
		globalVariables->SaveFile(kPrimitiveRegistryGroup);
		primitiveManagementMessage_ = "Saved primitive definition: " + selectedPrimitiveDefinitionName_;
	}

	DrawPrimitivePreviewControls(selectedPrimitiveDefinitionName_, data);
}

void EffectEditor::DrawPrimitivePreviewControls(const std::string& primitiveName, const Engine::ParticleGroupEditorData& data) {
	ImGui::SeparatorText("Primitive Preview");

	// プレビューは保存対象に含めない内部パーティクルで、編集中の形状を実画面へ確認するためだけに使う。
	if (ImGui::Checkbox("Show Editing Primitive", &isPrimitivePreviewEnabled_)) {
		primitivePreviewTimer_ = 0.0f;
		if (!isPrimitivePreviewEnabled_) {
			ClearPrimitivePreview();
		}
	}
	ImGui::DragFloat3("Preview Position", &primitivePreviewPosition_.x, 0.1f);
	ImGui::DragFloat3("Preview Size", &primitivePreviewSize_.x, 0.1f, 0.01f, 1000.0f);
	ImGui::DragFloat3("Preview Rotate", &primitivePreviewRotate_.x, 0.01f);
	ImGui::ColorEdit4("Preview Color", &primitivePreviewColor_.x);

	if (!isPrimitivePreviewEnabled_) {
		return;
	}

	EnsurePrimitivePreview(primitiveName, data);
	EmitPrimitivePreview();
}

void EffectEditor::EnsurePrimitivePreview(const std::string& primitiveName, const Engine::ParticleGroupEditorData& data) {
	if (effectComponent == nullptr) {
		return;
	}

	Engine::ParticleManager* particleManager = effectComponent->GetParticleManager();
	if (particleManager == nullptr) {
		return;
	}

	const bool needsRecreate =
		previewPrimitiveName_ != primitiveName ||
		previewPrimitiveShapeType_ != data.shapeType ||
		!particleManager->GetParticleGroups().Contains(kPrimitivePreviewParticleName);

	Engine::ParticleGroupEditorData previewData = data;
	previewData.meshSourceType = Engine::ParticleMeshSourceType::Primitive;
	previewData.texturePath = data.texturePath.empty() ? "resources/Texture/Image.dds" : data.texturePath;
	previewData.blendType = EmitData::BlendType::MODE_ADD;
	previewData.rasterizerType = EmitData::RasterizerType::MODE_SOLID_NONE;
	previewData.isEditorPrimitive = true;
	previewData.materialColor = primitivePreviewColor_;
	previewData.materialEnableLighting = false;
	previewData.materialAlpha = primitivePreviewColor_.w;

	if (needsRecreate) {
		// 形状種別が変わった時は、参照メッシュも変わるため一時パーティクル群を作り直す。
		particleManager->RemoveParticleGroup(kPrimitivePreviewParticleName);
		if (!particleManager->CreateEditorParticleGroup(kPrimitivePreviewParticleName, previewData)) {
			ClearPrimitivePreview();
			return;
		}
		previewPrimitiveName_ = primitiveName;
		previewPrimitiveShapeType_ = data.shapeType;
	}
	else {
		// サイズなどの形状パラメータ変更は、既存の一時パーティクル群へ即反映する。
		particleManager->ApplyEditorParticleGroupData(kPrimitivePreviewParticleName, previewData);
	}

	if (!effectComponent->HasEmitter(kPrimitivePreviewEmitterName)) {
		// 点エミッターを1つだけ持ち、プレビュー位置に同じ見た目の粒子を短周期で出す。
		effectComponent->AddEmitter(kPrimitivePreviewEmitterName, kPrimitivePreviewParticleName, EmitterShapeType::POINT);
	}
}

void EffectEditor::EmitPrimitivePreview() {
	Engine::BaseParticleEmitter* emitter = effectComponent->GetBaseEmitter(kPrimitivePreviewEmitterName);
	if (emitter == nullptr) {
		return;
	}

	// 短寿命の粒子を出し直して、カメラや描画順が通常パーティクルと同じプレビューにする。
	primitivePreviewTimer_ += ImGui::GetIO().DeltaTime;
	if (primitivePreviewTimer_ < 0.05f) {
		return;
	}
	primitivePreviewTimer_ = 0.0f;

	emitter->SetPos(primitivePreviewPosition_);
	emitter->SetCount(1, 0);
	emitter->SetFrequency(0.0f);
	emitter->SetLifeTime(0.12f, 0.0f);
	emitter->SetSize(primitivePreviewSize_, {});
	emitter->SetRotate(primitivePreviewRotate_, {});
	emitter->SetVelocity({}, {});
	emitter->SetColorMinMax(primitivePreviewColor_, primitivePreviewColor_);
	emitter->SetAlphaClipping(0.01f);
	emitter->SetEnableLighting(false);
	emitter->SetUsebillboard(false);
	emitter->SetIsAlpha(false);
	emitter->SetIsEmit(true);
	emitter->Emit();
	emitter->SetIsEmit(false);
}

void EffectEditor::ClearPrimitivePreview() {
	previewPrimitiveName_.clear();
	previewPrimitiveShapeType_ = Engine::ShapeParameter::ShapeType::None;
	primitivePreviewTimer_ = 0.0f;
	if (effectComponent == nullptr) {
		return;
	}

	// プレビューは内部用途なので、無効化時にエミッターとパーティクル群をまとめて破棄する。
	effectComponent->RemoveEmitter(kPrimitivePreviewEmitterName);
	if (Engine::ParticleManager* particleManager = effectComponent->GetParticleManager()) {
		particleManager->RemoveParticleGroup(kPrimitivePreviewParticleName);
	}
}

void EffectEditor::ApplyPrimitiveDefinitionToParticleGroup(const std::string& particleName, const std::string& primitiveName) {
	Engine::ParticleManager* particleManager = effectComponent->GetParticleManager();
	auto primitiveIt = primitiveDefinitionDatas_.find(primitiveName);
	if (primitiveIt == primitiveDefinitionDatas_.end()) {
		particleManagementMessage_ = "Primitive definition was not found.";
		return;
	}
	if (!particleManager->GetParticleGroups().Contains(particleName)) {
		particleManagementMessage_ = "Particle group was not found.";
		return;
	}

	// パーティクル群のテクスチャやブレンド設定を残したまま、形状定義だけ差し替える。
	Engine::ParticleGroupEditorData data = particleManager->GetEditorParticleGroupData(particleName);
	CopyPrimitiveShapeData(primitiveIt->second, data);
	particleManager->RecreateEditorParticleGroup(particleName, data);
	particleManager->ApplyEditorParticleGroupData(particleName, data);
	serializer_.RegisterParticleGroupData(particleName, data);
	serializer_.SaveParticleGroupData(particleName, data);
	globalVariables->SaveFile(kParticleDataPrefix + particleName);
	particleManagementMessage_ = "Applied primitive '" + primitiveName + "' to particle '" + particleName + "'.";
}

bool EffectEditor::ValidateNewPrimitiveDefinitionName(const std::string& primitiveName) {
	if (primitiveName.empty()) {
		primitiveManagementMessage_ = "Primitive name is required.";
		return false;
	}
	for (const unsigned char c : primitiveName) {
		if (!std::isalnum(c) && c != '_') {
			primitiveManagementMessage_ = "Use only letters, numbers, and underscores in primitive names.";
			return false;
		}
	}
	if (primitiveDefinitionDatas_.find(primitiveName) != primitiveDefinitionDatas_.end() ||
		globalVariables->HasGroup(kPrimitiveDataPrefix + primitiveName)) {
		primitiveManagementMessage_ = "A primitive definition with that name already exists.";
		return false;
	}
	return true;
}

void EffectEditor::AddPrimitiveDefinitionFromEditor() {
	const std::string primitiveName = newPrimitiveDefinitionNameBuffer_.data();
	if (!ValidateNewPrimitiveDefinitionName(primitiveName)) {
		return;
	}

	// 新規定義は形状だけを保存対象にする。
	primitiveDefinitionDatas_[primitiveName] = newPrimitiveDefinitionData_;
	RegisterPrimitiveDefinitionName(primitiveName);
	SavePrimitiveDefinitionData(primitiveName, primitiveDefinitionDatas_[primitiveName]);
	globalVariables->SaveFile(kPrimitiveDataPrefix + primitiveName);
	globalVariables->SaveFile(kPrimitiveRegistryGroup);

	selectedPrimitiveDefinitionName_ = primitiveName;
	newPrimitiveDefinitionNameBuffer_.fill('\0');
	primitiveManagementMessage_ = "Added primitive definition: " + primitiveName;
}

void EffectEditor::LoadRegisteredPrimitiveDefinitions() {
	if (!globalVariables) {
		return;
	}

	// レジストリグループを作成し、保存済み定義とレジストリ漏れの定義を復元する。
	globalVariables->CreateGroup(kPrimitiveRegistryGroup);
	globalVariables->SetGroupCategory(kPrimitiveRegistryGroup, "Effect/Primitive");

	std::set<std::string> primitiveNames;
	for (const std::string& primitiveName : globalVariables->GetKeys(kPrimitiveRegistryGroup)) {
		primitiveNames.insert(primitiveName);
	}
	for (const std::string& groupName : globalVariables->GetGroupNames()) {
		if (groupName.rfind(kPrimitiveDataPrefix, 0) == 0) {
			primitiveNames.insert(groupName.substr(std::strlen(kPrimitiveDataPrefix)));
		}
	}

	for (const std::string& primitiveName : primitiveNames) {
		const std::string groupName = kPrimitiveDataPrefix + primitiveName;
		if (!globalVariables->HasGroup(groupName)) {
			continue;
		}
		Engine::ParticleGroupEditorData data;
		LoadPrimitiveDefinitionData(primitiveName, data);
		primitiveDefinitionDatas_[primitiveName] = data;
		RegisterPrimitiveDefinitionName(primitiveName);
	}
}

void EffectEditor::RegisterPrimitiveDefinitionName(const std::string& name) {
	globalVariables->SetGroupCategory(kPrimitiveRegistryGroup, "Effect/Primitive");
	globalVariables->CreateGroup(kPrimitiveRegistryGroup);
	globalVariables->AddItem(kPrimitiveRegistryGroup, name, name);
}

void EffectEditor::UnregisterPrimitiveDefinitionName(const std::string& name) {
	globalVariables->RemoveItem(kPrimitiveRegistryGroup, name);
}

void EffectEditor::DeletePrimitiveDefinition(const std::string& name) {
	if (name.empty() || primitiveDefinitionDatas_.find(name) == primitiveDefinitionDatas_.end()) {
		primitiveManagementMessage_ = "The selected primitive definition no longer exists.";
		return;
	}

	primitiveDefinitionDatas_.erase(name);
	UnregisterPrimitiveDefinitionName(name);
	globalVariables->RemoveGroup(kPrimitiveDataPrefix + name);
	globalVariables->RemoveSavedFile(kPrimitiveDataPrefix + name);
	globalVariables->SaveFile(kPrimitiveRegistryGroup);
	selectedPrimitiveDefinitionName_.clear();
	primitiveManagementMessage_ = "Deleted primitive definition: " + name;
}

void EffectEditor::SavePrimitiveDefinitionData(const std::string& name, const Engine::ParticleGroupEditorData& data) {
	const std::string groupName = kPrimitiveDataPrefix + name;
	globalVariables->CreateGroup(groupName);
	globalVariables->SetGroupCategory(groupName, "Effect/Primitive");
	Engine::GlobalVariableWriter writer(globalVariables, Engine::GlobalVariableWriteMode::Save);

	// プリミティブ定義は形状パラメータだけを保存する。
	writer.EnumValue<Engine::ShapeParameter::ShapeType>(groupName, "shapeType", data.shapeType, "ShapeType");
	writer.Value(groupName, "shape.plane.width", data.plane.width);
	writer.Value(groupName, "shape.plane.height", data.plane.height);
	writer.Value(groupName, "shape.triangle.upPos", data.triangle.upPos);
	writer.Value(groupName, "shape.triangle.leftPos", data.triangle.leftPos);
	writer.Value(groupName, "shape.triangle.rightPos", data.triangle.rightPos);
	writer.Value(groupName, "shape.cross.armLength", data.cross.armLength);
	writer.Value(groupName, "shape.cross.armWidth", data.cross.armWidth);
	writer.Value(groupName, "shape.cube.size", data.cube.size);
	writer.Value(groupName, "shape.circle.radius", data.circle.radius);
	writer.Value(groupName, "shape.circle.segments", data.circle.segments);
	writer.Value(groupName, "shape.star.innerRadius", data.star.innerRadius);
	writer.Value(groupName, "shape.star.outerRadius", data.star.outerRadius);
	writer.Value(groupName, "shape.star.segments", data.star.segments);
	writer.Value(groupName, "shape.crescent.innerRadius", data.crescent.innerRadius);
	writer.Value(groupName, "shape.crescent.outerRadius", data.crescent.outerRadius);
	writer.Value(groupName, "shape.crescent.distance", data.crescent.distance);
	writer.Value(groupName, "shape.crescent.segments", data.crescent.segments);
	writer.Value(groupName, "shape.ring.innerRadius", data.ring.innerRadius);
	writer.Value(groupName, "shape.ring.outerRadius", data.ring.outerRadius);
	writer.Value(groupName, "shape.ring.segments", data.ring.segments);
	writer.Value(groupName, "shape.sphere.radius", data.sphere.radius);
	writer.Value(groupName, "shape.sphere.latitudeSegments", data.sphere.latitudeSegments);
	writer.Value(groupName, "shape.sphere.longitudeSegments", data.sphere.longitudeSegments);
	writer.Value(groupName, "shape.sphere.isTopBased", data.sphere.isTopBased);
	writer.Value(groupName, "shape.arrow.shaftLength", data.arrow.shaftLength);
	writer.Value(groupName, "shape.arrow.shaftRadius", data.arrow.shaftRadius);
	writer.Value(groupName, "shape.arrow.headLength", data.arrow.headLength);
	writer.Value(groupName, "shape.arrow.headRadius", data.arrow.headRadius);
	writer.Value(groupName, "shape.arrow.segments", data.arrow.segments);
	writer.Value(groupName, "shape.cylinder.innerRadius", data.cylinder.innerRadius);
	writer.Value(groupName, "shape.cylinder.outerRadius", data.cylinder.outerRadius);
	writer.Value(groupName, "shape.cylinder.height", data.cylinder.height);
	writer.Value(groupName, "shape.cylinder.segments", data.cylinder.segments);
	writer.Value(groupName, "shape.cylinder.isCover", data.cylinder.isCover);
	writer.Value(groupName, "shape.tube.radius", data.tube.radius);
	writer.Value(groupName, "shape.tube.innerRadius", data.tube.innerRadius);
	writer.Value(groupName, "shape.tube.height", data.tube.height);
	writer.Value(groupName, "shape.tube.segments", data.tube.segments);
	writer.Value(groupName, "shape.pyramid.radius", data.pyramid.radius);
	writer.Value(groupName, "shape.pyramid.height", data.pyramid.height);
	writer.Value(groupName, "shape.pyramid.segments", data.pyramid.segments);
	writer.Value(groupName, "shape.torus.innerRadius", data.torus.innerRadius);
	writer.Value(groupName, "shape.torus.outerRadius", data.torus.outerRadius);
	writer.Value(groupName, "shape.torus.segments", data.torus.segments);
	writer.Value(groupName, "shape.torus.tubeSegments", data.torus.tubeSegments);
}

void EffectEditor::LoadPrimitiveDefinitionData(const std::string& name, Engine::ParticleGroupEditorData& data) {
	const std::string groupName = kPrimitiveDataPrefix + name;
	if (globalVariables->HasKey(groupName, "shapeType")) {
		data.shapeType = globalVariables->GetEnumValue<Engine::ShapeParameter::ShapeType>(groupName, "shapeType");
	}
	if (globalVariables->HasKey(groupName, "shape.plane.width")) data.plane.width = globalVariables->GetValue<float>(groupName, "shape.plane.width");
	if (globalVariables->HasKey(groupName, "shape.plane.height")) data.plane.height = globalVariables->GetValue<float>(groupName, "shape.plane.height");
	if (globalVariables->HasKey(groupName, "shape.triangle.upPos")) data.triangle.upPos = globalVariables->GetValue<Vector2>(groupName, "shape.triangle.upPos");
	if (globalVariables->HasKey(groupName, "shape.triangle.leftPos")) data.triangle.leftPos = globalVariables->GetValue<Vector2>(groupName, "shape.triangle.leftPos");
	if (globalVariables->HasKey(groupName, "shape.triangle.rightPos")) data.triangle.rightPos = globalVariables->GetValue<Vector2>(groupName, "shape.triangle.rightPos");
	if (globalVariables->HasKey(groupName, "shape.cross.armLength")) data.cross.armLength = globalVariables->GetValue<float>(groupName, "shape.cross.armLength");
	if (globalVariables->HasKey(groupName, "shape.cross.armWidth")) data.cross.armWidth = globalVariables->GetValue<float>(groupName, "shape.cross.armWidth");
	if (globalVariables->HasKey(groupName, "shape.cube.size")) data.cube.size = globalVariables->GetValue<Vector3>(groupName, "shape.cube.size");
	if (globalVariables->HasKey(groupName, "shape.circle.radius")) data.circle.radius = globalVariables->GetValue<float>(groupName, "shape.circle.radius");
	if (globalVariables->HasKey(groupName, "shape.circle.segments")) data.circle.segments = globalVariables->GetValue<int32_t>(groupName, "shape.circle.segments");
	if (globalVariables->HasKey(groupName, "shape.star.innerRadius")) data.star.innerRadius = globalVariables->GetValue<float>(groupName, "shape.star.innerRadius");
	if (globalVariables->HasKey(groupName, "shape.star.outerRadius")) data.star.outerRadius = globalVariables->GetValue<float>(groupName, "shape.star.outerRadius");
	if (globalVariables->HasKey(groupName, "shape.star.segments")) data.star.segments = globalVariables->GetValue<int32_t>(groupName, "shape.star.segments");
	if (globalVariables->HasKey(groupName, "shape.crescent.innerRadius")) data.crescent.innerRadius = globalVariables->GetValue<float>(groupName, "shape.crescent.innerRadius");
	if (globalVariables->HasKey(groupName, "shape.crescent.outerRadius")) data.crescent.outerRadius = globalVariables->GetValue<float>(groupName, "shape.crescent.outerRadius");
	if (globalVariables->HasKey(groupName, "shape.crescent.distance")) data.crescent.distance = globalVariables->GetValue<float>(groupName, "shape.crescent.distance");
	if (globalVariables->HasKey(groupName, "shape.crescent.segments")) data.crescent.segments = globalVariables->GetValue<int32_t>(groupName, "shape.crescent.segments");
	if (globalVariables->HasKey(groupName, "shape.ring.innerRadius")) data.ring.innerRadius = globalVariables->GetValue<float>(groupName, "shape.ring.innerRadius");
	if (globalVariables->HasKey(groupName, "shape.ring.outerRadius")) data.ring.outerRadius = globalVariables->GetValue<float>(groupName, "shape.ring.outerRadius");
	if (globalVariables->HasKey(groupName, "shape.ring.segments")) data.ring.segments = globalVariables->GetValue<int32_t>(groupName, "shape.ring.segments");
	if (globalVariables->HasKey(groupName, "shape.sphere.radius")) data.sphere.radius = globalVariables->GetValue<float>(groupName, "shape.sphere.radius");
	if (globalVariables->HasKey(groupName, "shape.sphere.latitudeSegments")) data.sphere.latitudeSegments = globalVariables->GetValue<int32_t>(groupName, "shape.sphere.latitudeSegments");
	if (globalVariables->HasKey(groupName, "shape.sphere.longitudeSegments")) data.sphere.longitudeSegments = globalVariables->GetValue<int32_t>(groupName, "shape.sphere.longitudeSegments");
	if (globalVariables->HasKey(groupName, "shape.sphere.isTopBased")) data.sphere.isTopBased = globalVariables->GetValue<bool>(groupName, "shape.sphere.isTopBased");
	if (globalVariables->HasKey(groupName, "shape.arrow.shaftLength")) data.arrow.shaftLength = globalVariables->GetValue<float>(groupName, "shape.arrow.shaftLength");
	if (globalVariables->HasKey(groupName, "shape.arrow.shaftRadius")) data.arrow.shaftRadius = globalVariables->GetValue<float>(groupName, "shape.arrow.shaftRadius");
	if (globalVariables->HasKey(groupName, "shape.arrow.headLength")) data.arrow.headLength = globalVariables->GetValue<float>(groupName, "shape.arrow.headLength");
	if (globalVariables->HasKey(groupName, "shape.arrow.headRadius")) data.arrow.headRadius = globalVariables->GetValue<float>(groupName, "shape.arrow.headRadius");
	if (globalVariables->HasKey(groupName, "shape.arrow.segments")) data.arrow.segments = globalVariables->GetValue<int32_t>(groupName, "shape.arrow.segments");
	if (globalVariables->HasKey(groupName, "shape.cylinder.innerRadius")) data.cylinder.innerRadius = globalVariables->GetValue<float>(groupName, "shape.cylinder.innerRadius");
	if (globalVariables->HasKey(groupName, "shape.cylinder.outerRadius")) data.cylinder.outerRadius = globalVariables->GetValue<float>(groupName, "shape.cylinder.outerRadius");
	if (globalVariables->HasKey(groupName, "shape.cylinder.height")) data.cylinder.height = globalVariables->GetValue<float>(groupName, "shape.cylinder.height");
	if (globalVariables->HasKey(groupName, "shape.cylinder.segments")) data.cylinder.segments = globalVariables->GetValue<int32_t>(groupName, "shape.cylinder.segments");
	if (globalVariables->HasKey(groupName, "shape.cylinder.isCover")) data.cylinder.isCover = globalVariables->GetValue<bool>(groupName, "shape.cylinder.isCover");
	if (globalVariables->HasKey(groupName, "shape.tube.radius")) data.tube.radius = globalVariables->GetValue<float>(groupName, "shape.tube.radius");
	if (globalVariables->HasKey(groupName, "shape.tube.innerRadius")) data.tube.innerRadius = globalVariables->GetValue<float>(groupName, "shape.tube.innerRadius");
	if (globalVariables->HasKey(groupName, "shape.tube.height")) data.tube.height = globalVariables->GetValue<float>(groupName, "shape.tube.height");
	if (globalVariables->HasKey(groupName, "shape.tube.segments")) data.tube.segments = globalVariables->GetValue<int32_t>(groupName, "shape.tube.segments");
	if (globalVariables->HasKey(groupName, "shape.pyramid.radius")) data.pyramid.radius = globalVariables->GetValue<float>(groupName, "shape.pyramid.radius");
	if (globalVariables->HasKey(groupName, "shape.pyramid.height")) data.pyramid.height = globalVariables->GetValue<float>(groupName, "shape.pyramid.height");
	if (globalVariables->HasKey(groupName, "shape.pyramid.segments")) data.pyramid.segments = globalVariables->GetValue<int32_t>(groupName, "shape.pyramid.segments");
	if (globalVariables->HasKey(groupName, "shape.torus.innerRadius")) data.torus.innerRadius = globalVariables->GetValue<float>(groupName, "shape.torus.innerRadius");
	if (globalVariables->HasKey(groupName, "shape.torus.outerRadius")) data.torus.outerRadius = globalVariables->GetValue<float>(groupName, "shape.torus.outerRadius");
	if (globalVariables->HasKey(groupName, "shape.torus.segments")) data.torus.segments = globalVariables->GetValue<int32_t>(groupName, "shape.torus.segments");
	if (globalVariables->HasKey(groupName, "shape.torus.tubeSegments")) data.torus.tubeSegments = globalVariables->GetValue<int32_t>(groupName, "shape.torus.tubeSegments");
}

void EffectEditor::SaveAllEditorData() {
	// エフェクト、パーティクル群、プリミティブ定義、各レジストリをまとめて保存する。
	for (auto& [effectName, effectData] : effectGlobalDatas_) {
		serializer_.SaveEffectData(effectName, effectData);
		globalVariables->SaveFile(effectName);
	}

	Engine::ParticleManager* particleManager = effectComponent->GetParticleManager();
	std::set<std::string> particleNames;
	for (const auto& particle : particleManager->GetParticleGroups()) {
		particleNames.insert(particle.first);
	}
	for (const std::string& particleName : globalVariables->GetKeys(kParticleRegistryGroup)) {
		particleNames.insert(particleName);
	}
	for (const std::string& groupName : globalVariables->GetGroupNames()) {
		if (groupName.rfind(kParticleDataPrefix, 0) == 0) {
			particleNames.insert(groupName.substr(std::strlen(kParticleDataPrefix)));
		}
	}
	for (const std::string& particleName : particleNames) {
		if (particleName == kPrimitivePreviewParticleName || IsDeletedParticleGroupName(particleName)) {
			// プレビュー用・削除済みのパーティクル群は保存対象から除外する。
			continue;
		}
		const std::string dataGroupName = kParticleDataPrefix + particleName;
		if (!globalVariables->HasGroup(dataGroupName)) {
			continue;
		}

		Engine::ParticleGroupEditorData particleData;
		if (particleManager->GetParticleGroups().Contains(particleName)) {
			particleData = particleManager->GetEditorParticleGroupData(particleName);
		}
		else {
			// 実体を作れない状態でも、ファイルにあるMeshSourceTypeやモデル名をそのまま保存する。
			serializer_.LoadParticleGroupData(particleName, particleData);
		}
		serializer_.RegisterParticleGroupData(particleName, particleData);
		serializer_.SaveParticleGroupData(particleName, particleData);
		globalVariables->SaveFile(dataGroupName);
	}

	for (auto& [primitiveName, primitiveData] : primitiveDefinitionDatas_) {
		SavePrimitiveDefinitionData(primitiveName, primitiveData);
		globalVariables->SaveFile(kPrimitiveDataPrefix + primitiveName);
	}

	globalVariables->SaveFile(kEffectRegistryGroup);
	globalVariables->SaveFile(kDeletedEffectRegistryGroup);
	globalVariables->SaveFile(kParticleRegistryGroup);
	globalVariables->SaveFile(kDeletedParticleRegistryGroup);
	globalVariables->SaveFile(kPrimitiveRegistryGroup);
	SaveEditorUiSettings();
	managementMessage_ = "Saved all effect editor settings.";
	particleManagementMessage_ = managementMessage_;
	primitiveManagementMessage_ = managementMessage_;
}

void EffectEditor::SaveEmitterData(const std::string& name, const EffectGlobalData& data) {
	// 指定エミッタだけをGlobalVariablesへ反映し、該当ファイルを保存する。
	if (name.empty()) {
		emitterSettingsMessage_ = "Emitter name is required.";
		return;
	}

	globalVariables->SetGroupCategory(name, "Effect");
	serializer_.RegisterEffectData(name, data);
	serializer_.SaveEffectData(name, data);
	globalVariables->SaveFile(name);
	globalVariables->SaveFile(kEffectRegistryGroup);
	emitterSettingsMessage_ = "Saved emitter: " + name;
}

void EffectEditor::SaveParticleGroupData(const std::string& particleName, const Engine::ParticleGroupEditorData& data) {
	// 指定パーティクル群だけをGlobalVariablesへ反映し、該当ファイルを保存する。
	if (particleName.empty()) {
		particleManagementMessage_ = "Particle group name is required.";
		return;
	}

	serializer_.RegisterParticleGroupData(particleName, data);
	serializer_.SaveParticleGroupData(particleName, data);
	globalVariables->SaveFile(kParticleDataPrefix + particleName);
	globalVariables->SaveFile(kParticleRegistryGroup);
	particleManagementMessage_ = "Saved particle group: " + particleName;
}

void EffectEditor::SaveEditorUiSettings() {
	// データ本体ではなく、Effectエディタで最後に選んだUI状態だけを保存する。
	globalVariables->CreateGroup(kEffectEditorSettingsGroup);
	globalVariables->SetGroupCategory(kEffectEditorSettingsGroup, "Effect");
	Engine::GlobalVariableWriter writer(globalVariables, Engine::GlobalVariableWriteMode::Save);
	writer.Value(kEffectEditorSettingsGroup, "selectedApplyPrimitiveName", selectedApplyPrimitiveName_);
	globalVariables->SaveFile(kEffectEditorSettingsGroup);
}

void EffectEditor::LoadEditorUiSettings() {
	// 起動時にプリミティブ適用コンボの最後の選択状態を復元する。
	globalVariables->CreateGroup(kEffectEditorSettingsGroup);
	globalVariables->SetGroupCategory(kEffectEditorSettingsGroup, "Effect");
	Engine::GlobalVariableWriter writer(globalVariables, Engine::GlobalVariableWriteMode::Register);
	writer.Value(kEffectEditorSettingsGroup, "selectedApplyPrimitiveName", selectedApplyPrimitiveName_);

	if (globalVariables->HasKey(kEffectEditorSettingsGroup, "selectedApplyPrimitiveName")) {
		selectedApplyPrimitiveName_ = globalVariables->GetValue<std::string>(kEffectEditorSettingsGroup, "selectedApplyPrimitiveName");
	}
}

void EffectEditor::DrawParticleGroupEditor() {
	Engine::ParticleManager* particleManager = effectComponent->GetParticleManager();

	if (!ImGui::CollapsingHeader("Particle Groups", ImGuiTreeNodeFlags_DefaultOpen)) {
		return;
	}

	ImGui::SeparatorText("Particle Group Management");
	Engine::ImGuiUtility::InputText("New Particle Group Name", newParticleGroupNameBuffer_);
	DrawParticleTextureSelector("New Texture", "New Texture Path##newParticleTexture", newParticleTexturePathBuffer_, particleManager->GetTextureManager());

	DrawParticleMeshSourceCombo("New Mesh Source", newParticleGroupData_.meshSourceType);
	if (newParticleGroupData_.meshSourceType == Engine::ParticleMeshSourceType::Model) {
		DrawModelNameCombo("New Model", newParticleGroupData_.modelName, effectComponent->GetModelManager());
	}
	else {
		DrawShapeTypeCombo("New Shape", newParticleGroupData_.shapeType);
	}

	static const char* RasterizerLabels[] = {
		"Solid Back",
		"Solid None",
	};
	Engine::ImGuiUtility::SelectEnum("New Rasterizer", RasterizerLabels, newParticleGroupData_.rasterizerType);

	static const char* BlendLabels[] = {
		"Add",
		"Subtract",
		"Multiply",
	};
	Engine::ImGuiUtility::SelectEnum("New Blend", BlendLabels, newParticleGroupData_.blendType);

	if (ImGui::Button("Add Particle Group")) {
		AddParticleGroupFromEditor();
	}

	if (!particleManagementMessage_.empty()) {
		ImGui::TextWrapped("%s", particleManagementMessage_.c_str());
	}

	ImGui::SeparatorText("Particle Group");

	if (Engine::ImGuiUtility::SelectMapKey("Selected Particle Group", selectedParticleGroupName_,
		particleManager->GetParticleGroups(), "No Particle Group")) {
		editParticleTexturePathBuffer_.fill('\0');
	}

	if (!selectedParticleGroupName_.empty() &&
		particleManager->GetParticleGroups().Contains(selectedParticleGroupName_)) {
		ImGui::Text("Selected: %s", selectedParticleGroupName_.c_str());
		if (ImGui::Button("Rename Selected Particle Group")) {
			pendingRenameParticleGroupName_ = selectedParticleGroupName_;
			Engine::ImGuiUtility::SetInputTextBuffer(renameParticleGroupNameBuffer_, selectedParticleGroupName_);
			ImGui::OpenPopup("Confirm Particle Group Rename");
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete Selected Particle Group")) {
			pendingDeleteParticleGroupName_ = selectedParticleGroupName_;
			ImGui::OpenPopup("Confirm Particle Group Delete");
		}

		if (Engine::ImGuiUtility::ConfirmModal("Confirm Particle Group Delete",
			"Delete this particle group? Effects that use it will be cleared.",
			"Delete Permanently")) {
			DeleteParticleGroup(pendingDeleteParticleGroupName_);
			pendingDeleteParticleGroupName_.clear();
		}

		if (ImGui::BeginPopupModal("Confirm Particle Group Rename", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Rename particle group '%s'", pendingRenameParticleGroupName_.c_str());
			Engine::ImGuiUtility::InputText("New Particle Group Name", renameParticleGroupNameBuffer_);
			ImGui::TextWrapped("This updates effects that reference this particle group.");
			ImGui::Separator();
			if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f))) {
				pendingRenameParticleGroupName_.clear();
				renameParticleGroupNameBuffer_.fill('\0');
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Apply Rename", ImVec2(160.0f, 0.0f))) {
				RenameParticleGroup(pendingRenameParticleGroupName_, renameParticleGroupNameBuffer_.data());
				if (particleManagementMessage_.rfind("Renamed particle group:", 0) == 0) {
					pendingRenameParticleGroupName_.clear();
					renameParticleGroupNameBuffer_.fill('\0');
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}

		DrawParticleGroupDetail(selectedParticleGroupName_, particleManager->GetParticleGroups(selectedParticleGroupName_));
	}

	ImGui::Separator();
}

void EffectEditor::DrawParticleGroupPreviewControls(const std::string& particleName) {
	ImGui::SeparatorText("Particle Group Preview");

	// エフェクト定義を作らず、選択中のパーティクル群だけを一時エミッターで発生確認する。
	if (ImGui::Checkbox("出現##ParticleGroupPreview", &isParticleGroupPreviewEnabled_)) {
		particleGroupPreviewTimer_ = 0.0f;
		if (!isParticleGroupPreviewEnabled_) {
			ClearParticleGroupPreview();
		}
	}
	ImGui::DragFloat3("位置##ParticleGroupPreview", &particleGroupPreviewPosition_.x, 0.1f);
	ImGui::DragFloat("頻度##ParticleGroupPreview", &particleGroupPreviewFrequency_, 0.01f, 0.01f, 1000.0f);
	ImGui::DragInt("出現量##ParticleGroupPreview", &particleGroupPreviewCount_, 1, 1, 1024);
	ImGui::DragFloat("生存時間##ParticleGroupPreview", &particleGroupPreviewLifeTime_, 0.01f, 0.01f, 1000.0f);
	ImGui::DragFloat3("サイズ##ParticleGroupPreview", &particleGroupPreviewSize_.x, 0.1f, 0.01f, 1000.0f);
	ImGui::DragFloat3("回転##ParticleGroupPreview", &particleGroupPreviewRotate_.x, 0.01f);
	ImGui::ColorEdit4("色##ParticleGroupPreview", &particleGroupPreviewColor_.x);

	if (!isParticleGroupPreviewEnabled_) {
		return;
	}

	EnsureParticleGroupPreviewEmitter(particleName);
	particleGroupPreviewTimer_ += ImGui::GetIO().DeltaTime;
	if (particleGroupPreviewFrequency_ <= particleGroupPreviewTimer_) {
		EmitParticleGroupPreview(particleName);
		particleGroupPreviewTimer_ = 0.0f;
	}
}

void EffectEditor::EnsureParticleGroupPreviewEmitter(const std::string& particleName) {
	if (effectComponent == nullptr || particleName.empty()) {
		return;
	}
	Engine::ParticleManager* particleManager = effectComponent->GetParticleManager();
	if (particleManager == nullptr || !particleManager->GetParticleGroups().Contains(particleName)) {
		ClearParticleGroupPreview();
		return;
	}

	if (!effectComponent->HasEmitter(kParticleGroupPreviewEmitterName)) {
		// 選択中パーティクル群の見た目確認だけに使う内部Pointエミッターを作る。
		effectComponent->AddEmitter(kParticleGroupPreviewEmitterName, particleName, EmitterShapeType::POINT);
	}

	if (Engine::BaseParticleEmitter* emitter = effectComponent->GetBaseEmitter(kParticleGroupPreviewEmitterName)) {
		// Particle Groupsの選択が変わったら、同じ一時エミッターの参照先だけ差し替える。
		emitter->SetParticleName(particleName);
		emitter->SetIsEmit(false);
		previewParticleGroupName_ = particleName;
	}
}

void EffectEditor::EmitParticleGroupPreview(const std::string& particleName) {
	EnsureParticleGroupPreviewEmitter(particleName);
	Engine::BaseParticleEmitter* emitter = effectComponent ? effectComponent->GetBaseEmitter(kParticleGroupPreviewEmitterName) : nullptr;
	if (emitter == nullptr) {
		return;
	}

	Engine::ParticleGroupEditorData data = effectComponent->GetParticleManager()->GetEditorParticleGroupData(particleName);

	// Emitter Settingsの発生確認と同じく、現在の確認用パラメータで手動発生させる。
	// 挙動フラグはパーティクル群側の状態を反映し、見た目確認が保存済み設定からずれないようにする。
	emitter->SetPos(particleGroupPreviewPosition_);
	emitter->SetCount(particleGroupPreviewCount_, 0);
	emitter->SetFrequency(0.0f);
	emitter->SetLifeTime(particleGroupPreviewLifeTime_, 0.0f);
	emitter->SetSize(particleGroupPreviewSize_, {});
	emitter->SetRotate(particleGroupPreviewRotate_, {});
	emitter->SetVelocity({}, {});
	emitter->SetColorMinMax(particleGroupPreviewColor_, particleGroupPreviewColor_);
	emitter->SetIsFlag(data.isFlag);
	emitter->SetLifeTimeScaleTopBottom(data.topBottom);
	emitter->SetUvTransformVeloctiy(data.uvTransformVelocity);
	emitter->SetAlphaClipping(data.materialAlphaClipping);
	emitter->SetEnableLighting(data.materialEnableLighting);
	emitter->SetIsEmit(true);
	emitter->Emit();
	emitter->SetIsEmit(false);
}

void EffectEditor::ClearParticleGroupPreview() {
	previewParticleGroupName_.clear();
	particleGroupPreviewTimer_ = 0.0f;
	if (effectComponent == nullptr) {
		return;
	}

	// 内部確認用エミッターは保存対象ではないため、不要になったら即破棄する。
	effectComponent->RemoveEmitter(kParticleGroupPreviewEmitterName);
}

void EffectEditor::DrawParticleGroupDetail(const std::string& particleName, Engine::ParticleGroup& group) {
	Engine::ParticleManager* particleManager = effectComponent->GetParticleManager();
	Engine::ParticleGroupEditorData data = particleManager->GetEditorParticleGroupData(particleName);
	if (editParticleTexturePathBuffer_[0] == '\0') {
		Engine::ImGuiUtility::SetInputTextBuffer(editParticleTexturePathBuffer_, data.texturePath);
	}

	DrawParticleGroupPreviewControls(particleName);

	ImGui::SeparatorText("Particle Group Detail");
	bool recreate = false;
	bool saveData = false;
	Engine::TextureManager* textureManager = particleManager->GetTextureManager();
	const std::string resolvedTexturePath = ResolveParticleTexturePath(textureManager, data.texturePath);
	if (resolvedTexturePath != data.texturePath) {
		// 保存済みの無効パスも、画面を開いた時点で安全な仮テクスチャへ切り替える。
		data.texturePath = resolvedTexturePath;
		Engine::ImGuiUtility::SetInputTextBuffer(editParticleTexturePathBuffer_, data.texturePath);
		saveData = true;
		particleManagementMessage_ = "Texture path not found. Preview texture applied: " + data.texturePath;
	}
	if (!particleManagementMessage_.empty()) {
		ImGui::TextWrapped("%s", particleManagementMessage_.c_str());
	}

	if (DrawParticleTextureSelector("Texture", "Texture Path##editParticleTexture", editParticleTexturePathBuffer_, particleManager->GetTextureManager())) {
		data.texturePath = editParticleTexturePathBuffer_.data();
		saveData = true;
	}
	if (ImGui::Button("Apply Texture")) {
		const std::string requestedTexturePath = editParticleTexturePathBuffer_.data();
		data.texturePath = ResolveParticleTexturePath(particleManager->GetTextureManager(), requestedTexturePath);
		Engine::ImGuiUtility::SetInputTextBuffer(editParticleTexturePathBuffer_, data.texturePath);
		if (requestedTexturePath != data.texturePath) {
			particleManagementMessage_ = "Texture path not found. Preview texture applied: " + data.texturePath;
		}
		if (group.material) {
			group.material->tex_.diffuseFilePath = data.texturePath;
			group.material->LoadTex();
		}
		saveData = true;
	}
	Engine::ImGuiUtility::HelpMarker("Texture Path is applied immediately with Apply Texture. Shape changes recreate the group.");

	if (DrawParticleMeshSourceCombo("Mesh Source", data.meshSourceType)) {
		recreate = true;
	}

	if (data.meshSourceType == Engine::ParticleMeshSourceType::Model) {
		if (DrawModelNameCombo("Model", data.modelName, effectComponent->GetModelManager())) {
			recreate = true;
		}
	}
	else {
		if (!primitiveDefinitionDatas_.empty()) {
			ImGui::SeparatorText("Apply Primitive Definition");
			if (Engine::ImGuiUtility::SelectMapKey("Primitive Definition", selectedApplyPrimitiveName_,
				primitiveDefinitionDatas_, "No Primitive Definition")) {
				SaveEditorUiSettings();
			}
			if (ImGui::Button("Apply Primitive To Particle") && !selectedApplyPrimitiveName_.empty()) {
				ApplyPrimitiveDefinitionToParticleGroup(particleName, selectedApplyPrimitiveName_);
				data = particleManager->GetEditorParticleGroupData(particleName);
				recreate = false;
				saveData = false;
			}
		}

		if (DrawShapeTypeCombo("Shape", data.shapeType)) {
			recreate = true;
		}
		if (DrawPrimitiveShapeParameters(data)) {
			saveData = true;
		}
	}

	static const char* RasterizerLabels[] = {
		"Solid Back",
		"Solid None",
	};
	if (Engine::ImGuiUtility::SelectEnum("Rasterizer", RasterizerLabels, data.rasterizerType)) {
		saveData = true;
	}

	static const char* BlendLabels[] = {
		"Add",
		"Subtract",
		"Multiply",
	};
	if (Engine::ImGuiUtility::SelectEnum("Blend", BlendLabels, data.blendType)) {
		saveData = true;
	}

	if (ImGui::Checkbox("UV Clamp", &data.isUVClamp)) {
		saveData = true;
	}
	if (ImGui::DragFloat3("UV Translate Velocity", &data.uvTransformVelocity.translate.x, 0.001f)) {
		saveData = true;
	}
	if (ImGui::DragFloat3("UV Rotate Velocity", &data.uvTransformVelocity.rotate.x, 0.001f)) {
		saveData = true;
	}
	if (ImGui::DragFloat3("UV Scale Velocity", &data.uvTransformVelocity.scale.x, 0.001f)) {
		saveData = true;
	}
	if (ImGui::DragFloat3("UV Translate", &data.uvTransform.translate.x, 0.001f)) {
		data.materialTransform = data.uvTransform;
		saveData = true;
	}
	if (ImGui::DragFloat3("UV Rotate", &data.uvTransform.rotate.x, 0.001f)) {
		data.materialTransform = data.uvTransform;
		saveData = true;
	}
	if (ImGui::DragFloat3("UV Scale", &data.uvTransform.scale.x, 0.001f)) {
		data.materialTransform = data.uvTransform;
		saveData = true;
	}
	if (ImGui::Checkbox("Billboard", &data.isFlag.usebillboard)) {
		saveData = true;
	}
	if (ImGui::Checkbox("Billboard Y", &data.isFlag.usebillboardY)) {
		saveData = true;
	}
	if (ImGui::Checkbox("Billboard Z Rotation", &data.isFlag.billboardRotZ)) {
		saveData = true;
	}
	if (ImGui::Checkbox("ライン描画##ParticleGroup", &data.isFlag.isLine)) {
		// パーティクル群単体プレビューでもエミッター形状と発生位置ラインを確認できるようにする。
		saveData = true;
	}
	if (ImGui::Checkbox("Alpha Over Life", &data.isFlag.isAlpha)) {
		saveData = true;
	}
	if (ImGui::Checkbox("Gravity", &data.isFlag.isGravity)) {
		saveData = true;
	}
	if (ImGui::Checkbox("Bounce", &data.isFlag.isBounce)) {
		saveData = true;
	}
	if (ImGui::Checkbox("Life Time Scale", &data.isFlag.isLifeTimeScale_)) {
		saveData = true;
	}
	if (ImGui::Checkbox("Scaling", &data.isFlag.isScaling_)) {
		saveData = true;
	}
	if (ImGui::Checkbox("Rotate Velocity", &data.isFlag.isRotateVelocity)) {
		saveData = true;
	}
	if (ImGui::Checkbox("Acceleration", &data.isFlag.isAcceleration)) {
		saveData = true;
	}
	if (ImGui::DragFloat("Gravity Acceleration", &data.gravitationalAcceleration, 0.01f)) {
		saveData = true;
	}

	if (group.material) {
		if (ImGui::CollapsingHeader("Material")) {
			if (ImGui::ColorEdit4("Color", &data.materialColor.x)) {
				saveData = true;
			}
			if (ImGui::DragFloat("Alpha Clipping", &data.materialAlphaClipping, 0.01f, 0.0f, 1.0f)) {
				saveData = true;
			}
			if (ImGui::DragFloat("Alpha", &data.materialAlpha, 0.01f, 0.0f, 1.0f)) {
				saveData = true;
			}
			if (ImGui::Checkbox("Enable Lighting", &data.materialEnableLighting)) {
				saveData = true;
			}
			if (ImGui::DragFloat("Environment Coefficient", &data.materialEnvironmentCoefficient, 0.01f, 0.0f, 10.0f)) {
				saveData = true;
			}
			if (ImGui::DragFloat("Shininess", &data.materialShininess, 0.1f, 0.0f, 512.0f)) {
				saveData = true;
			}
			if (ImGui::Checkbox("Use Environment", &data.materialUseEnvironment)) {
				saveData = true;
			}
			if (ImGui::Checkbox("Use Light", &data.materialUseLig)) {
				saveData = true;
			}
			if (ImGui::Checkbox("Use Normal Map", &data.materialUseNormalMap)) {
				saveData = true;
			}
			if (ImGui::Checkbox("Use Specular Map", &data.materialUseSpecularMap)) {
				saveData = true;
			}
		}
	}

	if (recreate) {
		data.texturePath = ResolveParticleTexturePath(textureManager, editParticleTexturePathBuffer_.data());
		Engine::ImGuiUtility::SetInputTextBuffer(editParticleTexturePathBuffer_, data.texturePath);
		if (!particleManager->RecreateEditorParticleGroup(particleName, data)) {
			particleManagementMessage_ = "Could not recreate particle group. Check the mesh source settings.";
			return;
		}
		selectedParticleGroupName_ = particleName;
		Engine::ImGuiUtility::SetInputTextBuffer(editParticleTexturePathBuffer_, data.texturePath);
		saveData = true;
	}

	if (ImGui::Button("Save This Particle Group")) {
		// すべての編集UIを通った後のdataを使い、選択中パーティクル群だけを明示的に保存する。
		saveData = true;
	}

	if (saveData) {
		// UIで編集したMeshSourceやModel名をそのまま保存データとして保持する。
		data.texturePath = ResolveParticleTexturePath(textureManager, editParticleTexturePathBuffer_.data());
		Engine::ImGuiUtility::SetInputTextBuffer(editParticleTexturePathBuffer_, data.texturePath);
		particleManager->ApplyEditorParticleGroupData(particleName, data);
		particleManager->SetEditorParticleGroupData(particleName, data);
		SaveParticleGroupData(particleName, data);
	}
}

bool EffectEditor::ValidateNewEffectName(const std::string& effectName) {
	if (effectName.empty()) {
		managementMessage_ = "Effect name is required.";
		return false;
	}
	for (const unsigned char c : effectName) {
		if (!std::isalnum(c) && c != '_') {
			managementMessage_ = "Use only letters, numbers, and underscores in effect names.";
			return false;
		}
	}
	if (effectGlobalDatas_.find(effectName) != effectGlobalDatas_.end() ||
		globalVariables->HasGroup(effectName)) {
		managementMessage_ = "An effect or saved data with that name already exists.";
		return false;
	}
	return true;
}

void EffectEditor::AddEffectFromEditor() {
	const std::string effectName = newEffectNameBuffer_.data();
	if (!ValidateNewEffectName(effectName)) {
		return;
	}
	if (newEffectParticleName_.empty()) {
		managementMessage_ = "Particle name is required.";
		return;
	}

	// 同名で過去に削除していた場合は、再追加できるよう削除済みリストから外す。
	UnregisterDeletedEffectName(effectName);
	EffectGlobalData data;
	data.particleName = newEffectParticleName_;
	globalVariables->SetGroupCategory(effectName, "Effect");
	serializer_.RegisterEffectData(effectName, data);
	serializer_.LoadEffectData(effectName, data);
	effectGlobalDatas_[effectName] = data;
	RegisterEffectName(effectName);
	effectComponent->AddEmitter(effectName, data.particleName, data.shapeType);
	SetEffectGlobalData(effectName, data.shapeType, data);

	selectedBlockName_ = effectName;
	newEffectNameBuffer_.fill('\0');
	globalVariables->SaveFile(effectName);
	globalVariables->SaveFile(kEffectRegistryGroup);
	globalVariables->SaveFile(kDeletedEffectRegistryGroup);
	managementMessage_ = "Added effect: " + effectName;
}

void EffectEditor::RenameEffect(const std::string& oldName, const std::string& newName) {
	if (oldName.empty() || effectGlobalDatas_.find(oldName) == effectGlobalDatas_.end()) {
		managementMessage_ = "The selected effect no longer exists.";
		return;
	}
	if (!ValidateNewEffectName(newName)) {
		return;
	}

	serializer_.SaveEffectData(oldName, effectGlobalDatas_[oldName]);
	if (!globalVariables->RenameGroup(oldName, newName)) {
		managementMessage_ = "Could not rename the saved effect data.";
		return;
	}

	EffectGlobalData data = effectGlobalDatas_[oldName];
	effectGlobalDatas_.erase(oldName);
	effectGlobalDatas_[newName] = data;

	// エミッター名はmapキーなので、名前変更時は同じデータで作り直す。
	effectComponent->RemoveEmitter(oldName);
	effectComponent->AddEmitter(newName, data.particleName, data.shapeType);
	SetEffectGlobalData(newName, data.shapeType, data);

	UnregisterEffectName(oldName);
	RegisterEffectName(newName);
	RegisterDeletedEffectName(oldName);
	UnregisterDeletedEffectName(newName);

	globalVariables->SaveFile(newName);
	globalVariables->SaveFile(kEffectRegistryGroup);
	globalVariables->SaveFile(kDeletedEffectRegistryGroup);
	globalVariables->RemoveSavedFile(oldName);
	selectedBlockName_ = newName;
	managementMessage_ = "Renamed effect: " + oldName + " -> " + newName;
}

void EffectEditor::DeleteEffect(const std::string& effectName) {
	if (effectName.empty() || effectGlobalDatas_.find(effectName) == effectGlobalDatas_.end()) {
		managementMessage_ = "The selected effect no longer exists.";
		return;
	}

	effectComponent->RemoveEmitter(effectName);
	effectGlobalDatas_.erase(effectName);
	UnregisterEffectName(effectName);
	RegisterDeletedEffectName(effectName);
	globalVariables->RemoveGroup(effectName);
	const bool removedFile = globalVariables->RemoveSavedFile(effectName);
	globalVariables->SaveFile(kEffectRegistryGroup);
	globalVariables->SaveFile(kDeletedEffectRegistryGroup);

	if (!effectGlobalDatas_.empty()) {
		selectedBlockName_ = effectGlobalDatas_.begin()->first;
	}
	else {
		selectedBlockName_.clear();
		isSpawnEmit = false;
	}

	managementMessage_ = removedFile
		? "Deleted effect: " + effectName
		: "Removed effect from the editor, but its JSON file could not be deleted: " + effectName;
}

void EffectEditor::RegisterEffectName(const std::string& name) {
	// レジストリはキー名だけを一覧として使うため、値にも同じ名前を保存する。
	globalVariables->SetGroupCategory(kEffectRegistryGroup, "Effect");
	globalVariables->CreateGroup(kEffectRegistryGroup);
	globalVariables->AddItem(kEffectRegistryGroup, name, name);
}

void EffectEditor::UnregisterEffectName(const std::string& name) {
	globalVariables->RemoveItem(kEffectRegistryGroup, name);
}

void EffectEditor::RegisterDeletedEffectName(const std::string& name) {
	globalVariables->SetGroupCategory(kDeletedEffectRegistryGroup, "Effect");
	globalVariables->CreateGroup(kDeletedEffectRegistryGroup);
	globalVariables->AddItem(kDeletedEffectRegistryGroup, name, name);
}

void EffectEditor::UnregisterDeletedEffectName(const std::string& name) {
	globalVariables->RemoveItem(kDeletedEffectRegistryGroup, name);
}

bool EffectEditor::IsDeletedEffectName(const std::string& name) const {
	return globalVariables && globalVariables->HasKey(kDeletedEffectRegistryGroup, name);
}

bool EffectEditor::ValidateNewParticleGroupName(const std::string& particleName) {
	if (particleName.empty()) {
		particleManagementMessage_ = "Particle group name is required.";
		return false;
	}
	for (const unsigned char c : particleName) {
		if (!std::isalnum(c) && c != '_') {
			particleManagementMessage_ = "Use only letters, numbers, and underscores in particle group names.";
			return false;
		}
	}
	if (effectComponent->GetParticleManager()->GetParticleGroups().Contains(particleName) ||
		globalVariables->HasGroup(kParticleDataPrefix + particleName)) {
		particleManagementMessage_ = "A particle group or saved particle data with that name already exists.";
		return false;
	}
	return true;
}

void EffectEditor::AddParticleGroupFromEditor() {
	const std::string particleName = newParticleGroupNameBuffer_.data();
	if (!ValidateNewParticleGroupName(particleName)) {
		return;
	}

	const std::string requestedTexturePath = newParticleTexturePathBuffer_.data();
	newParticleGroupData_.texturePath = ResolveParticleTexturePath(effectComponent->GetParticleManager()->GetTextureManager(), requestedTexturePath);
	Engine::ImGuiUtility::SetInputTextBuffer(newParticleTexturePathBuffer_, newParticleGroupData_.texturePath);
	if (requestedTexturePath != newParticleGroupData_.texturePath) {
		particleManagementMessage_ = "Texture path not found. Preview texture applied: " + newParticleGroupData_.texturePath;
	}
	if (newParticleGroupData_.meshSourceType == Engine::ParticleMeshSourceType::Model) {
		if (newParticleGroupData_.modelName.empty()) {
			particleManagementMessage_ = "Choose a model for the particle group.";
			return;
		}
	}
	else if (newParticleGroupData_.shapeType == Engine::ShapeParameter::ShapeType::None ||
		newParticleGroupData_.shapeType == Engine::ShapeParameter::ShapeType::Max) {
		particleManagementMessage_ = "Choose a valid primitive shape.";
		return;
	}

	UnregisterDeletedParticleGroupName(particleName);
	if (!effectComponent->GetParticleManager()->CreateEditorParticleGroup(particleName, newParticleGroupData_)) {
		particleManagementMessage_ = "Could not create particle group.";
		return;
	}

	RegisterParticleGroupName(particleName);
	Engine::ParticleGroupEditorData savedData =
		effectComponent->GetParticleManager()->GetEditorParticleGroupData(particleName);
	serializer_.RegisterParticleGroupData(particleName, savedData);
	serializer_.SaveParticleGroupData(particleName, savedData);
	globalVariables->SaveFile(kParticleRegistryGroup);
	globalVariables->SaveFile(kDeletedParticleRegistryGroup);
	globalVariables->SaveFile(kParticleDataPrefix + particleName);

	selectedParticleGroupName_ = particleName;
	newParticleGroupNameBuffer_.fill('\0');
	editParticleTexturePathBuffer_.fill('\0');
	particleManagementMessage_ = "Added particle group: " + particleName;
}

void EffectEditor::RenameParticleGroup(const std::string& oldName, const std::string& newName) {
	if (oldName.empty() || !effectComponent->GetParticleManager()->GetParticleGroups().Contains(oldName)) {
		particleManagementMessage_ = "The selected particle group no longer exists.";
		return;
	}
	if (!ValidateNewParticleGroupName(newName)) {
		return;
	}

	Engine::ParticleGroupEditorData data = effectComponent->GetParticleManager()->GetEditorParticleGroupData(oldName);
	const bool previewWasTarget = previewParticleGroupName_ == oldName;
	if (!effectComponent->GetParticleManager()->RenameParticleGroup(oldName, newName)) {
		particleManagementMessage_ = "Could not rename particle group.";
		return;
	}

	RenameParticleReferences(oldName, newName);
	UnregisterParticleGroupName(oldName);
	RegisterParticleGroupName(newName);
	RegisterDeletedParticleGroupName(oldName);
	UnregisterDeletedParticleGroupName(newName);

	globalVariables->RemoveGroup(kParticleDataPrefix + oldName);
	globalVariables->RemoveSavedFile(kParticleDataPrefix + oldName);
	serializer_.RegisterParticleGroupData(newName, data);
	serializer_.SaveParticleGroupData(newName, data);
	globalVariables->SaveFile(kParticleDataPrefix + newName);
	globalVariables->SaveFile(kParticleRegistryGroup);
	globalVariables->SaveFile(kDeletedParticleRegistryGroup);
	for (auto& [effectName, effectData] : effectGlobalDatas_) {
		if (effectData.particleName == newName) {
			serializer_.SaveEffectData(effectName, effectData);
			globalVariables->SaveFile(effectName);
		}
	}

	if (previewWasTarget) {
		previewParticleGroupName_ = newName;
		EnsureParticleGroupPreviewEmitter(newName);
	}
	selectedParticleGroupName_ = newName;
	editParticleTexturePathBuffer_.fill('\0');
	particleManagementMessage_ = "Renamed particle group: " + oldName + " -> " + newName;
}

void EffectEditor::DeleteParticleGroup(const std::string& particleName) {
	if (particleName.empty() || !effectComponent->GetParticleManager()->GetParticleGroups().Contains(particleName)) {
		particleManagementMessage_ = "The selected particle group no longer exists.";
		return;
	}

	if (previewParticleGroupName_ == particleName) {
		ClearParticleGroupPreview();
	}

	for (auto& [effectName, effectData] : effectGlobalDatas_) {
		if (effectData.particleName == particleName) {
			effectData.particleName.clear();
			serializer_.SaveEffectData(effectName, effectData);
			globalVariables->SaveFile(effectName);
		}
	}

	effectComponent->GetParticleManager()->RemoveParticleGroup(particleName);
	UnregisterParticleGroupName(particleName);
	RegisterDeletedParticleGroupName(particleName);
	globalVariables->RemoveGroup(kParticleDataPrefix + particleName);
	globalVariables->RemoveSavedFile(kParticleDataPrefix + particleName);
	globalVariables->SaveFile(kParticleRegistryGroup);
	globalVariables->SaveFile(kDeletedParticleRegistryGroup);

	if (!effectComponent->GetParticleManager()->GetParticleGroups().empty()) {
		selectedParticleGroupName_ = effectComponent->GetParticleManager()->GetParticleGroups().begin()->first;
	}
	else {
		selectedParticleGroupName_.clear();
	}
	editParticleTexturePathBuffer_.fill('\0');
	particleManagementMessage_ = "Deleted particle group: " + particleName;
}

void EffectEditorSerializer::RegisterParticleGroupData(const std::string& particleName, const Engine::ParticleGroupEditorData& data) {
	const std::string groupName = std::string(kParticleDataPrefix) + particleName;
	// 個別保存とALL保存で同じEffect/Particleフォルダへ書き出し、読み込み時の重複を防ぐ。
	globalVariables_->SetGroupCategory(groupName, "Effect/Particle");
	globalVariables_->CreateGroup(groupName);
	WriteParticleGroupData(particleName, data, false);
}

void EffectEditorSerializer::SaveParticleGroupData(const std::string& particleName, const Engine::ParticleGroupEditorData& data) {
	WriteParticleGroupData(particleName, data, true);
}

void EffectEditorSerializer::WriteParticleGroupData(const std::string& particleName,
	const Engine::ParticleGroupEditorData& data, bool overwrite) {
	const std::string groupName = std::string(kParticleDataPrefix) + particleName;
	Engine::GlobalVariableWriter writer(globalVariables_, overwrite ?
		Engine::GlobalVariableWriteMode::Save : Engine::GlobalVariableWriteMode::Register);
	// パーティクル群の全保存項目を登録・保存で共有する
	writer.Value(groupName, "texturePath", data.texturePath);
	writer.EnumValue<Engine::ParticleMeshSourceType>(groupName, "meshSourceType", data.meshSourceType, "ParticleMeshSourceType");
	writer.Value(groupName, "modelName", data.modelName);
	writer.EnumValue<Engine::ShapeParameter::ShapeType>(groupName, "shapeType", data.shapeType, "ShapeType");
	writer.Value(groupName, "shape.plane.width", data.plane.width);
	writer.Value(groupName, "shape.plane.height", data.plane.height);
	writer.Value(groupName, "shape.triangle.upPos", data.triangle.upPos);
	writer.Value(groupName, "shape.triangle.leftPos", data.triangle.leftPos);
	writer.Value(groupName, "shape.triangle.rightPos", data.triangle.rightPos);
	writer.Value(groupName, "shape.cross.armLength", data.cross.armLength);
	writer.Value(groupName, "shape.cross.armWidth", data.cross.armWidth);
	writer.Value(groupName, "shape.cube.size", data.cube.size);
	writer.Value(groupName, "shape.circle.radius", data.circle.radius);
	writer.Value(groupName, "shape.circle.segments", data.circle.segments);
	writer.Value(groupName, "shape.star.innerRadius", data.star.innerRadius);
	writer.Value(groupName, "shape.star.outerRadius", data.star.outerRadius);
	writer.Value(groupName, "shape.star.segments", data.star.segments);
	writer.Value(groupName, "shape.crescent.innerRadius", data.crescent.innerRadius);
	writer.Value(groupName, "shape.crescent.outerRadius", data.crescent.outerRadius);
	writer.Value(groupName, "shape.crescent.distance", data.crescent.distance);
	writer.Value(groupName, "shape.crescent.segments", data.crescent.segments);
	writer.Value(groupName, "shape.ring.innerRadius", data.ring.innerRadius);
	writer.Value(groupName, "shape.ring.outerRadius", data.ring.outerRadius);
	writer.Value(groupName, "shape.ring.segments", data.ring.segments);
	writer.Value(groupName, "shape.sphere.radius", data.sphere.radius);
	writer.Value(groupName, "shape.sphere.latitudeSegments", data.sphere.latitudeSegments);
	writer.Value(groupName, "shape.sphere.longitudeSegments", data.sphere.longitudeSegments);
	writer.Value(groupName, "shape.sphere.isTopBased", data.sphere.isTopBased);
	writer.Value(groupName, "shape.arrow.shaftLength", data.arrow.shaftLength);
	writer.Value(groupName, "shape.arrow.shaftRadius", data.arrow.shaftRadius);
	writer.Value(groupName, "shape.arrow.headLength", data.arrow.headLength);
	writer.Value(groupName, "shape.arrow.headRadius", data.arrow.headRadius);
	writer.Value(groupName, "shape.arrow.segments", data.arrow.segments);
	writer.Value(groupName, "shape.cylinder.innerRadius", data.cylinder.innerRadius);
	writer.Value(groupName, "shape.cylinder.outerRadius", data.cylinder.outerRadius);
	writer.Value(groupName, "shape.cylinder.height", data.cylinder.height);
	writer.Value(groupName, "shape.cylinder.segments", data.cylinder.segments);
	writer.Value(groupName, "shape.cylinder.isCover", data.cylinder.isCover);
	writer.Value(groupName, "shape.tube.radius", data.tube.radius);
	writer.Value(groupName, "shape.tube.innerRadius", data.tube.innerRadius);
	writer.Value(groupName, "shape.tube.height", data.tube.height);
	writer.Value(groupName, "shape.tube.segments", data.tube.segments);
	writer.Value(groupName, "shape.pyramid.radius", data.pyramid.radius);
	writer.Value(groupName, "shape.pyramid.height", data.pyramid.height);
	writer.Value(groupName, "shape.pyramid.segments", data.pyramid.segments);
	writer.Value(groupName, "shape.torus.innerRadius", data.torus.innerRadius);
	writer.Value(groupName, "shape.torus.outerRadius", data.torus.outerRadius);
	writer.Value(groupName, "shape.torus.segments", data.torus.segments);
	writer.Value(groupName, "shape.torus.tubeSegments", data.torus.tubeSegments);
	writer.EnumValue<EmitData::RasterizerType>(groupName, "rasterizerType", data.rasterizerType, "RasterizerType");
	writer.EnumValue<EmitData::BlendType>(groupName, "blendType", data.blendType, "BlendType");
	writer.Value(groupName, "isEditorPrimitive", data.isEditorPrimitive);
	writer.Value(groupName, "isUVClamp", data.isUVClamp);
	writer.Value(groupName, "uvTransformVelocity", data.uvTransformVelocity);
	writer.Value(groupName, "uvTransform", data.uvTransform);
	writer.Value(groupName, "isFlag.usebillboard", data.isFlag.usebillboard);
	writer.Value(groupName, "isFlag.usebillboardY", data.isFlag.usebillboardY);
	writer.Value(groupName, "isFlag.billboardRotZ", data.isFlag.billboardRotZ);
	writer.Value(groupName, "isFlag.isAlpha", data.isFlag.isAlpha);
	writer.Value(groupName, "isFlag.isLine", data.isFlag.isLine);
	writer.Value(groupName, "isFlag.isGravity", data.isFlag.isGravity);
	writer.Value(groupName, "isFlag.isLifeTimeScale", data.isFlag.isLifeTimeScale_);
	writer.Value(groupName, "isFlag.isRotateVelocity", data.isFlag.isRotateVelocity);
	writer.Value(groupName, "isFlag.isLifeTimeVelocity", data.isFlag.isLifeTimeVelocity);
	writer.Value(groupName, "isFlag.isBounce", data.isFlag.isBounce);
	writer.Value(groupName, "isFlag.isAcceleration", data.isFlag.isAcceleration);
	writer.Value(groupName, "isFlag.isLineInterpolation", data.isFlag.isLineInterpolation);
	writer.Value(groupName, "isFlag.isScaling", data.isFlag.isScaling_);
	writer.EnumValue<EmitData::EmitType>(groupName, "emitType", data.emitType, "EmitType");
	writer.EnumValue<EmitData::TopBottom>(groupName, "topBottom", data.topBottom, "TopBottom");
	writer.Value(groupName, "gravitationalAcceleration", data.gravitationalAcceleration);
	writer.Value(groupName, "material.transform", data.materialTransform);
	writer.Value(groupName, "material.color", data.materialColor);
	writer.Value(groupName, "material.enableLighting", data.materialEnableLighting);
	writer.Value(groupName, "material.environmentCoefficient", data.materialEnvironmentCoefficient);
	writer.Value(groupName, "material.shininess", data.materialShininess);
	writer.Value(groupName, "material.useLig", data.materialUseLig);
	writer.Value(groupName, "material.useNormalMap", data.materialUseNormalMap);
	writer.Value(groupName, "material.useSpecularMap", data.materialUseSpecularMap);
	writer.Value(groupName, "material.useEnvironment", data.materialUseEnvironment);
	writer.Value(groupName, "material.alphaClipping", data.materialAlphaClipping);
	writer.Value(groupName, "material.alpha", data.materialAlpha);
}

void EffectEditorSerializer::LoadParticleGroupData(const std::string& particleName, Engine::ParticleGroupEditorData& data) const {
	const std::string groupName = kParticleDataPrefix + particleName;
	// 旧形式の保存ファイルでも読み込めるよう、追加項目はキーがある場合だけ上書きする。
	data.texturePath = globalVariables_->GetValue<std::string>(groupName, "texturePath");
	if (globalVariables_->HasKey(groupName, "meshSourceType")) data.meshSourceType = globalVariables_->GetEnumValue<Engine::ParticleMeshSourceType>(groupName, "meshSourceType");
	if (globalVariables_->HasKey(groupName, "modelName")) data.modelName = globalVariables_->GetValue<std::string>(groupName, "modelName");
	data.shapeType = globalVariables_->GetEnumValue<Engine::ShapeParameter::ShapeType>(groupName, "shapeType");
	if (globalVariables_->HasKey(groupName, "shape.plane.width")) data.plane.width = globalVariables_->GetValue<float>(groupName, "shape.plane.width");
	if (globalVariables_->HasKey(groupName, "shape.plane.height")) data.plane.height = globalVariables_->GetValue<float>(groupName, "shape.plane.height");
	if (globalVariables_->HasKey(groupName, "shape.triangle.upPos")) data.triangle.upPos = globalVariables_->GetValue<Vector2>(groupName, "shape.triangle.upPos");
	if (globalVariables_->HasKey(groupName, "shape.triangle.leftPos")) data.triangle.leftPos = globalVariables_->GetValue<Vector2>(groupName, "shape.triangle.leftPos");
	if (globalVariables_->HasKey(groupName, "shape.triangle.rightPos")) data.triangle.rightPos = globalVariables_->GetValue<Vector2>(groupName, "shape.triangle.rightPos");
	if (globalVariables_->HasKey(groupName, "shape.cross.armLength")) data.cross.armLength = globalVariables_->GetValue<float>(groupName, "shape.cross.armLength");
	if (globalVariables_->HasKey(groupName, "shape.cross.armWidth")) data.cross.armWidth = globalVariables_->GetValue<float>(groupName, "shape.cross.armWidth");
	if (globalVariables_->HasKey(groupName, "shape.cube.size")) data.cube.size = globalVariables_->GetValue<Vector3>(groupName, "shape.cube.size");
	if (globalVariables_->HasKey(groupName, "shape.circle.radius")) data.circle.radius = globalVariables_->GetValue<float>(groupName, "shape.circle.radius");
	if (globalVariables_->HasKey(groupName, "shape.circle.segments")) data.circle.segments = globalVariables_->GetValue<int32_t>(groupName, "shape.circle.segments");
	if (globalVariables_->HasKey(groupName, "shape.star.innerRadius")) data.star.innerRadius = globalVariables_->GetValue<float>(groupName, "shape.star.innerRadius");
	if (globalVariables_->HasKey(groupName, "shape.star.outerRadius")) data.star.outerRadius = globalVariables_->GetValue<float>(groupName, "shape.star.outerRadius");
	if (globalVariables_->HasKey(groupName, "shape.star.segments")) data.star.segments = globalVariables_->GetValue<int32_t>(groupName, "shape.star.segments");
	if (globalVariables_->HasKey(groupName, "shape.crescent.innerRadius")) data.crescent.innerRadius = globalVariables_->GetValue<float>(groupName, "shape.crescent.innerRadius");
	if (globalVariables_->HasKey(groupName, "shape.crescent.outerRadius")) data.crescent.outerRadius = globalVariables_->GetValue<float>(groupName, "shape.crescent.outerRadius");
	if (globalVariables_->HasKey(groupName, "shape.crescent.distance")) data.crescent.distance = globalVariables_->GetValue<float>(groupName, "shape.crescent.distance");
	if (globalVariables_->HasKey(groupName, "shape.crescent.segments")) data.crescent.segments = globalVariables_->GetValue<int32_t>(groupName, "shape.crescent.segments");
	if (globalVariables_->HasKey(groupName, "shape.ring.innerRadius")) data.ring.innerRadius = globalVariables_->GetValue<float>(groupName, "shape.ring.innerRadius");
	if (globalVariables_->HasKey(groupName, "shape.ring.outerRadius")) data.ring.outerRadius = globalVariables_->GetValue<float>(groupName, "shape.ring.outerRadius");
	if (globalVariables_->HasKey(groupName, "shape.ring.segments")) data.ring.segments = globalVariables_->GetValue<int32_t>(groupName, "shape.ring.segments");
	if (globalVariables_->HasKey(groupName, "shape.sphere.radius")) data.sphere.radius = globalVariables_->GetValue<float>(groupName, "shape.sphere.radius");
	if (globalVariables_->HasKey(groupName, "shape.sphere.latitudeSegments")) data.sphere.latitudeSegments = globalVariables_->GetValue<int32_t>(groupName, "shape.sphere.latitudeSegments");
	if (globalVariables_->HasKey(groupName, "shape.sphere.longitudeSegments")) data.sphere.longitudeSegments = globalVariables_->GetValue<int32_t>(groupName, "shape.sphere.longitudeSegments");
	if (globalVariables_->HasKey(groupName, "shape.sphere.isTopBased")) data.sphere.isTopBased = globalVariables_->GetValue<bool>(groupName, "shape.sphere.isTopBased");
	if (globalVariables_->HasKey(groupName, "shape.arrow.shaftLength")) data.arrow.shaftLength = globalVariables_->GetValue<float>(groupName, "shape.arrow.shaftLength");
	if (globalVariables_->HasKey(groupName, "shape.arrow.shaftRadius")) data.arrow.shaftRadius = globalVariables_->GetValue<float>(groupName, "shape.arrow.shaftRadius");
	if (globalVariables_->HasKey(groupName, "shape.arrow.headLength")) data.arrow.headLength = globalVariables_->GetValue<float>(groupName, "shape.arrow.headLength");
	if (globalVariables_->HasKey(groupName, "shape.arrow.headRadius")) data.arrow.headRadius = globalVariables_->GetValue<float>(groupName, "shape.arrow.headRadius");
	if (globalVariables_->HasKey(groupName, "shape.arrow.segments")) data.arrow.segments = globalVariables_->GetValue<int32_t>(groupName, "shape.arrow.segments");
	if (globalVariables_->HasKey(groupName, "shape.cylinder.innerRadius")) data.cylinder.innerRadius = globalVariables_->GetValue<float>(groupName, "shape.cylinder.innerRadius");
	if (globalVariables_->HasKey(groupName, "shape.cylinder.outerRadius")) data.cylinder.outerRadius = globalVariables_->GetValue<float>(groupName, "shape.cylinder.outerRadius");
	if (globalVariables_->HasKey(groupName, "shape.cylinder.height")) data.cylinder.height = globalVariables_->GetValue<float>(groupName, "shape.cylinder.height");
	if (globalVariables_->HasKey(groupName, "shape.cylinder.segments")) data.cylinder.segments = globalVariables_->GetValue<int32_t>(groupName, "shape.cylinder.segments");
	if (globalVariables_->HasKey(groupName, "shape.cylinder.isCover")) data.cylinder.isCover = globalVariables_->GetValue<bool>(groupName, "shape.cylinder.isCover");
	if (globalVariables_->HasKey(groupName, "shape.tube.radius")) data.tube.radius = globalVariables_->GetValue<float>(groupName, "shape.tube.radius");
	if (globalVariables_->HasKey(groupName, "shape.tube.innerRadius")) data.tube.innerRadius = globalVariables_->GetValue<float>(groupName, "shape.tube.innerRadius");
	if (globalVariables_->HasKey(groupName, "shape.tube.height")) data.tube.height = globalVariables_->GetValue<float>(groupName, "shape.tube.height");
	if (globalVariables_->HasKey(groupName, "shape.tube.segments")) data.tube.segments = globalVariables_->GetValue<int32_t>(groupName, "shape.tube.segments");
	if (globalVariables_->HasKey(groupName, "shape.pyramid.radius")) data.pyramid.radius = globalVariables_->GetValue<float>(groupName, "shape.pyramid.radius");
	if (globalVariables_->HasKey(groupName, "shape.pyramid.height")) data.pyramid.height = globalVariables_->GetValue<float>(groupName, "shape.pyramid.height");
	if (globalVariables_->HasKey(groupName, "shape.pyramid.segments")) data.pyramid.segments = globalVariables_->GetValue<int32_t>(groupName, "shape.pyramid.segments");
	if (globalVariables_->HasKey(groupName, "shape.torus.innerRadius")) data.torus.innerRadius = globalVariables_->GetValue<float>(groupName, "shape.torus.innerRadius");
	if (globalVariables_->HasKey(groupName, "shape.torus.outerRadius")) data.torus.outerRadius = globalVariables_->GetValue<float>(groupName, "shape.torus.outerRadius");
	if (globalVariables_->HasKey(groupName, "shape.torus.segments")) data.torus.segments = globalVariables_->GetValue<int32_t>(groupName, "shape.torus.segments");
	if (globalVariables_->HasKey(groupName, "shape.torus.tubeSegments")) data.torus.tubeSegments = globalVariables_->GetValue<int32_t>(groupName, "shape.torus.tubeSegments");
	data.rasterizerType = globalVariables_->GetEnumValue<EmitData::RasterizerType>(groupName, "rasterizerType");
	data.blendType = globalVariables_->GetEnumValue<EmitData::BlendType>(groupName, "blendType");
	data.isEditorPrimitive = globalVariables_->GetValue<bool>(groupName, "isEditorPrimitive");
	if (globalVariables_->HasKey(groupName, "isUVClamp")) data.isUVClamp = globalVariables_->GetValue<bool>(groupName, "isUVClamp");
	if (globalVariables_->HasKey(groupName, "uvTransformVelocity")) data.uvTransformVelocity = globalVariables_->GetValue<Transform>(groupName, "uvTransformVelocity");
	if (globalVariables_->HasKey(groupName, "isFlag.usebillboard")) data.isFlag.usebillboard = globalVariables_->GetValue<bool>(groupName, "isFlag.usebillboard");
	if (globalVariables_->HasKey(groupName, "isFlag.usebillboardY")) data.isFlag.usebillboardY = globalVariables_->GetValue<bool>(groupName, "isFlag.usebillboardY");
	if (globalVariables_->HasKey(groupName, "isFlag.billboardRotZ")) data.isFlag.billboardRotZ = globalVariables_->GetValue<bool>(groupName, "isFlag.billboardRotZ");
	if (globalVariables_->HasKey(groupName, "isFlag.isAlpha")) data.isFlag.isAlpha = globalVariables_->GetValue<bool>(groupName, "isFlag.isAlpha");
	if (globalVariables_->HasKey(groupName, "isFlag.isLine")) data.isFlag.isLine = globalVariables_->GetValue<bool>(groupName, "isFlag.isLine");
	if (globalVariables_->HasKey(groupName, "isFlag.isGravity")) data.isFlag.isGravity = globalVariables_->GetValue<bool>(groupName, "isFlag.isGravity");
	if (globalVariables_->HasKey(groupName, "isFlag.isLifeTimeScale")) data.isFlag.isLifeTimeScale_ = globalVariables_->GetValue<bool>(groupName, "isFlag.isLifeTimeScale");
	if (globalVariables_->HasKey(groupName, "isFlag.isRotateVelocity")) data.isFlag.isRotateVelocity = globalVariables_->GetValue<bool>(groupName, "isFlag.isRotateVelocity");
	if (globalVariables_->HasKey(groupName, "isFlag.isLifeTimeVelocity")) data.isFlag.isLifeTimeVelocity = globalVariables_->GetValue<bool>(groupName, "isFlag.isLifeTimeVelocity");
	if (globalVariables_->HasKey(groupName, "isFlag.isBounce")) data.isFlag.isBounce = globalVariables_->GetValue<bool>(groupName, "isFlag.isBounce");
	if (globalVariables_->HasKey(groupName, "isFlag.isAcceleration")) data.isFlag.isAcceleration = globalVariables_->GetValue<bool>(groupName, "isFlag.isAcceleration");
	if (globalVariables_->HasKey(groupName, "isFlag.isLineInterpolation")) data.isFlag.isLineInterpolation = globalVariables_->GetValue<bool>(groupName, "isFlag.isLineInterpolation");
	if (globalVariables_->HasKey(groupName, "isFlag.isScaling")) data.isFlag.isScaling_ = globalVariables_->GetValue<bool>(groupName, "isFlag.isScaling");
	if (globalVariables_->HasKey(groupName, "emitType")) data.emitType = globalVariables_->GetEnumValue<EmitData::EmitType>(groupName, "emitType");
	if (globalVariables_->HasKey(groupName, "topBottom")) data.topBottom = globalVariables_->GetEnumValue<EmitData::TopBottom>(groupName, "topBottom");
	if (globalVariables_->HasKey(groupName, "gravitationalAcceleration")) data.gravitationalAcceleration = globalVariables_->GetValue<float>(groupName, "gravitationalAcceleration");
	if (globalVariables_->HasKey(groupName, "material.transform")) data.materialTransform = globalVariables_->GetValue<Transform>(groupName, "material.transform");
	if (globalVariables_->HasKey(groupName, "uvTransform")) {
		data.uvTransform = globalVariables_->GetValue<Transform>(groupName, "uvTransform");
	}
	else {
		// 旧保存データは従来のmaterial.transformをUV本体として引き継ぐ。
		data.uvTransform = data.materialTransform;
	}
	if (globalVariables_->HasKey(groupName, "material.color")) data.materialColor = globalVariables_->GetValue<Vector4>(groupName, "material.color");
	if (globalVariables_->HasKey(groupName, "material.enableLighting")) data.materialEnableLighting = globalVariables_->GetValue<bool>(groupName, "material.enableLighting");
	if (globalVariables_->HasKey(groupName, "material.environmentCoefficient")) data.materialEnvironmentCoefficient = globalVariables_->GetValue<float>(groupName, "material.environmentCoefficient");
	if (globalVariables_->HasKey(groupName, "material.shininess")) data.materialShininess = globalVariables_->GetValue<float>(groupName, "material.shininess");
	if (globalVariables_->HasKey(groupName, "material.useLig")) data.materialUseLig = globalVariables_->GetValue<bool>(groupName, "material.useLig");
	if (globalVariables_->HasKey(groupName, "material.useNormalMap")) data.materialUseNormalMap = globalVariables_->GetValue<bool>(groupName, "material.useNormalMap");
	if (globalVariables_->HasKey(groupName, "material.useSpecularMap")) data.materialUseSpecularMap = globalVariables_->GetValue<bool>(groupName, "material.useSpecularMap");
	if (globalVariables_->HasKey(groupName, "material.useEnvironment")) data.materialUseEnvironment = globalVariables_->GetValue<bool>(groupName, "material.useEnvironment");
	if (globalVariables_->HasKey(groupName, "material.alphaClipping")) data.materialAlphaClipping = globalVariables_->GetValue<float>(groupName, "material.alphaClipping");
	if (globalVariables_->HasKey(groupName, "material.alpha")) data.materialAlpha = globalVariables_->GetValue<float>(groupName, "material.alpha");
}
void EffectEditor::RegisterParticleGroupName(const std::string& name) {
	globalVariables->SetGroupCategory(kParticleRegistryGroup, "Effect/Particle");
	globalVariables->CreateGroup(kParticleRegistryGroup);
	globalVariables->AddItem(kParticleRegistryGroup, name, name);
}

void EffectEditor::UnregisterParticleGroupName(const std::string& name) {
	globalVariables->RemoveItem(kParticleRegistryGroup, name);
}

void EffectEditor::RegisterDeletedParticleGroupName(const std::string& name) {
	globalVariables->SetGroupCategory(kDeletedParticleRegistryGroup, "Effect/Particle");
	globalVariables->CreateGroup(kDeletedParticleRegistryGroup);
	globalVariables->AddItem(kDeletedParticleRegistryGroup, name, name);
}

void EffectEditor::UnregisterDeletedParticleGroupName(const std::string& name) {
	globalVariables->RemoveItem(kDeletedParticleRegistryGroup, name);
}

bool EffectEditor::IsDeletedParticleGroupName(const std::string& name) const {
	return globalVariables && globalVariables->HasKey(kDeletedParticleRegistryGroup, name);
}

void EffectEditor::RenameParticleReferences(const std::string& oldName, const std::string& newName) {
	for (auto& [effectName, effectData] : effectGlobalDatas_) {
		if (effectData.particleName == oldName) {
			effectData.particleName = newName;
			if (Engine::BaseParticleEmitter* emitter = effectComponent->GetBaseEmitter(effectName)) {
				emitter->SetParticleName(newName);
			}
		}
	}
}

void EffectEditorSerializer::RegisterEffectData(const std::string& name, const EffectGlobalData& data) {
	globalVariables_->SetGroupCategory(name, "Effect");
	globalVariables_->CreateGroup(name);
	WriteEffectData(name, data, false);
}

void EffectEditorSerializer::SaveEffectData(const std::string& name, const EffectGlobalData& data) {
	WriteEffectData(name, data, true);
}

void EffectEditorSerializer::WriteEffectData(const std::string& name, const EffectGlobalData& data, bool overwrite) {
	Engine::GlobalVariableWriter writer(globalVariables_, overwrite ?
		Engine::GlobalVariableWriteMode::Save : Engine::GlobalVariableWriteMode::Register);
	// エフェクトの全保存項目を登録・保存で共有する
	writer.Value(name, "particleName", data.particleName);
	writer.Value(name, "frequency", data.frequency);
	writer.Value(name, "emitData.acceleration.median", data.emitData.acceleration.median);
	writer.Value(name, "emitData.acceleration.range", data.emitData.acceleration.range);
	writer.Value(name, "emitData.colorRange.min", data.emitData.colorRange.min);
	writer.Value(name, "emitData.colorRange.max", data.emitData.colorRange.max);
	writer.Value(name, "emitData.count.median", data.emitData.count.median);
	writer.Value(name, "emitData.count.range", data.emitData.count.range);
	writer.Value(name, "emitData.lifeTime.median", data.emitData.lifeTime.median);
	writer.Value(name, "emitData.lifeTime.range", data.emitData.lifeTime.range);
	writer.Value(name, "emitData.rotate.median", data.emitData.rotate.median);
	writer.Value(name, "emitData.rotate.range", data.emitData.rotate.range);
	writer.Value(name, "emitData.rotateVelocity.median", data.emitData.rotateVelocity.median);
	writer.Value(name, "emitData.rotateVelocity.range", data.emitData.rotateVelocity.range);
	writer.Value(name, "emitData.size.median", data.emitData.size.median);
	writer.Value(name, "emitData.size.range", data.emitData.size.range);
	writer.Value(name, "emitData.sizeAmount.median", data.emitData.sizeAmount.median);
	writer.Value(name, "emitData.sizeAmount.range", data.emitData.sizeAmount.range);
	writer.Value(name, "emitData.velocity.median", data.emitData.velocity.median);
	writer.Value(name, "emitData.velocity.range", data.emitData.velocity.range);
	writer.Value(name, "emitData.isEmit", data.emitData.isEmit);
	writer.Value(name, "emitData.isLoop", data.emitData.isLoop);
	writer.Value(name, "emitData.isUniformSize", data.emitData.isUniformSize);
	writer.Value(name, "emitData.isDirectionRotate", data.emitData.isDirectionRotate);
	writer.Value(name, "emitData.isNoise", data.emitData.isNoise);
	writer.Value(name, "emitData.direction", data.emitData.direction);
	writer.Value(name, "emitData.uvTransformVeloctiy", data.uvTransformVeloctiy);

	writer.Value(name, "emitData.alphaClipping", data.alphaClipping);
	writer.Value(name, "emitData.enableLighting", data.enableLighting);

	writer.Value(name, "emitData.isFlag.billboardRotZ", data.isFlag.billboardRotZ);
	writer.Value(name, "emitData.isFlag.isAcceleration", data.isFlag.isAcceleration);
	writer.Value(name, "emitData.isFlag.isAlpha", data.isFlag.isAlpha);
	writer.Value(name, "emitData.isFlag.isBounce", data.isFlag.isBounce);
	writer.Value(name, "emitData.isFlag.isGravity", data.isFlag.isGravity);
	writer.Value(name, "emitData.isFlag.isLifeTimeScale", data.isFlag.isLifeTimeScale_);
	writer.Value(name, "emitData.isFlag.isLifeTimeVelocity", data.isFlag.isLifeTimeVelocity);
	writer.Value(name, "emitData.isFlag.isLine", data.isFlag.isLine);
	writer.Value(name, "emitData.isFlag.isLineInterpolation", data.isFlag.isLineInterpolation);
	writer.Value(name, "emitData.isFlag.isRotateVelocity", data.isFlag.isRotateVelocity);
	writer.Value(name, "emitData.isFlag.isScaling", data.isFlag.isScaling_);
	writer.Value(name, "emitData.isFlag.usebillboard", data.isFlag.usebillboard);
	writer.Value(name, "emitData.isFlag.usebillboardY", data.isFlag.usebillboardY);


	writer.Value(name, "emitData.lineEnd", data.lineEnd);
	writer.Value(name, "emitData.lineStart", data.lineStart);

	writer.Value(name, "emitData.radius", data.radius);

	writer.Value(name, "emitData.rangeMax", data.rangeMax);
	writer.Value(name, "emitData.rangeMin", data.rangeMin);

	writer.Value(name, "emitData.segment", data.segment);

	writer.EnumValue<EmitterShapeType>(name, "emitData.shapeType", data.shapeType, "EmitterShapeType");
	writer.EnumValue<EmitData::EmitType>(name, "emitData.emitType", data.emitType, "EmitType");
	writer.EnumValue<EmitData::DirectionType>(name, "emitData.directionType", data.directionType, "DirectionType");
	writer.EnumValue<EmitData::TopBottom>(name, "emitData.topBottom", data.topBottom, "TopBottom");
}

void EffectEditorSerializer::LoadEffectData(const std::string& name, EffectGlobalData& data) const {
	data.particleName = globalVariables_->GetValue<std::string>(name, "particleName");

	data.frequency = globalVariables_->GetValue<float>(name, "frequency");
	data.emitData.acceleration.median = globalVariables_->GetValue<Vector3>(name, "emitData.acceleration.median");
	data.emitData.acceleration.range = globalVariables_->GetValue<Vector3>(name, "emitData.acceleration.range");
	data.emitData.colorRange.min = globalVariables_->GetValue<Vector4>(name, "emitData.colorRange.min");
	data.emitData.colorRange.max = globalVariables_->GetValue<Vector4>(name, "emitData.colorRange.max");
	data.emitData.count.median = globalVariables_->GetValue<int>(name, "emitData.count.median");
	data.emitData.count.range = globalVariables_->GetValue<int>(name, "emitData.count.range");
	data.emitData.lifeTime.median = globalVariables_->GetValue<float>(name, "emitData.lifeTime.median");
	data.emitData.lifeTime.range = globalVariables_->GetValue<float>(name, "emitData.lifeTime.range");
	data.emitData.rotate.median = globalVariables_->GetValue<Vector3>(name, "emitData.rotate.median");
	data.emitData.rotate.range = globalVariables_->GetValue<Vector3>(name, "emitData.rotate.range");
	data.emitData.rotateVelocity.median = globalVariables_->GetValue<Vector3>(name, "emitData.rotateVelocity.median");
	data.emitData.rotateVelocity.range = globalVariables_->GetValue<Vector3>(name, "emitData.rotateVelocity.range");
	data.emitData.size.median = globalVariables_->GetValue<Vector3>(name, "emitData.size.median");
	data.emitData.size.range = globalVariables_->GetValue<Vector3>(name, "emitData.size.range");
	data.emitData.sizeAmount.median = globalVariables_->GetValue<Vector3>(name, "emitData.sizeAmount.median");
	data.emitData.sizeAmount.range = globalVariables_->GetValue<Vector3>(name, "emitData.sizeAmount.range");
	data.emitData.velocity.median = globalVariables_->GetValue<Vector3>(name, "emitData.velocity.median");
	data.emitData.velocity.range = globalVariables_->GetValue<Vector3>(name, "emitData.velocity.range");
	if (globalVariables_->HasKey(name, "emitData.isEmit")) data.emitData.isEmit = globalVariables_->GetValue<bool>(name, "emitData.isEmit");
	if (globalVariables_->HasKey(name, "emitData.isLoop")) data.emitData.isLoop = globalVariables_->GetValue<bool>(name, "emitData.isLoop");
	if (globalVariables_->HasKey(name, "emitData.isUniformSize")) data.emitData.isUniformSize = globalVariables_->GetValue<bool>(name, "emitData.isUniformSize");
	if (globalVariables_->HasKey(name, "emitData.isDirectionRotate")) data.emitData.isDirectionRotate = globalVariables_->GetValue<bool>(name, "emitData.isDirectionRotate");
	if (globalVariables_->HasKey(name, "emitData.isNoise")) data.emitData.isNoise = globalVariables_->GetValue<bool>(name, "emitData.isNoise");
	if (globalVariables_->HasKey(name, "emitData.direction")) data.emitData.direction = globalVariables_->GetValue<int>(name, "emitData.direction");
	if (globalVariables_->HasKey(name, "emitData.uvTransformVeloctiy")) data.uvTransformVeloctiy = globalVariables_->GetValue<Transform>(name, "emitData.uvTransformVeloctiy");

	data.alphaClipping = globalVariables_->GetValue<float>(name, "emitData.alphaClipping");
	data.enableLighting = globalVariables_->GetValue<int>(name, "emitData.enableLighting");

	data.isFlag.billboardRotZ = globalVariables_->GetValue<bool>(name, "emitData.isFlag.billboardRotZ");
	data.isFlag.isAcceleration = globalVariables_->GetValue<bool>(name, "emitData.isFlag.isAcceleration");
	data.isFlag.isAlpha = globalVariables_->GetValue<bool>(name, "emitData.isFlag.isAlpha");
	data.isFlag.isBounce = globalVariables_->GetValue<bool>(name, "emitData.isFlag.isBounce");
	data.isFlag.isGravity = globalVariables_->GetValue<bool>(name, "emitData.isFlag.isGravity");
	data.isFlag.isLifeTimeScale_ = globalVariables_->GetValue<bool>(name, "emitData.isFlag.isLifeTimeScale");
	data.isFlag.isLifeTimeVelocity = globalVariables_->GetValue<bool>(name, "emitData.isFlag.isLifeTimeVelocity");
	data.isFlag.isLine = globalVariables_->GetValue<bool>(name, "emitData.isFlag.isLine");
	data.isFlag.isLineInterpolation = globalVariables_->GetValue<bool>(name, "emitData.isFlag.isLineInterpolation");
	data.isFlag.isRotateVelocity = globalVariables_->GetValue<bool>(name, "emitData.isFlag.isRotateVelocity");
	data.isFlag.isScaling_ = globalVariables_->GetValue<bool>(name, "emitData.isFlag.isScaling");
	data.isFlag.usebillboard = globalVariables_->GetValue<bool>(name, "emitData.isFlag.usebillboard");
	data.isFlag.usebillboardY = globalVariables_->GetValue<bool>(name, "emitData.isFlag.usebillboardY");


	data.lineEnd = globalVariables_->GetValue<Vector3>(name, "emitData.lineEnd");
	data.lineStart = globalVariables_->GetValue<Vector3>(name, "emitData.lineStart");

	data.radius = globalVariables_->GetValue<float>(name, "emitData.radius");

	data.rangeMax = globalVariables_->GetValue<Vector3>(name, "emitData.rangeMax");
	data.rangeMin = globalVariables_->GetValue<Vector3>(name, "emitData.rangeMin");

	data.segment = globalVariables_->GetValue<int>(name, "emitData.segment");

	data.shapeType = globalVariables_->GetEnumValue<EmitterShapeType>(name, "emitData.shapeType");
	if (globalVariables_->HasKey(name, "emitData.emitType")) {
		data.emitType = globalVariables_->GetEnumValue<EmitData::EmitType>(name, "emitData.emitType");
	}
	if (globalVariables_->HasKey(name, "emitData.directionType")) {
		data.directionType = globalVariables_->GetEnumValue<EmitData::DirectionType>(name, "emitData.directionType");
	}
	data.topBottom = globalVariables_->GetEnumValue<EmitData::TopBottom>(name, "emitData.topBottom");
}
void EffectEditor::DrawEffectDetail(const std::string& name, EffectGlobalData& data) {
	// EffectウィンドウのEmitter Settingsタブ内へ直接描画し、別ウィンドウに分かれないようにする。
	ImGui::SeparatorText("Emitter Detail");
	// パーティクル選択
	Engine::ImGuiManager::Select("Particle Group", data.particleName, effectComponent->GetParticleManager()->GetParticleGroups());
	if (ImGui::Button("Save This Emitter")) {
		SaveEmitterData(name, data);
	}
	if (!emitterSettingsMessage_.empty()) {
		ImGui::TextWrapped("%s", emitterSettingsMessage_.c_str());
	}

	static const char* ShapeTypeLabels[] = {
		"AABB",		// AABB
		"SPHERE",		// 球
		"CIRCLE",		// 円
		"POINT",		// 点
		"LINE",		// 線
		"SPLINE",		// スプライン
		"TRIANGLE",	// 三角形
		"MESH",		// メッシュ
	};

	EmitterShapeType keep = data.shapeType;
	Engine::ImGuiManager::Select("エミッタ形状", ShapeTypeLabels, data.shapeType);
	if (keep != data.shapeType) {
		effectComponent->RemoveEmitter(name);
		effectComponent->AddEmitter(name, data.particleName, data.shapeType);
		SetEffectGlobalData(name, data.shapeType, data);
	}
	static const char* EmitTypeLabels[] = {
		"ランダム",
		"面",
		"辺",
	};
	Engine::ImGuiManager::Select("出現場所", EmitTypeLabels, data.emitType);

	static const char* DirectionTypeLabels[] = {
		"なし",
		"外向き/法線",
		"速度方向",
		"中心向き",
		"ランダム",
		"固定",
		"速度依存",
	};
	Engine::ImGuiManager::Select("初速方向", DirectionTypeLabels, data.directionType);

	// 出現
	ImGui::DragInt("出現量(中央値)", &data.emitData.count.median, 0.1f);
	ImGui::DragInt("出現量(振れ幅)", &data.emitData.count.range, 0.1f);
	// エミット制御も保存対象なので、エディタから状態を確認・変更できるようにする。
	ImGui::Checkbox("出現フラグ", &data.emitData.isEmit);
	ImGui::Checkbox("ループ", &data.emitData.isLoop);

	// 生存時間
	ImGui::DragFloat("生存時間(中央値)", &data.emitData.lifeTime.median, 0.1f);
	ImGui::DragFloat("生存時間(振れ幅)", &data.emitData.lifeTime.range, 0.1f);

	ImGui::Checkbox("重力", &data.isFlag.isGravity);
	ImGui::Checkbox("跳ねるか", &data.isFlag.isBounce);
	bool isLight = data.enableLighting;
	ImGui::Checkbox("ライティング", &isLight);
	data.enableLighting = isLight;
	// エミッター形状ラインと実際の発生位置マーカーを表示するか。
	ImGui::Checkbox("ライン描画", &data.isFlag.isLine);

	if (ImGui::CollapsingHeader("ビルボード")) {
		ImGui::Checkbox("ビルボードのz回転するか", &data.isFlag.billboardRotZ);
		ImGui::Checkbox("ビルボードするか", &data.isFlag.usebillboard);
		ImGui::Checkbox("ビルボードY軸するか", &data.isFlag.usebillboardY);
		ImGui::Checkbox("進行方向で回転", &data.emitData.isDirectionRotate);

	}
	if (ImGui::CollapsingHeader("UV")) {
		// パーティクルマテリアルのUVスクロール速度を保存対象として編集する。
		ImGui::DragFloat3("UV移動速度", &data.uvTransformVeloctiy.translate.x, 0.001f);
		ImGui::DragFloat3("UV回転速度", &data.uvTransformVeloctiy.rotate.x, 0.001f);
		ImGui::DragFloat3("UV拡縮速度", &data.uvTransformVeloctiy.scale.x, 0.001f);
	}
	if (ImGui::CollapsingHeader("速度")) {
		ImGui::DragFloat3("速度(中央値)", &data.emitData.velocity.median.x, 0.1f);
		ImGui::DragFloat3("速度(振れ幅)", &data.emitData.velocity.range.x, 0.1f);
		ImGui::Checkbox("ノイズを使用", &data.emitData.isNoise);
		ImGui::DragInt("ノイズ方向", &data.emitData.direction, 1.0f, 0, 3);

		ImGui::Checkbox("生存時間による速度変化を使用するか", &data.isFlag.isLifeTimeVelocity);
		ImGui::Checkbox("加速度を使用するか", &data.isFlag.isAcceleration);
		ImGui::DragFloat3("加速度(中央値)", &data.emitData.acceleration.median.x,0.1f);
		ImGui::DragFloat3("加速度(振れ幅)", &data.emitData.acceleration.range.x, 0.1f);
	}
	if (ImGui::CollapsingHeader("回転")) {
		ImGui::DragFloat3("回転(中央値)", &data.emitData.rotate.median.x, 0.1f);
		ImGui::DragFloat3("回転(振れ幅)", &data.emitData.rotate.range.x, 0.1f);
		ImGui::Checkbox("回転速度を使用するか", &data.isFlag.isRotateVelocity);
		ImGui::DragFloat3("回転速度(中央値)", &data.emitData.rotateVelocity.median.x, 0.1f);
		ImGui::DragFloat3("回転速度(振れ幅)", &data.emitData.rotateVelocity.range.x, 0.1f);

	}
	if (ImGui::CollapsingHeader("拡縮")) {
		ImGui::DragFloat3("サイズ(中央値)", &data.emitData.size.median.x, 0.1f);
		ImGui::DragFloat3("サイズ(振れ幅)", &data.emitData.size.range.x, 0.1f);
		ImGui::Checkbox("XYZ一律サイズ", &data.emitData.isUniformSize);
		ImGui::Checkbox("生存時間でのサイズ変化を使用するか", &data.isFlag.isLifeTimeScale_);
		ImGui::Checkbox("サイズ変化を使用するか", &data.isFlag.isScaling_);

		ImGui::DragFloat3("サイズ変化量(中央値)", &data.emitData.sizeAmount.median.x, 0.1f);
		ImGui::DragFloat3("サイズ変化量(振れ幅)", &data.emitData.sizeAmount.range.x, 0.1f);
		static const char* TopBottomLabels[] = {
		"Top",
		"Bottom",
		};

		Engine::ImGuiManager::Select("拡縮方向", TopBottomLabels, data.topBottom);

	}
	if (ImGui::CollapsingHeader("色")) {
		ImGui::ColorEdit4("最小値", &data.emitData.colorRange.min.x);
		ImGui::ColorEdit4("最大値", &data.emitData.colorRange.max.x);
		ImGui::Checkbox("透過するか", &data.isFlag.isAlpha);
		ImGui::DragFloat("透過クリップ値", &data.alphaClipping);
	}

	if (ImGui::CollapsingHeader("形状によってのパラメータ")) {
		switch (data.shapeType)
		{
		case EmitterShapeType::AABB:
		{
			ImGui::DragFloat3("AABB(最小値)", &data.rangeMin.x, 0.1f);
			ImGui::DragFloat3("AABB(最大値)", &data.rangeMax.x, 0.1f);
			if (data.rangeMin.x > data.rangeMax.x) {
				data.rangeMax.x = data.rangeMin.x;
			}
			if (data.rangeMin.y > data.rangeMax.y) {
				data.rangeMax.y = data.rangeMin.y;
			}
			if (data.rangeMin.z > data.rangeMax.z) {
				data.rangeMax.z = data.rangeMin.z;
			}
		}
		break;
		case EmitterShapeType::SPHERE:
		{
			ImGui::DragFloat("球の半径", &data.radius, 0.1f, 0.0f, 1000.0f);
		}
		break;
		case EmitterShapeType::CIRCLE:
		{
			ImGui::DragFloat("円の半径", &data.radius, 0.1f, 0.0f, 1000.0f);
			ImGui::SliderInt("円の分割数", &data.segment, 3, 128);
			if (data.segment < 3) {
				data.segment = 3;
			}
		}
		break;
		case EmitterShapeType::POINT:
		{

		}
		break;
		case EmitterShapeType::LINE:
		{
			ImGui::DragFloat3("線の開始位置", &data.lineStart.x, 0.1f);
			ImGui::DragFloat3("線の終了位置", &data.lineEnd.x, 0.1f);
		}
		break;
		case EmitterShapeType::SPLINE:
		{
			Engine::SplineParticleEmitter* splineEmit = effectComponent->GetEmitterAs<Engine::SplineParticleEmitter>(name);
			for (const auto& point : data.splinePoints) {
				splineEmit->AddControlPoints(point);
			}
		}
		break;
		case EmitterShapeType::TRIANGLE:
		{
			ImGui::DragFloat3("三角形 頂点0", &data.triangle.vertices[0].x, 0.1f);
			ImGui::DragFloat3("三角形 頂点1", &data.triangle.vertices[1].x, 0.1f);
			ImGui::DragFloat3("三角形 頂点2", &data.triangle.vertices[2].x, 0.1f);
		}
		break;
		case EmitterShapeType::MESH:
			break;
		default:
			break;
		}
	}
}

void EffectEditor::Emit(const std::string& name, const Vector3& pos) {
	Engine::BaseParticleEmitter* emit = effectComponent->GetBaseEmitter(name);
	if (emit == nullptr) return;	// エミッターが存在しない場合は終了
	// 出現
	emit->SetPos(pos);		// 位置
	emit->SetIsEmit(false); // 出さない
	emit->Update();			// 更新
	emit->SetIsEmit(true);	// 出す
	emit->Emit();			// エフェクト出現
	emit->SetIsEmit(false); // 出さない
};
