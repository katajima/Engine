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
	
	/// <summary>
	/// 最小フレーム
	/// </summary>
	int GetFrameMin() const override { return 0; }		
	/// <summary>
	/// 全体フレーム幅
	/// </summary>
	int GetFrameMax() const override { return maxFrame_; }
	/// <summary>
	/// アイテム数
	/// </summary>
	int GetItemCount() const override { return (int)events.size(); }	

	/// <summary>
	/// 開始フレーム、終了フレーム、タイプ、色を返す
	/// </summary>
	void Get(int index, int** start, int** end, int* type, unsigned int* color) override {
		if (start) *start = &events[index].startFrame;
		if (end)   *end = &events[index].endFrame;
		if (type)  *type = 0;
		if (color) *color = events[index].color;
	}

	/// <summary>
	/// アイテムのラベルを返す
	/// </summary>
	const char* GetItemLabel(int index) const override {
		return events[index].name.c_str();
	}

	/// <summary>
	/// [+] 追加ボタンで呼ばれる処理
	/// </summary>
	void Add(int /*type*/) override {
		events.push_back({ 0, 10, 0xFF00FF00, "NewEvent" });
	}

	/// <summary>
	/// [-] 削除ボタンで呼ばれる処理
	/// </summary>
	void Del(int index) override {
		events.erase(events.begin() + index);
	}

	/// <summary>
	/// 複製ボタンで呼ばれる処理
	/// </summary>
	void Duplicate(int index) override {
		events.push_back(events[index]);
	}

	/// <summary>
	/// イベントの追加
	/// </summary>
	void AddEvent(const AttackEvent& event) {
		events.push_back(event);
	}

	/// <summary>
	/// 同名イベントがあれば更新し、なければ追加する
	/// </summary>
	void UpsertEvent(const AttackEvent& event) {
		for (AttackEvent& current : events) {
			if (current.name == event.name) {
				current = event;
				return;
			}
		}
		events.push_back(event);
	}

	/// <summary>
	/// 指定した接頭辞で始まるイベントを削除する
	/// </summary>
	void RemoveEventsByPrefix(const std::string& prefix) {
		events.erase(
			std::remove_if(events.begin(), events.end(),
				[&prefix](const AttackEvent& event) {
					return event.name.rfind(prefix, 0) == 0;
				}),
			events.end());
	}

	/// <summary>
	/// イベントのクリア
	/// </summary>
	void ClearEvents() {
		events.clear();
	}

public: // 編集系
	/// <summary>
	/// 最大フレーム設定
	/// </summary>
	void  SetFrameMax(int frame) { maxFrame_ = frame; }

public: // 取得系

	/// <summary>
	/// イベント一覧取得
	/// </summary>
	const std::vector<AttackEvent>& GetEvents() const {
		return events;
	}

	/// <summary>
	/// イベント取得
	/// </summary>
	const AttackEvent& GetEvent(int index) const {
		return events[index];
	}

	/// <summary>
	/// 名前でイベント取得
	/// </summary>
	const AttackEvent& GetEvent(const std::string name) const {
		/// <summary>
		/// / 名前で検索
		/// </summary>
		for(int i = 0; i < static_cast<int>(events.size()); ++i) {
			if (events[i].name == name) {
				return events[i];
			}
		}

		/// 見つからなかった場合は空のイベントを返す
		return events[0];
	}

	/// <summary>
	/// 名前でイベント取得できるか
	/// </summary>
	bool TryGetEvent(const std::string& name, AttackEvent& out) const {
		for (const AttackEvent& event : events) {
			if (event.name == name) {
				out = event;
				return true;
			}
		}
		return false;
	}

	/// <summary>
	/// イベント名取得
	/// </summary>
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
