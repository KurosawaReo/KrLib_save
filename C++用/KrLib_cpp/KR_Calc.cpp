/*
   - KR_Calc.cpp - (C++)
*/
#include "KR_Calc.h"

//KrLib名前空間.
namespace KR
{
	//計算用の関数群.
	namespace Calc
	{
		//範囲内に座標を補正する.
		void FixPosInArea(DBL_XY* pos, INT_XY size, DBL_RECT rect) {

			if (pos->x < rect.left  + size.x/2) { pos->x = rect.left  + size.x/2; }
			if (pos->y < rect.up    + size.y/2) { pos->y = rect.up    + size.y/2; }
			if (pos->x > rect.right - size.x/2) { pos->x = rect.right - size.x/2; }
			if (pos->y > rect.down  - size.y/2) { pos->y = rect.down  - size.y/2; }
		}
		//エリアの範囲外かどうか.
		bool IsOutInArea(DBL_XY pos, INT_XY size, DBL_RECT rect, bool isCompOut) {

			//完全に出たら範囲外とする.
			if (isCompOut) {
				if (pos.x < rect.left  - size.x/2) { return true; }
				if (pos.y < rect.up    - size.y/2) { return true; }
				if (pos.x > rect.right + size.x/2) { return true; }
				if (pos.y > rect.down  + size.y/2) { return true; }
			}
			//ちょっとでも出たら範囲外とする.
			else {
				if (pos.x < rect.left  + size.x/2) { return true; }
				if (pos.y < rect.up    + size.y/2) { return true; }
				if (pos.x > rect.right - size.x/2) { return true; }
				if (pos.y > rect.down  - size.y/2) { return true; }
			}

			return false; //範囲内.
		}

		//距離を求める.
		//[座標1,座標2 → 長さ]
		double Dist(INT_XY pos1, INT_XY pos2) {
			return (pos1 - pos2).Dist(); //斜辺の長さを返す.
		}
		double Dist(DBL_XY pos1, DBL_XY pos2) {
			return (pos1 - pos2).Dist(); //斜辺の長さを返す.
		}
		//中点座標を求める.
		//[座標1,座標2 → 中点座標]
		DBL_XY MidPos(DBL_XY pos1, DBL_XY pos2) {
			return (pos1 + pos2)/2; //xとyの平均を返す.
		}
		//角度と長さから円周上の座標を求める.
		//[座標1,角度,長さ → 座標2]
		DBL_XY ArcPos(DBL_XY pos, double ang, double len) {
			return pos + AngToVector(ang) * len; //終点座標を返す.
		}
		//始点座標から対象座標への方向を求める.
		//[座標1,座標2 → 角度]
		//[返り値:-180.0～180.0]
		double FacingAng(DBL_XY from, DBL_XY to) {
			return (to - from).Angle();
		}
		//角度からベクトルを求める.
		DBL_XY AngToVector(double ang) {
			return { cos(_rad(ang)), sin(_rad(ang)) }; //vector(-1.0～+1.0)を返す.
		}
		//角度の差を求める.
		//[返り値:-180.0～180.0]
		double AngleDiff(double now, double target) {
			//0～360度に正規化.
			double diff = fmod(target - now + 540.0, 360.0);
			//180度引く.
			return diff - 180.0;
		}

		/*
		   物理法則での速度減衰.
		   
		   vel     : 速度.
		   maxVel  : 限界速度(絶対値)
		   isGround: 接地しているか.
		   gravity : 重力.
		   friction: 摩擦.
		   airDrag : 空気抵抗.

		   (例)
		   初速度  : 10,-10
		   重力    : 0.8
		   摩擦    : 0.1
		   空気抵抗: 0.01
		*/
		void PhysicsVel(DBL_XY* vel, DBL_XY maxVel, bool isGround, double gravity, double friction, double airDrag)
		{
			//値の補正.
			maxVel.x = abs(maxVel.x);
			maxVel.y = abs(maxVel.y);
			NumLimRange(&friction,  0.0, 1.0);
			NumLimRange(&airDrag,   0.0, 1.0);

			//[1]速度.
			DBL_XY v = *vel;

			//[2]加速&抵抗.
			if (isGround) {
				v.x *= (1.0 - friction); //摩擦による減速.
				v.y = 0;                 //接地中は落下停止.
			}
			else {
				v *= (1.0 - airDrag); //空気抵抗による減速.
				v.y += gravity;       //重力.
			}

			//[3]限界速度.
			NumLimRange(&v.x, -maxVel.x, maxVel.x);
			NumLimRange(&v.y, -maxVel.y, maxVel.y);

			//[4]速度更新.
			*vel = v;
		}

		//値の曲線変動(アニメーション用)
		double AnimEase(EaseType type, double time) {

			//タイプ別.
			switch (type) {
				//InQuad: 徐々に加速.
				case EaseType::InQuad:
				{
					NumLimRange(&time, 0.0, 1.0); //0.0～1.0の範囲.
					return time * time;
				}
				//OutQuad: 徐々に減速.
				case EaseType::OutQuad:
				{
					NumLimRange(&time, 0.0, 1.0); //0.0～1.0の範囲.
					return 1 - (1 - time) * (1 - time);
				}
				//InOutQuad: 加速して減速.
				case EaseType::InOutQuad:
				{
					NumLimRange(&time, 0.0, 1.0); //0.0～1.0の範囲.
					return time < 0.5 ? 2 * time * time : 1 - pow(-2 * time + 2, 2) / 2;
				}
				//OutInQuad: 減速して加速.
				case EaseType::OutInQuad:
				{
					NumLimRange(&time, 0.0, 1.0); //0.0～1.0の範囲.
					if (time < 0.5) {
						//0.0→0.5まで.
						return 0.5 * sin(M_PI * time);
					}
					else {
						//0.5→1.0まで.
						return 0.5 + 0.5 * (1.0 - cos(M_PI * (time - 0.5)));
					}
				}
			}
			return 0; //未対応のタイプ.
		}
		double AnimWave(WaveType type, double time) {
			//タイプ別.
			switch (type) {
				//CosLoop: cos波のループ(0.0～1.0)
				case WaveType::CosLoop:
				{
					return 0.5 - cos(M_PI*time)/2;
				}
			}
			return 0; //未対応のタイプ.
		}

		//値の抽選.
		int RandNum(int st, int ed) {

			int rnd = 0;

			//edがst以上の時のみ抽選.
			_if_check(st <= ed) {
				rnd = rand() % ((ed - st) + 1); //st～endの差で抽選.
			}
			return st + rnd;
		}
		//値の抽選(重複なし複数)
		vector<int> RandNums(int st, int ed, int count) {

			int unUsedCnt = (ed-st)+1;      //未使用数字はいくつあるか.
			vector<bool> isUsed((ed-st)+1); //各数字が使用済かどうか.
			vector<int>  ret   (count);     //抽選結果を入れる用.
	
			//抽選する回数分ループ.
			for (int i = 0; i < count; i++) {

				//未使用数字から何番目を選ぶか抽選.
				int cnt = RandNum(1, unUsedCnt);
		
				//st～edの中で数字を選ぶ.
				for (int j = 0; j <= ed-st; j++) {
					//未使用数字なら.
					if (!isUsed[j]) {
						cnt--; //カウント-1
						if (cnt == 0) {
							ret[i] = st+j;    //この数字を抽選結果に保存.
							isUsed[j] = true; //この数字は使用済.
							unUsedCnt--;      //未使用枠-1.
							break;
						}
					}
				}
			}

			//抽選結果を返す.
			return ret;
		}
		//値から小数だけ取り出す.
		double GetDecimal(double num) {
			return fmod(num, 1.0); //1.0で割った余り.
		}
	}
}