/*
 * Lines.cpp
 *
 *  Created on: 21.05.2012
 *      Author: Poruchik-bI
 */

#include <stabilization/common/Lines.h>

namespace VideoLib {
namespace Common {

bool Lines::findIntersec(cv::Point2d const&A,
						 cv::Point2d const&B,
						 cv::Point2d const&C,
						 cv::Point2d const&D,
						 cv::Point2d &P){

	double num = (A.y-C.y)(D.x-C.x)-(A.x-C.x)(D.y-C.y);
	double den = (B.x-A.x)(D.y-C.y)-(B.y-A.y)(D.x-C.x);

	if(num == 0 || den == 0){
		return false;
	}
	double r = num/den;
	if(r < 0 || r > 1){
		return false;
	}
	P =	cv::Point2d(A.x+r(B.x-A.x),
					A.y+r(B.y-A.y));
	return true;
}

cv::Point2d Lines::unitVec(cv::Point2d const&A,
						   cv::Point2d const&B)
{
	cv::Point2d u_vec;
	if(A.x == B.x){
		if(B.y - A.y > 0){
			u_vec = cv::Point2d(0, 1);
		}else{
			u_vec = cv::Point2d(0,-1);
		}
		return u_vec;
	}

	if(A.y == B.y){
		if(B.x - A.x > 0){
			u_vec = cv::Point2d(1, 0);
		}else{
			u_vec = cv::Point2d(-1,0);
		}
		return u_vec;
	}

	double num = (B.y - A.y);
	double den = (B.x - A.x);

	double k = num/den;
	if( abs(k) > 1){
		u_vec = cv::Point2d(-1*(k) ,1);
	}else{
		u_vec = cv::Point2d(-1 ,1*(1/k));
	}

	if((k >0 && num < 0) || (k < 0 && num > 0)){
		u_vec.x *=-1;
		u_vec.y *=-1;
	}
	return u_vec;
}

} /* namespace Common */
} /* namespace VideoLib */
