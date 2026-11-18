#include "ModelAssetIdentity.h"
#include <algorithm>
#include <cctype>

std::string Engine::ModelAssetIdentity::Normalize(const std::string& filePath, const std::string& directory)
{
    std::filesystem::path path(filePath);
    if (!path.is_absolute()) {
        path = std::filesystem::path("resources/Models") / directory / path;
    }
    auto key = std::filesystem::absolute(path).lexically_normal().generic_string();
    std::ranges::transform(key, key.begin(), [](unsigned char character) { return static_cast<char>(std::tolower(character)); });
    return key;
}

bool Engine::ModelAssetIdentity::IsBasename(const std::string& filePath)
{
    return filePath.find_first_of("/\\") == std::string::npos;
}

