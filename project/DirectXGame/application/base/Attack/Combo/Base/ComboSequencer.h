#pragma once
#include"DirectXGame/engine/base/Imgui/ImGuiManager.h"


struct AttackEvent {
	int startFrame;
	int endFrame;
	unsigned int color;
	std::string name;
};

//  攻撃シーケンス
class AttackSequence : public ImSequencer::SequenceInterface {
public:
	
	// 最小フレーム
	int GetFrameMin() const override { return 0; }		
	// 全体フレーム幅
	int GetFrameMax() const override { return 300; }	
	// アイテム数
	int GetItemCount() const override { return (int)events.size(); }	

	// 開始フレーム、終了フレーム、タイプ、色を返す
	void Get(int index, int** start, int** end, int* type, unsigned int* color) override {
		if (start) *start = &events[index].startFrame;
		if (end)   *end = &events[index].endFrame;
		if (type)  *type = 0;
		if (color) *color = events[index].color;
	}

	// アイテムのラベルを返す
	const char* GetItemLabel(int index) const override {
		return events[index].name.c_str();
	}

	// [+] 追加ボタンで呼ばれる処理
	void Add(int /*type*/) override {
		events.push_back({ 0, 10, 0xFF00FF00, "NewEvent" });
	}

	// [-] 削除ボタンで呼ばれる処理
	void Del(int index) override {
		events.erase(events.begin() + index);
	}

	// 複製ボタンで呼ばれる処理
	void Duplicate(int index) override {
		events.push_back(events[index]);
	}

	// イベントの追加
	void AddEvent(const AttackEvent& event) {
		events.push_back(event);
	}

	
public: // 取得系

	// イベント一覧取得
	const std::vector<AttackEvent>& GetEvents() const {
		return events;
	}

	// イベント名取得
	std::string GetEventName(int index) const {
		if (index >= 0 && index < static_cast<int>(events.size())) {
			return events[index].name;
		}
		return "";
	}


private:
	std::vector<AttackEvent> events;
};
