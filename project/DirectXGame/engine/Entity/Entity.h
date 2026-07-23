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

		/// <summary>
		/// 物理や入力など、描画用更新の前に進めたい処理。
		/// </summary>
		virtual void RigidBodyUpdate() {}
		/// <summary>
		/// 毎フレーム更新。
		/// </summary>
		virtual void Update() {}
		/// <summary>
		/// 通常描画。
		/// </summary>
		virtual void Draw() {}
		/// <summary>
		/// シャドウマップ用の深度描画。不要なEntityは何もしない。
		/// </summary>
		virtual void DrawShadowMap(ShadowMap*) {}
		/// <summary>
		/// Entity固有のデバッグUI。必要な派生クラスだけ実装する。
		/// </summary>
		virtual void UpdateImgui() {}
	public:
		/// <summary>
		/// 削除フラグ取得
		/// </summary>
		virtual bool GetIsDelete() const { return isDelete_; }
		/// <summary>
		/// 削除フラグ設定
		/// </summary>
		virtual void IsDelete() { isDelete_ = true; }

		/// <summary>
		/// 名前取得
		/// </summary>
		virtual const std::string& GetName() const { return name_; }
		/// <summary>
		/// 名前設定
		/// </summary>
		virtual void SetName(const std::string& name) { name_ = name; }
		/// <summary>
		/// タグ取得
		/// </summary>
		virtual const std::string& GetNameTag() const { return nameTag_; }
		/// <summary>
		/// タグ設定
		/// </summary>
		virtual void SetNameTag(const std::string& name) { nameTag_ = name; }
		/// <summary>
		/// ワールド変換のポインタ取得。必要なEntityはオーバーライドして返す。
		/// </summary>
		virtual WorldTransform* GetWorldTransformPtr() { return nullptr; }
		/// <summary>
		/// ワールド変換のポインタ取得(定数)。必要なEntityはオーバーライドして返す。
		/// </summary>
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
