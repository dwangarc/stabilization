/*
 * Frame.cpp
 *
 *  Created on: 25.04.2012
 *      Author: Poruchik-bI
 */
#include <stabilization/video/Frame.h>

namespace VideoLib{

Frame::Frame()
:is_init(false) {
	// TODO Auto-generated constructor stub

}
Frame::Frame(cv::Mat const&src,bool fill_gray)
:is_init(true)
,m_mat(src)
{
	if(fill_gray){
		m_mat.convertTo(m_mat_gray,CV_8UC1);
	}
}
Frame::~Frame() {
	clear();
}

void Frame::clear(){
	is_init = false;

	m_mat.release();
	m_mat_gray.release();
}
void Frame::operator=(Frame const& frame){
	clear();
	m_mat = frame.mat().clone();
	m_mat_gray = frame.matGray().clone();
}
void Frame::operator=(cv::Mat const& mat){
	clear();
	m_mat = mat.clone();
}

}
