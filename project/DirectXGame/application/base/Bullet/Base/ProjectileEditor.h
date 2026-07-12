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
		/// <summary>
		/// Projectile定義エディターを初期化し、保存済み定義を読み込みます。
		/// </summary>
		/// <param name="modelManager">モデル選択コンボで使うModelManagerです。所有権は受け取りません。</param>
		void Initialize(Engine::ModelManager* modelManager);

		/// <summary>
		/// Projectile定義の追加、削除、編集、保存UIを更新します。
		/// </summary>
		void Update();

		/// <summary>
		/// 保存済み定義名から生成用パラメータを検索します。
		/// </summary>
		/// <param name="definitionName">検索するProjectile定義名です。</param>
		/// <returns>見つかった場合は定義へのポインタ、存在しない場合はnullptrです。</returns>
		const ProjectileParam* Find(const std::string& definitionName) const;

		/// <summary>
		/// 登録済みProjectile定義一覧を取得します。
		/// </summary>
		/// <returns>定義名をキーにしたProjectileパラメータ一覧への参照を返します。</returns>
		const std::map<std::string, ProjectileParam>& GetDefinitions() const { return definitions_; }

	private:
		/// <summary>
		/// UI入力名を使って新しいProjectile定義を追加します。
		/// </summary>
		void AddDefinition();

		/// <summary>
		/// 現在選択中のProjectile定義を削除します。
		/// </summary>
		void DeleteSelected();

		/// <summary>
		/// 指定Projectileパラメータの詳細編集UIを描画します。
		/// </summary>
		/// <param name="param">編集対象のProjectileパラメータです。</param>
		void DrawParamEditor(ProjectileParam& param);

		/// <summary>
		/// 現在のProjectile定義一覧をJSONへ保存します。
		/// </summary>
		void Save() const;

		/// <summary>
		/// JSONからProjectile定義一覧を読み込みます。
		/// </summary>
		void Load();

		std::map<std::string, ProjectileParam> definitions_; // 定義名をキーにしたProjectile生成パラメータ
		std::string selectedDefinitionName_;                 // 現在UIで選択中の定義名
		std::array<char, 128> newDefinitionName_{};           // 新規定義名の入力バッファ
		std::string message_;                                // UIへ表示する保存結果やエラーメッセージ

		// モデル名の手入力を避け、ロード済みモデル一覧から選ばせるために保持する。
		Engine::ModelManager* modelManager_ = nullptr;
		const std::string filePath_ = "resources/Projectiles/projectiles.json"; // Projectile定義を永続化するJSONパス
	};

}
