/*
 * Frame.h
 *
 *  Created on: 25.04.2012
 *      Author: Poruchik-bI
 */

#pragma once

#include <cv.h>
#include <boost/shared_ptr.hpp>

#include <vector>

namespace VideoLib{

typedef cv::Vector<cv::Point2d> HomoVec;

#define E cv::Mat(cv::Matx33d(1,   0,   0, \
							  0,   1,   0, \
							  0,   0,   1))

} /*VideoLib*/
