/*
 * FrameCapture.cpp
 *
 *  Created on: 25.04.2012
 *      Author: Poruchik-bI
 */

#include <stabilization/video/FrameCapture.h>

namespace VideoLib{

FrameCapture::FrameCapture(int const& device)
:m_type(CAMERA)
//,m_capture(device)
{
	time = GetTickCount();

	grab_interval = 40;
	m_cap = cvCaptureFromCAM(0);
}
FrameCapture::FrameCapture(std::string const& file_path)
:m_type(FILE)
//,m_capture(file_path)
{
	time = GetTickCount();

	grab_interval = 0;//50;
	m_cap = cvCreateFileCapture(file_path.c_str());
}
FrameCapture::FrameCapture()
:m_type(RECIEVE_BUFFER)
{
}
FrameCapture::~FrameCapture() {
	cvReleaseCapture(&m_cap);
	//m_capture.release();
}

bool FrameCapture::grab(){
	if(m_type == FILE || m_type == CAMERA){
		//bool opn = 		m_capture.isOpened();
		return cvGrabFrame(m_cap);
		return m_capture.grab();
	}else{
		/* TODO Check recieve buffer*/
		return false;
	}
}
void FrameCapture::read(cv::Mat& mat){
	if(m_type == FILE || m_type == CAMERA){
		//m_capture.read(mat);
	}else{
		/* TODO Check recieve buffer*/
	}
}
void FrameCapture::read(Frame& frame){
	if(m_type == FILE || m_type == CAMERA){
		int64_t delay = GetTickCount()-time;
		if(delay < grab_interval) {
			//std::cout << "Delay = " <<grab_interval - delay << std::endl;
			Sleep(grab_interval - delay);
		}
		time = GetTickCount();
		frame.mat() = cv::Mat(cvQueryFrame(m_cap));
		//m_capture.read(frame.mat());
	}else{
		/* TODO Check recieve buffer*/
	}
}
void FrameCapture::setProperty(int propety, double val){
	if(m_type == FILE || m_type == CAMERA){
		m_capture.set(propety,val);
	}else{
		/* TODO */
	}
}

} /*VideoLib*/
