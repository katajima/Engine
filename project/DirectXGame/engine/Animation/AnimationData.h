#pragma once
#include "imgui.h"
#include<string>
#include"DirectXGame/engine/math/MathFunctions.h"

// タイマーの構造体
struct Timer {
	float t = 0.0f;				// タイマーの値
	float maxT = 1.0f;			// タイマーの最大値
	bool isMaxOverStr = false;	// タイマーが最大値に達したらtを初期値に戻すフラグ
	bool isClock = false;		//クロック機能を使うか
	int clock = 1;				// クロック

	// 更新
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

	// 補間位置取得
	float LerpT() const {
		return t / maxT; // タイマーの値を最大値で割って0から1の範囲に変換する
	}
	// 反対補間位置取得
	float InvLerpT() const {
		return 1.0f - LerpT();
	}

	// タイマーが超えているか判定
	bool IsMaxOverT() const {
		if (t >= maxT) {
			return true; // タイマーの値が最大値を超えた場合
		}
		else {
			return false; // タイマーの値が最大値以下の場合
		}
	}

	// クロック取得
	int GetClock() const { return clock; }

	// デバッグImGui
	void DebugImGui(const std::string name = "") {
#ifdef _DEBUG
		ImGui::Begin(name.c_str());
		ImGui::DragFloat("t", &t, 0.01f);
		ImGui::DragFloat("MaxT", &maxT, 0.01f);
		ImGui::SliderInt("clock", &clock, -1, 1);
		float  lerpT = LerpT();
		ImGui::InputFloat("lerpT", &lerpT);
		ImGui::Checkbox("isMaxOverStr", &isMaxOverStr);
		ImGui::Checkbox("isClock", &isClock);
		ImGui::End();
#endif // _DEBUG
	}

private:
	// クロックする
	void Clock() {
		if (isClock) {
			clock *= -1;	// クロック反転
		}
	}

	// 時間が来たらクロック関数に
	void MaxOverSetTimer(float t) {
		this->t = t;
		Clock();		// クロック反転
	}

};


// 補間系構造体
struct LerpMinMax
{
	float strT = 0.0f;				// 初期T
	float currentT = 0.0f;			// 現在T
	float minT = 0.0f;				// 最小T
	float maxT = 1.0f;				// 最大T
	float speedT = 0.01f;			// 速度T
	float strP = 0.0f;				// スタート地点
	float endP = 0.0f;				// 最終地点
	bool isMinMaxOverStr = false;	// 初期値に戻すか
	bool isOver = false;			// 値が超えてもいいか

	// 初期化
	// 視点
	// 終点
	// 初期Tの位置
	// スピード設定
	// 最小値
	// 最大値
	// 時間オーバーしたら戻すか
	// 時間オーバーしたら止めるか
	void Initialize(float strP,float endP, float strT,float speed, float min = 0.0f, float max = 1.0f, bool isMinMaxOverStr = false, bool isOver = false) {
		
		this->strP = strP;
		this->endP = endP;
		this->strT = strT;
		currentT = strT;
		minT = min;
		maxT = max;
		speedT = speed;
		this->isMinMaxOverStr = isMinMaxOverStr;
		this->isOver = isOver;
	}

	// 更新
	void Update(float dt) {

		if (isOver) {
			currentT += speedT * dt;
		}
		else {
			if ((minT <= currentT && maxT >= currentT)) {
				currentT += speedT * dt;

				if (minT >= currentT) {
					if (isMinMaxOverStr) {
						currentT = strT;
					}
					else {
						currentT = minT;
					}
				}
				else if (maxT <= currentT) {
					if (isMinMaxOverStr) {
						currentT = strT;
					}
					else {
						currentT = maxT;
					}
				}
			}
		}

	}

	// Tの位置取得
	float LerpData() const { return Lerp(strP, endP, currentT); }
};