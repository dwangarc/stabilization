/*
 * Builder.cpp
 *
 *  Created on: 28.04.2012
 *      Author: Poruchik-bI
 */

#include <stabilization/gui/Builder.h>

namespace GUI {

std::map<DisplayFlag,std::string> Builder::wnd_aliases;
std::set<DisplayFlag> Builder::created_wnds;

void Builder::init(){
	wnd_aliases[DisplayFlags::ORIGINAL_WND] = "Original video";
	wnd_aliases[DisplayFlags::GRAY_WND] = "Gray scale video";
	wnd_aliases[DisplayFlags::STABILIZED_WND] = "Stabilised video";

}

void Builder::createWnd(DisplayFlag const& ds_flag){
	if(created_wnds.find(ds_flag) != created_wnds.end()){
		return;
	}
	cv::namedWindow(getWndName(ds_flag));
	created_wnds.insert(ds_flag);
}
void Builder::destroyWnd(DisplayFlag const& ds_flag){
	if(created_wnds.find(ds_flag) == created_wnds.end()){
		return;
	}
	cv::destroyWindow(getWndName(ds_flag));
	created_wnds.erase(ds_flag);
}

void Builder::showWnds(VisualHolder const& vs_holder){


	VideoLib::FrameBufferPtr const& framebuf = vs_holder.getStabilizer()->getFrameBuf();
	VideoLib::Frame::Features::const_iterator it;

	for(VisualFlags::const_iterator it = vs_holder.getFlags().begin(); it != vs_holder.getFlags().end(); it++){
		if(it->second != ShowFlags::NONE){
			cv::Mat img;

			if(it->second & ShowFlags::FRAME) showFrame(it->first,vs_holder.getStabilizer(),img);

			if(it->second & ShowFlags::PREV_FEATURES) showFeatures(framebuf->getDelayed(2)->features(),img);
			if(it->second & ShowFlags::CURR_FEATURES) showFeatures(framebuf->getCurrent()->features(),img);
			if(it->second & ShowFlags::OPTICAL_FLOW) showOpticalFlow(framebuf,img);
			//if(it->second & ShowFlags::PROJ_TRANSFORM) showTransform(it->first,vs_holder,img);
			//if(it->second & ShowFlags::BUFFER_MAP) showBufferMap(it->first,vs_holder,img);

		}
	}
}

void Builder::showFrame(DisplayFlag const& ds_flag, StabilizerPtr const& stabilizer, cv::Mat & img){

	if(ds_flag == DisplayFlags::ORIGINAL_WND){

		stabilizer->getFrameBuf()->getNext()->mat().copySize(img);
		stabilizer->getFrameBuf()->getNext()->mat().copyTo(img);

	}else if(ds_flag == DisplayFlags::GRAY_WND){

		stabilizer->getFrameBuf()->getNext()->matGray().copySize(img);
		stabilizer->getFrameBuf()->getNext()->matGray().copyTo(img);

	} else if(ds_flag == DisplayFlags::STABILIZED_WND){

		stabilizer->getStabFrameBuf()->getNext()->mat().copySize(img);
		stabilizer->getStabFrameBuf()->getNext()->mat().copyTo(img);

	}

}

void Builder::showFeatures(Frame::Features const& features, cv::Mat & img){
	VideoLib::Frame::Features::const_iterator it;
	cv::Scalar color = CV_RGB(255,rand()%255,rand()%255);

	for(it = features.begin();it != features.end();it++){
		cv::circle(img,*it,1, color,1);
	}

}

void Builder::showOpticalFlow(FrameBufferPtr const& framebuf, cv::Mat & img){

}
//void Builder::showTransform(DisplayFlag const& ds_flag, VisualHolder const& vs_holder, cv::Mat & img){
//
//}
//void Builder::showBufferMap(DisplayFlag const& ds_flag, VisualHolder const& vs_holder, cv::Mat & img){
//
//}

} /* namespace GUI */
