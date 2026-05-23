/*
   - KR_Sound.h - (DxLib)
   ver.2026/05/07

   サウンド再生機能。
*/
#pragma once
//[include] KR_Global.
#if !defined DEF_KR_DX_GLOBAL
  #include "KR_Global.h"
#endif
//[include] ".h"ファイルで使うもの.
#include "KR_ManagerBase.h"
#include "KR_Timer.h"

//KrLib名前空間.
namespace KR
{
	//サウンドクラス.
	class Sound
	{
		friend class SoundMng;   //アクセス許可.

	//▼ ===== 変数 ===== ▼.
	private:
		int        handle{};    //サウンドハンドル.
		
		int        nowVol{};    //現在の音量.
		int        aftVol{};    //変化後の音量.
		LONGLONG   aftUS{};     //変化時間(マイクロ秒)
		TimerMicro timer{};     //タイマー計測用.
		bool       isFadeOut{}; //フェードアウトで音を消すか.

	//▼ ===== 関数 ===== ▼.
	private:
		void LoadFile(MY_STRING fileName);	//読み込み.
		void Release();						//解放.
		void Update();						//更新.
		int	 GetVolumeRange(int volume);	//ボリューム値を有効範囲に変換.
		 
	public:
		//コンストラクタ, デストラクタ.
		//std::mapを使う関係でpublicに入れておく.
		Sound();
		~Sound();

		void Play        (bool isLoop, int volume = 100);		//再生.
		void Stop        ();									//停止.
		void ChangeVolume(int volume, float sec = 0);			//音量変更設定.
		void FadeInPlay  (bool isLoop, int volume, float sec);	//フェードイン再生.
		void FadeOutPlay (float sec);						    //フェードアウトする.

		//使用禁止(「=」で実体が複製されて、意図せずデストラクタが実行されるのを防ぐため)
		Sound& operator=(const Sound&) = delete;
	};

	//サウンド管理クラス.
	class SoundMng final : public ManagerBase
	{
	//▼ ===== 変数 ===== ▼.
	private: 
		umap<string, Sound> sounds; //サウンドデータ.
		MY_STRING           path;   //共通パス.

	//▼ ===== 関数 ===== ▼.
	public:
		//コンストラクタ.
		SoundMng(int order) : ManagerBase(order) {}
		//デストラクタ.
		~SoundMng();

		//set.
		void   SetPath (MY_STRING path);
		//get.
		Sound* Get     (string saveName);
		bool   TryGet  (string saveName, Sound* ptr);

		void   LoadFile(MY_STRING fileName, string saveName); //読み込み.
		void   StopAll ();									  //全サウンド停止.
	
		void   Init()   override {} //未使用.
		void   Reset()  override {} //未使用.
		void   Update() override;
		void   Draw()   override {} //未使用.

		//使用禁止.
		SoundMng(const SoundMng&) = delete;
		SoundMng& operator=(const SoundMng&) = delete;
	};
}