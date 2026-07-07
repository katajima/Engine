#pragma once
#include"GlobalVariablesFanction.h"
#include <filesystem>


namespace Engine {
	// 設定値を未登録時だけ追加するか、現在値で上書きするかを表す
	enum class GlobalVariableWriteMode {
		Register,
		Save,
	};

	/// <summary>
	/// グローバル変数
	/// </summary>
	class GlobalVariables {
	private:


	public:
		/// <summary>
		/// グループの作成
		/// </summary>
		/// <param name="groupName">グループ名</param>
		void CreateGroup(const std::string& groupName);

		// 値のセット
		template<typename T>
		void SetValue(const std::string& groupName, const std::string& key, T value);

		// 値の追加
		template<typename T>
		void AddItem(const std::string& groupName, const std::string& key, const T& value);

		// 値の取得
		template<typename T>
		T GetValue(const std::string& groupName, const std::string& key) const;


		// enum 専用：追加
		template<typename E>
		void AddEnumItem(const std::string& groupName, const std::string& key, E value, const std::string& enumType);

		// enum 専用：上書き
		template<typename E>
		void SetEnumValue(const std::string& groupName, const std::string& key, E value, const std::string& enumType);

		// enum 専用：取得
		template<typename E>
		E GetEnumValue(const std::string& groupName, const std::string& key) const;



		// グループが存在するか？
		bool HasGroup(const std::string& groupName) const;

		// キーが存在するか？
		bool HasKey(const std::string& groupName, const std::string& key) const;

		// グループやキーの一覧取得
		std::vector<std::string> GetGroupNames() const;
		// グループやキーの取得(名前取得)
		std::vector<std::string> GetKeys(const std::string& groupName) const;

		// 何の型か？
		std::string GetTypeName(const std::string& groupName, const std::string& key) const;

		// 削除(アイテム)
		void RemoveItem(const std::string& groupName, const std::string& key);
		// 削除(グループ)
		void RemoveGroup(const std::string& groupName);
		// グループ名変更
		bool RenameGroup(const std::string& oldName, const std::string& newName);
		// 保存カテゴリを設定
		void SetGroupCategory(const std::string& groupName, const std::string& categoryName);

		// 複製
		bool DuplicateItem(
			const std::string& srcGroupName, const std::string& srcKey,
			const std::string& dstGroupName, const std::string& dstKey);

		// 固有キー取得
		std::string MakeUniqueKey(const std::string& baseKey, const GvData::Group& group);

		/// <summary>
		/// ファイルに書き出し
		/// </summary>
		/// <param name="groupName"></param>
		void SaveFile(const std::string& groupName);

		// 保存ファイル削除
		bool RemoveSavedFile(const std::string& groupName);

		/// <summary>
		/// ディレクトリの全ファイル読み込み
		/// </summary>
		void LoadFiles();

		/// <summary>
		/// ファイルから読み込む
		/// </summary>
		/// <param name="groupName"></param>
		void LoadFile(const std::string& groupName);

		// データ取得
		GvData::Group GetGroupData(const std::string& name) { return datas_[name]; }

	public:
		/// <summary>
		/// 毎フレーム処理
		/// </summary>
		void Update();


		GlobalVariables() = default;
		~GlobalVariables() = default;
		GlobalVariables(const GlobalVariables& global) = default;
		GlobalVariables& operator=(const GlobalVariables& global) = default;
	private:

		// 全データ
		std::map<std::string, GvData::Group> datas_;
		std::map<std::string, std::vector<std::string>> groupKeys_; // 順序を保持するためのリスト
		std::map<std::string, std::string> groupCategories_; // グループごとの保存先カテゴリ

		//グローバル変数の保存先
		const std::string kDirectoryPath = "resources/GlobalVariables/";

		// 保存カテゴリ名を取得
		std::string ResolveCategory(const std::string& groupName) const;
		// 保存カテゴリのフォルダパスを取得
		std::filesystem::path GetCategoryDirectory(const std::string& groupName) const;
		// グループの保存ファイルパスを取得
		std::filesystem::path GetSaveFilePath(const std::string& groupName) const;
		// 旧形式を含めた既存保存ファイルパスを探す
		std::filesystem::path FindExistingFilePath(const std::string& groupName) const;
	};

	/// <summary>
	/// 設定項目の列挙処理から、登録と保存のAPI差を隠蔽するライター
	/// </summary>
	class GlobalVariableWriter {
	public:
		// 書き込み先と登録・保存モードを設定する
		GlobalVariableWriter(GlobalVariables* globalVariables, GlobalVariableWriteMode mode)
			: globalVariables_(globalVariables), mode_(mode) {
		}

		// 通常値を新規登録、または現在値で上書きする
		template<typename T>
		void Value(const std::string& groupName, const std::string& key, const T& value) {
			if (mode_ == GlobalVariableWriteMode::Save) {
				globalVariables_->SetValue(groupName, key, value);
			}
			else {
				globalVariables_->AddItem(groupName, key, value);
			}
		}

		// enum値を型情報付きで新規登録、または現在値で上書きする
		template<typename E>
		void EnumValue(const std::string& groupName, const std::string& key, E value, const std::string& enumType) {
			if (mode_ == GlobalVariableWriteMode::Save) {
				globalVariables_->SetEnumValue(groupName, key, value, enumType);
			}
			else {
				globalVariables_->AddEnumItem(groupName, key, value, enumType);
			}
		}

	private:
		GlobalVariables* globalVariables_ = nullptr; // 設定値の書き込み先
		GlobalVariableWriteMode mode_ = GlobalVariableWriteMode::Register; // 登録または保存モード
	};


	template<typename T>
	void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, T value)
	{
		// グループの参照を取得
		GvData::Group& group = datas_[groupName];
		// 新しい項目のデータを参照
		GvData::Item newItem{};
		newItem = value;
		// 設定した項目をstd::mapに追加
		group[key] = newItem;
	}

	template<typename T>
	void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, const T& value)
	{
		static_assert(
			std::is_same_v<T, int32_t> ||
			std::is_same_v<T, uint32_t> ||
			std::is_same_v<T, float> ||
			std::is_same_v<T, Vector2> ||
			std::is_same_v<T, Vector3> ||
			std::is_same_v<T, Vector4> ||
			std::is_same_v<T, bool> ||
			std::is_same_v<T, std::string> ||
			std::is_same_v<T, Transform>,
			"Unsupported type for AddItem"
			);

		// グループが存在しない場合は作成
		CreateGroup(groupName);

		// グループを取得
		GvData::Group& group = datas_[groupName];

		// キーが存在しなければ追加
		if (group.find(key) == group.end()) {
			group[key] = value;
		}
	};


	template<typename T>
	T GlobalVariables::GetValue(const std::string& groupName, const std::string& key) const
	{
		// グループ存在チェック
		assert(datas_.find(groupName) != datas_.end());
		const GvData::Group& group = datas_.at(groupName);

		// キー存在チェック
		assert(group.find(key) != group.end());

		// std::variantから型Tを取得。型が違うと例外 std::bad_variant_access が投げられるので注意
		return std::get<T>(group.at(key));
	}


	template<typename E>
	void GlobalVariables::AddEnumItem(const std::string& groupName, const std::string& key, E value, const std::string& enumType)
	{
		static_assert(std::is_enum_v<E>, "AddEnumItem requires enum type.");

		CreateGroup(groupName);

		GvData::Group& group = datas_[groupName];
		if (group.find(key) == group.end()) {
			GvData::EnumItem e;
			e.enumType = enumType;
			e.value = static_cast<int64_t>(static_cast<std::underlying_type_t<E>>(value));
			group[key] = e;
		}
	}

	template<typename E>
	void GlobalVariables::SetEnumValue(const std::string& groupName, const std::string& key, E value, const std::string& enumType)
	{
		static_assert(std::is_enum_v<E>, "SetEnumValue requires enum type.");

		GvData::Group& group = datas_[groupName];

		GvData::EnumItem e;
		e.enumType = enumType;
		e.value = static_cast<int64_t>(static_cast<std::underlying_type_t<E>>(value));
		group[key] = e;
	}

	template<typename E>
	E GlobalVariables::GetEnumValue(const std::string& groupName, const std::string& key) const
	{
		static_assert(std::is_enum_v<E>, "GetEnumValue requires enum type.");

		assert(datas_.find(groupName) != datas_.end());
		const GvData::Group& group = datas_.at(groupName);

		assert(group.find(key) != group.end());

		const auto& item = group.at(key);
		const auto& e = std::get<GvData::EnumItem>(item);
		return static_cast<E>(static_cast<std::underlying_type_t<E>>(e.value));
	}

}
