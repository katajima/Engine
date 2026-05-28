#pragma once

#include "ProjectileData.h"

#include <array>
#include <map>
#include <string>

namespace Engine {
	class ModelManager;
}

namespace Projectile {

	/// <summary>
	/// BaseProjectileで使うProjectileParamを編集・追加・削除・JSON保存するデバッグエディター。
	/// </summary>
	class ProjectileEditor {
	public:
		// ModelManagerのロード済みモデル名を、モデル選択コンボの候補として使う
		void Initialize(Engine::ModelManager* modelManager);
		void Update();

		// 保存済み定義名から生成用パラメータを取得する
		const ProjectileParam* Find(const std::string& definitionName) const;
		const std::map<std::string, ProjectileParam>& GetDefinitions() const { return definitions_; }

	private:
		void AddDefinition();
		void DeleteSelected();
		void DrawParamEditor(ProjectileParam& param);
		void Save() const;
		void Load();

		std::map<std::string, ProjectileParam> definitions_;
		std::string selectedDefinitionName_;
		std::array<char, 128> newDefinitionName_{};
		std::string message_;

		// モデル名の手入力を避け、ロード済みモデル一覧から選ばせるために保持する。
		Engine::ModelManager* modelManager_ = nullptr;
		const std::string filePath_ = "resources/Projectiles/projectiles.json";
	};

}
