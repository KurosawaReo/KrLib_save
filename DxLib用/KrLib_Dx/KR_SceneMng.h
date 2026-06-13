/*
   - KR_SceneMng.h - (DxLib)
   ver.2026/06/11

   シーン管理機能。
*/
#pragma once
//[include] KR_Global.
#if !defined DEF_KR_DX_GLOBAL
  #include "KR_Global.h"
#endif
//[include] ".h"ファイルで使うもの.
#include "KR_ManagerBase.h"
#include "KR_Statemachine.h"

//KrLib名前空間.
namespace KR
{
	/*
	   シーンクラスの基礎 [継承想定]

	   Init:   シーン管理クラスに登録した時に実行.
	   Reset:  Appクラスでリセットした時に実行.
	   Enter:  シーンに入った瞬間実行.
	   Exit:   シーンから抜けた瞬間実行.
	   Update: 自動実行.
	   Draw:   自動実行.
	*/
	class IScene : public IState
	{
	public:
		virtual void Init()  = 0;
		virtual void Reset() = 0;
	};

	/*
	   シーン管理クラス.
	*/
	class SceneMng final : public ManagerBase
	{
	//▼ ===== 変数 ===== ▼.
	private:
		umap<string, IScene*> scenes{};       //全シーン.
		StateMachine          sceneChanger{}; //ステートマシン.
		string                nowSceneName{}; //現シーン名.

	//▼ ===== 関数 ===== ▼.
	public:
		//コンストラクタ.
		SceneMng(int order) : ManagerBase(order) {}

		//get.
		string GetSceneName() { return nowSceneName; }
		//set.
		void SetAutoExeMode (MngAutoExe _state);
		void BackAutoExeMode();

		void Init()   override {} //未使用.
		void Reset()  override;
		void Update() override;
		void Draw()   override;

		void AddScene(IScene* sceneClass, string saveName); //シーン追加.
		void SetScene(string saveName);                     //シーン変更.

		//使用禁止.
		SceneMng(const SceneMng&) = delete;
		SceneMng& operator=(const SceneMng&) = delete;
	};
}