/*
   - KR_CurveLine.h - (DxLib)
   ver.2026/06/12

   ベジエ曲線と、スプライン曲線。
*/
#pragma once
//[include] KR_Global.
#if !defined DEF_KR_DX_GLOBAL
  #include "KR_Global.h"
#endif
//[include] ".h"ファイルで使うもの.
#include "KR_Object.h"

//KrLib名前空間.
namespace KR
{
	//制御点.
	class DragPoint : public ObjectCir
	{
	//▼ ===== 変数 ===== ▼.
	private:
		bool isDrag{}; //ドラッグされてるか.

	//▼ ===== 関数 ===== ▼.
	public:
		//set.
		void SetIsDrag(bool flag) { isDrag = flag; }
		//get.
		bool GetIsDrag() const { return isDrag; }

		//基本処理.
		void Init() override {
			GetCir()->r = 10;
			GetCir()->color = ColorID::White;
		}
		void Update ()       override;
		void Draw   ()       override {};               //未使用.
		bool IsErase() const override { return false; } //未使用.

		//描画.
		void DrawNormal();
		void DrawWithNum(int num);
	};

	//制御点管理 [継承想定]
	class ContrBase
	{
	//▼ ===== 変数 ===== ▼.
	protected:
		vector<DragPoint> points; //制御点.

		bool isDispLine;          //線を表示するか.
		bool isDispPoints;        //制御点を表示するか.

	//▼ ===== 関数 ===== ▼.
	protected:
		//コンストラクタ.
		ContrBase(bool _isDispLine, bool _isDispPoints);
		
		void UpdatePoint(); //制御点更新.
	};

	/*
	   制御点を操作できるベジエ曲線.
	*/
	class BezierLineContr : public ContrBase
	{
	//▼ ===== 変数 ===== ▼.
	private:
		BezierLine bezierLine; //ベジエ曲線.

	//▼ ===== 関数 ===== ▼.
	public:
		//コンストラクタ.
		BezierLineContr() :
			BezierLineContr(true, true)
		{}
		BezierLineContr(bool _isDispLine, bool _isDispPoints) : 
			ContrBase(_isDispLine, _isDispPoints)
		{
			bezierLine.color = ColorID::White; //初期カラー.
		}
		//set.
		void   SetBezierLine (const BezierLine& _bLine);
		void   SetColor      (ColorID _color);
		void   SetPoint      (DBL_XY _stPos, DBL_XY _edPos, DBL_XY _stContrPos, DBL_XY _edContrPos); //制御点設定.
		//get.
		DBL_XY GetBezierPoint(double time) const;

		void   Init();
		void   Update();
		void   Draw(bool isDot, bool isAnti = false, bool isCameraDisp = true);
	};

	/*
	   制御点を操作できるスプライン曲線.

	   degree = 次元(3にしたら3次元)
	*/
	class SplineContr : public ContrBase
	{
	//▼ ===== 変数 ===== ▼.
	private:
		Spline spline; //スプライン曲線.

	//▼ ===== 関数 ===== ▼.
	public:
		//コンストラクタ.
		SplineContr() :
			SplineContr(true, true)
		{}
		SplineContr(bool _isDispLine, bool _isDispPoints) :
			ContrBase(_isDispLine, _isDispPoints)
		{
			spline.color = ColorID::White; //初期カラー.
		}
		//set.
		void   SetColor(ColorID _color);
		//add.
		void   AddPoint(DBL_XY pos); //制御点追加.
		//get.
		DBL_XY GetSplinePoint(int degree, double time) const;

		void   Update();
		void   Draw(int degree, bool isDot, bool isAnti = false, bool isCameraDisp = true);
	};

	/*
	   制御点を自由に追加して操作できる.
	*/
	class FreeContr : public ContrBase
	{
	//▼ ===== 関数 ===== ▼.
	public:
		//コンストラクタ.
		FreeContr() :
			FreeContr(true)
		{}
		FreeContr(bool _isDispPoints) :
			ContrBase(false, _isDispPoints)
		{}
		//add.
		void           AddPoint (DBL_XY pos); //制御点追加.
		//get.
		vector<DBL_XY> GetPointPos() const;

		void           Update();
		void           Draw();
	};
}