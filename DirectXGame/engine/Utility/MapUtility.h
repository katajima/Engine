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
class MapContainer {
public:
    std::map<K, V> data;

    V& operator[](const K& key) {
        return data[key];
    }

    const V& operator[](const K& key) const {
        return data.at(key); // const版はatで例外を投げるようにするのも可
    }

    auto begin() { return data.begin(); }
    auto end() { return data.end(); }
    auto empty() { return data.empty(); }
    auto begin() const { return data.begin(); }
    auto end() const { return data.end(); }
    auto empty() const { return data.empty(); }

   
    void Insert(const K& key, V&& value) {
        data.insert(std::make_pair(key, std::move(value)));
        //data[key] = std::move(value);
    }


    // ポインタ参照を返す版（unique_ptr対応）
    const V* GetPtr(const K& key) const {
        auto it = data.find(key);
        if (it != data.end()) return &(it->second);
        return nullptr;
    }

	// 削除
    bool Remove(const K& key) {
        return data.erase(key) > 0;
    }

	// キーの存在チェック
    bool Contains(const K& key) const {
        return data.find(key) != data.end();
    }
};

// ハッシュアンオーダードマップ
template <typename K, typename V>
class UnorderedMapContainer {
public:
    std::unordered_map<K, V> data;

    V& operator[](const K& key) {
        return data[key];
    }

    const V& operator[](const K& key) const {
        return data.at(key);
    }

    auto begin() { return data.begin(); }
    auto end() { return data.end(); }
    auto empty() { return data.empty(); }
    auto begin() const { return data.begin(); }
    auto end() const { return data.end(); }
    auto empty() const { return data.empty(); }

    // 追加
    void Add(const K& key, const V& value) {
        data[key] = value;
    }

    // 取得（
    std::optional<V> Get(const K& key) const {
        auto it = data.find(key);
        if (it != data.end()) return it->second;
        return std::nullopt;
    }

    // キーの存在チェック
    bool Contains(const K& key) const {
        return data.find(key) != data.end();
    }
};