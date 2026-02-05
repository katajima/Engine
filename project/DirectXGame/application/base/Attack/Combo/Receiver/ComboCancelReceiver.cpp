#include "ComboCancelReceiver.h"

namespace Combo {

	void CancelReceiver::Enter() {

	};
	
	void CancelReceiver::Exit() {
	};

	void CancelReceiver::Update(float timer) {
		bool isCancelStart = data_.start <= timer;			// キャンセル開始時間を過ぎたら
		bool isCancelEnd = data_.end >= timer;				// キャンセル終了時間より前なら

		// キャンセル受付時間内なら
		if (isCancelStart && isCancelEnd && data_.isCancel) {
			isCancel_ = true;
			return;
		}

	};

}