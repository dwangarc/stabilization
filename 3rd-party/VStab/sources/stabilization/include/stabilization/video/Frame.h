/*
 * Frame.h
 *
 *  Created on: 25.04.2012
 *      Author: Poruchik-bI
 */

#pragma once

#include <cv.h>
#include <boost/shared_ptr.hpp>

namespace VideoLib{

class Frame {


public:

	typedef std::vector<cv::Point2f> Features;

	Frame();
	Frame(cv::Mat const&src,bool fill_gray=true);

	virtual ~Frame();

	void clear();

	inline cv::Mat& mat(){ return m_mat; }
	inline cv::Mat const& mat()const{ return m_mat; }

	inline cv::Mat& matGray(){ return m_mat_gray; }
	inline cv::Mat const& matGray()const{ return m_mat_gray; }

	inline Features const& features()const{ return m_features; }

	void operator=(Frame const& frame);
	void operator=(cv::Mat const& mat);


	inline bool isInit(){ return is_init;}
	inline void setInit(bool const& flag){is_init = flag;}
private:
	bool is_init;

	cv::Mat m_mat;
	cv::Mat m_mat_gray;

	Features m_features;
};

typedef boost::shared_ptr<Frame> FramePtr;

} /*VideoLib*/
