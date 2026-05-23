/*
   - KR_Sound.cpp - (DxLib)
*/
#include "KR_Sound.h"

//KrLib名前空間.
namespace KR
{
// ▼*--=<[ Sound ]>=--*▼ //

	//コンストラクタ.
	Sound::Sound() 
		: handle(-1), nowVol(-1), aftVol(-1) 
	{
		timer = TimerMicro(TimerMode::CountUp, 0);
	};
	//デストラクタ.
	Sound::~Sound() {
		Release();
	};

	//ファイル読み込み.
	void Sound::LoadFile(MY_STRING fileName) {

		//読み込み済のものは解放.
		Release();
		//サウンド読み込み.
		handle = LoadSoundMem(fileName.c_str());
		//エラー.
		if (handle < 0) {
			throw ErrorMsg(_T("Sound::LoadFile"), _T("読み込み失敗"));
		}
	}
	//サウンド解放.
	void Sound::Release() {
		//データが登録されていれば.
		if (handle >= 0) {
			DeleteSoundMem(handle); //解放.
			handle = 0;
		}
	}
	//サウンド更新.
	void Sound::Update() {
	
		//音量が変化するなら.
		if (nowVol != aftVol) {
			//変化時間がある.
			if (aftUS > 0) {

				LONGLONG us = timer.GetPassTime(); //経過時間入手.

				//現在のボリュームを求める.
				assert(aftUS != 0);                                          //0割対策.
				int vol = _int(nowVol - (nowVol - aftVol) * _flt(us)/aftUS); //now - 変化量 * 変化時間割合.

				//変化し終わったら.
				if (us >= aftUS) {
					vol    = aftVol; //目標音量に固定.
					nowVol = aftVol; //変化後の音量を設定しておく.
					aftUS  = 0;      //もう音量変化しない.
					//フェードアウトなら.
					if (isFadeOut) {
						Stop(); //停止する.
					}
				}

				//データが登録されていれば.
				if (handle >= 0) {
					assert(0 <= vol && vol <= 255);    //範囲内チェック.
					ChangeVolumeSoundMem(vol, handle); //音量反映.
				}
			}
			//変化時間がない.
			else {
				ChangeVolumeSoundMem(aftVol, handle); //音量反映.
				nowVol = aftVol;                      //変化後の音量に設定.
			}
		}
	}
	//ボリューム値を有効範囲に変換.
	int Sound::GetVolumeRange(int volume) {

		int vol255 = 255 * volume/100;        //有効範囲(0～255)に変換.
		assert(0 <= vol255 && vol255 <= 255); //範囲内チェック.
		return vol255;
	}

	//サウンド再生.
	void Sound::Play(bool isLoop, int volume) {

		//データが登録されていれば.
		if (handle >= 0) {
			//音量変更.
			int vol255 = GetVolumeRange(volume);  //0～255に変換.
			ChangeVolumeSoundMem(vol255, handle); //音量反映.
			nowVol = aftVol = vol255;             //音量を保存.

			PlaySoundMem(handle, (isLoop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK));
		}
	}
	//サウンド停止.
	void Sound::Stop() {

		//データが登録されていれば.
		if (handle >= 0) {
			//停止.
			StopSoundMem(handle);
			//リセット.
			nowVol = -1;
			aftVol = -1;
			isFadeOut = false;
		}
	}
	//音量変更設定.
	void Sound::ChangeVolume(int volume, float sec) {
	
		nowVol = GetVolumeSoundMem2(handle); //現在の音量.
		aftVol = GetVolumeRange(volume);     //変化後の音量.
		aftUS  = (LONGLONG)(1000000 * sec);  //変化時間.
		//変化時間があるなら.
		if (aftUS > 0) {
			timer.Start(); //タイマー開始.
		}
	}
	//フェードイン再生.
	void Sound::FadeInPlay(bool isLoop, int volume, float sec) {

		Play(isLoop, 0);           //最初は音量0で再生.
		ChangeVolume(volume, sec); //徐々に大きく.
	}
	//フェードアウトする.
	void Sound::FadeOutPlay(float sec) {

		ChangeVolume(0, sec); //徐々に小さく.
		isFadeOut = true;     //フェードアウトモードにする.
	}

// ▼*--=<[ SoundMng ]>=--*▼ //

	//デストラクタ.
	SoundMng::~SoundMng() {
		//サウンドデータを全て取り出す.
		for (auto& i : sounds) {
			i.second.Release(); //各サウンドの解放.
		}
		sounds.clear(); //データを空にする.
	}

	//共通パスを設定.
	void SoundMng::SetPath(MY_STRING _path) {
		path = _path;
	}

	//サウンド取得.
	Sound* SoundMng::Get(string saveName) {
		//存在すれば.
		if (sounds.count(saveName) > 0) {
			return &sounds[saveName]; //返す.
		}
		return nullptr;
	}
	//サウンド取得(チェックあり)
	bool SoundMng::TryGet(string saveName, Sound* ptr) {
		//存在すれば.
		if (sounds.count(saveName) > 0) {
			ptr = &sounds[saveName]; //返す.
			return true;             //取得成功.
		}
		return false; //取得失敗.
	}

	//サウンド読み込み.
	void SoundMng::LoadFile(MY_STRING fileName, string saveName) {
	
		//既に存在すれば.
		if (sounds.count(saveName) > 0) {
			throw ErrorMsg(_T("SoundMng::LoadFile"), _T("使用済みの保存名"));
			return;
		}

		//パスを作成.
		const MY_STRING pathFull = path + fileName;

		//ファイル読み込み.
		try {
			sounds[saveName].LoadFile(pathFull);
		}
		catch (const ErrorMsg& err) {
			throw ErrorMsg(_T("SoundMng::LoadFile"), err.GetMsg());
		}
	}
	//全サウンド停止.
	void SoundMng::StopAll() {
		for (auto& i : sounds) {
			i.second.Stop();
		}
	}

	//更新(自動実行)
	void SoundMng::Update() {
		for (auto& i : sounds) {
			i.second.Update(); //各サウンドの更新.
		}
	}
}