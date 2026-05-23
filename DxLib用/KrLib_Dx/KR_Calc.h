/*
   - KR_Calc.h - (DxLib)
   ver.2026/05/21

   当たり判定などの計算機能。
*/
#pragma once
//[include] KR_Global.
#if !defined DEF_KR_DX_GLOBAL
  #include "KR_Global.h"
#endif

//KrLib名前空間.
namespace KR
{
	//当たった面.
	enum class HitSurface
	{
		Horizontal, //左右面.
		Vertical,   //上下面.
		None        //当たってない.
	};

	//計算用の関数群.
	namespace Calc
	{
		//当たり判定.
		bool	HitCirCir	(const Circle& cir1, const Circle& cir2);
		bool	HitBoxBox	(const Box&    box1, const Box&    box2);
		bool	HitBoxCir	(const Box&    box,  const Circle& cir, HitSurface* ori = nullptr);
		bool	HitLineCir	(const Line&   line, const Circle& cir);
		bool	HitPie		(const Pie&    pie,  DBL_XY pos);

		//ベジエ曲線,スプライン曲線.
		DBL_XY	BezierPoint	(const BezierLine& bLine,  double time);
		DBL_XY	SplinePoint	(const Spline&     spline, int degree, double time);
	};
}