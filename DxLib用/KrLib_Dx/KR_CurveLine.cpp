/*
   - KR_CurveLine.cpp - (DxLib)
*/
#include "KR_CurveLine.h"

//[include] ".cpp"ファイルでのみ使うもの.
#include "KR_App.h"
#include "KR_Calc.h"
#include "KR_DrawShape.h"
#include "KR_Camera.h"
#include "KR_Input.h"
#include "KR_ManagerInsts.h"

//KrLib名前空間.
namespace KR
{
	//参照(KRライブラリ)
	static InputMng* inputMng = ManagerInsts::Get<InputMng>();

	// ▼*--=<[ DragPoint ]>=--*▼ //
	
	void DragPoint::Update() {
		//ドラッグされてる時.
		if (isDrag) {
			SetPos(inputMng->GetMousePos());
			GetCir()->color = 0x00ffff;
			//画面外には行かないように.
			if (IsOutInArea(App::GetWindowRect().ToDbl(), false)) {
				FixPosInArea(App::GetWindowRect().ToDbl());
			}
		}
		//通常時.
		else {
			GetCir()->color = 0xffffff;
		}
	}

	//描画(円だけ)
	void DragPoint::DrawNormal() {
		DrawShape();
	}
	//描画(数字つき)
	void DragPoint::DrawWithNum(int num) {
		//円.
		DrawShape();
		//数字.
		DrawStr str(_to_mystr(num), GetPos().ToInt() + INT_XY{1, 1}, ColorID::Black);
		str.Draw();
	}
	
// ▼*--=<[ ContrBase ]>=--*▼ //

	//コンストラクタ.
	ContrBase::ContrBase(bool _isDispLine, bool _isDispPoints) {
		isDispLine   = _isDispLine;
		isDispPoints = _isDispPoints;	
	}
	
	//制御点更新.
	void ContrBase::UpdatePoint() {

		//制御点が非表示なら操作不能に.
		if (!isDispPoints) { return; }

		Circle mouse; //マウスカーソル.
		mouse.pos = inputMng->GetMousePos();
		mouse.r = 0;

		//左クリックした瞬間.
		if (inputMng->IsPushMouseTime(MouseID::Left) == 1) {
			for (auto& i : points) {
				//カーソルが円の中なら.
				if (Calc::HitCirCir(mouse, *i.GetCir())) {
					i.SetIsDrag(true);
					break;
				}
			}
		}
		//左クリックしている間.
		if (inputMng->IsPushMouse(MouseID::Left)) {
		}
		else {
			for (auto& i : points) {
				i.SetIsDrag(false);
			}
		}
		//制御点.
		for (auto& i : points) {
			i.Update();
		}
	}

// ▼*--=<[ BezierLineContr ]>=--*▼ //

	//set.
	void BezierLineContr::SetBezierLine(const BezierLine& _bLine) {
		bezierLine = _bLine;
	}
	void BezierLineContr::SetPoint(DBL_XY _stPos, DBL_XY _edPos, DBL_XY _stContrPos, DBL_XY _edContrPos) {

		DragPoint tmp;	//仮の制御点.

		tmp.SetPos(_stPos);
		points.push_back(tmp);
		tmp.SetPos(_edPos);
		points.push_back(tmp);
		tmp.SetPos(_stContrPos);
		points.push_back(tmp);
		tmp.SetPos(_edContrPos);
		points.push_back(tmp);
	}
	void BezierLineContr::SetColor(ColorID _color) {
		bezierLine.color = _color;
	}
	//get.
	DBL_XY BezierLineContr::GetBezierPoint(double time) const {
		return Calc::BezierPoint(bezierLine, time);
	}

	//初期化.
	void BezierLineContr::Init() {
		//制御点.
		for (auto& i : points) {
			i.Init();
		}
	}
	//更新.
	void BezierLineContr::Update() {

		//制御点更新.
		UpdatePoint();
		//制御点の座標.
		bezierLine.stPos      = points[0].GetPos();
		bezierLine.edPos      = points[1].GetPos();
		bezierLine.stContrPos = points[2].GetPos();
		bezierLine.edContrPos = points[3].GetPos();
	}
	//描画.
	void BezierLineContr::Draw(bool isDot, bool isAnti, bool isCameraDisp) {
		//ベジエ曲線.
		if (isDispLine) {
			DrawBezierLineKR(bezierLine, isDot, isAnti, isCameraDisp);
		}
		//制御点.
		if (isDispPoints) {
			//制御点を結ぶ線.
			{
				Line tmp[2];
				tmp[0].stPos = bezierLine.stPos;
				tmp[0].edPos = bezierLine.stContrPos;
				tmp[1].stPos = bezierLine.edPos;
				tmp[1].edPos = bezierLine.edContrPos;
				//描画.
				for (auto& i : tmp) {
					i.color = 0x00ffff;
					DrawLineKR(i, isAnti, isCameraDisp);
				}
			}
			//制御点の円.
			for (auto& i : points) {
				i.Draw();
			}
		}
	}

// ▼*--=<[ SplineContr ]>=--*▼ //

	//set.
	void SplineContr::SetColor(ColorID _color) {
		spline.color = _color;
	}
	//add.
	void SplineContr::AddPoint(DBL_XY pos) {
		DragPoint tmp;	 //仮の制御点.
		tmp.Init();      //初期化.
		tmp.SetPos(pos); //座標設定.

		points.push_back(tmp);               //制御点を追加.
		spline.points.resize(points.size()); //サイズを増やす.
	}
	//get.
	DBL_XY SplineContr::GetSplinePoint(int degree, double time) const {
		return Calc::SplinePoint(spline, degree, time);
	}

	//更新.
	void SplineContr::Update() {

		//制御点の更新.
		UpdatePoint();
		//制御点の座標.
		for (int i = 0; i < points.size(); i++) {
			spline.points[i] = points[i].GetPos(); //座標登録.
		}
	}
	//描画.
	//isDotがtrueの場合、isAntiは使わない.
	void SplineContr::Draw(int degree, bool isDot, bool isAnti, bool isCameraDisp) {

		//制御点の数が足りなければ描画しない.
		if (spline.points.size() <= degree) {
			throw ErrorMsg(_T("SplineContr::Draw"), _T("制御点の数不足"));
			return;
		}

		//スプライン曲線.
		if (isDispLine) {
			try {
				DrawSplineKR(spline, degree, isDot, isAnti, isCameraDisp);
			}
			catch (const ErrorMsg& err) {
				throw ErrorMsg(_T("SplineContr::Draw"), err.GetMsg());
			}
		}
		//制御点.
		if (isDispPoints) {
			for (auto& i : points) {
				i.Draw();
			}
		}
	}

// ▼*--=<[ FreeContr ]>=--*▼ //

	//add.
	void FreeContr::AddPoint(DBL_XY pos) {
		DragPoint tmp;	 //仮の制御点.
		tmp.Init();      //初期化.
		tmp.SetPos(pos); //座標設定.

		points.push_back(tmp); //制御点を追加.
	}
	//get.
	vector<DBL_XY> FreeContr::GetPointPos() const {

		const int size = _int(points.size());

		vector<DBL_XY> posList; //座標リスト.

		for (const auto& i : points) {
			posList.push_back(i.GetPos()); //座標を保存.
		}
		return posList;
	}

	//更新.
	void FreeContr::Update() {
		//制御点の更新.
		UpdatePoint();
	}
	//描画.
	void FreeContr::Draw() {
		//制御点.
		if (isDispPoints) {
			for (int i = 0; i < points.size(); i++) {
				points[i].DrawWithNum(i);
			}
		}
	}
}