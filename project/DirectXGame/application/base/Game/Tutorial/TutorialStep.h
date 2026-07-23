#pragma once

namespace Engine {
	class DirectXCommon;
	class EntityManager;
	class GlobalVariables;
	class SceneManager;
}

namespace Character {
	class BaseCharacter;
	struct CharacterContext;
}

// イベントタイプ
enum class TutorialEventType {
    Move,   // 移動
	Camera, // カメラ操作
    Jump,   // ジャンプ
    Dodge,  // 回避
    Attack, // 攻撃
    Combo,  // コンボ
    Skill,  // スキル
    Special, // 特殊技
    EnemyDefeated,  // 敵撃破
    LockOn,         // ロックオン
	OpenMenu,       // メニューを開いた
};

/// <summary>
/// イベントデータ
/// </summary>
struct TutorialEvent {
    TutorialEventType type;

    int value = 1;     // 敵撃破数など
};

/// <summary>
/// チュートリアルにイベント基底クラス
/// </summary>
class TutorialStep {
public:

    virtual ~TutorialStep() = default;

    virtual void Enter() {}

    virtual void Update(float dt ,Character::BaseCharacter* character) {}

    virtual void Exit() {}

    virtual void OnEvent(const TutorialEvent& event) {}

    virtual bool IsCompleted() const = 0;
protected:
    bool completed_ = false;
};

// 移動チュートリアル
/// <summary>
/// TutorialMoveStepを管理・実装するクラス。
/// </summary>
class TutorialMoveStep : public TutorialStep {
public:
    void Enter() override {
        completed_ = false;
    }

	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt, Character::BaseCharacter* character) override;

    void OnEvent(const TutorialEvent& event) override {
        if (event.type == TutorialEventType::Move) {
            completed_ = true;
        }
    }

    bool IsCompleted() const override {
        return completed_;
    }
};

// カメラ操作チュートリアル
/// <summary>
/// TutorialCameraStepを管理・実装するクラス。
/// </summary>
class TutorialCameraStep : public TutorialStep {
public:
	void Enter() override {
		completed_ = false;
	}
	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt, Character::BaseCharacter* character) override;

	void OnEvent(const TutorialEvent& event) override {
		if (event.type == TutorialEventType::Camera) {
			completed_ = true;
		}
	}
	bool IsCompleted() const override {
		return completed_;
	}
};

// ジャンプチュートリアル
/// <summary>
/// TutorialJumpStepを管理・実装するクラス。
/// </summary>
class TutorialJumpStep : public TutorialStep {
public:
    void Enter() override {
        completed_ = false;
    }
	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt, Character::BaseCharacter* character) override;

    void OnEvent(const TutorialEvent& event) override {
        if (event.type == TutorialEventType::Jump) {
            completed_ = true;
        }
    }
    bool IsCompleted() const override {
        return completed_;
    }
};

// 回避
/// <summary>
/// TutorialDodgeStepを管理・実装するクラス。
/// </summary>
class TutorialDodgeStep : public TutorialStep {
public:
	void Enter() override {
		completed_ = false;
	}
	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt, Character::BaseCharacter* character) override;

	void OnEvent(const TutorialEvent& event) override {
		if (event.type == TutorialEventType::Dodge) {
			completed_ = true;
		}
	}
	bool IsCompleted() const override {
		return completed_;
	}
};


// 攻撃
/// <summary>
/// TutorialAttackStepを管理・実装するクラス。
/// </summary>
class TutorialAttackStep : public TutorialStep {
public:

	void Enter() override {
		completed_ = false;
	}
	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt, Character::BaseCharacter* character) override;

	void OnEvent(const TutorialEvent& event) override {
		if (event.type == TutorialEventType::Attack) {
			completed_ = true;
		}
	}
	bool IsCompleted() const override {
		return completed_;
	}
};

// コンボ
/// <summary>
/// TutorialComboStepを管理・実装するクラス。
/// </summary>
class TutorialComboStep : public TutorialStep {
public:
	void Enter() override {
		completed_ = false;
	}
	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt, Character::BaseCharacter* character) override;

	void OnEvent(const TutorialEvent& event) override {
		if (event.type == TutorialEventType::Combo) {
			completed_ = true;
		}
	}
	bool IsCompleted() const override {
		return completed_;
	}
};

// スキル
/// <summary>
/// TutorialSkillStepを管理・実装するクラス。
/// </summary>
class TutorialSkillStep : public TutorialStep {
public:
	void Enter() override {
		completed_ = false;
	}
	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt, Character::BaseCharacter* character) override;

	void OnEvent(const TutorialEvent& event) override {
		if (event.type == TutorialEventType::Skill) {
			completed_ = true;
		}
	}
	bool IsCompleted() const override {
		return completed_;
	}

};

// 特殊技
/// <summary>
/// TutorialSpecialStepを管理・実装するクラス。
/// </summary>
class TutorialSpecialStep : public TutorialStep {
public:
	void Enter() override {
		completed_ = false;
	}
	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt, Character::BaseCharacter* character) override;

	void OnEvent(const TutorialEvent& event) override {
		if (event.type == TutorialEventType::Special) {
			completed_ = true;
		}
	}

	bool IsCompleted() const override {
		return completed_;
	}

private:

};

// 敵撃破
/// <summary>
/// TutorialEnemyDefeatedStepを管理・実装するクラス。
/// </summary>
class TutorialEnemyDefeatedStep : public TutorialStep {
public:

	void Enter() override {
		completed_ = false;
	}
	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt, Character::BaseCharacter* character) override;

	void OnEvent(const TutorialEvent& event) override {
		if (event.type == TutorialEventType::EnemyDefeated && event.value >= 5) {
			completed_ = true;
		}
	}
	bool IsCompleted() const override {
		return completed_;
	}

};

// ロックオン
/// <summary>
/// TutorialLockOnStepを管理・実装するクラス。
/// </summary>
class TutorialLockOnStep : public TutorialStep {
public:

	void Enter() override {
		completed_ = false;
	}
	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt, Character::BaseCharacter* character) override;

	void OnEvent(const TutorialEvent& event) override {
		if (event.type == TutorialEventType::LockOn) {
			completed_ = true;
		}
	}

	bool IsCompleted() const override {
		return completed_;
	}
};

// メニューを開く
/// <summary>
/// TutorialOpenMenuStepを管理・実装するクラス。
/// </summary>
class TutorialOpenMenuStep : public TutorialStep {

	void Enter() override {
		completed_ = false;
	}
	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt, Character::BaseCharacter* character) override;

	void OnEvent(const TutorialEvent& event) override {
		if (event.type == TutorialEventType::OpenMenu) {
			completed_ = true;
		}
	}

	void Exit() override {
		// メニューを閉じるなどのクリーンアップ処理が必要な場合はここで行います
	}

	bool IsCompleted() const override {
		return completed_;
	}
};
