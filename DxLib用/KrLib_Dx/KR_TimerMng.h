/*
   - KR_TimerMng.h - (DxLib)
   ver.2026/06/14

   時間管理機能。[試作品]
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
	//予約実行関数.
	struct ReservFunc
	{
		function<void()> func{}; //関数ポインタ.
		float            time{}; //何秒後に実行するか.
	};

	class TimerMng : public ManagerBase
	{
	private:
		vector<ReservFunc> functions{}; //予約リスト.
		Timer              gameTimer{}; //計測タイマー.

	public:
		//コンストラクタ.
		TimerMng(int order) : ManagerBase(order) {}

		//予約実行する.
		void ReservExe(float timer, function<void()> func);
		//全ての予約を中止する.
		void ReservCancelAll();

		void Init()   override;
		void Reset()  override {}
		void Update() override;
		void Draw()   override {}
	};
}