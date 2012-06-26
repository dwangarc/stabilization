/*
 * Lines.h
 *
 *  Created on: 21.05.2012
 *      Author: Poruchik-bI
 */

#pragma once

namespace VideoLib {
namespace Common {

class Lines {
public:
	static bool findIntersec(cv::Point2d const&A,
							 cv::Point2d const&B,
							 cv::Point2d const&C,
							 cv::Point2d const&D,
							 cv::Point2d &P);
	static cv::Point2d unitVec(cv::Point2d const&A,
							   cv::Point2d const&B);

};

} /* namespace Common */
} /* namespace VideoLib */
