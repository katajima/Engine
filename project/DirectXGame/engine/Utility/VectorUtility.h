#pragma once
#include <vector>
#include <optional>
#include <algorithm>
#include <memory>

// ベクター
template <typename T>
class VectorContainer {
public:
    std::vector<T> data;

    // 削除
    template <typename Pred>
    void RemoveIf(Pred pred) {
        auto it = std::remove_if(data.begin(), data.end(), pred);
        data.erase(it, data.end());
    }

    // ムーブ対応のAdd
    void Add(T&& value) {
        data.push_back(std::move(value));
    }

	// const参照版Add
    void Add(const T& value) {
        data.push_back(value);
    }

    // 任意位置に挿入
    void Insert(size_t index, const T& value) {
        if (index <= data.size()) {
            data.insert(data.begin() + index, value);
        }
    }

    // インデックス指定で削除
    void Erase(size_t index) {
        if (index < data.size()) {
            data.erase(data.begin() + index);
        }
    }

    // 値を指定して削除
    void RemoveFirst(const T& value) {
        auto it = std::find(data.begin(), data.end(), value);
        if (it != data.end()) {
            data.erase(it);
        }
    }

    // 値を全て削除（同値を全部消す）
    void RemoveAll(const T& value) {
        data.erase(std::remove(data.begin(), data.end(), value), data.end());
    }

    // 全消去
    void Clear() {
        data.clear();
    }

    // インデックスで取得
    std::optional<T> Get(size_t index) const {
        if (index < data.size()) {
            return data[index];
        }
        return std::nullopt;
    }

    // 範囲forで使えるように
    // 最初
    auto begin() { return data.begin(); }
    // 最後
    auto end() { return data.end(); }
    // 最初
    auto begin() const { return data.begin(); }
    // 最後
    auto end() const { return data.end(); }

    // 要素数
    size_t Size() const { return data.size(); }

    // 添字アクセス演算子
    T& operator[](size_t idx) { return data[idx]; }
    const T& operator[](size_t idx) const { return data[idx]; }
};