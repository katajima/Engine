#pragma once
#include <cstdint>


// 一意なIDを生成するためのユーティリティクラス
class UniqueIdGenerator {
public:
    static uint32_t Generate() {
        static uint32_t currentId = 1;
        return currentId++;
    }
};

// インターフェースクラス
class IHitReceiver {
public:
    virtual ~IHitReceiver() = default;
};
