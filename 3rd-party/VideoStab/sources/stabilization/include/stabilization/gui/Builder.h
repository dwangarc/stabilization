/*
 * Builder.h
 *
 *  Created on: 28.04.2012
 *      Author: Poruchik-bI
 */

#pragma once
#include <stabilization/gui/VisualHolder.h>
#include <stabilization/Stabilizer.h>

#include <map>
#include <set>

using namespace VideoLib;


namespace GUI {

class Builder {

	static const size_t DEFAULT_DELAY ;
public:

	static void init();

	static void createWnd(DisplayFlag const& ds_flag);
	static void destroyWnd(DisplayFlag const& ds_flag);

	static void showWnds(VisualHolder const& vs_holder);

	inline static std::string const& getWndName(DisplayFlag const& flag){return wnd_aliases.at(flag);}
private:
	static void showFrame(DisplayFlag const& ds_flag, StabilizerPtr const& stabilizer, cv::Mat & img);
	static void showFeatures(Frame::Features const& features, cv::Scalar const& color, cv::Mat & img, double scale_factor = 1.0);
	static void showOpticalFlow(FramePtr const& frame, cv::Scalar const& color, cv::Mat & img);
	//static void showTransform(DisplayFlag const& ds_flag, VisualHolder const& vs_holder, cv::Mat & img);
	//static void showBufferMap(DisplayFlag const& ds_flag, VisualHolder const& vs_holder, cv::Mat & img);


//	void on_trackbar( int, void* )
//	{
//
//		/// Global Variables
//		const int alpha_slider_max = 100;
//		int alpha_slider;
//		double alpha;
//		double beta;
//
//
//		alpha = (double) alpha_slider/alpha_slider_max ;
//	 beta = ( 1.0 - alpha );
//
//	 cv::addWeighted( src1, alpha, src2, beta, 0.0, dst);
//
//	 imshow( "Linear Blend", dst );
//	}
//
//	static void buildStabWnd(){
//		alpha_slider = 0;
//
//		 /// Create Windows
//		 namedWindow("Linear Blend", 1);
//
//		 /// Create Trackbars
//		 char TrackbarName[50];
//		 sprintf( TrackbarName, "Alpha x %d", alpha_slider_max );
//
//		 cv::createTrackbar( TrackbarName, "Linear Blend", &alpha_slider, alpha_slider_max, on_trackbar );
//
//		 /// Show some stuff
//		 on_trackbar( alpha_slider, 0 );
//	}

	static std::map<DisplayFlag,std::string> wnd_aliases;
	static std::set<DisplayFlag> created_wnds;

};

} /* namespace GUI */
