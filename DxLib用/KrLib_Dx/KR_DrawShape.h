/*
   - KR_DrawShape.h - (DxLib)
   ver.2026/06/13

   図形や画像の描画機能。
   (オブジェクト指向ver → KR_Object)
*/
#pragma once
//[include] KR_Global.
#if !defined DEF_KR_DX_GLOBAL
  #include "KR_Global.h"
#endif
//[include] ".h"ファイルで使うもの.
#include "KR_Draw.h"

//KrLib名前空間.
namespace KR
{
	void DrawPixelKR	 (DBL_XY pos, MY_COLOR color,                                                                       bool isCameraDisp = true);
	void DrawCircleKR	 (const Circle&     cir,    Anchor anc = Anchor::Mid, bool isFill = true,      bool isAnti = false, bool isCameraDisp = true);
	void DrawBoxKR		 (const Box&        box,    Anchor anc = Anchor::Mid, bool isFill = true,      bool isAnti = false, bool isCameraDisp = true);
	void DrawTriangleKR	 (const Triangle&   tri,                              bool isFill = true,      bool isAnti = false, bool isCameraDisp = true);
	void DrawLineKR		 (const Line&       line,                                                      bool isAnti = false, bool isCameraDisp = true);
	void DrawPieKR		 (const Pie&        pie,                                                       bool isAnti = false, bool isCameraDisp = true);
	void DrawArcKR		 (const Pie&        pie,                                                       bool isAnti = false, bool isCameraDisp = true);
	void DrawBezierLineKR(const BezierLine& bLine,                            bool isDot,              bool isAnti = false, bool isCameraDisp = true);
	void DrawSplineKR    (const Spline&     spline, int degree,               bool isDot,              bool isAnti = false, bool isCameraDisp = true);
	void DrawPolygonKR   (const Polygon&    poly,                             bool isSurround = false, bool isAnti = false, bool isCameraDisp = true);
	void FillPolygon     (const Polygon&    poly,                             bool isSurround = false,                      bool isCameraDisp = true);
}