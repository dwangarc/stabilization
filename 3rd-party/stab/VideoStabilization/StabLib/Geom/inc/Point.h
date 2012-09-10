#pragma once

#include <string>
#include "Matrix/matrix.h"
#include <math.h>

struct Point3D
{
	
	struct PointException
	{
		std::string info;
		PointException(std::string inf):info(inf)
		{

		}
	};

	double x, y, z;

	Point3D(double x_ = 0, double y_ = 0, double z_ = 0);

	Point3D(Point3D const&);
	Point3D(Matrix const& matr);

	Point3D operator+(Point3D const&p) const;
	Point3D operator-(Point3D const&p) const;

	Point3D const& operator +=(Point3D const&p);
	Point3D const& operator -=(Point3D const&p);

	Point3D const& operator *=(double val);
	Point3D operator *(double val) const;

	Point3D const& operator=(Point3D const&p);

	void export3D(double* data) const;
	void import3D(double const* data);

	void set3D(double x, double y, double z);
	void set3D(Point3D const& p1);

	Point3D const& normalize(double length = 1.0);

	double dist(Point3D const& p1) const;
	bool isEqual(Point3D const& p1) const;

	void read();
	void print() const;

	static Point3D infinity();
	static Point3D random();
	bool isInfinite() const;
	void shiftZX();

	Matrix toMatrix() const;

	virtual ~Point3D(){}

private:
	double sqr(double val) const
	{
		return val * val;
	}
};

struct ParamGetter
{
	virtual int getParam(Point3D const* ) const= 0;
	virtual ~ParamGetter(){}
};

template<class T> struct Point2DTmpl
{
	T x, y;

	Point2DTmpl(T x_ = 0, T y_ = 0);

	Point2DTmpl(Point2DTmpl<T> const&);

	bool operator<(Point2DTmpl<T> const&) const;
	bool operator==(Point2DTmpl<T> const&) const;
	bool operator<=(Point2DTmpl<T> const&) const;

	bool outOfRange(T minX, T minY, T maxX, T maxY) const;

	Point2DTmpl applyHomo(sh_ptr_db homo) const;

	T length() const;

	Point2DTmpl const& operator +=(Point2DTmpl const&);
	Point2DTmpl operator +(Point2DTmpl const&) const;
	Point2DTmpl const& operator -=(Point2DTmpl const&);
	Point2DTmpl operator -(Point2DTmpl const&) const;
	Point2DTmpl operator *(T val) const;
	Point2DTmpl const& operator *=(T val);


};

typedef Point2DTmpl<double> Point2D;
typedef Point2DTmpl<int> Point2Di;

struct Point3DContainer
{
	Point3D const** points;
	size_t size;

	Point3DContainer()
	{
		points = 0;
		size = 0;
	}

	Point3DContainer(Point3D const** points, size_t size)
	{
		this->size = size;
		this->points = points;
	}
};


template<class T> Point2DTmpl<T>::Point2DTmpl(T x_, T y_):x(x_), y(y_)
{

}

template<class T> Point2DTmpl<T>::Point2DTmpl(Point2DTmpl<T> const& point): x(point.x), y(point.y)
{

}

template<class T> bool Point2DTmpl<T>::operator<(Point2DTmpl<T> const& p) const
{
	return x < p.x && y < p.y;
}

template<class T> bool Point2DTmpl<T>::operator==(Point2DTmpl<T> const&p) const
{
	return p.x == x && p.y == y;
}

template<class T> bool Point2DTmpl<T>::operator<=(Point2DTmpl<T> const&p) const
{
	return *this < p || *this == p;
}



template<class T> bool Point2DTmpl<T>::outOfRange(T minX, T minY, T maxX, T maxY) const
{
	if (x < minX || y < minY || x > maxX || y > maxY) return true;
	return false;
}

template<class T> Point2DTmpl<T> Point2DTmpl<T>::applyHomo(sh_ptr_db homo) const
{
	Point2DTmpl<T>	 x1y1;
	x1y1.x = x * homo.get()[0] + y * homo.get()[1] + homo.get()[2];
	x1y1.y = x * homo.get()[3] + y * homo.get()[4] + homo.get()[5];
	return x1y1;
}

template<class T> Point2DTmpl<T> const& Point2DTmpl<T>::operator +=(Point2DTmpl<T> const& p1)
{
	x += p1.x;
	y += p1.y;
	return *this;
}

template<class T> Point2DTmpl<T> Point2DTmpl<T>::operator +(Point2DTmpl<T> const&p1) const
{
	Point2DTmpl<T> p(*this);
	return p += p1;
}

template<class T> Point2DTmpl<T> const& Point2DTmpl<T>::operator -=(Point2DTmpl<T> const&p1)
{
	x -= p1.x;
	y -= p1.y;
	return *this;
}

template<class T> Point2DTmpl<T> Point2DTmpl<T>::operator -(Point2DTmpl<T> const& p1) const
{
	Point2DTmpl<T> p(*this);
	return p -= p1;
}

template<class T> Point2DTmpl<T> Point2DTmpl<T>::operator *(T val) const
{
	Point2DTmpl<T> p(*this);
	return p *= val;
}

template<class T> Point2DTmpl<T> const& Point2DTmpl<T>::operator *=(T val)
{
	x *= val;
	y *= val;
	return *this;
}

template<class T> T Point2DTmpl<T>::length() const
{
	return sqrt(x * x + y * y);
}
