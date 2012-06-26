/*
 * Frame.h
 *
 *  Created on: 25.04.2012
 *      Author: Poruchik-bI
 */

#pragma once

#include <stabilization/video/Types.h>

#include <cv.h>
#include <boost/shared_ptr.hpp>

#include <vector>

namespace VideoLib{

class Frame {


public:

	typedef cv::Mat Features;
	typedef std::vector<cv::Point2f> FeaturesLK;

	Frame();
	Frame(cv::Mat const&src,bool fill_gray=true);

	virtual ~Frame();

	void clear();

	inline cv::Mat& mat(){ return m_mat; }
	inline cv::Mat const& mat()const{ return m_mat; }

	inline cv::Mat& matGray(){ return m_mat_gray; }
	inline cv::Mat const& matGray()const{ return m_mat_gray; }

	inline Features & features(){ return m_features; }
	inline Features const& features()const{ return m_features; }

	inline FeaturesLK & featuresLK(){ return m_featuresLK; }
	inline FeaturesLK const& featuresLK()const{ return m_featuresLK; }

	inline cv::Mat& transformToPrev(){ return m_transform_prev; }
	inline void setTransformToPrev(cv::Mat& transform){ transform.copyTo(m_transform_prev); }

	inline cv::Mat& transformSum(){ return m_transform_sum; }
	inline void setTransformSum(cv::Mat& transform){ transform.copyTo(m_transform_sum); }

	inline cv::Mat& transformToOrig(){ return m_transform_orig; }
	inline void setTransformToOrig(cv::Mat & transform){ m_transform_orig = transform; }

	void operator=(Frame const& frame);
	void operator=(cv::Mat const& mat);


	inline bool isInit(){ return is_init;}
	inline void setInit(bool const& flag){is_init = flag;}

	inline bool isEnjection(){ return is_enjection;}
	inline void setEnjection(bool const& flag){is_enjection = flag;}

	inline int numOfStatic(){return num_of_static;}
	inline void setNumOfStatic(int const& num){num_of_static = num;}

private:
	bool is_init;
	bool is_enjection;

	cv::Mat m_mat;
	cv::Mat m_mat_gray;

	cv::Mat m_transform_prev;
	cv::Mat m_transform_sum;
	cv::Mat  m_transform_orig; /// transform to original( use for stab frames)

	Features m_features;
	FeaturesLK m_featuresLK;

	int num_of_static;
};

typedef boost::shared_ptr<Frame> FramePtr;

} /*VideoLib*/
