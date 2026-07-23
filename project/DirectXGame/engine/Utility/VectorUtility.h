#pragma once
#include <vector>
#include <optional>
#include <algorithm>
#include <memory>

// ベクター
template <typename T>
/// <summary>
/// VectorContainerを管理・実装するクラス。
/// </summary>
class VectorContainer {
public:
    std::vector<T> data;

    // 削除
    template <typename Pred>
    void RemoveIf(Pred pred) {
        auto it = std::remove_if(data.begin(), data.end(), pred);
        data.erase(it, data.end());
    }

    /// <summary>
    /// ムーブ対応のAdd
    /// </summary>
    void Add(T&& value) {
        data.push_back(std::move(value));
    }

	/// <summary>
	/// const参照版Add
	/// </summary>
    void Add(const T& value) {
        data.push_back(value);
    }

    /// <summary>
    /// 任意位置に挿入
    /// </summary>
    void Insert(size_t index, const T& value) {
        if (index <= data.size()) {
            data.insert(data.begin() + index, value);
        }
    }

    /// <summary>
    /// インデックス指定で削除
    /// </summary>
    void Erase(size_t index) {
        if (index < data.size()) {
            data.erase(data.begin() + index);
        }
    }

    /// <summary>
    /// 値を指定して削除
    /// </summary>
    void RemoveFirst(const T& value) {
        auto it = std::find(data.begin(), data.end(), value);
        if (it != data.end()) {
            data.erase(it);
        }
    }

    /// <summary>
    /// 値を全て削除（同値を全部消す）
    /// </summary>
    void RemoveAll(const T& value) {
        data.erase(std::remove(data.begin(), data.end(), value), data.end());
    }

    /// <summary>
    /// 全消去
    /// </summary>
    void Clear() {
        data.clear();
    }

    /// <summary>
    /// インデックスで取得
    /// </summary>
    std::optional<T> Get(size_t index) const {
        if (index < data.size()) {
            return data[index];
        }
        return std::nullopt;
    }

    // 範囲forで使えるように
    /// <summary>
    /// 最初
    /// </summary>
    auto begin() { return data.begin(); }
    /// <summary>
    /// 最後
    /// </summary>
    auto end() { return data.end(); }
    /// <summary>
    /// 最初
    /// </summary>
    auto begin() const { return data.begin(); }
    /// <summary>
    /// 最後
    /// </summary>
    auto end() const { return data.end(); }

    /// <summary>
    /// 要素数
    /// </summary>
    size_t Size() const { return data.size(); }

    /// <summary>
    /// 添字アクセス演算子
    /// </summary>
    T& operator[](size_t idx) { return data[idx]; }
    const T& operator[](size_t idx) const { return data[idx]; }
};