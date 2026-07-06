#pragma once
#include <DirectXGame/engine/input/Input.h>
#include <limits>

namespace Character {
	class BaseCharacter; // 前方宣言
}


/// <summary>
/// 入力管理クラス(action層)
/// </summary>
class InputManager {
public:
	enum class Context : uint8_t
	{
		Gameplay,
		UI,
		Pause,
	};

	enum class Action : uint8_t
	{
		Move,       // Vector2
		Look,       // Vector2
		LightAttack,// bool
		HeavyAttack,// bool 
		Dodge,      // bool
		Dash,       // bool (LT)
		LockOn,     // bool
		Pause,      // bool
		Skill,		// bool	
		Jump,		// bool
		Special,	// bool
		Max
	};

	// 入力イベント構造体
	struct InputEvent
	{
		Action action;
		enum class Type : uint8_t { Press, Release } type;

		uint32_t frame;      // 発生フレーム
		float value1 = 0.0f; // 例: LTの押し込み量
		Vector2 value2{};    // 例: Move/Look
	};

	// 初期化
	void Initialize(Engine::Input* input);
	// 更新
	void Update(float dt);

	// キャラクターの設定
	void SetOwner(Character::BaseCharacter* character) { this->character = character; }



	// Context
	void SetContext(Context c) { context_ = c; }
	Context GetContext() const { return context_; }

	// Action状態の取得（ゲーム側はこれだけ使う）
	bool Pressed(Action a) const { return pressed_[Idx(a)]; }
	bool Triggered(Action a) const { return triggered_[Idx(a)]; }
	bool Released(Action a) const { return released_[Idx(a)]; }

	Vector2 Value2(Action a) const { return value2_[Idx(a)]; }
	float   Value1(Action a) const { return value1_[Idx(a)]; }

	// 移動ベクトルは長さ1に制限（斜めが速くならない）
	Vector2 MoveVector() const { return value2_[Idx(Action::Move)]; }
	Vector2 LookVector() const { return value2_[Idx(Action::Look)]; }

	// 同時押し
	bool ConsumeChord(Action a, Action b, uint32_t toleranceFrames = 2, uint32_t windowFrames = 8)
	{
		const uint32_t oldest = (frameCounter_ > windowFrames) ? (frameCounter_ - windowFrames) : 0;

		// a の Press を起点に、b が近傍にあるか探す
		for (uint32_t k = 0; k < kBufferSize; ++k)
		{
			const uint32_t idx = (bufferHead_ + kBufferSize - 1 - k) % kBufferSize;
			const InputEvent& e = buffer_[idx];

			if (e.frame < oldest) break;
			if (e.action != a || e.type != InputEvent::Type::Press) continue;

			const uint32_t from = (e.frame > toleranceFrames) ? (e.frame - toleranceFrames) : 0;
			const uint32_t to = e.frame + toleranceFrames;

			if (HasPressInWindow(b, from, to))
			{
				// 消費（簡易に両方を “古くする” 実装を入れるのがベター）
				// ここは実装簡略化：ConsumePress を2回呼ぶ方式でもOK（ただし起点がずれる場合あり）
				ConsumePress(a, windowFrames);
				ConsumePress(b, windowFrames);
				return true;
			}
		}
		return false;
	}

	// 指定フレーム内に押されたか？（消費可能）
	bool ConsumePress(Action a, uint32_t windowFrames = 0)
	{
		if (windowFrames == 0) windowFrames = bufferWindowFrames_;
		const uint32_t oldest = (frameCounter_ > windowFrames) ? (frameCounter_ - windowFrames) : 0;

		for (uint32_t k = 0; k < kBufferSize; ++k)
		{
			const uint32_t idx = (bufferHead_ + kBufferSize - 1 - k) % kBufferSize;
			const InputEvent& e = buffer_[idx];

			if (e.frame < oldest) break;
			if (e.action == a && e.type == InputEvent::Type::Press)
			{
				// “消費”しても探索の break 条件を壊さないように無効化
				buffer_[idx].action = Action::Max;
				buffer_[idx].frame = (std::numeric_limits<uint32_t>::max)();
				return true;
			}
		}
		return false;
	}

	// 指定フレーム内に長押しされたか？
	bool IsHeld(Action a, uint32_t holdFrames) const
	{
		const int i = Idx(a);
		if (!isDown_[i]) return false;
		return (frameCounter_ - pressFrame_[i]) >= holdFrames;
	}

private:
	static constexpr int Idx(Action a) { return static_cast<int>(a); }

	void ClearFrame();
	void BuildGameplayActions();
	void BuildUIActions(); // 必要なら

	// 入力合成ユーティリティ


	void RecordBufferedEvents();

	void PushEvent(Action a, InputEvent::Type t, float v1 = 0.0f, Vector2 v2 = {})
	{
		buffer_[bufferHead_] = InputEvent{ a, t, frameCounter_, v1, v2 };
		bufferHead_ = (bufferHead_ + 1) % kBufferSize;
	}



	// 同時押し
	bool HasPressInWindow(Action a, uint32_t fromFrame, uint32_t toFrame) const
	{
		for (uint32_t k = 0; k < kBufferSize; ++k)
		{
			const uint32_t idx = (bufferHead_ + kBufferSize - 1 - k) % kBufferSize;
			const InputEvent& e = buffer_[idx];
			if (e.frame < fromFrame) break;
			if (e.frame <= toFrame && e.action == a && e.type == InputEvent::Type::Press)
				return true;
		}
		return false;
	}
private:
	// 入力クラスのポインタ
	Engine::Input* input_ = nullptr;
	Character::BaseCharacter* character = nullptr; // キャラクターのポインタ

private:
	Context context_ = Context::Gameplay;


	// Action 状態（毎フレーム Update で作る）
	bool pressed_[static_cast<int>(Action::Max)] = {};
	bool triggered_[static_cast<int>(Action::Max)] = {};
	bool released_[static_cast<int>(Action::Max)] = {};
	Vector2 value2_[static_cast<int>(Action::Max)] = {};
	float value1_[static_cast<int>(Action::Max)] = {};

private:
	uint32_t frameCounter_ = 0;

	// バッファ（固定長リング）
	static constexpr uint32_t kBufferSize = 128;
	InputEvent buffer_[kBufferSize]{};
	uint32_t bufferHead_ = 0;

	// 何フレーム有効にするか（例：6f=0.1秒）
	uint32_t bufferWindowFrames_ = 6;

	// 長押し判定用
	uint32_t pressFrame_[static_cast<int>(Action::Max)] = {};
	bool isDown_[static_cast<int>(Action::Max)] = {};
};
