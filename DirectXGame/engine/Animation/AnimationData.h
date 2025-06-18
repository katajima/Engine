#pragma once
#include "imgui.h"
#include<string>

// タイマーの構造体
struct Timer {
	float t = 0.0f;				// タイマーの値
	float maxT = 1.0f;			// タイマーの最大値
	bool isMaxOverStr = false;	// タイマーが最大値に達したらtを初期値に戻すフラグ
	bool isClock = false;		//クロック機能を使うか
	int clock = 1;				// クロック
	
	float Update(float dt) {
		if (clock == 1) {
			t += dt; // タイマーの値を更新する
			if (t > maxT) {
				if (isMaxOverStr) {
					MaxOverSetTimer(0); // 経過時間を0に
				}
				else {
					MaxOverSetTimer(maxT); // タイマーの値が最大値を超えないようにする
				}
			}
		}
		else {
			t -= dt; // タイマーの値を更新する
			if (t < 0) {
				if (isMaxOverStr) {
					MaxOverSetTimer(maxT); // タイマーの値が最大値を超えないようにする
				}
				else {
					MaxOverSetTimer(0); // 経過時間を0に
				}
			}
		}
		return t; // 更新後のタイマーの値を返す
	}

	float LerpT() const {
		return t / maxT; // タイマーの値を最大値で割って0から1の範囲に変換する
	}

	bool IsMaxOverT() const {
		if (t >= maxT) {
			return true; // タイマーの値が最大値を超えた場合
		}
		else {
			return false; // タイマーの値が最大値以下の場合
		}
	}

	int GetClock() const { return clock; }

	void DebugImGui(const std::string name = "") {
#ifdef _DEBUG
		ImGui::Begin(name.c_str());
		ImGui::DragFloat("t", &t, 0.01f);
		ImGui::DragFloat("MaxT", &maxT, 0.01f);
		ImGui::SliderInt("clock", &clock, -1,1);
		float  lerpT = LerpT();
		ImGui::InputFloat("lerpT", &lerpT);
		ImGui::Checkbox("isMaxOverStr", &isMaxOverStr);
		ImGui::Checkbox("isClock", &isClock);
		ImGui::End();
#endif // _DEBUG
	}

private:
	void Clock() {
		if (isClock) {
			clock *= -1;	// クロック反転
		}
	}

	void MaxOverSetTimer(float t) {
		this->t = t;	
		Clock();		// クロック反転
	}

};