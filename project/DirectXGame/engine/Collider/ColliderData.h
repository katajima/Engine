#pragma once
#include <cstdint>


// 一意なIDを生成するためのユーティリティクラス
/// <summary>
/// UniqueIdGeneratorを管理・実装するクラス。
/// </summary>
class UniqueIdGenerator {
public:
    static uint32_t Generate() {
        static uint32_t currentId = 1;
        return currentId++;
    }
};

// インターフェースクラス
/// <summary>
/// IHitReceiverを管理・実装するクラス。
/// </summary>
class IHitReceiver {
public:
    virtual ~IHitReceiver() = default;
};
