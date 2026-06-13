/*
   - KR_App.cpp - (DxLib)
*/
#include "KR_App.h"

//[include] ".cpp"ファイルでのみ使うもの.
#include "KR_Camera.h"
#include "KR_Debug.h"
#include "KR_ManagerBase.h"
#include "KR_ManagerInsts.h"

//KrLib名前空間.
namespace KR 
{
	App App::inst; //実体生成.

	//ウィンドウ矩形を取得(カメラ視点)
	INT_RECT App::GetWindowRect() {
		//カメラ座標取得.
		const INT_XY cmrPos = Camera::GetPos().ToInt();
		//矩形を返す.
		return { 
			-inst.windowSize.x/2 + cmrPos.x,
			-inst.windowSize.y/2 + cmrPos.y,
			 inst.windowSize.x/2 + cmrPos.x,
			 inst.windowSize.y/2 + cmrPos.y
		};
	}

	//ワールド座標に変換.
	DBL_XY App::ToWorldPos(DBL_XY pos) {
		DBL_XY wPos = pos;
		wPos += GetWindowSize().ToDbl()/2; //画面の左上が(0, 0)になるようにする.
		wPos -= Camera::GetPos();          //カメラ座標を除く.
		return wPos;
	}
	//カメラ座標に変換.
	DBL_XY App::ToCameraPos(DBL_XY pos) {
		DBL_XY cPos = pos;
		cPos -= GetWindowSize().ToDbl()/2; //画面の中央が(0, 0)になるようにする.
		cPos += Camera::GetPos();          //カメラ座標を反映.
		return cPos;
	}

	//DxLibの初期化処理.
	void App::InitDx(int windowWid, int windowHei, bool isWindowMode, int fps, bool isVSync) {

		try {
			ChangeWindowMode(isWindowMode);				//TRUEでwindow, FALSEで全画面にする.
			SetWindowSize   (windowWid, windowHei);		//ウィンドウサイズの設定.
			SetGraphMode    (windowWid, windowHei, 32); //解像度の設定.
			SetWaitVSyncFlag((isVSync) ? TRUE : FALSE);	//FALSEでVSyncを無効化(FPS制限なし)
			SetDrawScreen   (DX_SCREEN_BACK);			//裏画面へ描画(ダブルバッファ)
			SetOutApplicationLogValidFlag(FALSE);		//FALSEでLog.txtにログを書き込まない.

			//DxLibの初期化.
			if (DxLib_Init() == -1) {
				throw ErrorMsg(_T("App::InitDx"), _T("初期化エラー"));
				return;
			}

			//fps計測用タイマー.
			inst.tmFps = TimerMicro(TimerMode::CountDown, 1000000/fps);
			inst.tmFps.Start();
			//値の保存.
			inst.windowSize = { windowWid, windowHei };
			inst.fps = fps;
			//変数初期化.
			inst.isQuit = false;

			//order値で並べ替える.
			ManagerInsts::SortOrder();
			//Initを実行.
			for (const auto& i : ManagerInsts::GetAll()) {
				i->Init();
			}
			//Resetを実行.
			Reset();
		}
		catch (const ErrorMsg& err) {
			//エラーメッセージ.
			Debug::Log(err.GetMsg());
			//待機.
			WaitKey();
		}
	}

	//DxLibのループ処理.
	void App::LoopDx() {

		try {
			//メインループ.
			//ESCが押されるか、終了サインがあれば終了.
			while (ProcessMessage() == 0 && !inst.isQuit) {
				//一定時間ごとに処理.
				if (inst.tmFps.IntervalTime()) {
					//画面クリア.
					ClearDrawScreen();
					//Update, Drawを実行.
					for (const auto& i : ManagerInsts::GetAll()) {
						if (i->IsAutoUpdate()) { i->Update(); }
						if (i->IsAutoDraw())   { i->Draw();   }
					}
					//表画面へ描画.
					ScreenFlip();
				}
			}
		}
		catch (const ErrorMsg& err) {
			//エラーメッセージ.
			Debug::Log(err.GetMsg());
			//待機.
			WaitKey();
		}

		inst.EndDx(); //終了処理.
	}

	//DxLibの終了処理.
	void App::EndDx() {
		DxLib_End();               //DxLibの終了処理.
		DeleteFile(_T("Log.txt")); //Log.txtが生成されるので消去する.
	}

	//全管理クラスのリセット.
	void App::Reset() {
		//Resetを実行.
		for (const auto& i : ManagerInsts::GetAll()) {
			i->Reset();
		}
	}
	//ゲームを終了する.
	void App::Quit() {
		inst.isQuit = true;
	}
}