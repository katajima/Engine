#pragma once
#include"DirectXGame/engine/UI/UIElement.h"

// UIエレメントやウィンチェスターウィジットを収めるクラス


namespace Engine {
	// 前方宣言
	class EntityManager;

	/// <summary>
	/// UIボードクラス
	/// </summary>
	class UIBaseBoard {
	public:
		// 初期化
		void Init(InputSystem* inputSystem, EntityManager* entityManager, const std::string& name, Vector2 pos, const Vector2& size, bool isStatic = true, std::string textureName = "resources/Texture/Image.png");

		// 更新
		void Update(float deltaTime);
		// 描画
		void Draw();
		// 名前取得
		std::string GetName() { return name_; };
		//ボードを使うか
		void SetUse(bool use) { useBoard_ = use; }

		// UI生成
		void CreateUIElement(UIType type, std::string name, Vector2 pos, int instance = 1, bool useSprite = false);

		// UI取得
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

		// 画面比率設定
		void SetImageLeftTopPosAndRatio(Vector2 leftTopPos, Vector2 ratio) {
			leftTopPos_ = leftTopPos;
			ratio_ = ratio;
		};

	private:
		EntityManager* entityManager = nullptr;
		InputSystem* inputSystem = nullptr;

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
}