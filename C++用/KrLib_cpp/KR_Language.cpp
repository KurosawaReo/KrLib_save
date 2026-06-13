/*
   - KR_Language.cpp - (C++)
*/
#include "KR_Language.h"

//KrLib名前空間.
namespace KR
{
	Language Language::inst; //実体生成.

	//翻訳キーにテキストを登録.
	void Language::SetText(string key, string lang, MY_STRING text) {

		//空文字の場合は無視.
		if (key.empty())  { return; }
		if (lang.empty()) { return; }

		inst.textData[key][lang] = text;
	}
	//言語の設定.
	void Language::SetLang(string lang) {
		inst.nowLang = lang;
	}

	//翻訳キーからテキストを取得.
	MY_STRING Language::GetText(string key) {

		//翻訳キー取得.
		auto keyIt = inst.textData.find(key);
		if (keyIt == inst.textData.end()) {
			return _T("?"); //不明な翻訳キー.
		}

		//言語取得.
		auto langIt = keyIt->second.find(inst.nowLang);
		if (langIt == keyIt->second.end()) {
			return _T("?"); //不明な言語.
		}

		//テキストを返す.
		return langIt->second;
	};
}