/*
 * FrameBuffer.cpp
 *
 *  Created on: 25.04.2012
 *      Author: Poruchik-bI
 */

#include <stabilization/video/FrameBuffer.h>

namespace VideoLib{

FrameBuffer::FrameBuffer(size_t const& capacity)
:m_capacity(capacity)
,m_current(0)
{
	m_buffer.reserve(m_capacity);
}

FrameBuffer::~FrameBuffer() {
	clear();
}

FramePtr FrameBuffer::getCurrent(){
	return m_buffer[m_current];
}
FramePtr FrameBuffer::getNext(){
	setNext();
	return m_buffer[m_current];
}
FramePtr FrameBuffer::getLast(){
	return *(m_buffer.rbegin());
}
FramePtr FrameBuffer::getDelayed(size_t const& delay, bool from_last){
	int ind = 0;
	if(from_last){
		ind = m_buffer.size() - 1 - delay;
	}else{
		ind = (m_current - delay);
	}

	if(ind < 0){
		return m_buffer[0];
	}
	return m_buffer[ind];
}
void FrameBuffer::setNext(){
	m_current = (m_current < m_buffer.size() - 1 ? m_current + 1 : m_buffer.size() - 1 );
}
size_t FrameBuffer::size()const{
	return m_buffer.size();
}
bool FrameBuffer::empty(){
	return m_buffer.empty();
}
void FrameBuffer::clear(){
	m_buffer.clear();
	m_current = 0;
}

void FrameBuffer::push(FramePtr const& frame){
	m_buffer.push_back(frame);

	if(m_buffer.size() > m_capacity){
		pop();
	}
}
FramePtr FrameBuffer::pop(){
	FramePtr front = m_buffer.front();
	m_buffer.erase(m_buffer.begin());

	m_current = (m_current > 0 ? m_current - 1 : 0);

	return front;
}
FrameBuffer& FrameBuffer::operator << (FramePtr const& frame){
	push(frame);
	return *this;
}

} /*VideoLib*/
