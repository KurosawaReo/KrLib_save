/*
   - KR_Global.h - (C++)
   ver.2026/06/01

   KrLib全体で使う汎用プログラム。
*/
#pragma once

//このヘッダが定義されているか判別する用.
#define DEF_KR_CPP_GLOBAL

//定義用.
#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS
//C++用.
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <array>
#include <memory>
#include <string>  //string型用.
#include <cassert> //assert.h をラップしたもの.
#include <cmath>   //math.h   をラップしたもの.
#include <cstdlib> //stdlib.h をラップしたもの.
#include <ctime>   //time.h   をラップしたもの.
//C言語用.
#include <tchar.h>

//名前空間なしで使えるように.
using std::vector;
using std::list;
using std::map;
using std::unordered_map;
using std::array;
using std::unique_ptr;
using std::make_unique;
using std::string;
using std::wstring;
using std::to_string;
using std::to_wstring;

//型変換マクロ.
#define _int(n)   static_cast<int>   (n)        //int型変換マクロ.
#define _int_r(n) static_cast<int>   (round(n)) //int型変換マクロ(四捨五入)
#define _flt(n)   static_cast<float> (n)        //float型変換マクロ.
#define _dbl(n)   static_cast<double>(n)        //double型変換マクロ.
#define _byte(n)  static_cast<BYTE>  (n)        //BYTE型変換マクロ.
//角度変換マクロ.
#define _rad(n) (n)*(M_PI/180)
#define _deg(n) (n)*(180/M_PI)
//便利マクロ.
#define _if_check(n)        assert(n); if(n)             //if文の前に同条件のassertを挟む.
#define _return(num, condi) if (condi) { return num; }   //条件に合うならreturnする(cond = 条件)
#define _get_name(value)    #value                       //変数名や関数名を取得.
#define _elif               else if                      //「else if」の略.
//template用マクロ.
#define _type_num_only(T)	typename = typename std::enable_if_t<std::is_arithmetic<T>::value> //算術型(int/float/double/char)のみOKとし, そうでない場合は関数を無効にする.

//KrLib名前空間.
namespace KR
{
	//文字コードで切り替え.
#if defined UNICODE
	using MY_STRING = wstring;   //wchar_t型.
	#define _to_mystr to_wstring //to_wstring用.
#else
	using MY_STRING = string;    //char型.
	#define _to_mystr to_string  //to_string用.
#endif

	//省略名.
	template<class Key, class Value>
	using umap = unordered_map<Key, Value>;

	//[2D] xとyの凝縮型.
	template<typename T> //型を<>で入力して使う.
	struct XY
	{
		T x, y;

		//コンストラクタ.
		XY()           : x(0),  y(0)  {}
		XY(T _x, T _y) : x(_x), y(_y) {} //INT_XY n = {1, 0}; この書き方ができる.

		//int型に変換.
		XY<int>    ToInt() const {
			return {_int_r(x), _int_r(y)};
		}
		//double型に変換.
		XY<double> ToDbl() const {
			return {_dbl(x), _dbl(y)};
		}
		//加算した結果を返す.
		XY<T> Add(T _x, T _y) const {
			return { x + _x, y + _y};
		}
		XY<T> Add(XY<T> other) const {
			return *this + other;
		}
		//距離を求める.
		double Dist() const {
			return sqrt(x * x + y * y);
		}
		//方向ベクトル(長さは1.0の力の向き)を求める.
		XY<T> Normalize() const {
			const double len = Dist();     //距離を取得.
			if (len == 0) return { 0, 0 }; //距離0ならベクトルなし.
			return { x/len, y/len };
		}
		//ベクトルの角度を求める.
		double Angle() const {
			return _deg(atan2(y, x));
		}

		//演算子[+,-,*,/,%] [XY<T>・XY<T>]
		XY<T> operator+(const XY<T>& other) const {  //+の右側が引数に入り、返り値が左側に入る.
			return { x + other.x, y + other.y };     //xとyを加算して返す.
		}
		XY<T> operator-(const XY<T>& other) const {
			return { x - other.x, y - other.y };
		}
		XY<T> operator*(const XY<T>& other) const {
			return { x * other.x, y * other.y };
		}
		XY<T> operator/(const XY<T>& other) const {
			return { x / other.x, y / other.y };
		}
		XY<T> operator%(const XY<T>& other) const {
			return { x % other.x, y % other.y };
		}
		//演算子[+=,-=,*=,/=,%=] [XY<T>・XY<T>]
		XY<T>& operator+=(const XY<T>& other) {
			*this = *this + other;
			return *this; //自身の実体.
		}
		XY<T>& operator-=(const XY<T>& other) {
			*this = *this - other;
			return *this;
		}
		XY<T>& operator*=(const XY<T>& other) {
			*this = *this * other;
			return *this;
		}
		XY<T>& operator/=(const XY<T>& other) {
			*this = *this / other;
			return *this;
		}
		XY<T>& operator%=(const XY<T>& other) {
			*this = *this % other;
			return *this;
		}

		//演算子[+,-,*,/,%] [XY<T>・数値]
		//右側が数値でなければ無効にする.
		template<typename T2, _type_num_only(T2)>
		XY<T> operator+(T2 num) const {
			return { x + static_cast<T>(num), y + static_cast<T>(num) }; //cast後にxとyを加算して返す.
		}
		template<typename T2, _type_num_only(T2)>
		XY<T> operator-(T2 num) const {
			return { x - static_cast<T>(num), y - static_cast<T>(num) };
		}
		template<typename T2, _type_num_only(T2)>
		XY<T> operator*(T2 num) const {
			return { x * static_cast<T>(num), y * static_cast<T>(num) };
		}
		template<typename T2, _type_num_only(T2)>
		XY<T> operator/(T2 num) const {
			return { x / static_cast<T>(num), y / static_cast<T>(num) };
		}
		template<typename T2, _type_num_only(T2)>
		XY<T> operator%(T2 num) const {
			return { x % static_cast<T>(num), y % static_cast<T>(num) };
		}
		//演算子[+=,-=,*=,/=,%=] [XY<T>・数値]
		//右側が数値でなければ無効にする.
		template<typename T2, _type_num_only(T2)>
		XY<T>& operator+=(T2 num) {
			*this = *this + num;
			return *this; //自身の実体.
		}
		template<typename T2, _type_num_only(T2)>
		XY<T>& operator-=(T2 num) {
			*this = *this - num;
			return *this;
		}
		template<typename T2, _type_num_only(T2)>
		XY<T>& operator*=(T2 num) {
			*this = *this * num;
			return *this;
		}
		template<typename T2, _type_num_only(T2)>
		XY<T>& operator/=(T2 num) {
			*this = *this / num;
			return *this;
		}
		template<typename T2, _type_num_only(T2)>
		XY<T>& operator%=(T2 num) {
			*this = *this % num;
			return *this;
		}
	};
	using INT_XY = XY<int>;    //int型.
	using DBL_XY = XY<double>; //double型.

	//[3D] xとyとzの凝縮型.
	template<typename T> //型を<>で入力して使う.
	struct XYZ
	{
		T x, y, z;

		//コンストラクタ.
		XYZ()                 : x(0),  y(0),  z(0)  {}
		XYZ(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {} 

		//int型に変換.
		XYZ<int>    ToInt() const {
			return {_int_r(x), _int_r(y), _int_r(z)};
		}
		//double型に変換.
		XYZ<double> ToDbl() const {
			return {_dbl(x), _dbl(y), _dbl(z)};
		}
		//加算した結果を返す.
		XYZ<T> Add(T _x, T _y, T _z) const {
			return { x + _x, y + _y, z + _z};
		}
		XYZ<T> Add(XYZ<T> other) const {
			return *this + other;
		}

		//演算子[+,-,*,/,%] [XYZ<T>・XYZ<T>]
		XYZ<T> operator+(const XYZ<T>& other) const { 
			return { x + other.x, y + other.y, z + other.z };
		}
		XYZ<T> operator-(const XYZ<T>& other) const {
			return { x - other.x, y - other.y, z - other.z };
		}
		XYZ<T> operator*(const XYZ<T>& other) const {
			return { x * other.x, y * other.y, z * other.z };
		}
		XYZ<T> operator/(const XYZ<T>& other) const {
			return { x / other.x, y / other.y, z / other.z };
		}
		XYZ<T> operator%(const XYZ<T>& other) const {
			return { x % other.x, y % other.y, z % other.z };
		}
		//演算子[+=,-=,*=,/=,%=] [XYZ<T>・XYZ<T>]
		XYZ<T>& operator+=(const XYZ<T>& other) {
			*this = *this + other;
			return *this; //自身の実体.
		}
		XYZ<T>& operator-=(const XYZ<T>& other) {
			*this = *this - other;
			return *this;
		}
		XYZ<T>& operator*=(const XYZ<T>& other) {
			*this = *this * other;
			return *this;
		}
		XYZ<T>& operator/=(const XYZ<T>& other) {
			*this = *this / other;
			return *this;
		}
		XYZ<T>& operator%=(const XYZ<T>& other) {
			*this = *this % other;
			return *this;
		}

		//演算子[+,-,*,/,%] [XYZ<T>・数値]
		//右側が数値でなければ無効にする.
		template<typename T2, _type_num_only(T2)>
		XYZ<T> operator+(T2 num) const {
			return { x + static_cast<T>(num), y + static_cast<T>(num), z + static_cast<T>(num) }; //cast後にxとyを加算して返す.
		}
		template<typename T2, _type_num_only(T2)>
		XYZ<T> operator-(T2 num) const {
			return { x - static_cast<T>(num), y - static_cast<T>(num), z - static_cast<T>(num) };
		}
		template<typename T2, _type_num_only(T2)>
		XYZ<T> operator*(T2 num) const {
			return { x * static_cast<T>(num), y * static_cast<T>(num), z * static_cast<T>(num) };
		}
		template<typename T2, _type_num_only(T2)>
		XYZ<T> operator/(T2 num) const {
			return { x / static_cast<T>(num), y / static_cast<T>(num), z / static_cast<T>(num) };
		}
		template<typename T2, _type_num_only(T2)>
		XYZ<T> operator%(T2 num) const {
			return { x % static_cast<T>(num), y % static_cast<T>(num), z % static_cast<T>(num) };
		}
		//演算子[+=,-=,*=,/=,%=] [XYZ<T>・数値]
		//右側が数値でなければ無効にする.
		template<typename T2, _type_num_only(T2)>
		XYZ<T>& operator+=(T2 num) {
			*this = *this + num;
			return *this; //自身の実体.
		}
		template<typename T2, _type_num_only(T2)>
		XYZ<T>& operator-=(T2 num) {
			*this = *this - num;
			return *this;
		}
		template<typename T2, _type_num_only(T2)>
		XYZ<T>& operator*=(T2 num) {
			*this = *this * num;
			return *this;
		}
		template<typename T2, _type_num_only(T2)>
		XYZ<T>& operator/=(T2 num) {
			*this = *this / num;
			return *this;
		}
		template<typename T2, _type_num_only(T2)>
		XYZ<T>& operator%=(T2 num) {
			*this = *this % num;
			return *this;
		}
	};
	using INT_XYZ = XYZ<int>;    //int型.
	using DBL_XYZ = XYZ<double>; //double型.

	//四角形型.
	template<typename T, _type_num_only(T)>
	struct RECT
	{
		T left;
		T up;
		T right;
		T down;

		//int型に変換.
		RECT<int>    ToInt() const {
			return { _int_r(left), _int_r(up), _int_r(right), _int_r(down) };
		}
		//double型に変換.
		RECT<double> ToDbl() const {
			return { _dbl(left), _dbl(up), _dbl(right), _dbl(down) };
		}
		//拡大縮小した結果を返す(+で拡大, -で縮小)
		RECT<T> AddSize(T other) {
			return { left-other, up-other, right+other, down+other }; 
		}

		//get.
		XY<T> GetSize() const { return { right-left, down-up }; }			//サイズ.
		XY<T> GetL()    const { return { left,  (up+down)/2 }; }			//左中央.
		XY<T> GetR()    const { return { right, (up+down)/2 }; }			//右中央.
		XY<T> GetU()    const { return { (left+right)/2, up   }; }			//上中央.
		XY<T> GetD()    const { return { (left+right)/2, down }; }			//下中央.
		XY<T> GetLU()   const { return { left,  up   }; }					//左上.
		XY<T> GetRU()   const { return { right, up   }; }					//右上.
		XY<T> GetLD()   const { return { left,  down }; }					//左下.
		XY<T> GetRD()   const { return { right, down }; }					//右下.
		XY<T> GetMid()  const { return { (left+right)/2, (up+down)/2 }; }	//中央.
	};
	using INT_RECT = RECT<int>;    //int型.
	using DBL_RECT = RECT<double>; //double型.

	//エラー文(throw用)
	class ErrorMsg final
	{
	private:
		MY_STRING funcName; //関数名.
		MY_STRING msg;      //メッセージ.

	public:
		//コンストラクタ.
		ErrorMsg(MY_STRING _funcName, MY_STRING _msg) :
			funcName(_funcName), msg(_msg)
		{};
		//結果を取得.
		MY_STRING GetMsg() const {
			return _T("[Error] FuncName: ") + funcName + _T(", Msg: ") + msg;
		}
	};

	//<T> 数値が範囲内か.
	template<typename T, _type_num_only(T)>
	bool IsNumInRange(T _num, T _min, T _max) {
		return (_min <= _num && _num <= _max);
	}
	//<T> 数値の上限.
	template<typename T, _type_num_only(T)>
	void NumLimMax(T* _num, T _max) {
		*_num = std::min(*_num, _max);
	}
	//<T> 数値の下限.
	template<typename T, _type_num_only(T)>
	void NumLimMin(T* _num, T _min) {
		*_num = std::max(*_num, _min);
	}
	//<T> 数値の範囲.
	template<typename T, _type_num_only(T)>
	void NumLimRange(T* num, T low, T high) {
		*num = std::max(low, std::min(*num, high));
	}
}