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

	static const size_t DEFAULT_DELAY = 2;
public:

	static void init();

	static void createWnd(DisplayFlag const& ds_flag);
	static void destroyWnd(DisplayFlag const& ds_flag);

	static void showWnds(VisualHolder const& vs_holder);

	inline static std::string const& getWndName(DisplayFlag const& flag){return wnd_aliases.at(flag);}
private:
	static void showFrame(DisplayFlag const& ds_flag, StabilizerPtr const& stabilizer, cv::Mat & img);
	static void showFeatures(Frame::Features const& features, cv::Mat & img);
	static void showOpticalFlow(FrameBufferPtr const& framebuf, cv::Mat & img);
	//static void showTransform(DisplayFlag const& ds_flag, VisualHolder const& vs_holder, cv::Mat & img);
	//static void showBufferMap(DisplayFlag const& ds_flag, VisualHolder const& vs_holder, cv::Mat & img);


	static std::map<DisplayFlag,std::string> wnd_aliases;
	static std::set<DisplayFlag> created_wnds;

};

} /* namespace GUI */
