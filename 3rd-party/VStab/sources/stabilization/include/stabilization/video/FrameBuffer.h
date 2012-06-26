/*
 * FrameBuffer.h
 *
 *  Created on: 25.04.2012
 *      Author: Poruchik-bI
 */

#pragma once

#include <stabilization/video/Frame.h>

namespace VideoLib{

class FrameBuffer {
public:
	FrameBuffer(size_t const& capacity);
	virtual ~FrameBuffer();

	FramePtr getCurrent();
	FramePtr getNext();
	FramePtr getDelayed(size_t const& delay);
	size_t size()const;

	void setNext();

	bool empty();
	void clear();

	FrameBuffer& operator << (FramePtr const& frame);

	///Если размер буфера больше @m_capacity, то первый элемент вытесняется
	void push(FramePtr const& frame);
	FramePtr pop();

private:
	size_t m_capacity;

	int m_current;
	std::vector<FramePtr> m_buffer;

};
typedef boost::shared_ptr<FrameBuffer> FrameBufferPtr;

} /*VideoLib*/
