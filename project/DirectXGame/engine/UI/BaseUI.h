#pragma once
#include "DirectXGame/engine/UI/UIBoard.h"
#include <map>
#include <memory>
#include <string>
#include <vector>



// 前方宣言
class InputSystem;
namespace Engine {
	class EntityManager;
	class GlobalVariables;
}

/// <summary>
/// UI基底クラス
/// </summary>
class BaseUI
{
public:
	/// <summary>UIが使用する入力、描画リソース、調整値を初期化する。</summary>
	/// <param name="inputSystem">UI操作に使用する入力。表示専用UIではnullptrを許容する。</param>
	/// <param name="entityManager">スプライトとUI要素の生成元。</param>
	/// <param name="globalVariables">UI調整値の登録・保存先。調整値不要ならnullptrを許容する。</param>
	virtual void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables) = 0;

	/// <summary>UIの状態、アニメーション、入力を更新する。</summary>
	/// <param name="dt">秒単位のフレーム時間。</param>
	virtual void Update(float dt) = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>オフスクリーン画像の表示領域に合わせてUI座標変換を設定する。</summary>
	/// <param name="leftTopPos">表示画像の左上スクリーン座標。</param>
	/// <param name="ratio">実画面サイズに対する表示画像のXY倍率。</param>
	void SetImageLeftTopPosAndRatio(Vector2 leftTopPos, Vector2 ratio) {
		leftTopPos_ = leftTopPos;	// 画面左上座標
		ratio_ = ratio;				// 画面比率
	};


protected:
	/// <summary>呼び出し側が所有するスプライトへ共通表示設定を適用する。</summary>
	/// <param name="sprite">初期化対象。nullptr不可。</param>
	/// <param name="texFile">使用するテクスチャファイルパス。</param>
	/// <param name="pos">ピクセル単位の初期座標。</param>
	/// <param name="size">ピクセル単位の表示サイズ。</param>
	void InitSprite(Engine::Sprite* sprite,const std::string& texFile,Vector2 pos,Vector2 size);
	/// <summary>
	/// チェックボックス初期化
	/// </summary>
	void InitUICheckBox(std::string name,Vector2 pos);
	/// <summary>
	/// スライダー初期化
	/// </summary>
	void InitUISlider(std::string name, Vector2 pos);
	/// <summary>
	/// メーター初期化
	/// </summary>
	void InitUIMeter(std::string name, Vector2 pos, bool useSprite = false);
	/// <summary>
	/// ペア初期化
	/// </summary>
	void InitUIPair(std::string name, Vector2 pos);
	/// <summary>
	/// カウンター初期化
	/// </summary>
	void InitUICount(std::string name, Vector2 pos,int instance = 1,bool useSprite = false);


	/// <summary>登録済みの全UI要素を更新する。</summary>
	/// <param name="deltaTime">秒単位のフレーム時間。</param>
	void UpdateUIElement(float deltaTime);
	/// <summary>
	/// 描画
	/// </summary>
	void DrawUIElement();
	
	/// <summary>名前からチェックボックスを検索する。</summary>
	/// <param name="name">初期化時に登録した名前。</param>
	/// <returns>BaseUIが所有する非所有ポインター。未登録の場合はnullptr。</returns>
	Engine::UICheckBox* GetUICheckBox(const std::string& name);
	/// <summary>
	/// スライダー取得
	/// </summary>
	Engine::UISlider* GetUISlider(const std::string& name);
	/// <summary>
	/// メータ取得
	/// </summary>
	Engine::UIMeter* GetUIMeter(const std::string& name);
	/// <summary>
	/// ペア取得
	/// </summary>
	Engine::UIPair* GetUIPair(const std::string& name);
	/// <summary>
	/// カウンター取得
	/// </summary>
	Engine::UICount* GetUICount(const std::string& name);

protected:
	Engine::EntityManager* entityManager = nullptr;
	Engine::GlobalVariables* globalVariables = nullptr;
	InputSystem* inputSystem = nullptr;


	std::unique_ptr <Engine::UIBaseBoard> board_;


	std::map<std::string,std::unique_ptr<Engine::UICheckBox>> uiCheckBox_;
	std::map<std::string,std::unique_ptr<Engine::UISlider>> uiSlider_;
	std::map<std::string,std::unique_ptr<Engine::UIMeter>> uiMeter_;
	std::map<std::string,std::unique_ptr<Engine::UIPair>> uiPair_;
	std::map<std::string,std::unique_ptr<Engine::UICount>> uiCount_;

	


	Vector2 leftTopPos_{};	// 画面左上位置(ImGuiImage)
	Vector2 ratio_{};		// 画面サイズとImGuiImageにした比率



	std::vector<std::unique_ptr<Engine::Sprite>> sprite_;
private:
};

