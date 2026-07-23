#pragma once
#include"DirectXGame/engine/base/Imgui/ImGuiManager.h"
#include <algorithm>

// イベント構造体
struct AttackEvent {
	int startFrame;
	int endFrame;
	unsigned int color;
	std::string name;
};

//  攻撃シーケンス
/// <summary>
/// AttackSequenceを管理・実装するクラス。
/// </summary>
class AttackSequence : public ImSequencer::SequenceInterface {
public:
	
	// 最小フレーム
	int GetFrameMin() const override { return 0; }		
	// 全体フレーム幅
	int GetFrameMax() const override { return maxFrame_; }
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

	// 同名イベントがあれば更新し、なければ追加する
	void UpsertEvent(const AttackEvent& event) {
		for (AttackEvent& current : events) {
			if (current.name == event.name) {
				current = event;
				return;
			}
		}
		events.push_back(event);
	}

	// 指定した接頭辞で始まるイベントを削除する
	void RemoveEventsByPrefix(const std::string& prefix) {
		events.erase(
			std::remove_if(events.begin(), events.end(),
				[&prefix](const AttackEvent& event) {
					return event.name.rfind(prefix, 0) == 0;
				}),
			events.end());
	}

	// イベントのクリア
	void ClearEvents() {
		events.clear();
	}

public: // 編集系
	// 最大フレーム設定
	void  SetFrameMax(int frame) { maxFrame_ = frame; }

public: // 取得系

	// イベント一覧取得
	const std::vector<AttackEvent>& GetEvents() const {
		return events;
	}

	// イベント取得
	const AttackEvent& GetEvent(int index) const {
		return events[index];
	}

	// 名前でイベント取得
	const AttackEvent& GetEvent(const std::string name) const {
		// / 名前で検索
		for(int i = 0; i < static_cast<int>(events.size()); ++i) {
			if (events[i].name == name) {
				return events[i];
			}
		}

		/// 見つからなかった場合は空のイベントを返す
		return events[0];
	}

	// 名前でイベント取得できるか
	bool TryGetEvent(const std::string& name, AttackEvent& out) const {
		for (const AttackEvent& event : events) {
			if (event.name == name) {
				out = event;
				return true;
			}
		}
		return false;
	}

	// イベント名取得
	std::string GetEventName(int index) const {
		if (index >= 0 && index < static_cast<int>(events.size())) {
			return events[index].name;
		}
		return "";
	}


private:
	// イベント一覧
	std::vector<AttackEvent> events;
	// 最大フレーム
	int maxFrame_ = 300;

};
