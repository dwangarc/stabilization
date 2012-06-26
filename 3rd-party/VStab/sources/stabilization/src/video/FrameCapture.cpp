/*
 * FrameCapture.cpp
 *
 *  Created on: 25.04.2012
 *      Author: Poruchik-bI
 */

#include <stabilization/video/FrameCapture.h>

namespace VideoLib{

FrameCapture::FrameCapture(int &device)
:m_type(CAMERA)
,m_capture(device)
{
}
FrameCapture::FrameCapture(std::string const& file_path)
:m_type(FILE)
,m_capture(file_path)
{
}
FrameCapture::FrameCapture()
:m_type(RECIEVE_BUFFER)
{
}
FrameCapture::~FrameCapture() {
	m_capture.release();
}

bool FrameCapture::grab(){
	if(m_type == FILE || m_type == CAMERA){
		return m_capture.grab();
	}else{
		/* TODO Check recieve buffer*/
		return false;
	}
}
void FrameCapture::read(cv::Mat& mat){
	if(m_type == FILE || m_type == CAMERA){
		m_capture.read(mat);
	}else{
		/* TODO Check recieve buffer*/
	}
}
void FrameCapture::read(Frame& frame){
	if(m_type == FILE || m_type == CAMERA){
		m_capture.read(frame.mat());
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
