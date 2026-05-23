/*
   - KR_Calc.h - (C++)
   ver.2026/05/21

   C++だけでできる計算機能。
*/
#pragma once
//[include] KR_Global.
#if !defined DEF_KR_CPP_GLOBAL
  #include "KR_Global.h"
#endif

//KrLib名前空間.
namespace KR
{
	//イージングタイプ.
	enum class EaseType
	{
		InQuad,
		OutQuad,
		InOutQuad,
		OutInQuad,
	};
	//ループイージングタイプ.
	enum class WaveType
	{
		CosLoop,
	};

	//計算用の関数群.
	namespace Calc
	{
		//範囲限界.
		void		FixPosInArea(DBL_XY* pos, INT_XY size, DBL_RECT rect);
		bool		IsOutInArea	(DBL_XY  pos, INT_XY size, DBL_RECT rect, bool isCompOut);

		//角度,ベクトル.
		double		Dist		(INT_XY pos1, INT_XY pos2);
		double		Dist		(DBL_XY pos1, DBL_XY pos2);
		DBL_XY		MidPos		(DBL_XY pos1, DBL_XY pos2);
		DBL_XY		ArcPos		(DBL_XY pos, double ang, double len);
		double		FacingAng	(DBL_XY from, DBL_XY to);
		DBL_XY      AngToVector	(double ang);
		double      AngleDiff   (double now, double target);

		//物理系.
		void        PhysicsVel	(DBL_XY* vel, DBL_XY maxVel, bool isGround, double gravity, double friction, double airDrag);

		//値の曲線変動(アニメーション用)
		double      AnimEase	(EaseType type, double time);
		double      AnimWave	(WaveType type, double time);

		//値の操作.
		int         RandNum		(int st, int ed);
		vector<int> RandNums	(int st, int ed, int count);
		double      GetDecimal	(double num);
	}
}