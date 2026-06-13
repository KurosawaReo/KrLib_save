/*
   - KR_ObjectList.h - (DxLib)
   ver.2026/06/14

   オブジェクトを扱うリスト。
*/
#pragma once
//[include] KR_Global.
#if !defined DEF_KR_DX_GLOBAL
  #include "KR_Global.h"
#endif
//[include] ".h"で使うもの.
#include "KR_ManagerBase.h"
#include "KR_Object.h"

//KrLib名前空間.
namespace KR
{
	/*
	   オブジェクト管理クラス.
	   ObjectShapeを継承したクラスのみ指定可能.

	   [注意]
	   Generateに引数を設定したい場合
	   管理するobject(Tクラス)に、同じ引数のコンストラクタを用意すること.
	*/
	template<typename T> requires std::derived_from<T, ObjectShape>
	class ObjectList
	{
	//▼ ===== 変数 ===== ▼.
	private:
		vector<unique_ptr<T>> objects; //object実体配列.

	//▼ ===== 関数 ===== ▼.
	public:
		//get.
		const vector<unique_ptr<T>>& GetObjects() const { return objects; } //object全取得.
		int GetObjectCnt() const { return _int(objects.size()); }

		//object生成.
		//コンストラクタの引数も設定可能.
		template<typename... Args> requires std::constructible_from<T, Args...>
		void NewObject(Args&&... args) {

			try {
				//コンストラクタ実行(引数は任意)
				auto obj = make_unique<T>(std::forward<Args>(args)...);
				//初期化.
				obj->Init();
				//所有権移動.
				objects.push_back(std::move(obj));
			}
			catch (const std::bad_alloc&) {
				throw ErrorMsg(_T("ObjectList<T>::NewObject"), _T("make_uniqueエラー"));
			}
		}

		//更新.
		void Update() {

			for (auto i = objects.begin(); i != objects.end(); ) {
				//activeなobjectのみ.
				if ((*i)->isActive) {
					//更新.
					(*i)->Update();
					//消滅条件を満たしたら.
					if ((*i)->IsErase()) {
						i = objects.erase(i); //削除する.
					}
					else {
						i++; //次のオブジェクト.
					}
				}
				else {
					i++; //次のオブジェクト.
				}
			}
		}

		//描画.
		void Draw() {
			//activeなobjectのみ.
			for (auto& i : objects) if (i->isActive) {
				i->Draw();
			}
		}
	};
}