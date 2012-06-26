/*
 * FrameCapture.h
 *
 *  Created on: 25.04.2012
 *      Author: Poruchik-bI
 */

#pragma once

#include <stabilization/video/Frame.h>
#include <opencv2/opencv.hpp>
#include <boost/shared_ptr.hpp>

namespace VideoLib{

class FrameCapture {
public:

	enum Type{
		CAMERA,
		FILE,
		RECIEVE_BUFFER,
	};
	///Camera
	FrameCapture(int const& device);
	///File
	FrameCapture(std::string const& file_path);
	///recv buffer
	///TODO
	FrameCapture(/*some params*/);
	virtual ~FrameCapture();

	inline Type type(){return m_type;}
	inline cv::Size2i const& getSize(){return m_size;}
	inline void setSize(cv::Size2i const& size){m_size = size;
												setProperty(CV_CAP_PROP_FRAME_WIDTH,m_size.width);
												setProperty(CV_CAP_PROP_FRAME_HEIGHT,m_size.height);}
	bool grab();
	void read(cv::Mat& mat);
	void read(Frame& frame);
	void setProperty(int propety, double val);


private:
	Type m_type;
	int64_t time;
	int64_t grab_interval;

	cv::VideoCapture m_capture;
	CvCapture * m_cap;

	cv::Size2i m_size;
};

typedef boost::shared_ptr<FrameCapture> FrameCapturePtr;

} /*VideoLib*/
