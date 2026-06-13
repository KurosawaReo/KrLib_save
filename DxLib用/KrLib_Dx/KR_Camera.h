/*
   - KR_Camera.h - (DxLib)
   ver.2026/06/12

   カメラ機能。現状2Dのみ対応。
*/
#pragma once
//[include] KR_Global.
#if !defined DEF_KR_DX_GLOBAL
  #include "KR_Global.h"
#endif

//KrLib名前空間.
namespace KR
{
	/*
	   カメラ管理.

	   全ての描画関数(→KR_Draw)は、このカメラの位置を元に描画される.
	   (UIのようにカメラに追従することも可)

	   またDrawRota関数を使うと、カメラの角度も考慮して描画される.
	*/
	class Camera final
	{
	//▼ ===== 実体 ===== ▼.
	private:
		static Camera inst;  //実体を入れる用.

	//▼ ===== 変数 ===== ▼.
	private:
		DBL_XY  cameraPos{}; //カメラ座標.
		double  cameraAng{}; //カメラ角度(2D用)

	//▼ ===== 関数 ===== ▼.
	private:
		//コンストラクタ.
		Camera() : cameraPos(0, 0), cameraAng(0) {}

	public:
		//get.
		static DBL_XY GetPos() { return {inst.cameraPos.x, inst.cameraPos.y}; }
		static double GetAng() { return inst.cameraAng; }

		//set, add.
		static void SetPos(DBL_XY _pos);
		static void SetAng(double _ang);
		static void AddPos(DBL_XY _pos, bool isLocal = false);
		static void AddAng(double _ang);

		//使用禁止.
		Camera(const Camera&) = delete;
		Camera& operator=(const Camera&) = delete;
	};
}