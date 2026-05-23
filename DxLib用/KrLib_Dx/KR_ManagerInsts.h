/*
   - KR_ManagerInsts.h - (DxLib)
   ver.2026/05/07

   Managerを管理するクラス。
*/
#pragma once
//[include] KR_Global.
#if !defined DEF_KR_DX_GLOBAL
  #include "KR_Global.h"
#endif
//[include] ".h"ファイルで使うもの.
#include "KR_ManagerBase.h"

//KrLib名前空間.
namespace KR
{
	//Manager管理クラス.
	//ManagerBaseを継承したクラスを生成 & 保存する.
	class ManagerInsts
	{
	//▼ ===== 実体 ===== ▼.
	private:
		static ManagerInsts inst;

	//▼ ===== 変数 ===== ▼.
	private:
		vector<unique_ptr<ManagerBase>> mngInsts; //Managerの実体を保存する配列.

	//▼ ===== 関数 ===== ▼.
	private:
		//コンストラクタ.
		ManagerInsts(){}
		
		//管理クラスを探す.
		ManagerBase* GetMngClass(const std::type_info& type) const;

	public:
		//生成して登録.
		//ManagerBaseを継承したクラスのみ指定可能.
		template<class T, class... Args> requires std::derived_from<T, ManagerBase>
		static T* NewManager(Args&&... args) {

			//実体生成.
			auto obj = make_unique<T>(std::forward<Args>(args)...);
			T* ptr = obj.get(); //ポインタ取得.

			inst.mngInsts.emplace_back(std::move(obj)); //配列に所有権を移動.

			//ポインタを返す.
			return ptr;
		}

		//管理クラスを1つ取得.
		//ManagerBaseを継承したクラスのみ指定可能.
		template<class T> requires std::derived_from<T, ManagerBase>
		static T* Get() {
			//子クラスのポインタに変換できるようdynamic_castを使う.
			return dynamic_cast<T*>(inst.GetMngClass(typeid(T)));
		}
		
		//管理クラスを全て取得.
		static vector<unique_ptr<ManagerBase>>& GetAll() { return inst.mngInsts; }

		//order値で並べ替え.
		static void SortOrder();

		//使用禁止.
		ManagerInsts(const ManagerInsts&) = delete;
		ManagerInsts& operator=(const ManagerInsts&) = delete;
	};
}