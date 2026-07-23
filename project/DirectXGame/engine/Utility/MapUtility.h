#pragma once
#include <string>
#include <map>
#include <unordered_map>
#include <optional>
#include <iostream>
#include <utility>
#include <memory>

// ハッシュマップ
template <typename K, typename V>
/// <summary>
/// MapContainerを管理・実装するクラス。
/// </summary>
class MapContainer {
public:
    std::map<K, V> data;

    V& operator[](const K& key) {
        return data[key];
    }

    const V& operator[](const K& key) const {
        return data.at(key); // const版はatで例外を投げるようにするのも可
    }

    /// <summary>
    /// 開始
    /// </summary>
    auto begin() { return data.begin(); }
    /// <summary>
    /// 最後
    /// </summary>
    auto end() { return data.end(); }
    /// <summary>
    /// 空
    /// </summary>
    auto empty() { return data.empty(); }
    /// <summary>
    /// 開始
    /// </summary>
    auto begin() const { return data.begin(); }
    /// <summary>
    /// 最後
    /// </summary>
    auto end() const { return data.end(); }
    /// <summary>
    /// 空
    /// </summary>
    auto empty() const { return data.empty(); }

   /// <summary>
   /// 追加
   /// </summary>
    void Insert(const K& key, V&& value) {
        data.insert(std::make_pair(key, std::move(value)));
        //data[key] = std::move(value);
    }


    /// <summary>
    /// ポインタ参照を返す版（unique_ptr対応）
    /// </summary>
    const V* GetPtr(const K& key) const {
        auto it = data.find(key);
        if (it != data.end()) return &(it->second);
        return nullptr;
    }

	/// <summary>
	/// 削除
	/// </summary>
    bool Remove(const K& key) {
        return data.erase(key) > 0;
    }

	/// <summary>
	/// キーの存在チェック
	/// </summary>
    bool Contains(const K& key) const {
        return data.find(key) != data.end();
    }
};

// ハッシュアンオーダードマップ
template <typename K, typename V>
/// <summary>
/// UnorderedMapContainerを管理・実装するクラス。
/// </summary>
class UnorderedMapContainer {
public:
    std::unordered_map<K, V> data;

    V& operator[](const K& key) {
        return data[key];
    }

    const V& operator[](const K& key) const {
        return data.at(key);
    }

    /// <summary>
    /// 開始
    /// </summary>
    auto begin() { return data.begin(); }
    /// <summary>
    /// 最後
    /// </summary>
    auto end() { return data.end(); }
    /// <summary>
    /// 空
    /// </summary>
    auto empty() { return data.empty(); }
    /// <summary>
    /// 開始
    /// </summary>
    auto begin() const { return data.begin(); }
    /// <summary>
    /// 最後
    /// </summary>
    auto end() const { return data.end(); }
    /// <summary>
    /// 空
    /// </summary>
    auto empty() const { return data.empty(); }

    /// <summary>
    /// 追加
    /// </summary>
    void Add(const K& key, const V& value) {
        data[key] = value;
    }

    /// <summary>
    /// 取得（
    /// </summary>
    std::optional<V> Get(const K& key) const {
        auto it = data.find(key);
        if (it != data.end()) return it->second;
        return std::nullopt;
    }

    /// <summary>
    /// キーの存在チェック
    /// </summary>
    bool Contains(const K& key) const {
        return data.find(key) != data.end();
    }
};