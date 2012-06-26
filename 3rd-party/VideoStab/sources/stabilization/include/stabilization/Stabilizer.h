/*
 * Stabilizer.h
 *
 *  Created on: 25.04.2012
 *      Author: Poruchik-bI
 */

#pragma once

#include <stabilization/video/FrameBuffer.h>
#include <stabilization/video/FrameCapture.h>
#include <stabilization/video/Types.h>
#include <stabilization/common/Area.h>
#include <cmath>
#include <iostream>

namespace VideoLib{

using namespace Common;
using namespace cv;

class Stabilizer {
public:

	enum StabModel{
		OPTIMISTIC_MODEL,
		PESSEMISTIC_MODEL,
	};

	enum StabFunc{
		LINEAR 			= 1,
		POLYNOMIAL_2	= 2,
		POLYNOMIAL_4 	= 4,
		EXP				= 8,
	};

	static const size_t DEFAULT_CONERS_COUNT;
	static const cv::Size DEFAULT_WND_SIZE;

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
	void findFeatures(FramePtr const& frame);
	cv::Mat findTransform(cv::Mat const& fr,cv::Mat const& features, int const& step = 1);
	cv::Mat findTransformLK(Frame::FeaturesLK & ft1,Frame::FeaturesLK & ft2, std::vector<uchar> & status);

	int transformEstimation(FramePtr const& last_orig,
							FramePtr const& prev_orig,
							FramePtr const& last_stab,
							FramePtr const& prev_stab,
							cv::Mat & last_transform,
							cv::Mat & opt_transform);
	void fillEdges(FramePtr const& last_orig,
					  FramePtr const& prev_orig,
					  FramePtr const& last_stab,
					  FramePtr const& prev_stab);
	void findVoidAreas(cv::Mat & transform);

	void createArea(cv::Mat & transform);

	double calcDistanseTo(cv::Mat const& mat, cv::Mat const& src_mat);
	bool checkEjection(cv::Mat const& mat);
//	bool checkDistanse(cv::Mat const& mat1, cv::Mat const& mat1);

	/* Произвести сглаживание
	 * */
	void smooth();
	void generateSubFrame();

	void saveHistory();

	bool m_enable;
	size_t m_buf_capacity;
	cv::Size m_orig_im_size;
	cv::Size m_gray_im_size;

	FrameBufferPtr m_framebuf;
	FrameBufferPtr m_stabilized_framebuf;

	FrameCapturePtr m_capture;

	std::vector<cv::Mat> m_transform_history;

	double m_opt_dist; ///коэффициент оптимального расстояния меджду фреймами
	double m_min_dist; ///коэффициент минимального расстояния меджду фреймами

	StabModel stab_model;
	StabFunc stab_func;

	cv::Mat m_max_normal_transform; ///допустимые значения элементов матрицы преобразования, при которых стабилизация не требуется
	cv::Mat m_max_transform;
	cv::Mat m_min_transform;
	cv::Mat e;

	HomoVec support_homo_vec;

	int homo_count;
	int homo_ejections;

	double optimistic_k;

};

typedef boost::shared_ptr<Stabilizer> StabilizerPtr;

} /* VideoLib */
