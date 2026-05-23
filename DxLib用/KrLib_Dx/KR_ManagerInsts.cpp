/*
   - KR_ManagerInsts.cpp - (DxLib)
*/
#include "KR_ManagerInsts.h"

//[include] ".cpp"ファイルでのみ使うもの.
#include <algorithm>

//KrLib名前空間.
namespace KR
{
	ManagerInsts ManagerInsts::inst;

	//order値で並べ替え.
	void ManagerInsts::SortOrder() {

		std::sort(
			//管理クラス配列.
			inst.mngInsts.begin(), 
			inst.mngInsts.end(),
			//order値を比較.
			[](const auto& a, const auto& b) {
				return a->GetOrder() < b->GetOrder();
			}
		);
	}

	//管理クラスを探す.
	ManagerBase* ManagerInsts::GetMngClass(const std::type_info& type) const {
		//保存された管理クラスを全ループ.
		for (auto& m : mngInsts) {
			if (typeid(*m) == type) {
				return m.get(); //unique_ptrから取得
			}
		}
		return nullptr;
	}
}