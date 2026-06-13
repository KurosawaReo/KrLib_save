/*
   - KR_Global.h - (DxLib)
   ver.2026/05/21

   KrLib全体で使う汎用プログラム。
*/
#pragma once

//このヘッダが定義されているか判別する用.
#define DEF_KR_DX_GLOBAL

//C++用のグローバルを導入.
#if !defined DEF_KR_CPP_GLOBAL
  #include "../KrLib_cpp/KR_Global.h"
#endif
//DxLibを導入.
#include "DxLib.h"

//KrLib名前空間.
namespace KR
{
	//カラーID(15色対応)
	enum class ColorID
	{
		Red,
		Orange,
		Yellow,
		Lime,
		Green,
		SpringGreen,
		SkyBlue,
		Cyan,
		Blue,
		Purple,
		Pink,
		Magenta,
		White,
		Gray,
		Black,
	};

	//色データ.
	class MY_COLOR
	{
	private:
		COLOR_U8 color;

	public:
		//コンストラクタ.
		MY_COLOR();
		MY_COLOR(int _r, int _g, int _b);
		MY_COLOR(int _r, int _g, int _b, int _a);
		MY_COLOR(UINT _colorCode);
		MY_COLOR(ColorID id);
		//get.
		COLOR_U8 GetColorU8()   const;
		UINT     GetColorCode() const;

		//代入演算子.
		void operator=(const COLOR_U8& rgba);
		void operator=(UINT colorCode);
		void operator=(ColorID id);
		//比較演算子.
		bool operator==(ColorID id) const;
	};

	/*
	   ▼図形用の型
	   for文で回した時に、変えたい値はメンバにする.
	   共通でもいいものは描画関数の引数でもいい.
	*/

	//円データ.
	struct Circle
	{
		DBL_XY   pos;    //座標.
		float    r;      //半径.
		MY_COLOR color;  //色.
		float    thick;  //線の太さ.

		//コンストラクタ.
		Circle();
		Circle(DBL_XY _pos, float _r, MY_COLOR _color, float _thick);
	};
	//四角形データ.
	struct Box
	{
		DBL_XY   pos;    //座標.
		DBL_XY   size;   //サイズ.
		MY_COLOR color;  //色.
		float    thick;  //線の太さ.

		//コンストラクタ.
		Box();
		Box(DBL_XY _pos, DBL_XY _size, MY_COLOR _color, float _thick);
	};
	//三角形データ.
	struct Triangle
	{
		DBL_XY   pos[3]; //3点の座標.
		MY_COLOR color;  //色.
		float    thick;  //線の太さ.

		//コンストラクタ.
		Triangle();
		Triangle(DBL_XY _pos1, DBL_XY _pos2, DBL_XY _pos3, MY_COLOR _color, float _thick);
	};
	//線データ.
	struct Line
	{
		DBL_XY   stPos;  //始点座標.
		DBL_XY   edPos;  //終点座標.
		MY_COLOR color;  //色.
		float    thick;  //線の太さ.

		//コンストラクタ.
		Line();
		Line(DBL_XY _stPos, DBL_XY _edPos, MY_COLOR _color, float _thick);
	};
	//扇形データ.
	struct Pie
	{
		DBL_XY   pos;    //中心座標.
		double   r;      //半径.
		double   stAng;  //開始角度.
		double   arcAng; //弧の角度.
		MY_COLOR color;  //色.
		float    thick;  //線の太さ.

		//コンストラクタ.
		Pie();
		Pie(DBL_XY _pos, double _r, double _stAng, double _arcAng, MY_COLOR _color, float _thick);
	};
	//ベジエ曲線.
	struct BezierLine
	{
		DBL_XY   stPos;      //始点座標    (start pos)
		DBL_XY   edPos;      //終点座標    (end   pos)
		DBL_XY   stContrPos; //始点側制御点(start control pos)
		DBL_XY   edContrPos; //終点側制御点(end   control pos)
		MY_COLOR color;      //色.
		float    thick;      //線の太さ.

		//コンストラクタ.
		BezierLine();
		BezierLine(DBL_XY _stPos, DBL_XY _edPos, DBL_XY _stContrPos, DBL_XY _edContrPos, MY_COLOR _color, float _thick);
	};
	//スプライン曲線.
	struct Spline
	{
		vector<DBL_XY> points; //制御点(可変)
		MY_COLOR       color;  //色.
		float          thick;  //線の太さ.

		//コンストラクタ.
		Spline();
		Spline(const vector<DBL_XY>& _points, MY_COLOR _color, float _thick);
	};
	//ポリゴン.
	struct Polygon
	{
		DBL_XY         pos;    //座標(基準点)
		vector<DBL_XY> points; //頂点(相対座標)
		MY_COLOR       color;  //色.
		float          thick;  //線の太さ.

		//コンストラクタ.
		Polygon();
		Polygon(DBL_XY _pos, const vector<DBL_XY>& _points, MY_COLOR _color, float _thick);
	};
}