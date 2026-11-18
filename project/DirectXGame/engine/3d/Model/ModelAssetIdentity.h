#pragma once
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <string>
#include <stdexcept>

namespace Engine::ModelAssetIdentity {
    /// @brief モデルの完全パスを字句正規化し、Windowsの大文字小文字差を統一する。
    /// @param filePath モデルファイル名またはパス。
    /// @param directory モデルルートからの相対ディレクトリ。
    /// @return キャッシュで使用する絶対パス。
    std::string Normalize(const std::string& filePath, const std::string& directory = {});

    /// @brief 従来のファイル名だけの検索指定か判定する。
    /// @param filePath 検索指定。
    /// @return ディレクトリを含まない場合はtrue。
    bool IsBasename(const std::string& filePath);
    /// @brief 完全IDまたは一意な従来ファイル名から共有資産を検索する。
    /// @param assets 完全パスをキーに持つ資産マップ。
    /// @param filePath 検索する完全パス、相対パスまたはファイル名。
    /// @return 一致した位置。未登録の場合はend。
    template<class AssetMap>
    auto Find(const AssetMap& assets, const std::string& filePath)
    {
        const auto key = Normalize(filePath);
        if (!IsBasename(filePath)) return assets.find(key);
        auto result = assets.end();
        const auto basename = std::filesystem::path(key).filename();
        for (auto iterator = assets.begin(); iterator != assets.end(); ++iterator) {
            if (std::filesystem::path(iterator->first).filename() != basename) continue;
            if (result != assets.end()) throw std::runtime_error("Ambiguous model basename: " + filePath);
            result = iterator;
        }
        return result;
    }
}

