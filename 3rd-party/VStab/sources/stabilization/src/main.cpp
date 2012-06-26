#include <stabilization/gui/VisualHolder.h>
#include <stabilization/gui/Builder.h>

//#include <stabilization/Stabilizer.h>
//#include <stabilization/video/FrameBuffer.h>


#include <stdio.h>// includes C standard input/output definitions
#include <iostream>

#include <stdio.h>
#include <stdlib.h>

const size_t BUFFER_SIZE = 20;

int main( int argc, char** argv ) {
	setlocale(LC_ALL, "ru_RU.UTF-8");

	VideoLib::FrameCapturePtr capture(new VideoLib::FrameCapture(".\\vid2.avi"));
	VideoLib::StabilizerPtr stabilizer(new VideoLib::Stabilizer(BUFFER_SIZE,cv::Size(320,240),cv::Size(320,240)));

	stabilizer->setCapture(capture);

	GUI::VisualHolder vs_holder(stabilizer);

	if(!capture->grab()){
		std::cout << "Can't open video file";
		cv::waitKey();
		return 0;
	}

	for(;;){
		GUI::Builder::showWnds(vs_holder);
		if(cv::waitKey(30) >= 0) break;
	}

	return 0;
}
