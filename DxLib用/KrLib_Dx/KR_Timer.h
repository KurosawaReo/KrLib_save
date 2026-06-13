/*
   - KR_Timer.h - (DxLib)
   ver.2026/04/27

   タイマー計測機能。
*/
#pragma once
//[include] KR_Global.
#if !defined DEF_KR_DX_GLOBAL
  #include "KR_Global.h"
#endif

//KrLib名前空間.
namespace KR
{
	//タイマーモード.
	enum class TimerMode
	{
		CountUp,	//増えるモード.
		CountDown,	//減るモード.
	};
	//タイマー状態.
	enum class TimerState
	{
		Stop,	//停止中.
		Pause,	//ポーズ中.
		Active,	//稼働中.
	};

	//タイマー共通 [継承想定]
	class TimerBase
	{
	//▼ ===== 変数 ===== ▼.
	protected:
		TimerMode  mode{};  //計測モード.
		TimerState state{};	//タイマー状態.

	//▼ ===== 関数 ===== ▼.
	private:
		virtual bool TimerStop() = 0; //停止処理.

	public:
		//コンストラクタ.
		TimerBase(TimerMode _mode) :
			mode(_mode), state(TimerState::Stop)
		{}
		//get.
		TimerState GetState() const { return state; }

		void Stop() {
			if (TimerStop()) {
				state = TimerState::Stop;  //停止できたらStopへ.
			}
		}
		void Pause() {
			if (TimerStop()) {
				state = TimerState::Pause; //停止できたらPauseへ.
			}
		}
		bool IntervalTime(); //一定時間ごとにtrueを返す.

		virtual void Start() = 0; //開始.
		virtual void Reset() = 0; //リセット.

		virtual bool IsCountDownEnd() const = 0; //カウントダウンが終了したか.
	};

	//タイマー機能.
	class Timer : public TimerBase
	{
	//▼ ===== 変数 ===== ▼.
	private:
		float      tmInit{};     //init : 初期時刻.
		clock_t    tmStart{};    //start: 開始時刻.

		float      tmSavePass{}; //時刻保存用(Stop後にStartしたら再開できるように)

	//▼ ===== 関数 ===== ▼.
	private:
		bool TimerStop() override; //タイマー停止.

	public:
		//コンストラクタ.
		Timer() :
			TimerBase(TimerMode::CountUp), tmInit(0), tmSavePass(0) //初期化.
		{}
		Timer(TimerMode _mode, float _init) :
			TimerBase(_mode), tmInit(_init), tmSavePass(_init) //初期化.
		{}

		void Start() override;
		void Reset() override;

		float GetPassTime()    const;          //時間取得.
		bool  IsCountDownEnd() const override; //カウントダウンが終了したか.
	};

	//タイマー機能(マイクロ秒)
	class TimerMicro : public TimerBase
	{
	//▼ ===== 変数 ===== ▼.
	private:
		LONGLONG      tmInit{};     //init     : 初期時刻(マイクロ秒)
		LARGE_INTEGER tmStart{};    //start    : 開始時刻(カウント)
		LARGE_INTEGER freq{};       //frequency: 1秒で何カウント進むか.

		LONGLONG      tmSavePass{}; //時刻保存用(Stop後にStartしたら再開できるように)

	//▼ ===== 関数 ===== ▼.
	private:
		bool TimerStop() override; //タイマー停止.

	public:
		//コンストラクタ.
		TimerMicro() :
			TimerBase(TimerMode::CountUp), tmInit(0), tmSavePass(0) //初期化.
		{
			QueryPerformanceFrequency(&freq); //頻度の取得.
		}
		TimerMicro(TimerMode _mode, LONGLONG _init) :
			TimerBase(_mode), tmInit(_init), tmSavePass(_init) //初期化.
		{
			QueryPerformanceFrequency(&freq); //頻度の取得.
		}

		void Start() override;
		void Reset() override;

		LONGLONG GetPassTime()    const;          //時間取得.
		double   GetFps();                        //fps取得.
		bool     IsCountDownEnd() const override; //カウントダウンが終了したか.
	};
}