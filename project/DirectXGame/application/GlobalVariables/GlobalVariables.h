#pragma once
#include"GlobalVariablesFanction.h"

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
	void saveFile(const std::string& groupName);
	
	/// <summary>
	/// ディレクトリの全ファイル読み込み
	/// </summary>
	void LoadFiles();

	/// <summary>
	/// ファイルから読み込む
	/// </summary>
	/// <param name="groupName"></param>
	void LoadFile(const std::string& groupName);

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

	//グローバル変数の保存先
	const std::string kDirectoryPath = "resources/GlobalVariables/";
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
