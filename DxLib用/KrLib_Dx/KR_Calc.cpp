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
		bool HitBoxCir(const Box& box, const Circle& cir, Surface* surface) {

			DBL_XY nearest = cir.pos;

			//円の中心から一番近い四角形の点を求める.
			NumLimRange(&nearest.x, box.pos.x - box.size.x/2, box.pos.x + box.size.x/2);
			NumLimRange(&nearest.y, box.pos.y - box.size.y/2, box.pos.y + box.size.y/2);
			//円の中心との座標差.
			DBL_XY dist = nearest - cir.pos;

			//当たったかどうか(距離が半径以下ならhit)
			if (dist.Dist() <= cir.r) {
				//当たった方向を返す.
				if (abs(dist.x) > abs(dist.y)) {
					_param_ret_ptr(surface, Surface::Horizontal); //左右面.
				}
				else {
					_param_ret_ptr(surface, Surface::Vertical);   //上下面.
				}
				return true;
			}
			else {
				_param_ret_ptr(surface, Surface::None); //当たってない.
				return false;
			}
		}

		//当たり判定(線と円)
		bool HitLineCir(const Line& line, const Circle& cir) {

			//線の長さが0なら計算しない.
			if (Calc::Dist(line.stPos, line.edPos) == 0) { 
				return false;
			}

			//線の始点から終点までのベクトル.
			DBL_XY v1 = line.stPos - line.edPos;
			//線の終点から始点までのベクトル.
			DBL_XY v2 = line.edPos - line.stPos;
			//線の始点から円までのベクトル.
			DBL_XY v3 = line.stPos - cir.pos;
			//線の終点から円までのベクトル.
			DBL_XY v4 = line.edPos - cir.pos;

			//[外積] 線から円の距離を計算.
			double dist = abs(Calc::CrossProduct(v1.Normalize(), v3));
			//[内積] 線と円の角度差を計算.
			double ang1 = Calc::DotProduct(v1, v3);
			double ang2 = Calc::DotProduct(v2, v4);

			//当たり判定.
			if (dist <= cir.r          && //条件1: 線と円の距離が、円の半径以下.
				ang1 >= 0 && ang2 >= 0    //条件2: 線と円の角度差が2つとも鋭角.
			){
				return true; //当たった.
			}
			return false; //当たってない.
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