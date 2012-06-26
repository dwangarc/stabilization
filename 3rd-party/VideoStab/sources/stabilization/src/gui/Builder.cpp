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

const size_t Builder::DEFAULT_DELAY = 2;

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

	for(VisualFlags::const_iterator it = vs_holder.getFlags().begin(); it != vs_holder.getFlags().end(); it++){
		if(it->second != ShowFlags::NONE){
			cv::Mat img;

			if(it->second & ShowFlags::FRAME) showFrame(it->first,vs_holder.getStabilizer(),img);

			if(it->second & ShowFlags::PREV_FEATURES) showFeatures(framebuf->getDelayed(DEFAULT_DELAY)->features(),CV_RGB(255,0,0),img,1);//img.cols/320);
			if(it->second & ShowFlags::CURR_FEATURES) showFeatures(framebuf->getCurrent()->features(),CV_RGB(0,300,0),img,1);//img.cols/320);
			if(it->second & ShowFlags::OPTICAL_FLOW) showOpticalFlow(framebuf->getCurrent(),CV_RGB(0,300,0),img);
			//if(it->second & ShowFlags::PROJ_TRANSFORM) showTransform(it->first,vs_holder,img);
			//if(it->second & ShowFlags::BUFFER_MAP) showBufferMap(it->first,vs_holder,img);

			if(created_wnds.find(it->first) == created_wnds.end()){
				createWnd(it->first);
			}
			//cv::imshow(getWndName(it->first),framebuf->getCurrent()->matGray());
			cv::imshow(getWndName(it->first),img);
		}
	}
}

void Builder::showFrame(DisplayFlag const& ds_flag, StabilizerPtr const& stabilizer, cv::Mat & img){

	FramePtr frame;
	if(ds_flag == DisplayFlags::ORIGINAL_WND){
		frame = stabilizer->getFrameBuf()->getNext();

		//frame->mat().copySize(img);
		frame->mat().copyTo(img);

	}else if(ds_flag == DisplayFlags::GRAY_WND){
		frame = stabilizer->getFrameBuf()->getNext();

		//frame->matGray().copySize(img);
		frame->matGray().copyTo(img);
	} else if(ds_flag == DisplayFlags::STABILIZED_WND){
		frame = stabilizer->getStabFrameBuf()->getNext();

		//frame->mat().copySize(img);
		frame->mat().copyTo(img);
	}
}

void Builder::showFeatures(Frame::Features const& features, cv::Scalar const& color, cv::Mat & img, double scale_factor){
//	VideoLib::Frame::Features::const_iterator it;
//	cv::Point2f pt;
//
//
//	for(it = features.begin();it != features.end();it++){
//		if(scale_factor != 1.0){
//			cv::circle(img,cv::Point2f(it->x * scale_factor,it->y * scale_factor),2, color,2);
//		}else{
//			cv::circle(img,(*it),2, color,2);
//
//		}
//	}

}

void Builder::showOpticalFlow(FramePtr const& frame, cv::Scalar const& color, cv::Mat & img){
	cv::Point2f pt;
	VideoLib::Frame::Features& features = frame->features();
	for(int i = 0; i < img.rows; i+=10){
		for(int j = 0; j<img.cols; j+=10){
			pt = features.at<cv::Point2f>(i,j);
			if(pt.x != 0 && pt.y !=0){
				cv::line(img,cv::Point2f(i,j),cv::Point2f(i+pt.x,j+pt.y), color,1);
				//cv::circle(img, cv::Point(i,j), 2, color, -1);
			}
		}
	}

}
//void Builder::showTransform(DisplayFlag const& ds_flag, VisualHolder const& vs_holder, cv::Mat & img){
//
//}
//void Builder::showBufferMap(DisplayFlag const& ds_flag, VisualHolder const& vs_holder, cv::Mat & img){
//
//}

} /* namespace GUI */
