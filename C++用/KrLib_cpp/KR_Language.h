/*
   - KR_Language.h - (C++)
   ver.2026/06/11

   多言語対応用の文字列変換機能。
*/
#pragma once
//[include] KR_Global.
#if !defined DEF_KR_CPP_GLOBAL
  #include "KR_Global.h"
#endif

//KrLib名前空間.
namespace KR
{
	class Language
	{
	//▼ ===== 実体 ===== ▼.
	private:
		static Language inst; //実体を入れる用.

	//▼ ===== 変数 ===== ▼.
	private:
		umap<string, umap<string, MY_STRING>> textData;	//言語ごとのテキストデータ.

		string nowLang;	//現在の言語設定.

	//▼ ===== 関数 ===== ▼.
	public:
		//set.
		static void SetText(string key, string lang, MY_STRING text);
		static void SetLang(string lang);
		//get.
		static MY_STRING GetText(string key);
	};
}