#include "stdafx.h"
#include <math.h>
#include "matrix.h"
#include <float.h>
#include "Math/linear_algebra.h"
#include "Include/different_libs.h"

double Matrix::EPS1 = 1e-8;

double determinantFast(const Matrix& matr)
{
	if (matr.get_height() != matr.get_width())
	{
		throw Matrix::MatrixException("Non square matrix");
	}
	unsigned int size = matr.get_height();
	if (size == 1)
	{
		return matr.get_elem(0, 0);
	}
	Matrix tmp = matr;
	Matrix B(size, size);
	double res = tmp.Gauss(B);
	return res;
}


double determinant(const Matrix& matr)
{
	if (matr.get_height() != matr.get_width())
	{
		throw Matrix::MatrixException("Non square matrix");
	}
	unsigned int size = matr.get_height();
	if (size == 1)
	{
		return matr.get_elem(0, 0);
	}
	double ans = 0;
	double sign = 1;
	for (unsigned int i = 0; i < size; ++i)
	{
		Matrix mt(matr.dopoln(0, i));
		unsigned int place = 0;
		ans += matr.get_elem(0, i) * determinant(mt) * sign;
		sign *= -1;
	}
	return ans;

}

double Matrix::get_elem(unsigned int height_, unsigned int width_) const
{
	return mem.get()[height_ * width + width_];
}

unsigned int Matrix::get_height() const
{
	return height;
}

unsigned int Matrix::get_width() const
{
	return width;
}



unsigned int Matrix::posit(unsigned int i, unsigned int j) const
{
	return width * i + j;
}

double* Matrix::get_mem()
{
	return mem.get();
}

Matrix::Matrix():height(0),width(0)
{
	init();
}

void Matrix::init()
{
	Matrix::EPS1 = 1e-8;
}

Matrix::Matrix(unsigned int height_, unsigned int width_, double* mem_): height(height_), width(width_)
{
	init();
	if (mem_ != 0)
	{
		mem = sh_ptr_db(mem_);
	}
	else
	{
		mem = sh_ptr_db(new double[height * width]);
		for (unsigned int i = 0; i < height * width; ++i)
		{
			mem.get()[i] = 0;
		}
	}
}

Matrix::Matrix(unsigned int height_, unsigned int width_, sh_ptr_db mem_) : height(height_), width(width_)
{
	mem = mem_;
}


void Matrix::set_elem(unsigned int height_, unsigned int width_, double val)
{
	at(height_, width_) = val;
}


Matrix Matrix::transpose() const
{
	Matrix matr(width, height, new double[height * width]);
	for (unsigned int i = 0; i < height; ++i)
	{
		for (unsigned int j = 0; j < width; ++j)
		{
			matr.at(j, i) = get_elem(i, j);
		}
	}
	return matr;
}

Matrix Matrix::dopoln(unsigned int i, unsigned int j) const
{
	if (height == width)
	{
		unsigned int size = height;
		Matrix mt(size - 1, size - 1, new double[(size - 1) * (size - 1)]);
		unsigned int place = 0;
		for (unsigned int k = 0; k < size; ++k)
		{
			for (unsigned int g = 0; g < size; ++g)
			{
				if (k != i && g != j)
				{
					mt.get_mem()[place] = get_elem(k, g);
					place++;
				}
			}
		}
		return mt;
	}
	return Matrix();
}


Matrix Matrix::union_matr() const
{
	if (height == width)
	{
		unsigned int size = height;
		Matrix matr(size, size, new double[size * size]);
		for (unsigned int i = 0; i < size; ++i)
		{
			for (unsigned int j = 0; j < size; ++j)
			{
				matr.at(i, j) = pow(-1.0, (double)i + j) * determinantFast(dopoln(i, j));
			}
		}
		return matr.transpose();
	}
	return Matrix();
}

Matrix const& Matrix::mult(double val)
{
	for (unsigned int i = 0; i < height; ++i)
	{
		for (unsigned int j = 0; j < width; ++j)
		{
			at(i, j) *= val;
		}
	}
	return *this;
}

Matrix Matrix::obratn() const
{
	double det = determinantFast(*this);
	Matrix matr(union_matr());
	matr.mult(1 / det);
	return matr;
}

Matrix Matrix::square_root() const
{
	Matrix Z = identity();
	Matrix Y = Matrix(*this);
	double min_dist = 1e-6;
	int step = 0;
	while (Y.power(2).dist(*this) > min_dist && step++ < 1e3)
	{
		Matrix zobr = Z.obratn();
		Matrix yobr = Y.obratn();
		Matrix Y1 = (zobr + Y).mult(0.5);
		Matrix Z1 = (Z + yobr).mult(0.5);
		Y = Y1;
		Z = Z1;
	}
	return Y;
}

Matrix Matrix::power01(double pw, int iter) const
{
	Matrix res = Matrix::identity(height);
	if (iter == 0)
	{
		return res;
	}
	Matrix root = square_root();
	if (pw >= 0.5)
	{
		res = root;
		pw -= 0.5;
	}
	pw *= 2;
	res *= root.power01(pw, iter - 1);
	return res;
}



double& Matrix::at(unsigned int height, unsigned int width)
{
	return mem.get()[posit(height, width)];
}


Matrix Matrix::mult(Matrix const& matr) const
{
	if (width != matr.height)
	{
		throw MatrixException("Non equal number of columns and rows");
	}
	Matrix result(height, matr.width, new double[height * matr.width]);
	for (unsigned int i = 0; i < height; ++i)
	{
		for (unsigned int j = 0; j < matr.width; ++j)
		{
			result.at(i, j) = 0;
			for (unsigned int k = 0; k < width; ++k)
			{
				result.at(i,j) += get_elem(i, k) * matr.get_elem(k, j);
			}
		}
	}
	return result;
}

bool Matrix::put_non_zero(unsigned int i, unsigned int j, Matrix& B)
{
	for (unsigned int k = i + 1; k < height; ++k)
	{
		if (abs(get_elem(k, j)) > Matrix::EPS1)
		{
			double tmp = B.at(k, 0);
			B.at(k, 0) = B.at(i, 0);
			B.at(i, 0) = tmp;
			for (unsigned int g = 0; g < width; ++g)
			{
				double temp = get_elem(i, g);
				at(i, g) = get_elem(k, g);
				at(k, g) = temp;
			}
			return true;
		}
	}
	return false;
//	throw MatrixException("No non-zero elements under in the column.");
}

Matrix& Matrix::operator=(Matrix const& matr)
{
	height = matr.height;
	width = matr.width;
	mem = sh_ptr_db(new double[height * width]);
	for (unsigned int i = 0; i < height * width; ++i)
	{
		mem.get()[i] = matr.mem.get()[i];
	}
	return *this;
}

Matrix::Matrix(Matrix const& matr)
{
	height = matr.height;
	width = matr.width;
	mem = sh_ptr_db(new double[height * width]);
	for (unsigned int i = 0; i < height * width; ++i)
	{
		mem.get()[i] = matr.mem.get()[i];
	}
}


double Matrix::Gauss(Matrix& B)
{
	if (height != width || B.height != height )
	{
		throw MatrixException("Bad matrix dimension for gauss method.");
	}

	int swapLines = 0;
	double det = 1;

	for (unsigned int i = 0; i < height - 1; ++i) // Step of way down
	{
		if (abs(get_elem(i, i)) < Matrix::EPS1 )
		{
			if (!put_non_zero(i, i, B)) return 0;
			++swapLines;
		}
		det *= get_elem(i, i);
		for (unsigned int j = i + 1; j < height; ++j) // Step of subtraction.
		{
			double koeff = get_elem(j, i) / get_elem(i, i);
			for (unsigned int g = i; g < width; ++g)
			{
				at(j, g) =  get_elem(j, g) - koeff * get_elem(i, g);
			}
			B.at(j, 0) =  B.get_elem(j, 0) - koeff * B.get_elem(i, 0);
		}
	}
	det *= get_elem(height - 1, height - 1);

	for (int i = height - 1; i >= 0; --i) // way back.
	{
		double koeff = get_elem(i, i);
		B.at(i, 0) = B.get_elem(i, 0) / koeff;
		set_elem(i, i, 1);
		for (unsigned int j = 0; j < (unsigned int)i; ++j)
		{
			if (abs(get_elem(j, i)) > Matrix::EPS1)
			{
				B.at(j, 0) = B.get_elem(j, 0) - B.get_elem(i, 0) * get_elem(j, i);
				set_elem(j, i, 0);
			}
		}
	}

	return swapLines % 2 == 0 ? det : -det;
	// After all we have matrix I and at the left new matrix B.
}

Matrix Matrix::power(int pw) const
{
	if (pw == 0) return identity();
	if (pw == 1) return Matrix(*this);
	if (pw % 2 == 0)
	{
		Matrix tmp = power(pw / 2);
		return tmp.mult(tmp);
	}
	return this->mult(power(pw - 1));
}


double Matrix::get_EPS() const
{
	return Matrix::EPS1;
}

void Matrix::fixDet()
{
	if (width != height) return;
	double det = determinant(*this);
	double det1 = determinantFast(*this);
	if (abs(det - det1) > 0.0001)
	{
		double det2 = determinantFast(*this);
		throw new MatrixException("Bad determinant fast in fix det");
	}
	if (det < 0)
	{
		if (width % 2 == 0) return;
		mult(-1);
		det *= -1;
	}
	det = pow(det, 1 / (double)width);
	for (unsigned int i = 0; i < height; ++i)
	{
		for (unsigned int j = 0; j < width; ++j)
		{
			mem.get()[i * width + j] /= det;
		}
	}
}

double Matrix::dist(Matrix const& matrix) const
{
	if (height != matrix.height || width != matrix.width) return DBL_MAX;
	double dst = 0;
	for (unsigned int i = 0; i < height; ++i)
	{
		for (unsigned int j = 0; j < width; ++j)
		{
			dst += sqr(get_elem(i, j) - matrix.get_elem(i, j));
		}
	}
	dst /= height * width;
	dst = sqrt(dst);
	return dst;
}


bool Matrix::isEqual(Matrix const& matrix) const
{
	if (height != matrix.height || width != matrix.width) return false;
	for (unsigned int i = 0; i < height; ++i)
	{
		for (unsigned int j = 0; j < width; ++j)
		{
			if (abs(get_elem(i, j) - matrix.get_elem(i, j)) < 1e-6) return false;
		}
	}
	return true;
}


Matrix Matrix::solve(Matrix const& b) const
{
	if (height != width || b.get_width() != 1 || b.get_height() != height) return Matrix();
	Matrix x(height, 1);
	char* buf = new char[height * (sizeof(int) + sizeof(double))];
	double* a = new double[height * width];
	for (unsigned int i = 0; i < width * height; ++i)
	{
		a[i] = mem.get()[i];
	}
	int res = la_solve_lu(a, b.get_const_mem(), x.get_mem(), height, buf);
	delete[] buf;
	delete[] a;
	if (res == 0) return Matrix();
	return x;
}

void Matrix::operator +=(Matrix const& matr)
{
	if (height != matr.height || width != matr.width) return;
	for (unsigned int i = 0; i < height; ++i)
	{
		for (unsigned int j = 0; j < width; ++j)
		{
			at(i, j) += matr.get_elem(i, j);
		}
	}
}

Matrix Matrix::operator +(Matrix const& matr) const
{
	Matrix mt(*this);
	mt += matr;
	return mt;
}

void Matrix::operator -=(Matrix const& matr)
{
	if (height != matr.height || width != matr.width) return;
	for (unsigned int i = 0; i < height; ++i)
	{
		for (unsigned int j = 0; j < width; ++j)
		{
			at(i, j) -= matr.get_elem(i, j);
		}
	}
}

Matrix Matrix::operator -(Matrix const& matr) const
{
	Matrix mt(*this);
	mt -= matr;
	return mt;
}

void Matrix::operator *=(Matrix const& matr)
{
	*this = this->mult(matr);
}

Matrix Matrix::operator *(Matrix const& matr) const
{
	return this->mult(matr);
}



double const* Matrix::get_const_mem() const
{
	return (double const*)mem.get();
}

Matrix Matrix::identity() const
{
	Matrix ident(height, height);
	for (unsigned int i = 0; i < height; ++i)
	{
		ident.at(i, i) = 1;
	}
	return ident;
}

Matrix Matrix::identity(unsigned int size)
{
	Matrix mt(size, size);
	return mt.identity();
}

Matrix Matrix::read(std::ifstream& in)
{
	unsigned int h, w;
	in >> h >> w;
	double* data = new double[h * w];
	for (unsigned int i = 0; i < w * h; ++i)
	{
		in >> data[i];
	}
	return Matrix(h, w, data);
}

void Matrix::print(std::ofstream& out) const
{
	out << height << " " << width << "\n";
	for (unsigned int i = 0; i < height; ++i)
	{
		for (unsigned int j = 0; j < width; ++j)
		{
			out << get_elem(i, j) << " ";
		}
		out << "\n";
	}
}

double* Matrix::operator[](unsigned int row)
{
	if (row < height)
	{
		return get_mem() + row * width;
	}
	return 0;
}

double Matrix::elemSumAbs() const
{
	double sum = 0;
	for (unsigned int i = 0; i < height; ++i)
	{
		for (unsigned int j = 0; j < width; ++j)
		{
			sum += abs(get_elem(i, j));
		}
	}
	return sum;
}

