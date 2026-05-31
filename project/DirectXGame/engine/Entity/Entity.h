#pragma once

#include <string>

#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"

namespace Engine {
	class ShadowMap;

	/// <summary>
	/// EntityManagerが一括管理するための共通基底クラス。
	/// </summary>
	class Entity {
	public:
		virtual ~Entity() = default;

		// 物理や入力など、描画用更新の前に進めたい処理。
		virtual void RigidBodyUpdate() {}
		// 毎フレーム更新。
		virtual void Update() {}
		// 通常描画。
		virtual void Draw() {}
		// シャドウマップ用の深度描画。不要なEntityは何もしない。
		virtual void DrawShadowMap(ShadowMap*) {}
	public:
		// 削除フラグ取得
		virtual bool GetIsDelete() const { return isDelete_; }
		// 削除フラグ設定
		virtual void IsDelete() { isDelete_ = true; }

		// 名前取得
		virtual const std::string& GetName() const { return name_; }
		// 名前設定
		virtual void SetName(const std::string& name) { name_ = name; }
		// タグ取得
		virtual const std::string& GetNameTag() const { return nameTag_; }
		// タグ設定
		virtual void SetNameTag(const std::string& name) { nameTag_ = name; }
		// ワールド変換のポインタ取得。必要なEntityはオーバーライドして返す。
		virtual WorldTransform* GetWorldTransformPtr() { return nullptr; }
		// ワールド変換のポインタ取得(定数)。必要なEntityはオーバーライドして返す。
		virtual const WorldTransform* GetWorldTransformPtr() const { return nullptr; }

	protected:
		// 名前
		std::string name_{};
		// タグ
		std::string nameTag_{};
		// 削除フラグ
		bool isDelete_ = false;
	};
}
