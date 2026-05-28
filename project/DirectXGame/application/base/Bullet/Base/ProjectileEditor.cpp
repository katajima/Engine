#include "ProjectileEditor.h"

#include "DirectXGame/engine/3d/Model/ModelManager.h"
#include "imgui.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <json.hpp>

namespace {

	using json = nlohmann::json;

	template <typename T>
	T GetValue(const json& object, const char* key, const T& defaultValue) {
		const auto it = object.find(key);
		// 古いJSONに新しい項目が無い場合でも、既定値で読み込めるようにする。
		return it == object.end() ? defaultValue : it->get<T>();
	}

	json ToJson(const Projectile::ProjectileParam& param) {
		// enumはJSON上では整数として保存し、読み込み時に元の型へ戻す。
		return {
			{ "name", param.name },
			{ "modelName", param.modelName },
			{ "textureName", param.textureName },
			{ "spawnSoundName", param.spawnSoundName },
			{ "hitSoundName", param.hitSoundName },
			{ "moveEffectName", param.moveEffectName },
			{ "moveEffectSpreadScale", param.moveEffectSpreadScale },
			{ "hitEffectName", param.hitEffectName },
			{ "explosionEffectName", param.explosionEffectName },
			{ "speed", param.speed },
			{ "maxLifeTime", param.maxLifeTime },
			{ "radius", param.radius },
			{ "damage", param.damage },
			{ "type", static_cast<int>(param.type) },
			{ "moveType", static_cast<int>(param.moveType) },
			{ "hitType", static_cast<int>(param.hitType) },
			{ "collisionTag", static_cast<int>(param.collisionTag) },
			{ "collisionLayer", static_cast<uint32_t>(param.collisionLayer) },
			{ "collisionMask", param.collisionMask },
			{ "isPierce", param.isPierce },
			{ "maxPierceCount", param.maxPierceCount },
			{ "maxBounceCount", param.maxBounceCount },
			{ "enableGravity", param.enableGravity },
			{ "gravityScale", param.gravityScale },
			{ "enableHoming", param.enableHoming },
			{ "homingStrength", param.homingStrength },
			{ "homingRange", param.homingRange },
			{ "explosionLifeTime", param.explosionLifeTime },
			{ "explosionRadius", param.explosionRadius },
			{ "explosionDamage", param.explosionDamage },
			{ "generateTrailEffect", param.generateTrailEffect },
			{ "trailEffectLifeTime", param.trailEffectLifeTime },
		};
	}

	Projectile::ProjectileParam FromJson(const json& object) {
		Projectile::ProjectileParam param{};
		param.name = GetValue(object, "name", param.name);
		param.modelName = GetValue(object, "modelName", param.modelName);
		param.textureName = GetValue(object, "textureName", param.textureName);
		param.spawnSoundName = GetValue(object, "spawnSoundName", param.spawnSoundName);
		param.hitSoundName = GetValue(object, "hitSoundName", param.hitSoundName);
		param.moveEffectName = GetValue(object, "moveEffectName", param.moveEffectName);
		param.moveEffectSpreadScale = GetValue(object, "moveEffectSpreadScale", param.moveEffectSpreadScale);
		param.hitEffectName = GetValue(object, "hitEffectName", param.hitEffectName);
		param.explosionEffectName = GetValue(object, "explosionEffectName", param.explosionEffectName);
		param.speed = GetValue(object, "speed", param.speed);
		param.maxLifeTime = GetValue(object, "maxLifeTime", param.maxLifeTime);
		param.radius = GetValue(object, "radius", param.radius);
		param.damage = GetValue(object, "damage", param.damage);
		param.type = static_cast<Projectile::Type>(GetValue(object, "type", static_cast<int>(param.type)));
		param.moveType = static_cast<Projectile::ProjectileMoveType>(
			GetValue(object, "moveType", static_cast<int>(param.moveType)));
		param.hitType = static_cast<Projectile::ProjectileHitType>(
			GetValue(object, "hitType", static_cast<int>(param.hitType)));
		param.collisionTag = static_cast<CollisionTag>(
			GetValue(object, "collisionTag", static_cast<int>(param.collisionTag)));
		param.collisionLayer = static_cast<CollisionLayer>(
			GetValue(object, "collisionLayer", static_cast<uint32_t>(param.collisionLayer)));
		param.collisionMask = GetValue(object, "collisionMask", param.collisionMask);
		param.isPierce = GetValue(object, "isPierce", param.isPierce);
		param.maxPierceCount = GetValue(object, "maxPierceCount", param.maxPierceCount);
		param.maxBounceCount = GetValue(object, "maxBounceCount", param.maxBounceCount);
		param.enableGravity = GetValue(object, "enableGravity", param.enableGravity);
		param.gravityScale = GetValue(object, "gravityScale", param.gravityScale);
		param.enableHoming = GetValue(object, "enableHoming", param.enableHoming);
		param.homingStrength = GetValue(object, "homingStrength", param.homingStrength);
		param.homingRange = GetValue(object, "homingRange", param.homingRange);
		param.explosionLifeTime = GetValue(object, "explosionLifeTime", param.explosionLifeTime);
		param.explosionRadius = GetValue(object, "explosionRadius", param.explosionRadius);
		param.explosionDamage = GetValue(object, "explosionDamage", param.explosionDamage);
		param.generateTrailEffect = GetValue(object, "generateTrailEffect", param.generateTrailEffect);
		param.trailEffectLifeTime = GetValue(object, "trailEffectLifeTime", param.trailEffectLifeTime);
		return param;
	}

	void EditString(const char* label, std::string& value) {
		// ImGui::InputTextは固定長バッファを要求するため、std::stringとの橋渡しを行う。
		std::array<char, 256> buffer{};
		strncpy_s(buffer.data(), buffer.size(), value.c_str(), _TRUNCATE);
		if (ImGui::InputText(label, buffer.data(), buffer.size())) {
			value = buffer.data();
		}
	}

}

void Projectile::ProjectileEditor::Initialize(Engine::ModelManager* modelManager) {
	modelManager_ = modelManager;
	Load();
}

void Projectile::ProjectileEditor::Update() {
#ifdef _DEBUG
	ImGui::Begin("Projectile Editor");

	ImGui::InputText("New Definition", newDefinitionName_.data(), newDefinitionName_.size());
	ImGui::SameLine();
	if (ImGui::Button("Add")) {
		AddDefinition();
	}

	const char* preview = selectedDefinitionName_.empty() ? "<none>" : selectedDefinitionName_.c_str();
	if (ImGui::BeginCombo("Definition", preview)) {
		for (const auto& [name, _] : definitions_) {
			const bool selected = name == selectedDefinitionName_;
			if (ImGui::Selectable(name.c_str(), selected)) {
				selectedDefinitionName_ = name;
			}
			if (selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	if (!selectedDefinitionName_.empty()) {
		auto it = definitions_.find(selectedDefinitionName_);
		if (it != definitions_.end()) {
			ImGui::SameLine();
			if (ImGui::Button("Delete")) {
				ImGui::OpenPopup("Delete Projectile Definition");
			}
			if (ImGui::BeginPopupModal("Delete Projectile Definition", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::Text("Delete definition '%s'?", selectedDefinitionName_.c_str());
				if (ImGui::Button("Cancel")) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Delete Permanently")) {
					DeleteSelected();
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			// 削除後はイテレータが無効になるため、選択名から取り直す。
			it = definitions_.find(selectedDefinitionName_);
			if (it != definitions_.end()) {
				ImGui::SeparatorText("Parameters");
				DrawParamEditor(it->second);
			}
		}
	}

	ImGui::Separator();
	if (ImGui::Button("Save JSON")) {
		Save();
		message_ = "Saved: " + filePath_;
	}
	ImGui::SameLine();
	if (ImGui::Button("Reload JSON")) {
		Load();
		message_ = "Reloaded: " + filePath_;
	}
	if (!message_.empty()) {
		ImGui::TextWrapped("%s", message_.c_str());
	}
	ImGui::End();
#endif
}

const Projectile::ProjectileParam* Projectile::ProjectileEditor::Find(const std::string& definitionName) const {
	const auto it = definitions_.find(definitionName);
	return it == definitions_.end() ? nullptr : &it->second;
}

void Projectile::ProjectileEditor::AddDefinition() {
	const std::string name = newDefinitionName_.data();
	if (name.empty()) {
		message_ = "Definition name is required.";
		return;
	}
	if (definitions_.contains(name)) {
		message_ = "A definition with that name already exists.";
		return;
	}

	ProjectileParam param{};
	param.name = name;
	if (modelManager_ && !modelManager_->GetModel().empty()) {
		// 新規作成時はロード済みモデルの先頭を仮設定し、空モデルでの生成を避ける。
		param.modelName = modelManager_->GetModel().begin()->first;
	}
	param.maxLifeTime = 5.0f;
	param.speed = 10.0f;
	definitions_[name] = param;
	selectedDefinitionName_ = name;
	newDefinitionName_.fill('\0');
	message_ = "Added definition: " + name;
}

void Projectile::ProjectileEditor::DeleteSelected() {
	definitions_.erase(selectedDefinitionName_);
	if (definitions_.empty()) {
		selectedDefinitionName_.clear();
	}
	else {
		selectedDefinitionName_ = definitions_.begin()->first;
	}
	message_ = "Deleted definition. Save JSON to persist the change.";
}

void Projectile::ProjectileEditor::DrawParamEditor(ProjectileParam& param) {
	EditString("Object Name", param.name);
	const char* modelPreview = param.modelName.empty() ? "<none>" : param.modelName.c_str();
	if (ImGui::BeginCombo("Model", modelPreview)) {
		if (modelManager_) {
			// ModelManagerにロード済みのモデル名のみ選ばせる。
			for (const auto& [modelName, _] : modelManager_->GetModel()) {
				const bool selected = param.modelName == modelName;
				if (ImGui::Selectable(modelName.c_str(), selected)) {
					param.modelName = modelName;
				}
				if (selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
		}
		ImGui::EndCombo();
	}
	EditString("Texture", param.textureName);
	EditString("Spawn Sound", param.spawnSoundName);
	EditString("Hit Sound", param.hitSoundName);

	ImGui::SeparatorText("Effects");
	EditString("Move Effect", param.moveEffectName);
	ImGui::DragFloat("Move Effect Spread", &param.moveEffectSpreadScale, 0.01f, 0.0f);
	EditString("Hit Effect", param.hitEffectName);
	EditString("Explosion Effect", param.explosionEffectName);
	ImGui::Checkbox("Generate Trail", &param.generateTrailEffect);
	ImGui::DragFloat("Trail Lifetime", &param.trailEffectLifeTime, 0.01f, 0.0f);

	ImGui::SeparatorText("General");
	ImGui::DragFloat("Speed", &param.speed, 0.1f, 0.0f);
	ImGui::DragFloat("Lifetime", &param.maxLifeTime, 0.01f, 0.0f);
	ImGui::DragFloat("Radius", &param.radius, 0.01f, 0.0f);
	ImGui::DragFloat("Damage", &param.damage, 0.1f, 0.0f);

	static const char* typeLabels[] = { "Normal", "Homing", "Laser", "Shotgun", "Rapid", "Penetration" };
	static const char* moveLabels[] = { "Straight", "Homing", "Parabola", "Stay" };
	static const char* hitLabels[] = { "Destroy", "Penetrate", "Explode", "Bounce" };
	static const char* tagLabels[] = { "None", "Player", "Enemy", "PlayerAttack", "EnemyAttack", "Wall", "Item", "PlayerEffect", "EnemyEffect" };

	int type = static_cast<int>(param.type);
	int moveType = static_cast<int>(param.moveType);
	int hitType = static_cast<int>(param.hitType);
	int collisionTag = static_cast<int>(param.collisionTag);
	if (ImGui::Combo("Type", &type, typeLabels, IM_ARRAYSIZE(typeLabels))) {
		param.type = static_cast<Type>(type);
	}
	if (ImGui::Combo("Movement", &moveType, moveLabels, IM_ARRAYSIZE(moveLabels))) {
		param.moveType = static_cast<ProjectileMoveType>(moveType);
	}
	if (ImGui::Combo("On Hit", &hitType, hitLabels, IM_ARRAYSIZE(hitLabels))) {
		param.hitType = static_cast<ProjectileHitType>(hitType);
	}
	if (ImGui::Combo("Collision Tag", &collisionTag, tagLabels, IM_ARRAYSIZE(tagLabels))) {
		param.collisionTag = static_cast<CollisionTag>(collisionTag);
	}

	uint32_t layer = static_cast<uint32_t>(param.collisionLayer);
	ImGui::InputScalar("Collision Layer Bits", ImGuiDataType_U32, &layer);
	param.collisionLayer = static_cast<CollisionLayer>(layer);
	ImGui::InputScalar("Collision Mask Bits", ImGuiDataType_U32, &param.collisionMask);

	ImGui::SeparatorText("Hit Behavior");
	ImGui::Checkbox("Pierce", &param.isPierce);
	ImGui::DragInt("Max Pierce Count", &param.maxPierceCount, 1.0f, 0);
	ImGui::DragInt("Max Bounce Count", &param.maxBounceCount, 1.0f, 0);

	ImGui::SeparatorText("Movement Details");
	ImGui::Checkbox("Gravity", &param.enableGravity);
	ImGui::DragFloat("Gravity Scale", &param.gravityScale, 0.01f);
	ImGui::Checkbox("Homing", &param.enableHoming);
	ImGui::DragFloat("Homing Strength", &param.homingStrength, 0.01f, 0.0f);
	ImGui::DragFloat("Homing Range", &param.homingRange, 0.1f, 0.0f);

	ImGui::SeparatorText("Explosion");
	ImGui::DragFloat("Explosion Lifetime", &param.explosionLifeTime, 0.01f, 0.0f);
	ImGui::DragFloat("Explosion Radius", &param.explosionRadius, 0.01f, 0.0f);
	ImGui::DragFloat("Explosion Damage", &param.explosionDamage, 0.1f, 0.0f);
}

void Projectile::ProjectileEditor::Save() const {
	json root = json::object();
	root["projectiles"] = json::object();
	for (const auto& [name, param] : definitions_) {
		root["projectiles"][name] = ToJson(param);
	}

	const std::filesystem::path path(filePath_);
	// resources/Projectilesが無い初回保存でも、そのまま書き出せるようにする。
	std::filesystem::create_directories(path.parent_path());
	std::ofstream stream(path);
	if (stream) {
		stream << std::setw(4) << root << std::endl;
	}
}

void Projectile::ProjectileEditor::Load() {
	definitions_.clear();

	std::ifstream stream(filePath_);
	if (stream) {
		json root;
		stream >> root;
		const auto it = root.find("projectiles");
		if (it != root.end() && it->is_object()) {
			for (const auto& [name, object] : it->items()) {
				definitions_[name] = FromJson(object);
			}
		}
	}

	if (!definitions_.empty()) {
		if (!definitions_.contains(selectedDefinitionName_)) {
			// 初回ロードや削除後のロードでは、存在する定義を自動選択する。
			selectedDefinitionName_ = definitions_.begin()->first;
		}
	}
	else {
		selectedDefinitionName_.clear();
	}
}
