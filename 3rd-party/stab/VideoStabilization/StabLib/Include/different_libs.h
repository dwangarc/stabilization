#pragma once

#include <stdlib.h>
#include <Windows.h>
#include <atlstr.h>

#include "Geom/inc/Point.h"
#include "Matrix/matrix.h"

namespace dif_lib
{

	inline std::string CStrToStr(CString const& cStr)
	{
		CT2CA pszConvertedAnsiString (cStr);
		return std::string(pszConvertedAnsiString);
	}

	inline int countNotLessThan(int* data, int size, int val)
	{
		int num = 0;
		for (int i = 0; i < size; ++i)
		{
			if (data[i] >= val)	++num;
		}
		return num;
	}

	template<class T> T* newInit(size_t size, T val)
	{
		T* data = new T[size];
		for (size_t i = 0; i < size; ++i)
		{
			data[i] = val;
		}
		return data;
	}

	inline int randomBool(bool* data, int size)
	{
		srand(GetTickCount() % 10000);
		int tr = 0;
		for (int i = 0; i < size; ++i)
		{
			data[i] = rand() % 2 == 0 ? false : true;
			if (data[i]) ++tr;
		}
		return tr;
	}

	inline double randomDouble(double val)
	{
		return (((double)rand()) / (double)RAND_MAX) * val;
	}

	inline double randomDoubleSigned(double val)
	{
		return randomDouble(2.0 * val) - val;
	}

	inline int randomInt(int val)
	{
		return rand() % val;
	}

	inline float descr_dist_sq( float const* descr1, float const* descr2, int len = 64)
	{
		float diff, dsq = 0;
		for( int i = 0; i < len; ++i )
		{
			diff = descr1[i] - descr2[i];
			dsq += diff * diff;
		}
		return dsq;
	}

	template<class T> inline void swap(T*& t1, T*& t2)
	{
		T* tmp = t1;
		t1 = t2;
		t2 = tmp;
	}

	inline std::wstring s2ws(const std::string& s)
	{
		int len;
		int slength = (int)s.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
		std::wstring r(buf);
		delete[] buf;
		return r;
	}



}

inline double sqr(double val)
{
	return val * val;
}

inline int sign(int val)
{
	if (val > 0)
	{
		return 1;
	}
	return val < 0 ? -1 : 0;
}

template <class Comp, class T> class SortContainer
{
public:
	Comp val;
	T* data;

	int operator<(SortContainer const& cont)
	{
		return val < cont.val;
	}
};

