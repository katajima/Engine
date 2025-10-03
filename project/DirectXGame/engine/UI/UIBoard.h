#pragma once
#include "UIData.h"
#include "BaseSprite.h"
#include"DirectXGame/engine/UI/UIElement.h"

// UIエレメントやウィンチェスターウィジットを収めるクラス


class Entity2DManager;
class UIBaseBoard {
public:

	void Init(Input* input,Entity2DManager* entity2DManager, const std::string& name, Vector2 pos, const Vector2& size, bool isStatic = true, std::string textureName = "resources/Texture/Image.png");


	void Update(float deltaTime);

	void Draw();

	std::string GetName() { return name_; };

	void SetUse(bool use) { useBoard_ = use; }


	void CreateUIElement(UIType type, std::string name, Vector2 pos, int instance = 1, bool useSprite = false);


	template <typename T>
	T* GetUIElement(UIType type, std::string name)
	{
		// type（＝外側のマップ） が存在するか確認
		auto outerIt = uiElement_.find(type);
		if (outerIt == uiElement_.end()) {
			return nullptr;
		}

		// name（＝内側のマップ） が存在するか確認
		auto& innerMap = outerIt->second;
		auto innerIt = innerMap.find(name);
		if (innerIt == innerMap.end()) {
			return nullptr;
		}

		// UIElement のポインタを返す（unique_ptr から生ポインタに）
		return dynamic_cast<T*>(innerIt->second.get());
	}

	void SetImageLeftTopPosAndRatio(Vector2 leftTopPos, Vector2 ratio) {
		leftTopPos_ = leftTopPos;
		ratio_ = ratio;
	};

private:
	Entity2DManager* entity2DManager_;
	Input* input_;

	// ボードスプライト
	std::unique_ptr<BaseSprite> spriteBoard_ = nullptr;
	// バー
	std::unique_ptr<BaseSprite> spriteBar_ = nullptr;
	// UIタイプ
	std::map<UIType, std::map<std::string, std::unique_ptr<UIElement>>> uiElement_;



	Vector2 leftTopPos_{};	// 画面左上位置(ImGuiImage)
	Vector2 ratio_{};		// 画面サイズとImGuiImageにした比率
private:
	// ボードの名前
	std::string name_ = "no";
	//
	std::string textureName_ = "no";
	// 動かないか
	bool isStatic_ = true;
	// ボードのサイズ
	Vector2 size_ = { 400,400 };
	// ボード色
	Color color = { 1.0f,1.0f,1.0f,1.0f };
	// スクロール
	bool boardScrol_ = false;
	// 使っているか
	bool useBoard_ = true;
};
