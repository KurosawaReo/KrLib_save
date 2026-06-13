/*
   - KR_Draw.h - (DxLib)
   ver.2026/06/13

   図形や画像の描画機能。
   (オブジェクト指向ver → KR_Object)
*/
#pragma once
//[include] KR_Global.
#if !defined DEF_KR_DX_GLOBAL
  #include "KR_Global.h"
#endif

//KrLib名前空間.
namespace KR
{
	//フォントタイプID(入力しやすくする用)
	enum class FontTypeID
	{
		Normal   = DX_FONTTYPE_NORMAL,
		Edge     = DX_FONTTYPE_EDGE,
		Anti     = DX_FONTTYPE_ANTIALIASING,      //アンチエイリアス(滑らかになる)
		AntiEdge = DX_FONTTYPE_ANTIALIASING_EDGE, //アンチエイリアス & エッジ.

		None     = -1, //指定なし.
	};
	//描画モードID(入力しやすくする用)
	enum class DrawModeID
	{
		None        = DX_DRAWMODE_NEAREST,      //指定なし.
		Nearest		= DX_DRAWMODE_NEAREST,		//ネアレストネイバー法  (通常)
		Bilinear	= DX_DRAWMODE_BILINEAR,		//バイリニア法          (画像を縮小しても滑らかになる)
		Anisotropic	= DX_DRAWMODE_ANISOTROPIC,	//異方性フィルタリング法.
	};
	enum class DrawBlendModeID
	{
		None		= DX_BLENDMODE_NOBLEND,		//デフォルト.
		Alpha		= DX_BLENDMODE_ALPHA,		//αブレンド  (重なると透過する)
		Add			= DX_BLENDMODE_ADD,			//加算ブレンド(重なると明度が明るくなる)
		Sub			= DX_BLENDMODE_SUB,			//減算ブレンド(重なると明度が暗くなる)
		Mul			= DX_BLENDMODE_MUL			//乗算ブレンド.
	};
	//アンカー(描画の基準点)
	enum class Anchor
	{
		LU,	U,   RU, //[例] Anchor::LU = Left Up.
		L,  Mid, R,
		LD,	D,   RD,
	};

	//画像クラス.
	class DrawImg
	{
		friend class DrawImgMng; //アクセス許可.

	//▼ ===== 変数 ===== ▼.
	private:
		int    handle; //ハンドル.
		INT_XY size;   //画像のサイズ.

	//▼ ===== 関数 ===== ▼.
	private:
		void SetImage(int _handle); //画像登録.
		void Release();             //画像解放.

	public:
		//コンストラクタ, デストラクタ.
		//std::mapを使う関係でpublicに入れておく.
		DrawImg();
		~DrawImg();
		//get.
		INT_XY GetSize() const { return size; }

		//描画.
		void Draw      (DBL_XY pos,                                                   Anchor anc = Anchor::Mid, bool isTrans = true, bool isFloat = false, bool isCameraDisp = true) const;
		void DrawRect  (DBL_XY pos, DBL_RECT rect,                                    Anchor anc = Anchor::Mid, bool isTrans = true, bool isFloat = false, bool isCameraDisp = true) const;
		void DrawExtend(DBL_XY pos, DBL_XY sizeRate,                                  Anchor anc = Anchor::Mid, bool isTrans = true, bool isFloat = false, bool isCameraDisp = true) const;
		void DrawRota  (DBL_XY pos, double extend, double ang, INT_XY pivot = {0, 0},                           bool isTrans = true, bool isFloat = false, bool isCameraDisp = true) const;
		void DrawModi  (DBL_XY luPos, DBL_XY ruPos, DBL_XY rdPos, DBL_XY ldPos,                                 bool isTrans = true, bool isFloat = false, bool isCameraDisp = true) const;

		//使用禁止(「=」で実体が複製されて、意図せずデストラクタが実行されるのを防ぐため)
		DrawImg& operator=(const DrawImg&) = delete;
	};

	//画像管理クラス.
	class DrawImgMng final
	{
	//▼ ===== 実体 ===== ▼.
	private:
		static DrawImgMng inst; //実体を入れる用.

	//▼ ===== 変数 ===== ▼.
	private:
		umap<string, DrawImg> images; //画像データ.
		MY_STRING             path;   //共通パス.

	//▼ ===== 関数 ===== ▼.
	private:
		//コンストラクタ.
		DrawImgMng(){}
		//デストラクタ.
		~DrawImgMng();

	public:
		//set.
		static void      SetPath    (MY_STRING path);
		//get.
		static DrawImg*  Get        (string saveName);
		static bool      TryGet     (string saveName, DrawImg* ptr);
		
		//画像読み込み.
		static void      LoadFile   (MY_STRING fileName, string saveName);
		static void      LoadFileDiv(MY_STRING fileName, INT_XY size, INT_XY cnt, vector<string> saveName);

		//使用禁止.
		DrawImgMng(const DrawImgMng&) = delete;
		DrawImgMng& operator=(const DrawImgMng&) = delete;
	};

	//テキスト描画クラス.
	class DrawStr
	{
	public:
		MY_STRING text;  //テキスト.
		INT_XY    pos;   //画面のどこに描画するか.
		MY_COLOR  color; //文字の色.

		//コンストラクタ.
		DrawStr(MY_STRING _text, INT_XY _pos, MY_COLOR _color) : text(_text), pos(_pos), color(_color) {}

		//描画.
		void   Draw	   (                                                                         Anchor anc = Anchor::Mid, int font = -1, bool isCameraDisp = true);
		void   DrawRota(INT_XY extend, INT_XY pivot, double ang, bool isVertical,                                          int font = -1, bool isCameraDisp = true);
		void   DrawModi(INT_XY luPos, INT_XY ruPos, INT_XY rdPos, INT_XY ldPos, bool isVertical,                           int font = -1, bool isCameraDisp = true);
		//その他.
		INT_XY GetTextSize(int font = -1);
	};

	//フォントクラス.
	class Font
	{
	private:
		int handle;

	public:
		//コンストラクタ, デストラクタ.
		Font();
		~Font();
		//get.
		int  GetFont() const { return handle; }
		//フォント作成.
		void CreateFontH(MY_STRING fontName, int size, int thick, FontTypeID fontId = FontTypeID::None);

		//使用禁止(「=」で実体が複製されて、意図せずデストラクタが実行されるのを防ぐため)
		Font& operator=(const Font&) = delete;
	};

	//グラデーション線.
	class GradLine
	{
	private:
		vector<VERTEX2D> points; //頂点データ.

	public:
		//頂点追加.
		void AddPoint(DBL_XY pos, MY_COLOR color);
		//描画.
		void Draw(bool isClose = false, bool isCameraDisp = true);
	};

	//描画モード設定(スコープ内のみ有効)
	class DrawMode
	{
	private:
		int oldMode1;
		int oldMode2;
		int oldMode2Param;

	public:
		//コンストラクタ.
		DrawMode(DrawModeID mode1, DrawBlendModeID mode2) :
			DrawMode(mode1, mode2, 255)
		{}
		DrawMode(DrawModeID mode1, DrawBlendModeID mode2, double mode2Param) :
			DrawMode(mode1, mode2, _int(mode2Param))
		{}
		DrawMode(DrawModeID mode1, DrawBlendModeID mode2, int mode2Param){
			//現在の設定を保存.
			oldMode1 = GetDrawMode();
			GetDrawBlendMode(&oldMode2, &oldMode2Param);
			//modeを設定.
			SetDrawMode(_int(mode1));
			SetDrawBlendMode(_int(mode2), mode2Param);
		}
		//デストラクタ.
		~DrawMode() {
			//設定を戻す.
			SetDrawMode(oldMode1);
			SetDrawBlendMode(oldMode2, oldMode2Param);
		}
	};
}