#pragma once

#include <MMSystem.h>
#include <stdlib.h>
#include <pmmintrin.h>

#include "Geom/inc/Point.h"
#include "capture_log.h"
#include "Matrix/matrix.h"

namespace dif_lib
{
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

	inline void print(Point3D const** points, int size, ParamGetter const* getter, bool* use = 0)
	{
		Capture_Log::getInstance()->log_write("Points\n");
		for (int i = 0; i < size; ++i)
		{
			if (!use || use[i])
			{
				Capture_Log::getInstance()->log_write("(%7.2f,%7.2f,%7.2f |%3d); ", points[i]->x, points[i]->y, points[i]->z, getter->getParam(points[i]));
			}
		}
		Capture_Log::getInstance()->log_write("\n");
//		Capture_Log::getInstance()->log_write("%d\n", size);
		/*		
		for (int i = 0; i < size; ++i)
		{
			Capture_Log::getInstance()->log_write("%.4f %.4f %.4f ", points[i]->x, points[i]->y, points[i]->z );
		}
		Capture_Log::getInstance()->log_write("\n");
		*/
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

	__forceinline float descr_dist_sq_sse3(float *descr1, float *descr2, int d = 64) {
		float dist;
		__m128 sum = _mm_setzero_ps();
		for (int k = 0; k < d; k += 16) {
			__m128 d1 = _mm_sub_ps(_mm_load_ps(descr1 + k     ), _mm_load_ps(descr2 + k     ));
			__m128 d2 = _mm_sub_ps(_mm_load_ps(descr1 + k +  4), _mm_load_ps(descr2 + k +  4));
			__m128 d3 = _mm_sub_ps(_mm_load_ps(descr1 + k +  8), _mm_load_ps(descr2 + k +  8));
			__m128 d4 = _mm_sub_ps(_mm_load_ps(descr1 + k + 12), _mm_load_ps(descr2 + k + 12));
			d1 = _mm_mul_ps(d1, d1);
			d2 = _mm_mul_ps(d2, d2);
			d3 = _mm_mul_ps(d3, d3);
			d4 = _mm_mul_ps(d4, d4);

			d1 = _mm_add_ps(d1, d2);
			d3 = _mm_add_ps(d3, d4);
			sum = _mm_add_ps(sum, _mm_add_ps(d1, d3));
		}

		sum = _mm_hadd_ps(sum, sum);
		sum = _mm_hadd_ps(sum, sum);
		_mm_store_ss(&dist, sum);

		return dist;
	}

	__forceinline float descr_dist_sq_sse3_bound(float *descr1, float *descr2, int d, float dist_max) {
		float dist;

		__m128 sum		= _mm_setzero_ps();
		__m128 thresh	= _mm_load_ss(&dist_max);

		for (int k = 0; k < d; k += 16) {
			__m128 d1 = _mm_sub_ps(_mm_load_ps(descr1 + k     ), _mm_load_ps(descr2 + k     ));
			__m128 d2 = _mm_sub_ps(_mm_load_ps(descr1 + k +  4), _mm_load_ps(descr2 + k +  4));
			__m128 d3 = _mm_sub_ps(_mm_load_ps(descr1 + k +  8), _mm_load_ps(descr2 + k +  8));
			__m128 d4 = _mm_sub_ps(_mm_load_ps(descr1 + k + 12), _mm_load_ps(descr2 + k + 12));
			d1 = _mm_mul_ps(d1, d1);
			d2 = _mm_mul_ps(d2, d2);
			d3 = _mm_mul_ps(d3, d3);
			d4 = _mm_mul_ps(d4, d4);

			d1 = _mm_add_ps(d1, d2);
			d3 = _mm_add_ps(d3, d4);
			d1 = _mm_add_ps(d1, d3);

			d1 = _mm_hadd_ps(d1, d1);
			d1 = _mm_hadd_ps(d1, d1);

			sum = _mm_add_ss(sum, d1);
			if (_mm_comige_ss(sum, thresh)) break;
		}

		_mm_store_ss(&dist, sum);

		return dist;
	}

	inline void print(Matrix const& matr)
	{
		for (unsigned int i = 0; i < matr.get_height(); ++i)
		{
			for (unsigned int j = 0; j < matr.get_width(); ++j)
			{
				Capture_Log::getInstance()->log_write("%.4f ", matr.get_elem(i, j));
			}
			Capture_Log::getInstance()->log_write("\n");
		}
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

