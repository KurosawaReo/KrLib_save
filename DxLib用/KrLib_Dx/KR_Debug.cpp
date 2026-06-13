/*
   - KR_Debug.cpp - (DxLib)
*/
#include "KR_Debug.h"

//[include] ".cpp"ファイルでのみ使うもの.
#include "KR_App.h"
#include "KR_DrawShape.h"
#include "KR_Camera.h"

//KrLib名前空間.
namespace KR
{
	//デバッグ用の関数群.
	namespace Debug 
	{
		//値を表示.
		void Log(MY_STRING text) {
			printfDx(_T("%s\n"),     text.c_str());
		}
		void Log(MY_STRING text, int value) {
			printfDx(_T("%s: %d\n"), text.c_str(), value);
		}
		void Log(MY_STRING text, float value) {
			printfDx(_T("%s: %f\n"), text.c_str(), value);
		}
		void Log(MY_STRING text, double value) {
			printfDx(_T("%s: %f\n"), text.c_str(), value);
		}
		void Log(MY_STRING text, bool value) {
			printfDx(_T("%s: %s\n"), text.c_str(), (value) ? _T("true") : _T("false"));
		}
		void Log(MY_STRING text, INT_XY pos) {
			printfDx(_T("%s: %d %d\n"),     text.c_str(), pos.x, pos.y);
		}
		void Log(MY_STRING text, DBL_XY pos) {
			printfDx(_T("%s: %.2f %.2f\n"), text.c_str(), pos.x, pos.y);
		}
		void Log(MY_STRING text1, MY_STRING text2) {
			printfDx(_T("%s: %s\n"), text1.c_str(), text2.c_str());
		}
		
		//マウス座標を表示(ウィンドウ座標)
		void LogMousePos() {

			//マウス座標取得.
			int mx = -1, my = -1;
			GetMousePoint(&mx, &my);
			//ウィンドウ座標を取得.
			INT_XY pos = App::ToCameraPos(DBL_XY(mx, my)).ToInt();
			
			//座標表示.
			DrawFormatString(100, 300, 0xFFFFFF, _T("マウス座標: %d, %d"), pos.x, pos.y);
			//円.
			DrawCircle(mx, my, 10, 0x60FFFF, false, 1);
			DrawCircle(mx, my, 7,  0x30A0A0);
			DrawCircle(mx, my, 5,  0x60FFFF);
			DrawCircle(mx, my, 3,  0xC0FFFF);
		}
		//コントローラIDの表示.
		void LogPadID() {
			DrawFormatString(100, 300, 0xFFFFFF, _T("pad:%d"), GetJoypadInputState(DX_INPUT_PAD1));
		}
		//行列の値を表示.
		void LogMatrix3(Matrix3 mat) {
			//サイズ取得.
			const int size = _int(mat.m.size());
			//行列ループ.
			for (int x = 0; x < size; x++) {
				for (int y = 0; y < size; y++) {
					DrawFormatString(50+x*50, 50+y*50, 0xFFFFFF, _T("%.2f"), mat.m[x][y]);
				}
			}
		}
		//ログ消去.
		void LogClear() {
			clsDx();
		}

		//座標を円で表示.
		void DispPos(INT_XY pos, MY_COLOR color) {
			Circle cir;
			cir.pos   = {_dbl(pos.x), _dbl(pos.y)};
			cir.r     = 10;
			cir.color = color;
			DrawCircleKR(cir);
		}
		void DispPos(DBL_XY pos, MY_COLOR color) {
			Circle cir;
			cir.pos   = pos;
			cir.r     = 10;
			cir.color = color;
			DrawCircleKR(cir);
		}

		//マウスカーソルに合わせてカメラを動かす.
		void MouseToMoveCamera() {

			//マウス座標取得.
			int mx = -1, my = -1;
			GetMousePoint(&mx, &my);
			//画面中央を(0, 0)とする.
			INT_XY mPos = INT_XY(mx, my) - App::GetWindowSize()/2;
			//カメラを動かす.
			Camera::SetPos(mPos.ToDbl());
		}
	}
}