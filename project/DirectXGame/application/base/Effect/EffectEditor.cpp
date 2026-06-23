#include "EffectEditor.h"
#include "DirectXGame/engine/Base/Imgui/ImGuiUtility.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <set>

namespace {
	const char* kEffectRegistryGroup = "EffectEditor";
	const char* kDeletedEffectRegistryGroup = "EffectEditorDeleted";
	const char* kParticleRegistryGroup = "ParticleEditor";
	const char* kDeletedParticleRegistryGroup = "ParticleEditorDeleted";
	const char* kParticleDataPrefix = "ParticleGroup_";

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
}

void EffectEditor::Initialize(Engine::EffectComponent* effectComponent, 
	Engine::GlobalVariables* globalVariables) {

	this->effectComponent = effectComponent;
	this->globalVariables = globalVariables;
	Engine::ImGuiUtility::SetInputTextBuffer(newParticleTexturePathBuffer_, newParticleGroupData_.texturePath);

}

// 更新
void EffectEditor::Update(float dt) {
#ifdef _DEBUG
	ImGui::Begin("Effect");

	DrawParticleGroupEditor();
	DrawEffectManagement();
	
	if (effectGlobalDatas_.empty()) {
		ImGui::TextDisabled("エフェクトを追加してください。");
	}
	else {
		// エディターに表示変更するエフェクト選定
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
	

		// --- 選択されているブロックだけ表示 ---
		for (auto& combo : effectGlobalDatas_) {
			const std::string& name = combo.first;

			// 選択中だけ表示
			const bool nowChoice = (name == selectedBlockName_);

			if (!nowChoice) continue;

			// エディターでの調整
			AAAA(name,combo.second);
			// データの保存
			SetValue(name, combo.second);
		}

		// セーブ
		if (ImGui::Button("Save")) {
			for (auto& it : effectGlobalDatas_) {
				globalVariables->SaveFile(it.first);
			}
			for (auto& particle : effectComponent->GetParticleManager()->GetParticleGroups()) {
				// 手動セーブ時は現在のパーティクル群実体を外部保存データへ反映してから書き出す。
				Engine::ParticleGroupEditorData particleData =
					effectComponent->GetParticleManager()->GetEditorParticleGroupData(particle.first);
				AddParticleGroupItem(particle.first, particleData);
				SetParticleGroupValue(particle.first, particleData);
				globalVariables->SaveFile(kParticleDataPrefix + particle.first);
			}
			globalVariables->SaveFile(kEffectRegistryGroup);
			globalVariables->SaveFile(kDeletedEffectRegistryGroup);
			globalVariables->SaveFile(kParticleRegistryGroup);
			globalVariables->SaveFile(kDeletedParticleRegistryGroup);
		}
	}
	
	ImGui::End();
#endif // _DEBUG

#ifdef _DEBUG
	for (auto& [name, data] : effectGlobalDatas_) {
		GetValue(name, data);
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
	AddItem(name, data);
	// データを取得
	GetValue(name, data);
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
		GetValue(effectName, data);
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
		GetParticleGroupValue(particleName, data);
		if (!particleManager->GetParticleGroups().Contains(particleName)) {
			particleManager->CreateEditorParticleGroup(particleName, data);
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

void EffectEditor::DrawParticleGroupEditor() {
	Engine::ParticleManager* particleManager = effectComponent->GetParticleManager();

	if (!ImGui::CollapsingHeader("Particle Groups", ImGuiTreeNodeFlags_DefaultOpen)) {
		return;
	}

	ImGui::SeparatorText("Particle Group Management");
	Engine::ImGuiUtility::InputText("New Particle Group Name", newParticleGroupNameBuffer_);
	Engine::ImGuiUtility::InputText("New Texture Path", newParticleTexturePathBuffer_);

	static const char* ShapeLabels[] = {
		"None",
		"Plane",
		"Triangle",
		"Cross",
		"Cube",
		"Circle",
		"Star",
		"Crescent",
		"Ring",
		"Sphere",
		"Arrow",
		"Cylinder",
		"Tube",
		"Pyramid",
		"Torus",
	};
	Engine::ImGuiUtility::SelectEnum("New Shape", ShapeLabels, newParticleGroupData_.shapeType);

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

void EffectEditor::DrawParticleGroupDetail(const std::string& particleName, Engine::ParticleGroup& group) {
	Engine::ParticleManager* particleManager = effectComponent->GetParticleManager();
	Engine::ParticleGroupEditorData data = particleManager->GetEditorParticleGroupData(particleName);
	if (editParticleTexturePathBuffer_[0] == '\0') {
		Engine::ImGuiUtility::SetInputTextBuffer(editParticleTexturePathBuffer_, data.texturePath);
	}

	ImGui::SeparatorText("Particle Group Detail");
	bool recreate = false;
	bool saveData = false;

	if (Engine::ImGuiUtility::InputText("Texture Path", editParticleTexturePathBuffer_)) {
		data.texturePath = editParticleTexturePathBuffer_.data();
		saveData = true;
	}
	if (ImGui::Button("Apply Texture")) {
		data.texturePath = editParticleTexturePathBuffer_.data();
		if (group.material) {
			group.material->tex_.diffuseFilePath = data.texturePath;
			group.material->LoadTex();
		}
		saveData = true;
	}
	Engine::ImGuiUtility::HelpMarker("Texture Path is applied immediately with Apply Texture. Shape changes recreate the group.");

	static const char* ShapeLabels[] = {
		"None",
		"Plane",
		"Triangle",
		"Cross",
		"Cube",
		"Circle",
		"Star",
		"Crescent",
		"Ring",
		"Sphere",
		"Arrow",
		"Cylinder",
		"Tube",
		"Pyramid",
		"Torus",
	};
	if (Engine::ImGuiUtility::SelectEnum("Shape", ShapeLabels, data.shapeType)) {
		recreate = true;
	}
	if (DrawPrimitiveShapeParameters(data)) {
		saveData = true;
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
	if (ImGui::Checkbox("Billboard", &data.isFlag.usebillboard)) {
		saveData = true;
	}
	if (ImGui::Checkbox("Billboard Y", &data.isFlag.usebillboardY)) {
		saveData = true;
	}
	if (ImGui::Checkbox("Billboard Z Rotation", &data.isFlag.billboardRotZ)) {
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
			if (ImGui::Checkbox("Use Specular Map", &data.materialUseSpeculerMap)) {
				saveData = true;
			}
		}
	}

	if (recreate) {
		data.texturePath = editParticleTexturePathBuffer_.data();
		particleManager->RecreateEditorParticleGroup(particleName, data);
		selectedParticleGroupName_ = particleName;
		Engine::ImGuiUtility::SetInputTextBuffer(editParticleTexturePathBuffer_, data.texturePath);
		saveData = true;
	}

	if (saveData) {
		particleManager->ApplyEditorParticleGroupData(particleName, data);
		data = particleManager->GetEditorParticleGroupData(particleName);
		AddParticleGroupItem(particleName, data);
		SetParticleGroupValue(particleName, data);
		globalVariables->SaveFile(kParticleDataPrefix + particleName);
		globalVariables->SaveFile(kParticleRegistryGroup);
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
	AddItem(effectName, data);
	GetValue(effectName, data);
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

	SetValue(oldName, effectGlobalDatas_[oldName]);
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

	newParticleGroupData_.texturePath = newParticleTexturePathBuffer_.data();
	if (newParticleGroupData_.texturePath.empty()) {
		particleManagementMessage_ = "Texture path is required.";
		return;
	}
	if (newParticleGroupData_.shapeType == Engine::ShapeParameter::ShapeType::None ||
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
	AddParticleGroupItem(particleName, savedData);
	SetParticleGroupValue(particleName, savedData);
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
	AddParticleGroupItem(newName, data);
	SetParticleGroupValue(newName, data);
	globalVariables->SaveFile(kParticleDataPrefix + newName);
	globalVariables->SaveFile(kParticleRegistryGroup);
	globalVariables->SaveFile(kDeletedParticleRegistryGroup);
	for (auto& [effectName, effectData] : effectGlobalDatas_) {
		if (effectData.particleName == newName) {
			SetValue(effectName, effectData);
			globalVariables->SaveFile(effectName);
		}
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

	for (auto& [effectName, effectData] : effectGlobalDatas_) {
		if (effectData.particleName == particleName) {
			effectData.particleName.clear();
			SetValue(effectName, effectData);
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

void EffectEditor::AddParticleGroupItem(const std::string& particleName, const Engine::ParticleGroupEditorData& data) {
	const std::string groupName = kParticleDataPrefix + particleName;
	globalVariables->SetGroupCategory(groupName, "Effect/Particle");
	globalVariables->CreateGroup(groupName);
	// パーティクル群を外部ファイルから復元するため、生成情報と実行時の編集値をまとめて登録する。
	globalVariables->AddItem(groupName, "texturePath", data.texturePath);
	globalVariables->AddEnumItem<Engine::ShapeParameter::ShapeType>(groupName, "shapeType", data.shapeType, "ShapeType");
	globalVariables->AddItem(groupName, "shape.plane.width", data.plane.width);
	globalVariables->AddItem(groupName, "shape.plane.height", data.plane.height);
	globalVariables->AddItem(groupName, "shape.triangle.upPos", data.triangle.upPos);
	globalVariables->AddItem(groupName, "shape.triangle.leftPos", data.triangle.leftPos);
	globalVariables->AddItem(groupName, "shape.triangle.rightPos", data.triangle.rightPos);
	globalVariables->AddItem(groupName, "shape.cross.armLength", data.cross.armLength);
	globalVariables->AddItem(groupName, "shape.cross.armWidth", data.cross.armWidth);
	globalVariables->AddItem(groupName, "shape.cube.size", data.cube.size);
	globalVariables->AddItem(groupName, "shape.circle.radius", data.circle.radius);
	globalVariables->AddItem(groupName, "shape.circle.segments", data.circle.segments);
	globalVariables->AddItem(groupName, "shape.star.innerRadius", data.star.innerRadius);
	globalVariables->AddItem(groupName, "shape.star.outerRadius", data.star.outerRadius);
	globalVariables->AddItem(groupName, "shape.star.segments", data.star.segments);
	globalVariables->AddItem(groupName, "shape.crescent.innerRadius", data.crescent.innerRadius);
	globalVariables->AddItem(groupName, "shape.crescent.outerRadius", data.crescent.outerRadius);
	globalVariables->AddItem(groupName, "shape.crescent.distance", data.crescent.distance);
	globalVariables->AddItem(groupName, "shape.crescent.segments", data.crescent.segments);
	globalVariables->AddItem(groupName, "shape.ring.innerRadius", data.ring.innerRadius);
	globalVariables->AddItem(groupName, "shape.ring.outerRadius", data.ring.outerRadius);
	globalVariables->AddItem(groupName, "shape.ring.segments", data.ring.segments);
	globalVariables->AddItem(groupName, "shape.sphere.radius", data.sphere.radius);
	globalVariables->AddItem(groupName, "shape.sphere.latitudeSegments", data.sphere.latitudeSegments);
	globalVariables->AddItem(groupName, "shape.sphere.longitudeSegments", data.sphere.longitudeSegments);
	globalVariables->AddItem(groupName, "shape.sphere.isTopBased", data.sphere.isTopBased);
	globalVariables->AddItem(groupName, "shape.arrow.shaftLength", data.arrow.shaftLength);
	globalVariables->AddItem(groupName, "shape.arrow.shaftRadius", data.arrow.shaftRadius);
	globalVariables->AddItem(groupName, "shape.arrow.headLength", data.arrow.headLength);
	globalVariables->AddItem(groupName, "shape.arrow.headRadius", data.arrow.headRadius);
	globalVariables->AddItem(groupName, "shape.arrow.segments", data.arrow.segments);
	globalVariables->AddItem(groupName, "shape.cylinder.innerRadius", data.cylinder.innerRadius);
	globalVariables->AddItem(groupName, "shape.cylinder.outerRadius", data.cylinder.outerRadius);
	globalVariables->AddItem(groupName, "shape.cylinder.height", data.cylinder.height);
	globalVariables->AddItem(groupName, "shape.cylinder.segments", data.cylinder.segments);
	globalVariables->AddItem(groupName, "shape.cylinder.isCover", data.cylinder.isCover);
	globalVariables->AddItem(groupName, "shape.tube.radius", data.tube.radius);
	globalVariables->AddItem(groupName, "shape.tube.innerRadius", data.tube.innerRadius);
	globalVariables->AddItem(groupName, "shape.tube.height", data.tube.height);
	globalVariables->AddItem(groupName, "shape.tube.segments", data.tube.segments);
	globalVariables->AddItem(groupName, "shape.pyramid.radius", data.pyramid.radius);
	globalVariables->AddItem(groupName, "shape.pyramid.height", data.pyramid.height);
	globalVariables->AddItem(groupName, "shape.pyramid.segments", data.pyramid.segments);
	globalVariables->AddItem(groupName, "shape.torus.innerRadius", data.torus.innerRadius);
	globalVariables->AddItem(groupName, "shape.torus.outerRadius", data.torus.outerRadius);
	globalVariables->AddItem(groupName, "shape.torus.segments", data.torus.segments);
	globalVariables->AddItem(groupName, "shape.torus.tubeSegments", data.torus.tubeSegments);
	globalVariables->AddEnumItem<EmitData::RasterizerType>(groupName, "rasterizerType", data.rasterizerType, "RasterizerType");
	globalVariables->AddEnumItem<EmitData::BlendType>(groupName, "blendType", data.blendType, "BlendType");
	globalVariables->AddItem(groupName, "isEditorPrimitive", data.isEditorPrimitive);
	globalVariables->AddItem(groupName, "isUVClamp", data.isUVClamp);
	globalVariables->AddItem(groupName, "uvTransformVelocity", data.uvTransformVelocity);
	globalVariables->AddItem(groupName, "isFlag.usebillboard", data.isFlag.usebillboard);
	globalVariables->AddItem(groupName, "isFlag.usebillboardY", data.isFlag.usebillboardY);
	globalVariables->AddItem(groupName, "isFlag.billboardRotZ", data.isFlag.billboardRotZ);
	globalVariables->AddItem(groupName, "isFlag.isAlpha", data.isFlag.isAlpha);
	globalVariables->AddItem(groupName, "isFlag.isLine", data.isFlag.isLine);
	globalVariables->AddItem(groupName, "isFlag.isGravity", data.isFlag.isGravity);
	globalVariables->AddItem(groupName, "isFlag.isLifeTimeScale", data.isFlag.isLifeTimeScale_);
	globalVariables->AddItem(groupName, "isFlag.isRotateVelocity", data.isFlag.isRotateVelocity);
	globalVariables->AddItem(groupName, "isFlag.isLifeTimeVelocity", data.isFlag.isLifeTimeVelocity);
	globalVariables->AddItem(groupName, "isFlag.isBounce", data.isFlag.isBounce);
	globalVariables->AddItem(groupName, "isFlag.isAcceleration", data.isFlag.isAcceleration);
	globalVariables->AddItem(groupName, "isFlag.isLineInterpolation", data.isFlag.isLineInterpolation);
	globalVariables->AddItem(groupName, "isFlag.isScaling", data.isFlag.isScaling_);
	globalVariables->AddEnumItem<EmitData::EmitType>(groupName, "emitType", data.emitType, "EmitType");
	globalVariables->AddEnumItem<EmitData::TopBottom>(groupName, "topBottom", data.topBottom, "TopBottom");
	globalVariables->AddItem(groupName, "gravitationalAcceleration", data.gravitationalAcceleration);
	globalVariables->AddItem(groupName, "material.transform", data.materialTransform);
	globalVariables->AddItem(groupName, "material.color", data.materialColor);
	globalVariables->AddItem(groupName, "material.enableLighting", data.materialEnableLighting);
	globalVariables->AddItem(groupName, "material.environmentCoefficient", data.materialEnvironmentCoefficient);
	globalVariables->AddItem(groupName, "material.shininess", data.materialShininess);
	globalVariables->AddItem(groupName, "material.useLig", data.materialUseLig);
	globalVariables->AddItem(groupName, "material.useNormalMap", data.materialUseNormalMap);
	globalVariables->AddItem(groupName, "material.useSpeculerMap", data.materialUseSpeculerMap);
	globalVariables->AddItem(groupName, "material.useEnvironment", data.materialUseEnvironment);
	globalVariables->AddItem(groupName, "material.alphaClipping", data.materialAlphaClipping);
	globalVariables->AddItem(groupName, "material.alpha", data.materialAlpha);
}

void EffectEditor::GetParticleGroupValue(const std::string& particleName, Engine::ParticleGroupEditorData& data) {
	const std::string groupName = kParticleDataPrefix + particleName;
	// 旧形式の保存ファイルでも読み込めるよう、追加項目はキーがある場合だけ上書きする。
	data.texturePath = globalVariables->GetValue<std::string>(groupName, "texturePath");
	data.shapeType = globalVariables->GetEnumValue<Engine::ShapeParameter::ShapeType>(groupName, "shapeType");
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
	data.rasterizerType = globalVariables->GetEnumValue<EmitData::RasterizerType>(groupName, "rasterizerType");
	data.blendType = globalVariables->GetEnumValue<EmitData::BlendType>(groupName, "blendType");
	data.isEditorPrimitive = globalVariables->GetValue<bool>(groupName, "isEditorPrimitive");
	if (globalVariables->HasKey(groupName, "isUVClamp")) data.isUVClamp = globalVariables->GetValue<bool>(groupName, "isUVClamp");
	if (globalVariables->HasKey(groupName, "uvTransformVelocity")) data.uvTransformVelocity = globalVariables->GetValue<Transform>(groupName, "uvTransformVelocity");
	if (globalVariables->HasKey(groupName, "isFlag.usebillboard")) data.isFlag.usebillboard = globalVariables->GetValue<bool>(groupName, "isFlag.usebillboard");
	if (globalVariables->HasKey(groupName, "isFlag.usebillboardY")) data.isFlag.usebillboardY = globalVariables->GetValue<bool>(groupName, "isFlag.usebillboardY");
	if (globalVariables->HasKey(groupName, "isFlag.billboardRotZ")) data.isFlag.billboardRotZ = globalVariables->GetValue<bool>(groupName, "isFlag.billboardRotZ");
	if (globalVariables->HasKey(groupName, "isFlag.isAlpha")) data.isFlag.isAlpha = globalVariables->GetValue<bool>(groupName, "isFlag.isAlpha");
	if (globalVariables->HasKey(groupName, "isFlag.isLine")) data.isFlag.isLine = globalVariables->GetValue<bool>(groupName, "isFlag.isLine");
	if (globalVariables->HasKey(groupName, "isFlag.isGravity")) data.isFlag.isGravity = globalVariables->GetValue<bool>(groupName, "isFlag.isGravity");
	if (globalVariables->HasKey(groupName, "isFlag.isLifeTimeScale")) data.isFlag.isLifeTimeScale_ = globalVariables->GetValue<bool>(groupName, "isFlag.isLifeTimeScale");
	if (globalVariables->HasKey(groupName, "isFlag.isRotateVelocity")) data.isFlag.isRotateVelocity = globalVariables->GetValue<bool>(groupName, "isFlag.isRotateVelocity");
	if (globalVariables->HasKey(groupName, "isFlag.isLifeTimeVelocity")) data.isFlag.isLifeTimeVelocity = globalVariables->GetValue<bool>(groupName, "isFlag.isLifeTimeVelocity");
	if (globalVariables->HasKey(groupName, "isFlag.isBounce")) data.isFlag.isBounce = globalVariables->GetValue<bool>(groupName, "isFlag.isBounce");
	if (globalVariables->HasKey(groupName, "isFlag.isAcceleration")) data.isFlag.isAcceleration = globalVariables->GetValue<bool>(groupName, "isFlag.isAcceleration");
	if (globalVariables->HasKey(groupName, "isFlag.isLineInterpolation")) data.isFlag.isLineInterpolation = globalVariables->GetValue<bool>(groupName, "isFlag.isLineInterpolation");
	if (globalVariables->HasKey(groupName, "isFlag.isScaling")) data.isFlag.isScaling_ = globalVariables->GetValue<bool>(groupName, "isFlag.isScaling");
	if (globalVariables->HasKey(groupName, "emitType")) data.emitType = globalVariables->GetEnumValue<EmitData::EmitType>(groupName, "emitType");
	if (globalVariables->HasKey(groupName, "topBottom")) data.topBottom = globalVariables->GetEnumValue<EmitData::TopBottom>(groupName, "topBottom");
	if (globalVariables->HasKey(groupName, "gravitationalAcceleration")) data.gravitationalAcceleration = globalVariables->GetValue<float>(groupName, "gravitationalAcceleration");
	if (globalVariables->HasKey(groupName, "material.transform")) data.materialTransform = globalVariables->GetValue<Transform>(groupName, "material.transform");
	if (globalVariables->HasKey(groupName, "material.color")) data.materialColor = globalVariables->GetValue<Vector4>(groupName, "material.color");
	if (globalVariables->HasKey(groupName, "material.enableLighting")) data.materialEnableLighting = globalVariables->GetValue<bool>(groupName, "material.enableLighting");
	if (globalVariables->HasKey(groupName, "material.environmentCoefficient")) data.materialEnvironmentCoefficient = globalVariables->GetValue<float>(groupName, "material.environmentCoefficient");
	if (globalVariables->HasKey(groupName, "material.shininess")) data.materialShininess = globalVariables->GetValue<float>(groupName, "material.shininess");
	if (globalVariables->HasKey(groupName, "material.useLig")) data.materialUseLig = globalVariables->GetValue<bool>(groupName, "material.useLig");
	if (globalVariables->HasKey(groupName, "material.useNormalMap")) data.materialUseNormalMap = globalVariables->GetValue<bool>(groupName, "material.useNormalMap");
	if (globalVariables->HasKey(groupName, "material.useSpeculerMap")) data.materialUseSpeculerMap = globalVariables->GetValue<bool>(groupName, "material.useSpeculerMap");
	if (globalVariables->HasKey(groupName, "material.useEnvironment")) data.materialUseEnvironment = globalVariables->GetValue<bool>(groupName, "material.useEnvironment");
	if (globalVariables->HasKey(groupName, "material.alphaClipping")) data.materialAlphaClipping = globalVariables->GetValue<float>(groupName, "material.alphaClipping");
	if (globalVariables->HasKey(groupName, "material.alpha")) data.materialAlpha = globalVariables->GetValue<float>(groupName, "material.alpha");
}

void EffectEditor::SetParticleGroupValue(const std::string& particleName, const Engine::ParticleGroupEditorData& data) {
	const std::string groupName = kParticleDataPrefix + particleName;
	// AddItemでキーを揃えたあと、現在の編集内容で外部保存用データを上書きする。
	globalVariables->SetValue(groupName, "texturePath", data.texturePath);
	globalVariables->SetEnumValue<Engine::ShapeParameter::ShapeType>(groupName, "shapeType", data.shapeType, "ShapeType");
	globalVariables->SetValue(groupName, "shape.plane.width", data.plane.width);
	globalVariables->SetValue(groupName, "shape.plane.height", data.plane.height);
	globalVariables->SetValue(groupName, "shape.triangle.upPos", data.triangle.upPos);
	globalVariables->SetValue(groupName, "shape.triangle.leftPos", data.triangle.leftPos);
	globalVariables->SetValue(groupName, "shape.triangle.rightPos", data.triangle.rightPos);
	globalVariables->SetValue(groupName, "shape.cross.armLength", data.cross.armLength);
	globalVariables->SetValue(groupName, "shape.cross.armWidth", data.cross.armWidth);
	globalVariables->SetValue(groupName, "shape.cube.size", data.cube.size);
	globalVariables->SetValue(groupName, "shape.circle.radius", data.circle.radius);
	globalVariables->SetValue(groupName, "shape.circle.segments", data.circle.segments);
	globalVariables->SetValue(groupName, "shape.star.innerRadius", data.star.innerRadius);
	globalVariables->SetValue(groupName, "shape.star.outerRadius", data.star.outerRadius);
	globalVariables->SetValue(groupName, "shape.star.segments", data.star.segments);
	globalVariables->SetValue(groupName, "shape.crescent.innerRadius", data.crescent.innerRadius);
	globalVariables->SetValue(groupName, "shape.crescent.outerRadius", data.crescent.outerRadius);
	globalVariables->SetValue(groupName, "shape.crescent.distance", data.crescent.distance);
	globalVariables->SetValue(groupName, "shape.crescent.segments", data.crescent.segments);
	globalVariables->SetValue(groupName, "shape.ring.innerRadius", data.ring.innerRadius);
	globalVariables->SetValue(groupName, "shape.ring.outerRadius", data.ring.outerRadius);
	globalVariables->SetValue(groupName, "shape.ring.segments", data.ring.segments);
	globalVariables->SetValue(groupName, "shape.sphere.radius", data.sphere.radius);
	globalVariables->SetValue(groupName, "shape.sphere.latitudeSegments", data.sphere.latitudeSegments);
	globalVariables->SetValue(groupName, "shape.sphere.longitudeSegments", data.sphere.longitudeSegments);
	globalVariables->SetValue(groupName, "shape.sphere.isTopBased", data.sphere.isTopBased);
	globalVariables->SetValue(groupName, "shape.arrow.shaftLength", data.arrow.shaftLength);
	globalVariables->SetValue(groupName, "shape.arrow.shaftRadius", data.arrow.shaftRadius);
	globalVariables->SetValue(groupName, "shape.arrow.headLength", data.arrow.headLength);
	globalVariables->SetValue(groupName, "shape.arrow.headRadius", data.arrow.headRadius);
	globalVariables->SetValue(groupName, "shape.arrow.segments", data.arrow.segments);
	globalVariables->SetValue(groupName, "shape.cylinder.innerRadius", data.cylinder.innerRadius);
	globalVariables->SetValue(groupName, "shape.cylinder.outerRadius", data.cylinder.outerRadius);
	globalVariables->SetValue(groupName, "shape.cylinder.height", data.cylinder.height);
	globalVariables->SetValue(groupName, "shape.cylinder.segments", data.cylinder.segments);
	globalVariables->SetValue(groupName, "shape.cylinder.isCover", data.cylinder.isCover);
	globalVariables->SetValue(groupName, "shape.tube.radius", data.tube.radius);
	globalVariables->SetValue(groupName, "shape.tube.innerRadius", data.tube.innerRadius);
	globalVariables->SetValue(groupName, "shape.tube.height", data.tube.height);
	globalVariables->SetValue(groupName, "shape.tube.segments", data.tube.segments);
	globalVariables->SetValue(groupName, "shape.pyramid.radius", data.pyramid.radius);
	globalVariables->SetValue(groupName, "shape.pyramid.height", data.pyramid.height);
	globalVariables->SetValue(groupName, "shape.pyramid.segments", data.pyramid.segments);
	globalVariables->SetValue(groupName, "shape.torus.innerRadius", data.torus.innerRadius);
	globalVariables->SetValue(groupName, "shape.torus.outerRadius", data.torus.outerRadius);
	globalVariables->SetValue(groupName, "shape.torus.segments", data.torus.segments);
	globalVariables->SetValue(groupName, "shape.torus.tubeSegments", data.torus.tubeSegments);
	globalVariables->SetEnumValue<EmitData::RasterizerType>(groupName, "rasterizerType", data.rasterizerType, "RasterizerType");
	globalVariables->SetEnumValue<EmitData::BlendType>(groupName, "blendType", data.blendType, "BlendType");
	globalVariables->SetValue(groupName, "isEditorPrimitive", data.isEditorPrimitive);
	globalVariables->SetValue(groupName, "isUVClamp", data.isUVClamp);
	globalVariables->SetValue(groupName, "uvTransformVelocity", data.uvTransformVelocity);
	globalVariables->SetValue(groupName, "isFlag.usebillboard", data.isFlag.usebillboard);
	globalVariables->SetValue(groupName, "isFlag.usebillboardY", data.isFlag.usebillboardY);
	globalVariables->SetValue(groupName, "isFlag.billboardRotZ", data.isFlag.billboardRotZ);
	globalVariables->SetValue(groupName, "isFlag.isAlpha", data.isFlag.isAlpha);
	globalVariables->SetValue(groupName, "isFlag.isLine", data.isFlag.isLine);
	globalVariables->SetValue(groupName, "isFlag.isGravity", data.isFlag.isGravity);
	globalVariables->SetValue(groupName, "isFlag.isLifeTimeScale", data.isFlag.isLifeTimeScale_);
	globalVariables->SetValue(groupName, "isFlag.isRotateVelocity", data.isFlag.isRotateVelocity);
	globalVariables->SetValue(groupName, "isFlag.isLifeTimeVelocity", data.isFlag.isLifeTimeVelocity);
	globalVariables->SetValue(groupName, "isFlag.isBounce", data.isFlag.isBounce);
	globalVariables->SetValue(groupName, "isFlag.isAcceleration", data.isFlag.isAcceleration);
	globalVariables->SetValue(groupName, "isFlag.isLineInterpolation", data.isFlag.isLineInterpolation);
	globalVariables->SetValue(groupName, "isFlag.isScaling", data.isFlag.isScaling_);
	globalVariables->SetEnumValue<EmitData::EmitType>(groupName, "emitType", data.emitType, "EmitType");
	globalVariables->SetEnumValue<EmitData::TopBottom>(groupName, "topBottom", data.topBottom, "TopBottom");
	globalVariables->SetValue(groupName, "gravitationalAcceleration", data.gravitationalAcceleration);
	globalVariables->SetValue(groupName, "material.transform", data.materialTransform);
	globalVariables->SetValue(groupName, "material.color", data.materialColor);
	globalVariables->SetValue(groupName, "material.enableLighting", data.materialEnableLighting);
	globalVariables->SetValue(groupName, "material.environmentCoefficient", data.materialEnvironmentCoefficient);
	globalVariables->SetValue(groupName, "material.shininess", data.materialShininess);
	globalVariables->SetValue(groupName, "material.useLig", data.materialUseLig);
	globalVariables->SetValue(groupName, "material.useNormalMap", data.materialUseNormalMap);
	globalVariables->SetValue(groupName, "material.useSpeculerMap", data.materialUseSpeculerMap);
	globalVariables->SetValue(groupName, "material.useEnvironment", data.materialUseEnvironment);
	globalVariables->SetValue(groupName, "material.alphaClipping", data.materialAlphaClipping);
	globalVariables->SetValue(groupName, "material.alpha", data.materialAlpha);
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

void EffectEditor::AddItem(const std::string& name, const EffectGlobalData& data) {
	globalVariables->SetGroupCategory(name, "Effect");
	globalVariables->CreateGroup(name);
	globalVariables->AddItem(name, "particleName", data.particleName);
	globalVariables->AddItem(name, "frequency", data.frequency);
	globalVariables->AddItem(name, "emitData.acceleration.median", data.emitData.acceleration.median);
	globalVariables->AddItem(name, "emitData.acceleration.range", data.emitData.acceleration.range);
	globalVariables->AddItem(name, "emitData.colorRange.min", data.emitData.colorRange.min);
	globalVariables->AddItem(name, "emitData.colorRange.max", data.emitData.colorRange.max);
	globalVariables->AddItem(name, "emitData.count.median", data.emitData.count.median);
	globalVariables->AddItem(name, "emitData.count.range", data.emitData.count.range);
	globalVariables->AddItem(name, "emitData.lifeTime.median", data.emitData.lifeTime.median);
	globalVariables->AddItem(name, "emitData.lifeTime.range", data.emitData.lifeTime.range);
	globalVariables->AddItem(name, "emitData.rotate.median", data.emitData.rotate.median);
	globalVariables->AddItem(name, "emitData.rotate.range", data.emitData.rotate.range);
	globalVariables->AddItem(name, "emitData.rotateVelocity.median", data.emitData.rotateVelocity.median);
	globalVariables->AddItem(name, "emitData.rotateVelocity.range", data.emitData.rotateVelocity.range);
	globalVariables->AddItem(name, "emitData.size.median", data.emitData.size.median);
	globalVariables->AddItem(name, "emitData.size.range", data.emitData.size.range);
	globalVariables->AddItem(name, "emitData.sizeAmount.median", data.emitData.sizeAmount.median);
	globalVariables->AddItem(name, "emitData.sizeAmount.range", data.emitData.sizeAmount.range);
	globalVariables->AddItem(name, "emitData.velocity.median", data.emitData.velocity.median);
	globalVariables->AddItem(name, "emitData.velocity.range", data.emitData.velocity.range);

	globalVariables->AddItem(name, "emitData.alphaClipping", data.alphaClipping);
	globalVariables->AddItem(name, "emitData.enableLighting", data.enableLighting);

	globalVariables->AddItem(name, "emitData.isFlag.billboardRotZ", data.isFlag.billboardRotZ);
	globalVariables->AddItem(name, "emitData.isFlag.isAcceleration", data.isFlag.isAcceleration);
	globalVariables->AddItem(name, "emitData.isFlag.isAlpha", data.isFlag.isAlpha);
	globalVariables->AddItem(name, "emitData.isFlag.isBounce", data.isFlag.isBounce);
	globalVariables->AddItem(name, "emitData.isFlag.isGravity", data.isFlag.isGravity);
	globalVariables->AddItem(name, "emitData.isFlag.isLifeTimeScale", data.isFlag.isLifeTimeScale_);
	globalVariables->AddItem(name, "emitData.isFlag.isLifeTimeVelocity", data.isFlag.isLifeTimeVelocity);
	globalVariables->AddItem(name, "emitData.isFlag.isLine", data.isFlag.isLine);
	globalVariables->AddItem(name, "emitData.isFlag.isLineInterpolation", data.isFlag.isLineInterpolation);
	globalVariables->AddItem(name, "emitData.isFlag.isRotateVelocity", data.isFlag.isRotateVelocity);
	globalVariables->AddItem(name, "emitData.isFlag.isScaling", data.isFlag.isScaling_);
	globalVariables->AddItem(name, "emitData.isFlag.usebillboard", data.isFlag.usebillboard);
	globalVariables->AddItem(name, "emitData.isFlag.usebillboardY", data.isFlag.usebillboardY);


	globalVariables->AddItem(name, "emitData.lineEnd", data.lineEnd);
	globalVariables->AddItem(name, "emitData.lineStart", data.lineStart);

	globalVariables->AddItem(name, "emitData.radius", data.radius);

	globalVariables->AddItem(name, "emitData.rangeMax", data.rangeMax);
	globalVariables->AddItem(name, "emitData.rangeMin", data.rangeMin);

	globalVariables->AddItem(name, "emitData.segment", data.segment);

	globalVariables->AddEnumItem<EmitterShapeType>(name, "emitData.shapeType", data.shapeType, "EmitterShapeType");
	globalVariables->AddEnumItem<EmitData::EmitType>(name, "emitData.emitType", data.emitType, "EmitType");
	globalVariables->AddEnumItem<EmitData::DirectionType>(name, "emitData.directionType", data.directionType, "DirectionType");
	globalVariables->AddEnumItem<EmitData::TopBottom>(name, "emitData.topBottom", data.topBottom, "TopBottom");
}

void EffectEditor::GetValue(const std::string& name, EffectGlobalData& data) {
	data.particleName = globalVariables->GetValue<std::string>(name, "particleName");

	data.frequency = globalVariables->GetValue<float>(name, "frequency");
	data.emitData.acceleration.median = globalVariables->GetValue<Vector3>(name, "emitData.acceleration.median");
	data.emitData.acceleration.range = globalVariables->GetValue<Vector3>(name, "emitData.acceleration.range");
	data.emitData.colorRange.min = globalVariables->GetValue<Vector4>(name, "emitData.colorRange.min");
	data.emitData.colorRange.max = globalVariables->GetValue<Vector4>(name, "emitData.colorRange.max");
	data.emitData.count.median = globalVariables->GetValue<int>(name, "emitData.count.median");
	data.emitData.count.range = globalVariables->GetValue<int>(name, "emitData.count.range");
	data.emitData.lifeTime.median = globalVariables->GetValue<float>(name, "emitData.lifeTime.median");
	data.emitData.lifeTime.range = globalVariables->GetValue<float>(name, "emitData.lifeTime.range");
	data.emitData.rotate.median = globalVariables->GetValue<Vector3>(name, "emitData.rotate.median");
	data.emitData.rotate.range = globalVariables->GetValue<Vector3>(name, "emitData.rotate.range");
	data.emitData.rotateVelocity.median = globalVariables->GetValue<Vector3>(name, "emitData.rotateVelocity.median");
	data.emitData.rotateVelocity.range = globalVariables->GetValue<Vector3>(name, "emitData.rotateVelocity.range");
	data.emitData.size.median = globalVariables->GetValue<Vector3>(name, "emitData.size.median");
	data.emitData.size.range = globalVariables->GetValue<Vector3>(name, "emitData.size.range");
	data.emitData.sizeAmount.median = globalVariables->GetValue<Vector3>(name, "emitData.sizeAmount.median");
	data.emitData.sizeAmount.range = globalVariables->GetValue<Vector3>(name, "emitData.sizeAmount.range");
	data.emitData.velocity.median = globalVariables->GetValue<Vector3>(name, "emitData.velocity.median");
	data.emitData.velocity.range = globalVariables->GetValue<Vector3>(name, "emitData.velocity.range");

	data.alphaClipping = globalVariables->GetValue<float>(name, "emitData.alphaClipping");
	data.enableLighting = globalVariables->GetValue<int>(name, "emitData.enableLighting");

	data.isFlag.billboardRotZ = globalVariables->GetValue<bool>(name, "emitData.isFlag.billboardRotZ");
	data.isFlag.isAcceleration = globalVariables->GetValue<bool>(name, "emitData.isFlag.isAcceleration");
	data.isFlag.isAlpha = globalVariables->GetValue<bool>(name, "emitData.isFlag.isAlpha");
	data.isFlag.isBounce = globalVariables->GetValue<bool>(name, "emitData.isFlag.isBounce");
	data.isFlag.isGravity = globalVariables->GetValue<bool>(name, "emitData.isFlag.isGravity");
	data.isFlag.isLifeTimeScale_ = globalVariables->GetValue<bool>(name, "emitData.isFlag.isLifeTimeScale");
	data.isFlag.isLifeTimeVelocity = globalVariables->GetValue<bool>(name, "emitData.isFlag.isLifeTimeVelocity");
	data.isFlag.isLine = globalVariables->GetValue<bool>(name, "emitData.isFlag.isLine");
	data.isFlag.isLineInterpolation = globalVariables->GetValue<bool>(name, "emitData.isFlag.isLineInterpolation");
	data.isFlag.isRotateVelocity = globalVariables->GetValue<bool>(name, "emitData.isFlag.isRotateVelocity");
	data.isFlag.isScaling_ = globalVariables->GetValue<bool>(name, "emitData.isFlag.isScaling");
	data.isFlag.usebillboard = globalVariables->GetValue<bool>(name, "emitData.isFlag.usebillboard");
	data.isFlag.usebillboardY = globalVariables->GetValue<bool>(name, "emitData.isFlag.usebillboardY");


	data.lineEnd = globalVariables->GetValue<Vector3>(name, "emitData.lineEnd");
	data.lineStart = globalVariables->GetValue<Vector3>(name, "emitData.lineStart");

	data.radius = globalVariables->GetValue<float>(name, "emitData.radius");

	data.rangeMax = globalVariables->GetValue<Vector3>(name, "emitData.rangeMax");
	data.rangeMin = globalVariables->GetValue<Vector3>(name, "emitData.rangeMin");

	data.segment = globalVariables->GetValue<int>(name, "emitData.segment");

	data.shapeType = globalVariables->GetEnumValue<EmitterShapeType>(name, "emitData.shapeType");
	if (globalVariables->HasKey(name, "emitData.emitType")) {
		data.emitType = globalVariables->GetEnumValue<EmitData::EmitType>(name, "emitData.emitType");
	}
	if (globalVariables->HasKey(name, "emitData.directionType")) {
		data.directionType = globalVariables->GetEnumValue<EmitData::DirectionType>(name, "emitData.directionType");
	}
	data.topBottom = globalVariables->GetEnumValue<EmitData::TopBottom>(name, "emitData.topBottom");
}

void EffectEditor::SetValue(const std::string& name, const EffectGlobalData& data) {
	globalVariables->SetValue(name, "particleName", data.particleName);
	globalVariables->SetValue(name, "frequency", data.frequency);
	globalVariables->SetValue(name, "emitData.acceleration.median", data.emitData.acceleration.median);
	globalVariables->SetValue(name, "emitData.acceleration.range", data.emitData.acceleration.range);
	globalVariables->SetValue(name, "emitData.colorRange.min", data.emitData.colorRange.min);
	globalVariables->SetValue(name, "emitData.colorRange.max", data.emitData.colorRange.max);
	globalVariables->SetValue(name, "emitData.count.median", data.emitData.count.median);
	globalVariables->SetValue(name, "emitData.count.range", data.emitData.count.range);
	globalVariables->SetValue(name, "emitData.lifeTime.median", data.emitData.lifeTime.median);
	globalVariables->SetValue(name, "emitData.lifeTime.range", data.emitData.lifeTime.range);
	globalVariables->SetValue(name, "emitData.rotate.median", data.emitData.rotate.median);
	globalVariables->SetValue(name, "emitData.rotate.range", data.emitData.rotate.range);
	globalVariables->SetValue(name, "emitData.rotateVelocity.median", data.emitData.rotateVelocity.median);
	globalVariables->SetValue(name, "emitData.rotateVelocity.range", data.emitData.rotateVelocity.range);
	globalVariables->SetValue(name, "emitData.size.median", data.emitData.size.median);
	globalVariables->SetValue(name, "emitData.size.range", data.emitData.size.range);
	globalVariables->SetValue(name, "emitData.sizeAmount.median", data.emitData.sizeAmount.median);
	globalVariables->SetValue(name, "emitData.sizeAmount.range", data.emitData.sizeAmount.range);
	globalVariables->SetValue(name, "emitData.velocity.median", data.emitData.velocity.median);
	globalVariables->SetValue(name, "emitData.velocity.range", data.emitData.velocity.range);

	globalVariables->SetValue(name, "emitData.alphaClipping", data.alphaClipping);
	globalVariables->SetValue(name, "emitData.enableLighting", data.enableLighting);

	globalVariables->SetValue(name, "emitData.isFlag.billboardRotZ", data.isFlag.billboardRotZ);
	globalVariables->SetValue(name, "emitData.isFlag.isAcceleration", data.isFlag.isAcceleration);
	globalVariables->SetValue(name, "emitData.isFlag.isAlpha", data.isFlag.isAlpha);
	globalVariables->SetValue(name, "emitData.isFlag.isBounce", data.isFlag.isBounce);
	globalVariables->SetValue(name, "emitData.isFlag.isGravity", data.isFlag.isGravity);
	globalVariables->SetValue(name, "emitData.isFlag.isLifeTimeScale", data.isFlag.isLifeTimeScale_);
	globalVariables->SetValue(name, "emitData.isFlag.isLifeTimeVelocity", data.isFlag.isLifeTimeVelocity);
	globalVariables->SetValue(name, "emitData.isFlag.isLine", data.isFlag.isLine);
	globalVariables->SetValue(name, "emitData.isFlag.isLineInterpolation", data.isFlag.isLineInterpolation);
	globalVariables->SetValue(name, "emitData.isFlag.isRotateVelocity", data.isFlag.isRotateVelocity);
	globalVariables->SetValue(name, "emitData.isFlag.isScaling", data.isFlag.isScaling_);
	globalVariables->SetValue(name, "emitData.isFlag.usebillboard", data.isFlag.usebillboard);
	globalVariables->SetValue(name, "emitData.isFlag.usebillboardY", data.isFlag.usebillboardY);


	globalVariables->SetValue(name, "emitData.lineEnd", data.lineEnd);
	globalVariables->SetValue(name, "emitData.lineStart", data.lineStart);

	globalVariables->SetValue(name, "emitData.radius", data.radius);

	globalVariables->SetValue(name, "emitData.rangeMax", data.rangeMax);
	globalVariables->SetValue(name, "emitData.rangeMin", data.rangeMin);

	globalVariables->SetValue(name, "emitData.segment", data.segment);

	globalVariables->SetEnumValue<EmitterShapeType>(name, "emitData.shapeType", data.shapeType, "EmitterShapeType");
	globalVariables->SetEnumValue<EmitData::EmitType>(name, "emitData.emitType", data.emitType, "EmitType");
	globalVariables->SetEnumValue<EmitData::DirectionType>(name, "emitData.directionType", data.directionType, "DirectionType");
	globalVariables->SetEnumValue<EmitData::TopBottom>(name, "emitData.topBottom", data.topBottom, "TopBottom");
}

void EffectEditor::AAAA(const std::string& name,EffectGlobalData& data) {
	ImGui::Begin("EffectEditor");

	// パーティクル選択
	Engine::ImGuiManager::Select("Selected Effect", data.particleName, effectComponent->GetParticleManager()->GetParticleGroups());

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

	// 生存時間
	ImGui::DragFloat("生存時間(中央値)", &data.emitData.lifeTime.median, 0.1f);
	ImGui::DragFloat("生存時間(振れ幅)", &data.emitData.lifeTime.range, 0.1f);

	ImGui::Checkbox("重力", &data.isFlag.isGravity);
	ImGui::Checkbox("跳ねるか", &data.isFlag.isBounce);
	bool isLight = data.enableLighting;
	ImGui::Checkbox("ライティング", &isLight);
	data.enableLighting = isLight;
	
	if (ImGui::CollapsingHeader("ビルボード")) {
		ImGui::Checkbox("ビルボードのz回転するか", &data.isFlag.billboardRotZ);
		ImGui::Checkbox("ビルボードするか", &data.isFlag.usebillboard);
		ImGui::Checkbox("ビルボードY軸するか", &data.isFlag.usebillboardY);
		
	}
	if (ImGui::CollapsingHeader("速度")) {
		ImGui::DragFloat3("速度(中央値)", &data.emitData.velocity.median.x, 0.1f);
		ImGui::DragFloat3("速度(振れ幅)", &data.emitData.velocity.range.x, 0.1f);

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
	ImGui::End();
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
