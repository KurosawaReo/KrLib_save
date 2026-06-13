/*
   - KR_DrawShape.cpp - (DxLib)
*/
#include "KR_DrawShape.h"

//[include] ".cpp"ファイルでのみ使うもの.
#include "KR_App.h"
#include "KR_Calc.h"
#include "../KrLib_cpp/KR_Calc.h"
#include <algorithm>

//KrLib名前空間.
namespace KR
{
	//アンカー座標.
	static const DBL_XY ANCHOR_POS[9] = {
		{0.0, 0.0}, {0.5, 0.0}, {1.0, 0.0},
		{0.0, 0.5}, {0.5, 0.5}, {1.0, 0.5},
		{0.0, 1.0}, {0.5, 1.0}, {1.0, 1.0}
	};

	//ポリゴン塗りつぶし用.
	struct CrossData {
		double x;     //交点のx座標.
		int    delta; //巻数の変化量(+1/-1)
	};

// ▼*--=<[ function ]>=--*▼ //

	/*
	   DrawPixelの改造版.
	*/
	void DrawPixelKR(DBL_XY pos, MY_COLOR color, bool isCameraDisp) {
		//描画座標.
		DBL_XY newPos = (isCameraDisp) ? App::ToWorldPos(pos) : pos;
		//描画.
		int err = DrawPixel(_int_r(newPos.x), _int_r(newPos.y), color.GetColorCode());
		if (err < 0) {
			throw ErrorMsg(_T("DrawPixelKR"), _T("DrawPixelエラー"));
			return;
		}
	}

	/*
	   DrawCircleの改造版.
	*/
	void DrawCircleKR(const Circle& cir, Anchor anc, bool isFill, bool isAnti, bool isCameraDisp) {

		//描画座標.
		DBL_XY newPos = (isCameraDisp) ? App::ToWorldPos(cir.pos) : cir.pos;
		//基準点に座標をずらす.
		float x = _flt(newPos.x - (cir.r * 2 - 1) * (ANCHOR_POS[_int(anc)].x - 0.5));
		float y = _flt(newPos.y - (cir.r * 2 - 1) * (ANCHOR_POS[_int(anc)].y - 0.5));

		//アンチエイリアスあり.
		if (isAnti) {
			//posnum(角形数)は60に設定する.
			int err = DrawCircleAA(x, y, cir.r, 60, cir.color.GetColorCode(), isFill, cir.thick);
			if (err < 0) {
				throw ErrorMsg(_T("DrawCircleKR"), _T("DrawCircleAAエラー"));
				return;
			}
		}
		//アンチエイリアスなし.
		else {
			int err = DrawCircle(_int_r(x), _int_r(y), _int_r(cir.r), cir.color.GetColorCode(), isFill, _int_r(cir.thick));
			if (err < 0) {
				throw ErrorMsg(_T("DrawCircleKR"), _T("DrawCircleエラー"));
				return;
			}
		}
	}

	/*
	   DrawBoxの改造版.
	   thickはAA版にしかないため、AAなしの場合は使わない.
	*/
	void DrawBoxKR(const Box& box, Anchor anc, bool isFill, bool isAnti, bool isCameraDisp) {

		if (box.size.x <= 0.0 || box.size.y <= 0.0) {
			return;
		}

		//描画座標.
		DBL_XY newPos = (isCameraDisp) ? App::ToWorldPos(box.pos) : box.pos;
		//基準点に座標をずらす.
		float x1 = _flt(newPos.x - (box.size.x - 1) * ANCHOR_POS[_int(anc)].x); //始点.
		float y1 = _flt(newPos.y - (box.size.y - 1) * ANCHOR_POS[_int(anc)].y);
		float x2 = _flt(x1 + box.size.x - 1);                                   //終点.
		float y2 = _flt(y1 + box.size.y - 1);

		//アンチエイリアスあり.
		if (isAnti) {
			int err = DrawBoxAA(x1, y1, x2 + 1, y2 + 1, box.color.GetColorCode(), isFill, box.thick);
			if (err < 0) {
				throw ErrorMsg(_T("DrawBoxKR"), _T("DrawBoxAAエラー"));
				return;
			}
		}
		//アンチエイリアスなし.
		else {
			int err = DrawBox(_int(x1), _int(y1), _int(x2 + 1), _int(y2 + 1), box.color.GetColorCode(), isFill);
			if (err < 0) {
				throw ErrorMsg(_T("DrawBoxKR"), _T("DrawBoxエラー"));
				return;
			}
		}
	}

	/*
	   DrawTriangleの改造版.
	   thickはAA版にしかないため、AAなしの場合は使わない.
	*/
	void DrawTriangleKR(const Triangle& tri, bool isFill, bool isAnti, bool isCameraDisp) {

		//描画座標.
		DBL_XY newPos[3] = { tri.pos[0], tri.pos[1], tri.pos[2] };
		//カメラ基準に変換.
		if (isCameraDisp) {
			for (auto& i : newPos) { i = App::ToWorldPos(i); }
		}

		//アンチエイリアスあり.
		if (isAnti) {
			int err = DrawTriangleAA(
				_flt(newPos[0].x), _flt(newPos[0].y),
				_flt(newPos[1].x), _flt(newPos[1].y),
				_flt(newPos[2].x), _flt(newPos[2].y), tri.color.GetColorCode(), isFill, tri.thick
			);
			if (err < 0) {
				throw ErrorMsg(_T("DrawTriangleKR"), _T("DrawTriangleAAエラー"));
				return;
			}
		}
		//アンチエイリアスなし.
		else {
			int err = DrawTriangle(
				_int_r(newPos[0].x), _int_r(newPos[0].y),
				_int_r(newPos[1].x), _int_r(newPos[1].y),
				_int_r(newPos[2].x), _int_r(newPos[2].y), tri.color.GetColorCode(), isFill
			);
			if (err < 0) {
				throw ErrorMsg(_T("DrawTriangleKR"), _T("DrawTriangleエラー"));
				return;
			}
		}
	}

	/*
	   DrawLineの改造版.
	*/
	void DrawLineKR(const Line& line, bool isAnti, bool isCameraDisp) {

		//描画座標.
		DBL_XY newPos[2] = { line.stPos, line.edPos };
		//カメラ基準に変換.
		if (isCameraDisp) {
			for (auto& i : newPos) { i = App::ToWorldPos(i); }
		}

		//アンチエイリアスあり.
		if (isAnti) {
			int err = DrawLineAA(
				_flt(newPos[0].x), _flt(newPos[0].y),
				_flt(newPos[1].x), _flt(newPos[1].y), line.color.GetColorCode(), line.thick
			);
			if (err < 0) {
				throw ErrorMsg(_T("DrawLineKR"), _T("DrawLineAAエラー"));
				return;
			}
		}
		//アンチエイリアスなし.
		else {
			int err = DrawLine(
				_int_r(newPos[0].x), _int_r(newPos[0].y),
				_int_r(newPos[1].x), _int_r(newPos[1].y), line.color.GetColorCode(), _int_r(line.thick)
			);
			if (err < 0) {
				throw ErrorMsg(_T("DrawLineKR"), _T("DrawLineエラー"));
				return;
			}
		}
	}

	/*
	   扇形を描画.
	*/
	void DrawPieKR(const Pie& pie, bool isAnti, bool isCameraDisp) {

		DrawArcKR(pie, isAnti, isCameraDisp); //そのまま弧も描く.

		Line line; //描画用の線.

		//ベクトルを求める.
		DBL_XY vec1 = Calc::AngToVector(pie.stAng);            //扇の始まりの角度.
		DBL_XY vec2 = Calc::AngToVector(pie.stAng + pie.arcAng); //扇の終わりの角度.
		//座標を求める.
		DBL_XY pos1 = pie.pos + vec1 * pie.r;
		DBL_XY pos2 = pie.pos + vec2 * pie.r;

		//線1を描画.
		line.stPos = pos1;
		line.edPos = pie.pos;
		line.color = pie.color;
		line.thick = pie.thick;
		try {
			DrawLineKR(line, isAnti, isCameraDisp);
		}
		catch (const ErrorMsg& err) {
			throw ErrorMsg(_T("DrawPieKR"), err.GetMsg());
			return;
		}

		//線2を描画.
		line.stPos = pos2;
		try {
			DrawLineKR(line, isAnti, isCameraDisp);
		}
		catch (const ErrorMsg& err) {
			throw ErrorMsg(_T("DrawPieKR"), err.GetMsg());
			return;
		}
	}

	/*
	   円弧を描画.
	*/
	void DrawArcKR(const Pie& pie, bool isAnti, bool isCameraDisp) {

		const double addAng = 1.0;                    //一度で描く線の長さ.
		const double edAng = pie.stAng + pie.arcAng; //弧の終わりの角度.

		for (double i = pie.stAng; i <= edAng - addAng; i += addAng) {

			//角度の設定.
			double ang1 = i - 1;
			ang1 = max(ang1, pie.stAng); //下限.
			double ang2 = i + addAng + 1;
			ang2 = min(ang2, edAng);     //上限.
			//座標の設定.
			DBL_XY pos1 = Calc::ArcPos(pie.pos, ang1, pie.r); //繋ぎ目が綺麗になるよう角度を-1する.
			DBL_XY pos2 = Calc::ArcPos(pie.pos, ang2, pie.r); //繋ぎ目が綺麗になるよう角度を+1する.

			Line line;
			line.stPos = pos1;
			line.edPos = pie.pos;
			line.color = pie.color;
			line.thick = pie.thick;
			//線を描画.
			try {
				DrawLineKR(line, isAnti, isCameraDisp);
			}
			catch (const ErrorMsg& err) {
				throw ErrorMsg(_T("DrawArcKR"), err.GetMsg());
				return;
			}
		}
	}

	/*
	   ベジエ曲線描画.
	   isDotがtrueの場合、isAntiは使わない.
	*/
	void DrawBezierLineKR(const BezierLine& bLine, bool isDot, bool isAnti, bool isCameraDisp) {

		const int segments = 100; //曲線全体の分割数.

		Line line;                //描画用の線.
		line.color = bLine.color;
		line.thick = bLine.thick;

		//ベジェ曲線を細かい線分に分割.
		for (int i = 0; i < segments; i++) {
			//タイム値.
			const double t = _dbl(i) / segments;
			const double nt = _dbl(i + 1) / segments;
			if (isDot) {
				//1点を取得.
				DBL_XY pos = Calc::BezierPoint(bLine, t);
				//描画.
				try {
					DrawPixelKR(pos, bLine.color, isCameraDisp);
				}
				catch (const ErrorMsg& err) {
					throw ErrorMsg(_T("DrawBezierLineKR"), err.GetMsg());
					return;
				}
			}
			else {
				//2点を取得.
				line.stPos = Calc::BezierPoint(bLine, t);
				line.edPos = Calc::BezierPoint(bLine, nt);
				//線描画.
				try {
					DrawLineKR(line, isAnti, isCameraDisp);
				}
				catch (const ErrorMsg& err) {
					throw ErrorMsg(_T("DrawBezierLineKR"), err.GetMsg());
					return;
				}
			}
		}
	}

	/*
	   スプライン曲線描画.
	   isDotがtrueの場合、isAntiは使わない.
	*/
	void DrawSplineKR(const Spline& spline, int degree, bool isDot, bool isAnti, bool isCameraDisp) {

		const int segments = 100; //曲線全体の分割数.

		//スプライン曲線を細かい線分に分割.
		for (int i = 0; i < segments; i++) {
			//タイム値.
			double t = _dbl(i) / segments;
			double nt = _dbl(i + 1) / segments;

			if (isDot) {
				//1点を取得.
				DBL_XY pos = Calc::SplinePoint(spline, degree, t);
				//描画.
				try {
					DrawPixelKR(pos, spline.color, isCameraDisp);
				}
				catch (const ErrorMsg& err) {
					throw ErrorMsg(_T("DrawSplineKR"), err.GetMsg());
					return;
				}
			}
			else {
				//2点を取得.
				Line line;
				line.stPos = Calc::SplinePoint(spline, degree, t);
				line.edPos = Calc::SplinePoint(spline, degree, nt);
				line.thick = spline.thick;
				//描画.
				try {
					DrawLineKR(line, isAnti, isCameraDisp);
				}
				catch (const ErrorMsg& err) {
					throw ErrorMsg(_T("DrawSplineKR"), err.GetMsg());
					return;
				}
			}
		}
	}

	/*
	   ポリゴン描画.
	   基準点から頂点を相対座標で描く.
	*/
	void DrawPolygonKR(const Polygon& poly, bool isSurround, bool isAnti, bool isCameraDisp) {

		Line line; //描画用.
		line.color = poly.color;
		line.thick = poly.thick;

		//ポリゴンの全頂点.
		for (int i = 0; i < poly.points.size(); i++) {
			//囲わないなら最後はスキップ.
			if (i == poly.points.size() - 1) {
				if (!isSurround) { break; }
			}
			//始点と終点.
			line.stPos = poly.pos + poly.points[i];
			line.edPos = poly.pos + poly.points[(i + 1) % poly.points.size()]; //サイズを超えたら0に戻す.
			//描画.
			try {
				DrawLineKR(line, isAnti, isCameraDisp);
			}
			catch (const ErrorMsg& err) {
				throw ErrorMsg(_T("DrawPolygonKR"), err.GetMsg());
				return;
			}
		}
	}
	/*
	   ===== 問題 =====
	   以下のように、4本の線・3つの空間があるとする。

	   ｜□□｜□□｜□□｜

	   横に1列ずつ塗りつぶし処理を行い
	   偶奇規則で2つの線の間を塗るようにすると、内側に塗られない空間ができる。

	   ｜■■｜□□｜■■｜
	　 　　　　	  ↑ここも塗ってほしい

		   しかし、この空間が完全に囲まれているのか、外側へ通じているかは分からない。

		   ===== 解決法 =====
		   非ゼロ巻数規則を使い、線が下から来たか上から来たかを判定する。
		   「線が下に行って上に行った時、1つの"囲い"が成立する」この性質を利用したもの。

		   例: 真ん中が囲われてないケース
		   ↓■■↑□□↑■■↓

		   例: 真ん中が囲われてるケース
		   ↓■■↓■■↑■■↑

		   これを数値化すればプログラムで実装できる。

		   ===== プログラム =====
		   横一列を左から右へ調べる。"↓"を通過したら+1 / "↑"を通過したら-1
		   数値が0以外なら塗る。0なら塗らない。

		   ↓１１↓２２↑１１↑００
		*/
		//ポリゴンの1辺(pos1→pos2)と、水平線yが交差するか判定.
	bool IntersectEdge(const DBL_XY pos1, const DBL_XY pos2, double y, double* x, int* delta) {

		//yがpos1.y～pos2.yの間にある(=交差している)
		if ((pos1.y <= y && pos2.y > y) || (pos2.y <= y && pos1.y > y)) {

			//線分上の交点比率"t"を求める.
			//y = pos1.y + t * (pos2.y - pos1.y)
			double t = (y - pos1.y) / (pos2.y - pos1.y);

			//交点のx座標を返す.
			*x = pos1.x + t * (pos2.x - pos1.x);
			//巻数を増減する, yの高さを比較して傾きを調べる.
			*delta = (pos2.y > pos1.y) ? +1 : -1;

			return true;
		}
		return false;
	}
	//ポリゴンを塗りつぶす(非ゼロ巻数規則を使用)
	void FillPolygon(const KR::Polygon& poly, bool isSurround, bool isCameraDisp) {

		//画面の上下y座標を取得.
		const int upY = App::GetWindowRect().GetU().y;
		const int downY = App::GetWindowRect().GetD().y;

		//一列ずつループ.
		for (int y = upY; y < downY; y++) {

			//交点リスト.
			vector<CrossData> crossList;

			//ポリゴンの全頂点.
			for (int i = 0; i < poly.points.size(); i++) {
				//囲わないなら最後はスキップ.
				if (i == poly.points.size() - 1) {
					if (!isSurround) { break; }
				}
				//絶対座標を取得.
				const DBL_XY& p0 = poly.pos + poly.points[i];
				const DBL_XY& p1 = poly.pos + poly.points[(i + 1) % poly.points.size()];
				//引数受け取り用.
				double x; int delta;
				//今の一列がポリゴンの一辺と交差するなら.
				if (IntersectEdge(p0, p1, y, &x, &delta)) {
					crossList.push_back({ x, delta }); //交点として追加.
				}
			}

			//交点をx座標の昇順(左→右)にソート.
			std::sort(crossList.begin(), crossList.end(),
				[](auto& a, auto& b) { return a.x < b.x; });

			//左から右へ調べ、交点を通過する.
			int winding = 0;
			for (int i = 0; i + 1 < crossList.size(); i++) {
				//交点を通過(→巻数値が+1/-1)
				winding += crossList[i].delta;
				//巻数が0でなければ内側.
				if (winding != 0) {
					//次の交点まで塗りつぶす.
					Line line;
					line.stPos = { crossList[i].x,   _dbl(y) };
					line.edPos = { crossList[i + 1].x, _dbl(y) };
					line.color = poly.color;
					DrawLineKR(line, false, isCameraDisp);
				}
			}
		}
	}
}