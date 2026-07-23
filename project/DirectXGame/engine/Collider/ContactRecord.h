#pragma once
#include"DirectXGame/engine/math/MathFunctions.h"
#include<vector>
#include <algorithm> // std::any_of を使用するために必要

/// <summary>
/// 接触記録
/// </summary>
namespace Engine {
/// <summary>
/// ContactRecordを管理・実装するクラス。
/// </summary>
	class ContactRecord {
	public:
		/// <summary>
		/// 登録 or 上書き
		/// </summary>
		void AddHistory(uint32_t id, float nowTime) {
			for (auto& info : history_) {
				if (info.id == id) {
					info.timeStamp = nowTime; // 上書き
					return;
				}
			}
			history_.push_back({ id, nowTime }); // 新規追加
		}

		/// <summary>
		/// 履歴チェック
		/// </summary>
		bool CheckHistory(uint32_t number) {
			return std::any_of(history_.begin(), history_.end(), [number](const ContactInfo& info) {
				return info.id == number;
				});
		};
		/// <summary>
		/// クールタイム付きチェック
		/// </summary>
		bool CheckHistory(uint32_t id, float nowTime, float coolTimeSec) {
			Cleanup(nowTime, coolTimeSec);
			for (const auto& info : history_) {
				if (info.id == id) {
					if ((nowTime - info.timeStamp) <= coolTimeSec) {
						return true; // クールタイム中
					}
				}
			}
			return false; // ヒットしても良い
		}

		/// <summary>
		/// 履歴抹消
		/// </summary>
		void Clear() { history_.clear(); };

	private:
		struct ContactInfo {
			uint32_t id = 0;	// ID
			float timeStamp = 0;// ゲーム内時間（秒）
		};
		// 履歴
		std::vector<ContactInfo> history_;
	private:
		/// <summary>
		/// 有効期限切れの履歴を削除
		/// </summary>
		void Cleanup(float nowTime, float coolTimeSec) {
			history_.erase(
				std::remove_if(history_.begin(), history_.end(),
					[nowTime, coolTimeSec](const ContactInfo& info) {
						return (nowTime - info.timeStamp) > coolTimeSec;
					}),
				history_.end());
		}

	};
}