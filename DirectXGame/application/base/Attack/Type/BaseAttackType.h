#pragma once
#include "BaseAttack.h"

class BlowAttack : public BaseAttack {
public:
    void Update(float dt) override {
        BaseAttack::Update(dt);
        // 一撃系：一度だけヒット処理
        if (!hasHit_ && timer_ >= attackData_.startupFrames / 60.0f) {
            ApplyEffect();
            hasHit_ = true;
        }
    }

    void ApplyEffect() override {
        // 一発ヒット処理
        // 例: ダメージ適用、SE再生など
    }

private:
    bool hasHit_ = false;
};



class ConsecutiveHitsAttack : public BaseAttack {
public:
    void Start() override {
        BaseAttack::Start();
        hitInterval_ = attackData_.activeFrames / attackData_.hitNum / 60.0f;
        nextHitTime_ = attackData_.startupFrames / 60.0f;
        hitCount_ = 0;
    }

    void Update(float dt) override {
        timer_ += dt;

        if (timer_ >= nextHitTime_ && hitCount_ < attackData_.hitNum) {
            ApplyEffect();
            ++hitCount_;
            nextHitTime_ += hitInterval_;
        }

        if (timer_ >= GetTotalDuration()) {
            isFinished_ = true;
        }
    }

    void ApplyEffect() override {
        // 多段ヒット処理（例：ヒットごとにSEやエフェクト）
    }

private:
    float hitInterval_ = 0.0f;
    float nextHitTime_ = 0.0f;
    int hitCount_ = 0;
};


class DurationAttack : public BaseAttack {
public:
    void Start() override {
        BaseAttack::Start();
        hasActivated_ = false;
    }

    void Update(float dt) override {
        timer_ += dt;

        // アクティブフレーム中ずっとヒットを発生させる（例：毎フレーム）
        float startTime = attackData_.startupFrames / 60.0f;
        float endTime = (attackData_.startupFrames + attackData_.activeFrames) / 60.0f;
        if (timer_ >= startTime && timer_ <= endTime) {
            ApplyEffect();
            hasActivated_ = true;
        }

        if (timer_ >= GetTotalDuration()) {
            isFinished_ = true;
        }
    }

    void ApplyEffect() override {
        // 持続的な攻撃処理（例：範囲ヒットチェック）
    }

private:
    bool hasActivated_ = false;
};


class LastConsecutiveHitsAttack : public BaseAttack {
public:
    void Start() override {
        BaseAttack::Start();
        float activeTime = attackData_.activeFrames / 60.0f;
        float baseHitInterval = activeTime / attackData_.hitNum;

        nextHitTime_ = attackData_.startupFrames / 60.0f;
        hitInterval_ = baseHitInterval;
        hitCount_ = 0;
        finishedStrongHit_ = false;
    }

    void Update(float dt) override {
        timer_ += dt;

        float totalDuration = GetTotalDuration();
        if (timer_ >= nextHitTime_ && hitCount_ < attackData_.hitNum - 1) {
            ApplyEffect(); // 通常連撃
            ++hitCount_;
            nextHitTime_ += hitInterval_;
        }
        else if (!finishedStrongHit_ &&
            timer_ >= (attackData_.startupFrames + attackData_.activeFrames) / 60.0f)
        {
            ApplyStrongHit(); // 最後の強攻撃
            finishedStrongHit_ = true;
        }

        if (timer_ >= totalDuration) {
            isFinished_ = true;
        }
    }

    void ApplyEffect() override {
        // 通常の連撃ヒット処理
    }

    void ApplyStrongHit() {
        // 最後の強打（大ダメージなど）処理
    }

private:
    float hitInterval_ = 0.0f;
    float nextHitTime_ = 0.0f;
    int hitCount_ = 0;
    bool finishedStrongHit_ = false;
};



class LastBlowAttack : public BaseAttack {
public:
    void Start() override {
        BaseAttack::Start();
        hasHit_ = false;
    }

    void Update(float dt) override {
        timer_ += dt;

        float hitTime = (attackData_.startupFrames + attackData_.activeFrames) / 60.0f;

        // 最後の一撃を active 終了時に一度だけ発生させる
        if (!hasHit_ && timer_ >= hitTime) {
            ApplyStrongHit();
            hasHit_ = true;
        }

        if (timer_ >= GetTotalDuration()) {
            isFinished_ = true;
        }
    }

    void ApplyEffect() override {
        // 通常攻撃では何もしない（最後の一撃だけ）
    }

    void ApplyStrongHit() {
        // ここで当たり判定生成、エフェクト、SEなどを再生
        // 例：Damage = attackData_.damage * 2.0f など
    }

private:
    bool hasHit_ = false;
};


class ChargeAttack : public BaseAttack {
public:
    void SetInput(Input* input) override {
        input_ = input;
    }

    void Start() override {
        BaseAttack::Start();
        chargeTime_ = 0.0f;
        hasReleased_ = false;
    }

    void Update(float dt) override {
        if (hasReleased_) {
            // 攻撃中
            timer_ += dt;
            if (timer_ >= GetTotalDuration()) {
                isFinished_ = true;
            }
        }
        else {
            // チャージ中
            if (IsCharging()) {
                chargeTime_ += dt;
                if (chargeTime_ > maxChargeTime_) {
                    chargeTime_ = maxChargeTime_;
                }
                // チャージ中エフェクトなどを入れるならここ
            }
            else {
                // ボタン離したら攻撃を解放
                ApplyChargedAttack();
                hasReleased_ = true;
                timer_ = 0.0f;
            }
        }
    }

    bool CanTransition() const override {
        // チャージ攻撃中はキャンセル不可とする（必要に応じて変更）
        return hasReleased_ && timer_ >= attackData_.startupFrames / 60.0f &&
            timer_ < (attackData_.startupFrames + attackData_.activeFrames) / 60.0f;
    }

private:
    Input* input_ = nullptr;

    float chargeTime_ = 0.0f;
    const float maxChargeTime_ = 2.0f; // 最大チャージ時間（秒）
    bool hasReleased_ = false;

    bool IsCharging() const {
        // 柔軟に：Input* を使用
        return input_->IsPushKey(DIK_B); // 実際のプロジェクトに合わせてキー設定は外部化してもOK
    }

    void ApplyChargedAttack() {
        float chargeRatio = chargeTime_ / maxChargeTime_;
        float clampedRatio = (std::min)(1.0f, (std::max)(0.0f, chargeRatio));

        // ダメージ倍率を調整
        attackData_.damage *= (1.0f + clampedRatio * 2.0f); // 最大3倍まで増加

        // その他：ノックバック、ヒット演出の強化なども clampedRatio に応じて変更可能

        // ヒット判定作成、演出など
    }
};