/*
   - KR_Calc.cpp - (DxLib)
*/
#include "KR_Calc.h"

//[include] ".cpp"ファイルでのみ使うもの.
#include "../KrLib_cpp/KR_Calc.h"

//KrLib名前空間.
namespace KR
{
	//計算用の関数群.
	namespace Calc
	{
		//当たり判定(円と円)
		bool HitCirCir(const Circle& cir1, const Circle& cir2) {

			//距離差.
			double x = cir1.pos.x - cir2.pos.x;
			double y = cir1.pos.y - cir2.pos.y;
			//距離が半径の合計以下なら(√を削減するために2乗して計算)
			if (x*x + y*y <= pow(cir1.r+cir2.r, 2)) {
				return true; //hit.
			}
			else {
				return false;
			}
		}
		//当たり判定(四角と四角)
		bool HitBoxBox(const Box& box1, const Box& box2) {

			//中央基準座標での判定.
			if (fabs(box1.pos.x - box2.pos.x) <= (box1.size.x + box2.size.x)/2 &&
				fabs(box1.pos.y - box2.pos.y) <= (box1.size.y + box2.size.y)/2
			){
				return true; //hit.
			}
			else {
				return false;
			}
		}
		//当たり判定(四角と円)
		//任意で引数から当たった面を受け取れる.
		bool HitBoxCir(const Box& box, const Circle& cir, HitSurface* ori) {

			DBL_XY nearest = cir.pos;

			//円の中心から一番近い四角形の点を求める.
			NumLimRange(&nearest.x, box.pos.x - box.size.x/2, box.pos.x + box.size.x/2);
			NumLimRange(&nearest.y, box.pos.y - box.size.y/2, box.pos.y + box.size.y/2);
			//円の中心との座標差.
			DBL_XY dist = nearest - cir.pos;

			//当たったかどうか(距離が半径以下ならhit)
			if (dist.Dist() <= cir.r) {
				//当たった方向を返す.
				if (ori) {
					if (abs(dist.x) > abs(dist.y)) {
						*ori = HitSurface::Horizontal; //左右の面.
					}
					else {
						*ori = HitSurface::Vertical;   //上下の面.
					}
				}
				return true;
			}
			else {
				if (ori) {
					*ori = HitSurface::None; //当たってない.
				}
				return false;
			}
		}
		//当たり判定(線と円)
		bool HitLineCir(const Line& line, const Circle& cir) {

			//線の始点と終点から傾きを求める.
			double katamuki;
			{
				double x = line.edPos.x - line.stPos.x;
				double y = line.edPos.y - line.stPos.y;
				if (x != 0) {
					katamuki = y / x;
				}
				else {
					katamuki = 999; //0割対策.
				}
			}
			//線を方程式にした時の切片.
			double seppen = line.stPos.y - line.stPos.x * katamuki;

			//線～円の距離.
			double dis1;
			{
				//直線: ax + by + c = 0
				//bを1として「y = 」の形にする→ y = -ax - c
				double a = -katamuki;
				double b = 1;
				double c = -seppen;
				//公式: d = |ax + by + c|/√(a^2 + b^2)
				dis1 = fabs(a*cir.pos.x + b*cir.pos.y + c) / sqrt(a*a + b*b);
			}
			//線の中点～円の中心の距離.
			double dis2;
			{
				double x = cir.pos.x - MidPos(line.stPos, line.edPos).x;
				double y = cir.pos.y - MidPos(line.stPos, line.edPos).y;
				//距離: d = √(x^2 + y^2) (三平方の定理)
				dis2 = sqrt(x*x + y*y);
			}

			//hit条件.
			if (dis1 <= cir.r &&                               //条件1: 線に触れている.
				dis2 <= Dist(line.stPos, line.edPos)/2 + cir.r //条件2: 線を直径とする円に触れている.
			){
				return true;
			}
			return false;
		}
		//当たり判定(扇形と点)
		bool HitPie(const Pie& pie, DBL_XY pos) {

			//扇形の中心からの距離.
			double distLen = Dist(pie.pos, pos);
			//扇形の中心からの角度.
			double ang = FacingAng(pie.pos, pos);
			//扇形の中心角.
			double centerAng = pie.stAng + pie.arcAng/2;
			//角度差(1.0～-1.0の範囲, 距離差が少ないほど1.0に近づく)
			double distAng = cos(_rad(centerAng-ang));

			//hit条件.
			if (distLen <= pie.r                   && //条件1: 扇形の半径内に入っている.
				distAng >= cos(_rad(pie.arcAng/2))    //条件2: 扇形の角度の範囲に入っている.
			){
				return true;
			}
			return false;
		}

		//前方宣言(.cpp内でのみ使うため)
		vector<double> CreateOpenUniformKnots(int controlCount, int degree);
		double         BSplineBasis          (int i, int degree, double t, const vector<double>& knots);

		//ベジエ曲線から一点を取得.
		DBL_XY BezierPoint(const BezierLine& bLine, double time) {

			double x = 
				      (1 - time) * (1 - time) * (1 - time) * bLine.stPos.x 
				+ 3 * (1 - time) * (1 - time) *      time  * bLine.stContrPos.x
				+ 3 * (1 - time) *      time  *      time  * bLine.edContrPos.x 
				+          time  *      time  *      time  * bLine.edPos.x;
			double y = 
				      (1 - time) * (1 - time) * (1 - time) * bLine.stPos.y
				+ 3 * (1 - time) * (1 - time) *      time  * bLine.stContrPos.y
				+ 3 * (1 - time) *      time  *      time  * bLine.edContrPos.y
				+          time  *      time  *      time  * bLine.edPos.y;

			return { x, y };
		}
		//スプライン曲線から一点を取得.
		DBL_XY SplinePoint(const Spline& spline, int degree, double time)
		{
			const int  count = _int(spline.points.size());
			const auto knots = CreateOpenUniformKnots(count, degree);

			//timeが0.0(始点)以前なら.
			if (time <= 0.0) {
				return spline.points.front(); //始点の座標.
			}
			//timeが1.0(終点)以降なら.
			if (time >= 1.0) {
				return spline.points.back();  //終点の座標.
			}

			DBL_XY p{ -1, -1 };

			for (int j = 0; j < count; j++) {
				double b = BSplineBasis(j, degree, time, knots);
				p.x += b * spline.points[j].x;
				p.y += b * spline.points[j].y;
			}
			return p;
		}
		//Knots生成.
		vector<double> CreateOpenUniformKnots(int controlCount, int degree)
		{
			//TODO: 整理する.
			const int knotCount = controlCount + degree + 1;
			vector<double> knots(knotCount);

			for (int i = 0; i < knotCount; i++) {
				if (i <= degree) {
					knots[i] = 0.0f;
				}
				else if (i >= controlCount) {
					knots[i] = 1.0f;
				}
				else {
					knots[i] =
						(double)(i - degree) /
						(double)(controlCount - degree);
				}
			}
			return knots;
		}
		//Bスプライン基底関数.
		double BSplineBasis(int i, int degree, double t, const vector<double>& knots)
		{
			//TODO: 整理する.
			//0次.
			if (degree == 0) {
				return (knots[i] <= t && t < knots[i + 1]) ? 1.0f : 0.0f;
			}

			double left = 0.0;
			double right = 0.0;

			const double denom1 = knots[i + degree] - knots[i];
			const double denom2 = knots[i + degree + 1] - knots[i + 1];

			if (denom1 > 0.0f) {
				left = (t - knots[i]) / denom1
					* BSplineBasis(i, degree - 1, t, knots);
			}
			if (denom2 > 0.0f) {
				right = (knots[i + degree + 1] - t) / denom2
					* BSplineBasis(i + 1, degree - 1, t, knots);
			}

			return left + right;
		}
	}
}