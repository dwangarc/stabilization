/*
 * Stabilizer.cpp
 *
 *  Created on: 25.04.2012
 *      Author: Poruchik-bI
 */

#include <stabilization/Stabilizer.h>

namespace VideoLib{


Stabilizer::Stabilizer(size_t const& buf_capacity)
:m_enable(false)
,m_buf_capacity(buf_capacity)
,m_orig_im_size(0,0)
,m_gray_im_size(0,0)
,m_framebuf(new FrameBuffer(buf_capacity))
,m_stabilized_framebuf(new FrameBuffer(buf_capacity))
{
}
Stabilizer::Stabilizer(size_t const& buf_capacity, cv::Size const& orig_im_size, cv::Size const& gray_im_size)
:m_enable(true)
,m_buf_capacity(buf_capacity)
,m_orig_im_size(orig_im_size)
,m_gray_im_size(gray_im_size)
,m_framebuf(new FrameBuffer(buf_capacity))
,m_stabilized_framebuf(new FrameBuffer(buf_capacity))
{

}

Stabilizer::~Stabilizer() {

}

bool Stabilizer::captureFrame(){
	FramePtr frame(new Frame());

	frame->mat().create(m_orig_im_size,CV_8UC3);
	frame->matGray().create(m_gray_im_size,CV_8UC1);

	m_capture->read(*frame);

	cv::cvtColor(frame->mat(),frame->matGray(), CV_RGB2GRAY,1);

	m_framebuf->push(frame);
	m_stabilized_framebuf->push(frame);
	return true;
}
void Stabilizer::stabilize(){

}
FramePtr Stabilizer::getNextFrame(){
	m_framebuf->setNext();
	return m_stabilized_framebuf->getNext();
}
void Stabilizer::findFeatures(){

}
void Stabilizer::findTransform(){

}

/* Произвести сглаживание
 * */
void Stabilizer::smooth(){

}
void Stabilizer::generateSubFrame(){

}


} /*VideoLib*/
