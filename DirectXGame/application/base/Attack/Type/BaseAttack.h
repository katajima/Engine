#pragma once
#include "DirectXGame/engine/input/Input.h"
#include "DirectXGame/application/base/Attack/AttackData.h"

#include "imgui.h"

class BaseAttack {
public:
    virtual ~BaseAttack() = default;

    virtual void Start() {
        timer_ = 0;
        isFinished_ = false;
    }

    virtual void Update(float dt) {
        timer_ += dt;
        if (timer_ >= GetTotalDuration()) {
            isFinished_ = true;
        }
    }

    virtual void SetInput(Input*) {} // デフォルトは何もしない

    // すべてのTransformをまとめて設定
    virtual void SetTransforms(const std::unordered_map<std::string, WorldTransform*>& transforms) {
        transforms_ = transforms;
    }

    virtual bool IsFinished() const { return isFinished_; }

    virtual bool CanTransition() const {
        return timer_ >= attackData_.startupFrames / 60.0f &&
            timer_ < (attackData_.startupFrames + attackData_.activeFrames) / 60.0f;
    }

    virtual void ApplyEffect() {} // 各攻撃タイプごとの効果処理（任意）

    void SetAttackData(const AttackData& data) { attackData_ = data; }
    const AttackData& GetAttackData() const { return attackData_; }

protected:
    float timer_ = 0.0f;
    bool isFinished_ = false;
    AttackData attackData_;

    std::unordered_map<std::string, WorldTransform*> transforms_;

    // 例：特定のTransformを使う
    WorldTransform* GetTransform(const std::string& name) const {
        auto it = transforms_.find(name);
        return (it != transforms_.end()) ? it->second : nullptr;
    }
    // 全体時間
    float GetTotalDuration() const {
        return (attackData_.startupFrames + attackData_.activeFrames + attackData_.recoveryFrames) / 60.0f;
    }

    // 発生＋持続フレーム
    float GetStartupAndActiveFrame() const { return (attackData_.startupFrames + attackData_.activeFrames) / 60.0f; };

    // 発生フレーム
    float GetStartupFrame() const { return (attackData_.startupFrames) / 60.0f;}

};
