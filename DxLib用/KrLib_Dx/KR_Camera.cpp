/*
   - KR_Camera.cpp - (DxLib)
*/
#include "KR_Camera.h"

//[include] ".cpp"ファイルでのみ使うもの.
#include "KR_App.h"
#include "KR_Calc.h"
#include "../KrLib_cpp/KR_Calc.h"

//KrLib名前空間.
namespace KR
{
	Camera Camera::inst; //実体生成.

	//角度補正.
	void Camera::FixAng360() {
		inst.cameraAng = fmod(inst.cameraAng, 360); //360の余り.
		//マイナスの値なら.
		if (cameraAng < 0) {
			inst.cameraAng += 360; //360度加算.
		}
	}

	//set.
	void Camera::SetPos(DBL_XY _pos) {
		inst.cameraPos = { _pos.x, _pos.y, 0 };
	}
	void Camera::SetAng(double _ang) {
		inst.cameraAng = _ang;
		inst.FixAng360();
	}

	//add.
	void Camera::AddPos(DBL_XY _pos, bool isLocal) {
		//[local] 角度を考慮して加算.
		if (isLocal) {
			DBL_XY vec  = Calc::AngToVector(inst.cameraAng-90);
			double dist = Calc::Dist(DBL_XY(0, 0), _pos);
			inst.cameraPos.x += vec.x * dist;
			inst.cameraPos.y += vec.y * dist;
		}
		//[world] 角度を考慮せず加算.
		else {
			inst.cameraPos.x += _pos.x;
			inst.cameraPos.y += _pos.y;
		}
	}
	void Camera::AddAng(double _ang) {
		inst.cameraAng += _ang;
		inst.FixAng360();
	}
}