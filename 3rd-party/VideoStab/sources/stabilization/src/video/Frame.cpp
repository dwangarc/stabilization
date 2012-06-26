/*
 * Frame.cpp
 *
 *  Created on: 25.04.2012
 *      Author: Poruchik-bI
 */
#include <stabilization/video/Frame.h>

namespace VideoLib{

Frame::Frame()
:is_init(false)
,is_enjection(false)
,m_transform_prev(E)
,m_transform_sum(E)
,m_transform_orig(E)
,num_of_static(0)
{
}

Frame::Frame(cv::Mat const&src,bool fill_gray)
:is_init(true)
,is_enjection(false)
,m_mat(src)
,m_transform_prev(E)
,m_transform_sum(E)
,m_transform_orig(E)
,num_of_static(0)
{
	m_features.push_back(cv::Point2f(0,0));
	if(fill_gray){
		m_mat.convertTo(m_mat_gray,CV_8UC1);
	}
}
Frame::~Frame() {
	clear();
}

void Frame::clear(){
	is_init = false;
	m_features.release();
	m_mat.release();
	m_mat_gray.release();

	num_of_static= 0;
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
