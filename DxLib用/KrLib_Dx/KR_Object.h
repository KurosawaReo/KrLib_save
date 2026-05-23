/*
   - KR_Object.h - (DxLib)
   ver.2026/05/07

   オブジェクト機能。
   継承して使うことで、Draw/Calc/Inputの一部機能をオブジェクト指向で使える。
*/
#pragma once
//[include] KR_Global.
#if !defined DEF_KR_DX_GLOBAL
  #include "KR_Global.h"
#endif
//[include] ".h"ファイルで使うもの.
#include "KR_Draw.h"
#include "KR_Timer.h"

//KrLib名前空間.
namespace KR
{
	//オブジェクト(図形) [継承想定]
	class ObjectShape
	{
	//▼ ===== 変数 ===== ▼.
	private:
		vector<string> useImg{};    //使う画像データ.
		int            useImgNo{};  //使う画像データのindex.
		Timer          tmImgAnim{}; //画像切り替え用タイマー.

	public:
		DBL_XY offset{};   //画像をずらす量.
		bool   isActive{}; //有効かどうか.

	//▼ ===== 関数 ===== ▼.
	protected:
		//コンストラクタ.
		ObjectShape() : 
			useImg(0), tmImgAnim(TimerMode::CountDown, 0), offset(0, 0), isActive(true)
		{}
		//画像更新.
		void UpdateImg();

	public:
		//基本処理.
		//(ManagerBaseやObjectMngで管理する用)
		virtual void Init   () = 0; //初期化.
		virtual void Update () = 0; //更新.
		virtual void Draw   () = 0; //描画.
		virtual bool IsErase() const { return false; } //消滅条件(ObjectMng専用)

		//座標, サイズ.
		virtual void    SetPos   (DBL_XY)       = 0;
		virtual void    AddPos   (DBL_XY)       = 0;
		virtual DBL_XY  GetPos   ()       const = 0;
		virtual DBL_XY* GetPosPtr()             = 0;
		virtual DBL_XY  GetSize  ()       const = 0;
		//画像.
		void      SetDrawImg     (string name);
		void      SetDrawImgs    (vector<string> names, float changeTime);
		void      SetStopImgAnim (bool isStop);
		//Calcの機能.
		void      FixPosInArea   (DBL_RECT rect);
		bool      IsOutInArea    (DBL_RECT rect, bool isCompOut);
		double    Dist			 (DBL_XY pos);
		DBL_XY    ArcPos		 (double ang, double len);
		double    FacingAng		 (DBL_XY targetPos);
		//Inputの機能.
		void      MoveKey4Dir    (float speed);
		void      MovePad4Dir    (float speed);
		void      MovePadStick   (float speed);
		void      MoveMousePos   (bool isMoveX = true, bool isMoveY = true);
		//Drawの機能.
		virtual void DrawShape(bool isFill = true, bool isAnti = false, bool isCameraDisp = true) const = 0;
		void      DrawGraph      (                                                          Anchor anc = Anchor::Mid, bool isFloat = false, bool isCameraDisp = true);
		void      DrawRectGraph  (DBL_RECT rect,                                            Anchor anc = Anchor::Mid, bool isFloat = false, bool isCameraDisp = true);
		void      DrawExtendGraph(DBL_XY sizeRate,                                          Anchor anc = Anchor::Mid, bool isFloat = false, bool isCameraDisp = true);
		void      DrawRotaGraph  (double ang, double sizeRate = 1.0, INT_XY pivot = {0, 0},                           bool isFloat = false, bool isCameraDisp = true);
	};

	//オブジェクト(円) [継承想定]
	class ObjectCir : public ObjectShape
	{
	//▼ ===== 変数 ===== ▼.
	private:
		Circle cir{}; //円のデータ(当たり判定, 座標)

	//▼ ===== 関数 ===== ▼.
	public:
		//コンストラクタ.
		ObjectCir() {
			cir.r     = 10;       //デフォルト半径.
			cir.color = 0xFFFFFF; //デフォルト色.
		}
		//get.
		Circle*   GetCir   () { return &cir; } //円を取得.

		//座標, サイズ.
		void      SetPos   (DBL_XY _pos)       override { cir.pos = _pos; }
		void      AddPos   (DBL_XY _pos)       override { cir.pos += _pos; }
		DBL_XY    GetPos   ()            const override { return cir.pos; }
		DBL_XY*   GetPosPtr()                  override { return &cir.pos; }
		DBL_XY    GetSize  ()            const override { return {cir.r*2, cir.r*2}; }
		//当たり判定.
		bool      HitCheckCir (const Circle& cir)  const;
		bool      HitCheckBox (const Box&    box)  const;
		bool      HitCheckLine(const Line&   line) const;
		//描画.
		void      DrawShape(bool isFill = true, bool isAnti = false, bool isCameraDisp = true) const override;
	};

	//オブジェクト(四角形) [継承想定]
	class ObjectBox : public ObjectShape
	{
	//▼ ===== 変数 ===== ▼.
	private:
		Box box{}; //四角形のデータ(当たり判定, 座標)

	//▼ ===== 関数 ===== ▼.
	public:
		//コンストラクタ.
		ObjectBox() {
			box.size  = {20, 20}; //デフォルトサイズ.
			box.color = 0xFFFFFF; //デフォルト色.
		}
		//get.
		Box*      GetBox() { return &box; } //四角形を取得.

		//座標, サイズ.
		void      SetPos   (DBL_XY _pos)       override { box.pos = _pos; }
		void      AddPos   (DBL_XY _pos)       override { box.pos += _pos; }
		DBL_XY    GetPos   ()            const override { return box.pos; }
		DBL_XY*   GetPosPtr()                  override { return &box.pos; }
		DBL_XY    GetSize  ()            const override { return box.size; }
		//当たり判定.
		bool      HitCheckCir(const Circle& cir) const;
		bool      HitCheckBox(const Box&    box) const;
		//描画.
		void      DrawShape(bool isFill = true, bool isAnti = false, bool isCameraDisp = true) const override;
	};
}