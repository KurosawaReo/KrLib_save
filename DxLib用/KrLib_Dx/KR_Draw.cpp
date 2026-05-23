/*
   - KR_Draw.cpp - (DxLib)
*/
#include "KR_Draw.h"

//[include] ".cpp"ファイルでのみ使うもの.
#include "KR_App.h"
#include "KR_Calc.h"
#include "KR_Camera.h"
#include "../KrLib_cpp/KR_Calc.h"
#include <algorithm>

/*
   [終点の注意]
   DrawBox, DrawLine, DrawExtendGraphなどの終点を設定するものは
   始点から終点-1まで描画される仕様(その方が直感的だと作者は判断したそう)

   例: DrawBox(1, 1, 5, 5, color, false)とした場合

   　１２３４５
   １■■■■
   ２■　　■
   ３■　　■
   ４■■■■
   ５

   [サイズの-1について]
   画像やテキストの描画で、調整でサイズを-1している。
   これがないとタイルのように並べた時にずれが起きるため、多分必要。
*/

//KrLib名前空間.
namespace KR
{
	constexpr int NONE_HANDLE = -1; //ハンドルなし.

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

// ▼*--=<[ DrawImg ]>=--*▼ //

	//コンストラクタ.
	DrawImg::DrawImg() {
		handle = NONE_HANDLE;
	}
	//デストラクタ.
	DrawImg::~DrawImg() {
		Release();
	}

	//画像登録.
	void DrawImg::SetImage(int _handle) {
		handle = _handle;
		GetGraphSize(_handle, &size.x, &size.y); //サイズ取得.
	}
	//画像解放.
	void DrawImg::Release() {
		//データが登録されていれば.
		if (handle != NONE_HANDLE) {
			DeleteGraph(handle);  //解放.
			handle = NONE_HANDLE; //NONEにする.
		}
	}

	/*
	   DrawGraphの改造版.
	*/
	void DrawImg::Draw(DBL_XY pos, Anchor anc, bool isTrans, bool isFloat, bool isCameraDisp) const {

		if (handle == NONE_HANDLE) {
			throw ErrorMsg(_T("DrawImg::Draw"), _T("ハンドル未設定"));
			return;
		}

		//描画座標.
		DBL_XY newPos = (isCameraDisp) ? App::ToWorldPos(pos) : pos;

		//float型かどうか.
		if (isFloat) {
			//基準点に座標をずらす.
			float x = _flt(newPos.x - (size.x) * ANCHOR_POS[_int(anc)].x);
			float y = _flt(newPos.y - (size.y) * ANCHOR_POS[_int(anc)].y);
			//float型描画.
			int err = DrawGraphF(x, y, handle, isTrans);
			if (err < 0) {
				throw ErrorMsg(_T("DrawImg::Draw"), _T("DrawGraphFエラー"));
				return;
			}
		}
		else {
			//基準点に座標をずらす.
			int x = _int(newPos.x - (size.x-1) * ANCHOR_POS[_int(anc)].x);
			int y = _int(newPos.y - (size.y-1) * ANCHOR_POS[_int(anc)].y);
			//int型描画.
			int err = DrawGraph(x, y, handle, isTrans);
			if (err < 0) {
				throw ErrorMsg(_T("DrawImg::Draw"), _T("DrawGraphエラー"));
				return;
			}
		}
	}

	/*
	   DrawRectGraphの改造版.
	   Rect = 矩形(正方形や長方形のこと)
	*/
	void DrawImg::DrawRect(DBL_XY pos, DBL_RECT rect, Anchor anc, bool isTrans, bool isFloat, bool isCameraDisp) const {

		if (handle == NONE_HANDLE) {
			throw ErrorMsg(_T("DrawImg::DrawRect"), _T("ハンドル未設定"));
			return;
		}

		//描画座標.
		DBL_XY newPos = (isCameraDisp) ? App::ToWorldPos(pos) : pos;
		//基準点に座標をずらす.
		newPos -= size.ToDbl() * ANCHOR_POS[_int(anc)];
		//画像の矩形.
		INT_XY drawStart = {0, 0};
		INT_XY drawSize  = size;

		//クリッピング(画像の切り取り処理)
		if (newPos.x < rect.left) {
			drawStart.x += _int(rect.left - newPos.x);
			drawSize.x  -= _int(rect.left - newPos.x);
			newPos.x = rect.left;
		}
		if (newPos.y < rect.up) {
			drawStart.y += _int(rect.up - newPos.y);
			drawSize.y  -= _int(rect.up - newPos.y);
			newPos.y = rect.up;
		}
		if (newPos.x + drawSize.x > rect.right) {
			drawSize.x -= _int((newPos.x + drawSize.x) - rect.right);
		}
		if (newPos.y + drawSize.y > rect.down) {
			drawSize.y -= _int((newPos.y + drawSize.y) - rect.down);
		}

		//描画する範囲があるなら描画.
		if (drawSize.x > 0 && drawSize.y > 0) {
			if (isFloat) {
				//float型描画.
				int err = DrawRectGraphF(
					_flt(newPos.x), _flt(newPos.y), 
					drawStart.x, drawStart.y, drawSize.x, drawSize.y, 
					handle, isTrans
				);
				if (err < 0) {
					throw ErrorMsg(_T("DrawImg::DrawRect"), _T("DrawRectGraphFエラー"));
					return;
				}
			}
			else {
				//int型描画.
				int err = DrawRectGraph(
					_int(newPos.x), _int(newPos.y), 
					drawStart.x, drawStart.y, drawSize.x, drawSize.y, 
					handle, isTrans
				);
				if (err < 0) {
					throw ErrorMsg(_T("DrawImg::DrawRect"), _T("DrawRectGraphエラー"));
					return;
				}
			}
		}
	}
	
	/*
	   DrawExtendGraphの改造版.
	*/
	void DrawImg::DrawExtend(DBL_XY pos, DBL_XY sizeRate, Anchor anc, bool isTrans, bool isFloat, bool isCameraDisp) const {

		if (handle == NONE_HANDLE) {
			throw ErrorMsg(_T("DrawImg::DrawExtend"), _T("ハンドル未設定"));
			return;
		}

		//描画座標.
		DBL_XY newPos = (isCameraDisp) ? App::ToWorldPos(pos) : pos;

		//float型かどうか.
		if (isFloat) {
			//基準点に座標をずらす.
			float x1 = _flt(newPos.x - (size.x * sizeRate.x) * ANCHOR_POS[_int(anc)].x);
			float y1 = _flt(newPos.y - (size.y * sizeRate.y) * ANCHOR_POS[_int(anc)].y);
			float x2 = _flt(x1 + size.x * sizeRate.x);
			float y2 = _flt(y1 + size.y * sizeRate.y);
			//float型描画.
			int err = DrawExtendGraphF(x1, y1, x2+1, y2+1, handle, isTrans);
			if (err < 0) {
				throw ErrorMsg(_T("DrawImg::DrawExtend"), _T("DrawExtendGraphFエラー"));
				return;
			}
		}
		else {
			//基準点に座標をずらす.
			int x1 = _int(newPos.x - ((size.x * sizeRate.x)-1) * ANCHOR_POS[_int(anc)].x);
			int y1 = _int(newPos.y - ((size.y * sizeRate.y)-1) * ANCHOR_POS[_int(anc)].y);
			int x2 = _int(x1 + ((size.x * sizeRate.x)-1));
			int y2 = _int(y1 + ((size.y * sizeRate.y)-1));
			//int型描画.
			int err = DrawExtendGraph(x1, y1, x2+1, y2+1, handle, isTrans);
			if (err < 0) {
				throw ErrorMsg(_T("DrawImg::DrawExtend"), _T("DrawExtendGraphエラー"));
				return;
			}
		}
	}

	/*
	   DrawRotaGraphの改造版.

	   DrawRotaGraphFast2F()について
	   2   : 回転基準位置も指定する.
	   F   : float型描画.
	   Fast: アルゴリズムが高速になったver.
	*/
	void DrawImg::DrawRota(DBL_XY pos, double extend, double ang, INT_XY pivot, bool isTrans, bool isFloat, bool isCameraDisp) const {

		if (handle == NONE_HANDLE) {
			throw ErrorMsg(_T("DrawImg::DrawRota"), _T("ハンドル未設定"));
			return;
		}

		DBL_XY newPos = pos; //描画座標.
		double newAng = ang; //描画角度.

		//カメラ基準に変換.
		if (isCameraDisp) { 

			//[world基準]
			//カメラ→画像 の距離.
			const double wrdDist = Calc::Dist     (Camera::GetPos(), pos);
			//カメラ→画像 の角度.
			const double wrdAng  = Calc::FacingAng(Camera::GetPos(), pos);

			//[camera基準]
			//カメラ→画像 の角度.
			const double cmrAng = wrdAng - Camera::GetAng(); //カメラの角度を0とする.
			//カメラ→画像 の位置.
			const DBL_XY cmrPos = Calc::AngToVector(cmrAng) * wrdDist; //方向 * 距離.
			
			//描画座標が確定.
			//カメラ基準にするため、画面の半分ずらす.
			newPos = cmrPos + App::GetWindowSize().ToDbl()/2;
			//描画角度が確定.
			newAng -= Camera::GetAng();
		}

		//float型かどうか.
		if (isFloat) {
			//回転軸(デフォルトは画像の中心とする)
			float cx = _flt(size.x/2 + pivot.x);
			float cy = _flt(size.y/2 + pivot.y);
			//float型描画.
			int err = DrawRotaGraphFast2F(
				_flt(newPos.x), _flt(newPos.y), cx, cy, 
				_flt(extend), _flt(_rad(newAng)), handle, isTrans
			);
			if (err < 0) {
				throw ErrorMsg(_T("DrawImg::DrawRota"), _T("DrawRotaGraph2Fエラー"));
				return;
			}
		}
		else {
			//回転軸(デフォルトは画像の中心とする)
			int cx = size.x/2 + pivot.x;
			int cy = size.y/2 + pivot.y;
			//int型描画.
			int err = DrawRotaGraphFast2(
				_int_r(newPos.x), _int_r(newPos.y), cx, cy, _flt(extend), 
				_flt(_rad(newAng)), handle, isTrans
			);
			if (err < 0) {
				throw ErrorMsg(_T("DrawImg::DrawRota"), _T("DrawRotaGraph2エラー"));
				return;
			}
		}
	}
	
	/*
	   DrawModiGraphの改造版.
	*/
	void DrawImg::DrawModi(DBL_XY luPos, DBL_XY ruPos, DBL_XY rdPos, DBL_XY ldPos, bool isTrans, bool isFloat, bool isCameraDisp) const {
	
		if (handle == NONE_HANDLE) {
			throw ErrorMsg(_T("DrawImg::DrawModi"), _T("ハンドル未設定"));
			return;
		}

		//描画座標.
		DBL_XY newPos[4] = { luPos, ruPos, rdPos, ldPos };
		//カメラ基準に変換.
		if (isCameraDisp) {
			for (auto& i : newPos) { i = App::ToWorldPos(i); }
		}

		//float型かどうか.
		if (isFloat) {
			//float型描画.
			int err = DrawModiGraphF(
				_flt(newPos[0].x), _flt(newPos[0].y), _flt(newPos[1].x), _flt(newPos[1].y),
				_flt(newPos[2].x), _flt(newPos[2].y), _flt(newPos[3].x), _flt(newPos[3].y), handle, isTrans
			);
			if (err < 0) {
				throw ErrorMsg(_T("DrawImg::DrawModi"), _T("DrawModiGraphFエラー"));
				return;
			}
		}
		else {
			//int型描画.
			int err = DrawModiGraph(
				_int_r(newPos[0].x), _int_r(newPos[0].y), _int_r(newPos[1].x), _int_r(newPos[1].y),
				_int_r(newPos[2].x), _int_r(newPos[2].y), _int_r(newPos[3].x), _int_r(newPos[3].y), handle, isTrans
			);
			if (err < 0) {
				throw ErrorMsg(_T("DrawImg::DrawModi"), _T("DrawModiGraphエラー"));
				return;
			}
		}
	}

// ▼*--=<[ DrawImgMng ]>=--*▼ //

	DrawImgMng DrawImgMng::inst; //実体生成.

	//デストラクタ.
	DrawImgMng::~DrawImgMng() {
		//画像データを全て取り出す.
		for (auto& i : images) {
			i.second.Release(); //各画像の解放.
		}
		images.clear(); //データを空にする.
	}

	//共通パスを設定.
	void DrawImgMng::SetPath(MY_STRING _path) {
		inst.path = _path;
	}

	//画像取得.
	DrawImg* DrawImgMng::Get(string saveName) {
		//存在すれば.
		if (inst.images.count(saveName) > 0) {
			return &inst.images[saveName]; //返す.
		}
		return nullptr;
	}
	//画像取得(チェックあり)
	bool DrawImgMng::TryGet(string saveName, DrawImg* ptr) {
		//存在すれば.
		if (inst.images.count(saveName) > 0) {
			ptr = &inst.images[saveName]; //返す.
			return true; //取得成功.
		}
		return false; //取得失敗.
	}

	/*
	   画像読み込み.
	*/
	void DrawImgMng::LoadFile(MY_STRING fileName, string saveName) {

		//既に存在すれば.
		if (inst.images.count(saveName) > 0) {
			throw ErrorMsg(_T("DrawImgMng::LoadFile"), _T("使用済みの保存名"));
			return;
		}

		//パスを作成.
		const MY_STRING pathFull = inst.path + fileName;

		//ファイル読み込み.
		int handle = LoadGraph(pathFull.c_str());
		if (handle < 0) {
			throw ErrorMsg(_T("DrawImgMng::LoadFile"), _T("LoadGraphエラー"));
			return;
		}
		//画像登録.
		inst.images[saveName].SetImage(handle);
	}

	/*
	   画像読み込み(分割)

	   [例]
	   DrawImgMng::LoadFileDiv(
	       _T("test.png"), {64, 64}, {4, 1}, //64×64のサイズで横4つ縦1つに分割.
	       {"idle","run1","jump","death"}    //登録名.
	   );
	*/
	void DrawImgMng::LoadFileDiv(MY_STRING fileName, INT_XY size, INT_XY cnt, vector<string> saveName) {
		
		const int divCnt = cnt.x * cnt.y; //分割数.

		//分割数と一致しない場合.
		if (saveName.size() != divCnt) {
			throw ErrorMsg(_T("DrawImgMng::LoadFileDiv"), _T("保存名の数が分割数と不一致"));
			return;
		}

		//LoadDivGraphからハンドルを取り出す用.
		int* pHandle = new int[divCnt];
		//パスを作成.
		const MY_STRING pathFull = inst.path + fileName;

		//画像読み込み(分割)
		int err = LoadDivGraph(pathFull.c_str(), divCnt, cnt.x, cnt.y, size.x, size.y, pHandle);
		if (err < 0) {
			delete[] pHandle; pHandle = nullptr; //配列破棄.
			throw ErrorMsg(_T("DrawImgMng::LoadFileDiv"), _T("LoadDivGraphエラー"));
			return;
		}

		//読み込んだ画像をループ.
		for (int i = 0; i < divCnt; i++) {
			//既に存在すれば.
			if (inst.images.count(saveName[i]) > 0) {
				delete[] pHandle; pHandle = nullptr; //配列破棄.
				throw ErrorMsg(_T("DrawImgMng::LoadFileDiv"), _T("使用済みの保存名が含まれている"));
				return;
			}
			//画像登録.
			inst.images[saveName[i]].SetImage(pHandle[i]);
		}

		delete[] pHandle; pHandle = nullptr; //配列破棄.
	}

// ▼*--=<[ DrawStr ]>=--*▼ //

	//DrawStringの改造版.
	void DrawStr::Draw(Anchor anc, int font, bool isCameraDisp) {
	
		//描画座標.
		DBL_XY newPos = (isCameraDisp) ? App::ToWorldPos(pos.ToDbl()) : pos.ToDbl();

		//基準点に座標をずらす.
		int x = _int_r(newPos.x - (GetTextSize(font).x-1) * ANCHOR_POS[_int(anc)].x);
		int y = _int_r(newPos.y - (GetTextSize(font).y-1) * ANCHOR_POS[_int(anc)].y);

		//デフォルトフォント.
		if (font < 0) {
			int err = DrawString(x, y, text.c_str(), color.GetColorCode());
			if (err < 0) {
				throw ErrorMsg(_T("DrawStr::Draw"), _T("DrawStringエラー"));
				return;
			}
		}
		//フォント設定あり.
		else {
			int err = DrawStringToHandle(x, y, text.c_str(), color.GetColorCode(), font);
			if (err < 0) {
				throw ErrorMsg(_T("DrawStr::Draw"), _T("DrawStringToHandleエラー"));
				return;
			}
		}
	}
	//DrawRotaStringの改造版.
	void DrawStr::DrawRota(INT_XY extend, INT_XY pivot, double ang, bool isVertical, int font, bool isCameraDisp) {

		DBL_XY newPos = pos.ToDbl(); //描画座標.
		double newAng = ang;         //描画角度.

		//カメラ基準に変換.
		if (isCameraDisp) {
			//TODO: 画像のDrawRotaみたいに回転対応させる.
		}

		//デフォルトフォント.
		if (font < 0) {
			int err = DrawRotaString(
				_int_r(newPos.x), _int_r(newPos.y), extend.x, extend.y, pivot.x, pivot.y,
				_rad(newAng), color.GetColorCode(), 0, isVertical, text.c_str()
			);
			if (err < 0) {
				throw ErrorMsg(_T("DrawStr::DrawRota"), _T("DrawRotaStringエラー"));
				return;
			}
		}
		//フォント設定あり.
		else {
			int err = DrawRotaStringToHandle(
				_int_r(newPos.x), _int_r(newPos.y), extend.x, extend.y, pivot.x, pivot.y,
				_rad(newAng), color.GetColorCode(), font, 0, isVertical, text.c_str()
			);
			if (err < 0) {
				throw ErrorMsg(_T("DrawStr::DrawRota"), _T("DrawRotaStringToHandleエラー"));
				return;
			}
		}
	}
	//DrawModiStringの改造版.
	void DrawStr::DrawModi(INT_XY luPos, INT_XY ruPos, INT_XY rdPos, INT_XY ldPos, bool isVertical, int font, bool isCameraDisp) {

		//描画座標.
		DBL_XY newPos[4] = { luPos.ToDbl(), ruPos.ToDbl(), rdPos.ToDbl(), ldPos.ToDbl() };
		//カメラ基準に変換.
		if (isCameraDisp) {
			for (auto& i : newPos) { i = App::ToWorldPos(i); }
		}

		//デフォルトフォント.
		if (font < 0) {
			int err = DrawModiString(
				_int_r(newPos[0].x), _int_r(newPos[0].y), _int_r(newPos[1].x), _int_r(newPos[1].y),
				_int_r(newPos[2].x), _int_r(newPos[2].y), _int_r(newPos[3].x), _int_r(newPos[3].y),
				color.GetColorCode(), 0, isVertical, text.c_str()
			);
			if (err < 0) {
				throw ErrorMsg(_T("DrawStr::DrawModi"), _T("DrawModiStringエラー"));
				return;
			}
		}
		//フォント設定あり.
		else {
			int err = DrawModiStringToHandle(
				_int_r(newPos[0].x), _int_r(newPos[0].y), _int_r(newPos[1].x), _int_r(newPos[1].y),
				_int_r(newPos[2].x), _int_r(newPos[2].y), _int_r(newPos[3].x), _int_r(newPos[3].y),
				color.GetColorCode(), font, 0, isVertical, text.c_str()
			);
			if (err < 0) {
				throw ErrorMsg(_T("DrawStr::DrawModi"), _T("DrawModiStringToHandleエラー"));
				return;
			}
		}
	}

	//テキストのサイズ取得.
	INT_XY DrawStr::GetTextSize(int font) {
	
		INT_XY size{};

		TCHAR name[256]{}; //無視.
		int   line{};      //無視.
		int   thick{};     //無視.

		//デフォルトフォント.
		if (font < 0) {
			GetDrawStringSize(&size.x, &size.y, &line, text.c_str(), 255);
		}
		//フォント設定あり.
		else {
			GetDrawStringSizeToHandle(&size.x, &size.y, &line, text.c_str(), 255, font);
			GetFontStateToHandle(name, &size.y, &thick, font); //size.yはフォントから取得.
		}

		return size;
	}

// ▼*--=<[ Font ]>=--*▼ //

	//コンストラクタ, デストラクタ.
	Font::Font() {
		handle = NONE_HANDLE;
	}
	Font::~Font() {
		//ハンドルがあれば.
		if (handle != NONE_HANDLE) {
			DeleteFontToHandle(handle); //解放.
		}
	}
	//フォント作成.
	void Font::CreateFontH(MY_STRING fontName, int size, int thick, FontTypeID fontId) {
		handle = CreateFontToHandle(fontName.c_str(), size, thick, _int(fontId));
	}

// ▼*--=<[ GradLine ]>=--*▼ //
	
	//頂点追加.
	void GradLine::AddPoint(DBL_XY pos, MY_COLOR color) {

		VERTEX2D point;
		point.pos = VGet(_flt(pos.x), _flt(pos.y), 0); //VECTOR型にして登録.
		point.dif = color.GetColorU8();                //COLOR_U8で登録.
		point.rhw = 1.0f;                              //2Dでは不要?
		point.u = point.v = 0.0f;                      //テクスチャUVは使わない.

		points.push_back(point); //頂点追加.
	}
	//描画.
	void GradLine::Draw(bool isClose, bool isCameraDisp) {

		//頂点の数.
		int count = _int((isClose) ? points.size()+1 : points.size());
		//頂点配列.
		vector<VERTEX2D> tmp(count);

		//頂点データをコピー.
		for (int i = 0; i < points.size(); i++) {
			tmp[i] = points[i]; //頂点を登録.
			//カメラ基準に変換.
			if (isCameraDisp) {
				tmp[i].pos.x -= _flt(App::GetWindowRect().GetLU().x);
				tmp[i].pos.y -= _flt(App::GetWindowRect().GetLU().y);
			}
		}
		if (isClose) {
			tmp[points.size()] = tmp[0]; //終点に始点を入れる.
		}

		//描画.
		{
			DrawMode _(DrawModeID::None, DrawBlendModeID::Alpha, 255);

			//TODO: ↓DX_PRIMTYPE_LINESTRIP以外の機能.
			DrawPrimitive2D(tmp.data(), count, DX_PRIMTYPE_LINESTRIP, DX_NONE_GRAPH, FALSE);
		}
	}

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
		float x = _flt(newPos.x - (cir.r*2-1) * (ANCHOR_POS[_int(anc)].x - 0.5));
		float y = _flt(newPos.y - (cir.r*2-1) * (ANCHOR_POS[_int(anc)].y - 0.5));

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
		else{
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
		float x1 = _flt(newPos.x - (box.size.x-1) * ANCHOR_POS[_int(anc)].x); //始点.
		float y1 = _flt(newPos.y - (box.size.y-1) * ANCHOR_POS[_int(anc)].y);
		float x2 = _flt(x1 + box.size.x-1);                                   //終点.
		float y2 = _flt(y1 + box.size.y-1);

		//アンチエイリアスあり.
		if (isAnti) {
			int err = DrawBoxAA(x1, y1, x2+1, y2+1, box.color.GetColorCode(), isFill, box.thick);
			if (err < 0) {
				throw ErrorMsg(_T("DrawBoxKR"), _T("DrawBoxAAエラー"));
				return;
			}
		}
		//アンチエイリアスなし.
		else {
			int err = DrawBox(_int(x1), _int(y1), _int(x2+1), _int(y2+1), box.color.GetColorCode(), isFill);
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
		DBL_XY vec2 = Calc::AngToVector(pie.stAng+pie.arcAng); //扇の終わりの角度.
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
		const double edAng  = pie.stAng + pie.arcAng; //弧の終わりの角度.

		for (double i = pie.stAng; i <= edAng-addAng; i += addAng) {

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
			const double t  = _dbl(i  )/segments;
			const double nt = _dbl(i+1)/segments;
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
			double t  = _dbl(i)  /segments;
			double nt = _dbl(i+1)/segments;

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
			if (i == poly.points.size()-1) {
				if (!isSurround) { break; }
			}
			//始点と終点.
			line.stPos = poly.pos + poly.points[i];
			line.edPos = poly.pos + poly.points[(i+1) % poly.points.size()]; //サイズを超えたら0に戻す.
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
		const int upY   = App::GetWindowRect().GetU().y;
		const int downY = App::GetWindowRect().GetD().y;

		//一列ずつループ.
		for (int y = upY; y < downY; y++) {

			//交点リスト.
			vector<CrossData> crossList;

			//ポリゴンの全頂点.
			for (int i = 0; i < poly.points.size(); i++) {
				//囲わないなら最後はスキップ.
				if (i == poly.points.size()-1) {
					if (!isSurround) { break; }
				}
				//絶対座標を取得.
				const DBL_XY& p0 = poly.pos + poly.points[i];
				const DBL_XY& p1 = poly.pos + poly.points[(i+1) % poly.points.size()];
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

	/*
	   画面全体にグリッド線を描画.
	*/
	void DrawWindowGrid(int wid, int hei, int size, MY_COLOR clrWid, MY_COLOR clrHei) {

		Line line;

		//縦線の描画.
		for (int x = 0; x < wid; x += size) {
			//線の設定.
			line.stPos = { _dbl(x), 0 };
			line.edPos = { _dbl(x), _dbl(hei) };
			line.color = clrHei;
			//描画.
			try {
				DrawLineKR(line);
			}
			catch (const ErrorMsg& err) {
				throw ErrorMsg(_T("DrawWindowGrid"), err.GetMsg());
				return;
			}
		}
		//横線の描画.
		for (int y = 0; y < hei; y += size) {
			//線の設定.
			line.stPos = { 0, _dbl(y) };
			line.edPos = { _dbl(wid), _dbl(y) };
			line.color = clrWid;
			//描画.
			try {
				DrawLineKR(line);
			}
			catch (const ErrorMsg& err) {
				throw ErrorMsg(_T("DrawWindowGrid"), err.GetMsg());
				return;
			}
		}
	}

	/*
	   キューブ(3D) [試作品]
	*/
	void DrawBox3DKR(const Box3D& box, bool isFill) {

		//描画座標.
		DBL_XYZ newPos[2] = { box.stPos, box.edPos };

		//座標.
		VECTOR vec1 = { _flt(newPos[0].x), _flt(newPos[0].y), _flt(newPos[0].z) };
		VECTOR vec2 = { _flt(newPos[1].x), _flt(newPos[1].y), _flt(newPos[1].z) };
		//描画.
		int err = DrawCube3D(vec1, vec2, box.difColor.GetColorCode(), box.spcColor.GetColorCode(), isFill);
		if (err < 0) {
			throw ErrorMsg(_T("DrawBox3DKR"), _T("描画エラー"));
			return;
		}
	}
}