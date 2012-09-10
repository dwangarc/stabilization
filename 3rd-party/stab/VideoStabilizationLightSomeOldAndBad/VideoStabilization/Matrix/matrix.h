#pragma once
#include <boost/shared_ptr.hpp>
#include <string.h>
#include <iostream>
#include <fstream>

#include "Include/define.h"



class Matrix
{

	sh_ptr_db mem;
	unsigned int height, width;

	void init();
	static double EPS1;

public:

	struct MatrixException
	{
		std::string info;
		MatrixException (std::string inf):info(inf)
		{

		}
	};

	Matrix& operator=(Matrix const&);

	Matrix(Matrix const&);

	Matrix();

	double* get_mem();
	double const* get_const_mem() const;

	double get_elem(unsigned int height_, unsigned int width_) const;

	void set_elem(unsigned int height_, unsigned int width_, double val);

	double& at(unsigned int height, unsigned int width);
	
	unsigned int get_height() const;

	unsigned int get_width() const;

	double get_EPS() const;

	/*
	mem_ should not be deleted from heap by delete[] mem_;
	*/
	Matrix(unsigned int height_, unsigned int width_, double* mem_ = 0);

	Matrix(unsigned int height_, unsigned int width_, sh_ptr_db mem_);

	size_t posit (unsigned int i, unsigned int j) const;

	Matrix transpose() const;

	Matrix dopoln(unsigned int i, unsigned int j) const;

	Matrix union_matr() const;

	Matrix const& mult(double val);

	Matrix mult(Matrix const& matr) const;
	Matrix power(int pw) const;

	Matrix obratn() const;

	Matrix square_root() const;

	double Gauss(Matrix& B);

	bool put_non_zero(unsigned int i, unsigned int j, Matrix& B);

	void fixDet();


	double dist(Matrix const& matrix) const;
	bool isEqual(Matrix const& matrix) const;

	Matrix solve(Matrix const& b) const;

	void operator +=(Matrix const&);
	Matrix operator +(Matrix const&) const;

	void operator -=(Matrix const&);
	Matrix operator -(Matrix const&) const;

	void operator *=(Matrix const&);
	Matrix operator *(Matrix const&) const;

	Matrix identity() const;
	static Matrix identity(unsigned int size);

	static Matrix read(std::ifstream& in);
	void print(std::ofstream& out) const;

	double* operator[](size_t row);

	double elemSumAbs() const;

};

typedef boost::shared_ptr<Matrix> sh_ptr_mt;

extern double determinant(const Matrix& matr);
extern double determinantFast(const Matrix& matr);
