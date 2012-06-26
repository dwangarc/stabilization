/*
 * Stabilizer.h
 *
 *  Created on: 25.04.2012
 *      Author: Poruchik-bI
 */

#pragma once

#include <stabilization/video/FrameBuffer.h>
#include <stabilization/video/FrameCapture.h>

namespace VideoLib{

class Stabilizer {
public:

	enum Params{
		FEATURES_STRENGTH,
		MAX_FEATURES,
		FILL_CONERS,

	};

	Stabilizer(size_t const& buf_capacity);
	Stabilizer(size_t const& buf_capacity, cv::Size const& orig_im_size, cv::Size const& gray_im_size);

	virtual ~Stabilizer();

	void init();

	inline FrameBufferPtr const& getFrameBuf()const{return m_framebuf;}
	inline FrameBufferPtr const& getStabFrameBuf()const{return m_stabilized_framebuf;}

	inline FrameCapturePtr const& getCapture()const {return m_capture;}
	inline void setCapture(FrameCapturePtr const& capture){m_capture = capture;}

	inline bool isEnable(){return m_enable; };
	inline void setEnable(bool flag){m_enable = flag; };

	bool captureFrame();
	void stabilize();

	FramePtr getNextFrame();


private:
	void findFeatures();
	void findTransform();

	/* Произвести сглаживание
	 * */
	void smooth();
	void generateSubFrame();


	bool m_enable;
	size_t m_buf_capacity;
	cv::Size m_orig_im_size;
	cv::Size m_gray_im_size;

	FrameBufferPtr m_framebuf;
	FrameBufferPtr m_stabilized_framebuf;

	FrameCapturePtr m_capture;

};

typedef boost::shared_ptr<Stabilizer> StabilizerPtr;

} /* VideoLib */
