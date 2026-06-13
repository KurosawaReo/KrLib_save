/*
   - KR_Scene.cpp - (DxLib)
*/
#include "KR_SceneMng.h"

//KrLib名前空間.
namespace KR
{
// ▼*--=<[ SceneMng ]>=--*▼ //

	/*
	   InputMngやSoundMngは自動実行を止める必要はないが
	   SceneMngは自動実行を止めれるよう、専用の関数を用意.
	*/
	//自動実行設定.
	void SceneMng::SetAutoExeMode(MngAutoExe _state) {
		ManagerBase::SetAutoExeMode(_state);
	}
	//1つ前のモードに戻す.
	void SceneMng::BackAutoExeMode() {
		ManagerBase::BackAutoExeMode();
	}

	//シーン追加.
	void SceneMng::AddScene(IScene* sceneClass, string saveName) {
		sceneClass->Init();				//初期化.
		scenes[saveName] = sceneClass;	//クラスを登録.
	}
	//シーン変更.
	void SceneMng::SetScene(string saveName) {
		//登録されてなければ.
		if (scenes.count(saveName) <= 0) {
			throw ErrorMsg(_T("SceneMng::SetScene"), _T("未登録シーン"));
			return;
		}
		//次のシーンに設定.
		sceneChanger.RequestChange(scenes[saveName]);
		nowSceneName = saveName;
	}

	//基本処理(自動実行)
	void SceneMng::Reset() {
		//全シーンをリセット.
		for (auto& i : scenes) {
			i.second->Reset();
		}
	}
	void SceneMng::Update() {
		sceneChanger.Update(); //更新.
	}
	void SceneMng::Draw() {
		sceneChanger.Draw(); //描画.
	}
}