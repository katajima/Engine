#include "ComboNextReceiver.h"


namespace Combo {

	// 開始
	void NextReceiver::Enter() {
	};
	//　終了
	void NextReceiver::Exit() {
	};
	// 更新
	void NextReceiver::Update(float timer) {
		bool isInputStart = data_.inputStart <= timer;		// 受付開始時間を過ぎたら
		bool isInputEnd = data_.inputEnd >= timer;			// 受付終了時間より前なら



	};
	
}
