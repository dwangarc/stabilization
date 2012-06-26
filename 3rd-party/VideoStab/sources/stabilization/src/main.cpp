#include <stabilization/gui/VisualHolder.h>
#include <stabilization/gui/Builder.h>


#include <iostream>
#include <conio.h>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <stdlib.h>

const size_t BUFFER_SIZE = 10;

int main( int argc, char** argv ) {
	setlocale(LC_ALL, "ru_RU.UTF-8");

	VideoLib::FrameCapturePtr capture(new VideoLib::FrameCapture("C:\\Users\\Poruchik-bI\\workspace\\VideoStab\\bin\\vid1.avi"));
	//VideoLib::FrameCapturePtr capture(new VideoLib::FrameCapture(0));
	//VideoLib::StabilizerPtr stabilizer(new VideoLib::Stabilizer(BUFFER_SIZE,cv::Size(640,480),cv::Size(320,240)));
	VideoLib::StabilizerPtr stabilizer(new VideoLib::Stabilizer(BUFFER_SIZE,cv::Size(320,240),cv::Size(320,240)));
	//VideoLib::StabilizerPtr stabilizer(new VideoLib::Stabilizer(BUFFER_SIZE,cv::Size(160,120),cv::Size(160,120)));

	stabilizer->setCapture(capture);

	GUI::VisualHolder vs_holder(stabilizer);

	bool scroling = false;

	if(!capture->grab()){
		std::cout << "Can't open video file";
		cv::waitKey();
		return 0;
	}
	GUI::Builder::init();
	bool success_capture = false;
	int key = 0;
	success_capture = stabilizer->captureFrame();
	stabilizer->stabilize();
	GUI::Builder::showWnds(vs_holder);

	for(;;){
		key = cv::waitKey(1);
		if(scroling){
			if(key == 13) {
				success_capture = stabilizer->captureFrame();
				stabilizer->stabilize();
				GUI::Builder::showWnds(vs_holder);
			}
		}else{
			success_capture = stabilizer->captureFrame();
			stabilizer->stabilize();
			GUI::Builder::showWnds(vs_holder);
		}

		if(key == 32){
			break;
		}

		//key = getchar();
		if(key == 49){
			int i = (int)scroling;
			scroling = (i + 1) % 2;
		}
		vs_holder.processKey(key);
	}

	return 0;
}


//int main( int argc, char** argv ) {
//
//	const int MAX_CORNERS = 100;
//
//	setlocale(LC_ALL, "ru_RU.UTF-8");
//
//
//
//	 CvCapture *cap = cvCaptureFromCAM(CV_CAP_ANY); // open the default camera
//
//	    int n_features = 20;
//	    std::vector<Point> features;
//	    std::vector<Point> next_features;
//	    std::vector<int> status_features;
//	    std::vector<Point> err_features;
//	    CvSize frame_sz = cvSize(cvGetCaptureProperty(cap, CV_CAP_PROP_FRAME_WIDTH),cvGetCaptureProperty(cap, CV_CAP_PROP_FRAME_HEIGHT));
//
//
//
//
//	    unsigned int  buf_size = 20;
//	    std::vector<Mat> frameBuf;
//	    frameBuf.clear();
//	   // frameBuf.reserve(frame_sz.height*frame_sz.width*buf_size);
//
//	    CvScalar line_color;
//	    line_color = CV_RGB(255, 0, 0);
//	    Mat mask(frame_sz,CV_8UC1 );
//
//	    namedWindow("Origin",CV_WINDOW_AUTOSIZE);
//	    cvNamedWindow ("Horizontal", CV_WINDOW_AUTOSIZE);
//	    cvNamedWindow ("Vertical", CV_WINDOW_AUTOSIZE);
//
//	    for(;;)
//	   	    {
//
//	    //////////////////////////////////////////////////////////////////////////////////////////////////
//	    // Load two images and allocate other structures
//
//	    	IplImage* imgOrigA = cvQueryFrame(cap);
//			IplImage* imgOrigB = cvQueryFrame(cap);
//
//	    	CvSize img_sz = cvGetSize( imgOrigA );
//
//	    	IplImage* imgA = cvCreateImage( img_sz, IPL_DEPTH_8U, 1 );
//	    	IplImage* imgB = cvCreateImage( img_sz, IPL_DEPTH_8U, 1 );
//
//	    	cvCvtColor(imgOrigA,imgA, CV_BGR2GRAY);
//	    	cvCvtColor(imgOrigB,imgB, CV_BGR2GRAY);
//
//	    	int win_size = 15;
//
//	    	// Get the features for tracking
//	    	IplImage* eig_image = cvCreateImage( img_sz, IPL_DEPTH_32F, 1 );
//	    	IplImage* tmp_image = cvCreateImage( img_sz, IPL_DEPTH_32F, 1 );
//
//	    	int corner_count = MAX_CORNERS;
//	    	CvPoint2D32f* cornersA = new CvPoint2D32f[ MAX_CORNERS ];
//
//	    	cvGoodFeaturesToTrack( imgA, eig_image, tmp_image, cornersA, &corner_count,
//	    		0.05, 5.0, 0, 3, 0, 0.04 );
//
//	    	cvFindCornerSubPix( imgA, cornersA, corner_count, cvSize( win_size, win_size ),
//	    		cvSize( -1, -1 ), cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03 ) );
//
//	    	// Call Lucas Kanade algorithm
//	    	char features_found[ MAX_CORNERS ];
//	    	float feature_errors[ MAX_CORNERS ];
//
//	    	CvSize pyr_sz = cvSize( imgA->width+8, imgB->height/3 );
//
//	    	IplImage* pyrA = cvCreateImage( pyr_sz, IPL_DEPTH_32F, 1 );
//	    	IplImage* pyrB = cvCreateImage( pyr_sz, IPL_DEPTH_32F, 1 );
//
//	    	CvPoint2D32f* cornersB = new CvPoint2D32f[ MAX_CORNERS ];
//
//	    	cvCalcOpticalFlowPyrLK( imgA, imgB, pyrA, pyrB, cornersA, cornersB, corner_count,
//	    		cvSize( win_size, win_size ), 5, features_found, feature_errors,
//	    		 cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.3 ), 0 );
//
//	    	// Make an image of the results
//
//	    	for( int i=0; i < MAX_CORNERS; i++){
//	    		if(!feature_errors[i])
//	    		{
//	    			printf("Error is %f/n", feature_errors[i]);
//	    			continue;
//	    		}
//
//	    		CvPoint p0 = cvPoint( cvRound( cornersA[i].x ), cvRound( cornersA[i].y ) );
//	    		CvPoint p1 = cvPoint( cvRound( cornersB[i].x ), cvRound( cornersB[i].y ) );
//	    		cvLine( imgOrigB, p0, p1, CV_RGB(255,0,0), 2 );
//	    	}
//
//	    	CvSize block_size = cvSize(16,16);
//	    	CvSize shift_size = cvSize(1,1);
//
//	    	//CvSize im_sz1 = cvSize(img_sz.width/16,img_sz.height/16);
//	    	CvSize im_sz1 = cvSize((img_sz.width - block_size.width)/shift_size.width,
//	    						   (img_sz.height - block_size.height)/shift_size.height);
//
//	    	int bw_points_count = img_sz.height/block_size.height * img_sz.width/block_size.width;
//	    	IplImage* velx  = cvCreateImage(im_sz1, IPL_DEPTH_32F, 1);
//	    	IplImage* vely =  cvCreateImage(im_sz1, IPL_DEPTH_32F, 1);
//
//	    //	cvCalcOpticalFlowBM(imgA,imgB,block_size,shift_size,cvSize(3,2),0,velx,vely);
//
//
//
//	    	cvShowImage( "Origin", imgOrigB );
//	    	cvShowImage ("Horizontal", velx);
//	    	cvShowImage ("Vertical", vely);
//
//	    	if(cvWaitKey(30) >= 0) break;
//
//	    	//cvReleaseImage(&imgOrigA);
//	    //	cvReleaseImage(&imgOrigB);
//
//	    	//cvReleaseImage(&imgA);
//			//cvReleaseImage(&imgB);
//
//	    //////////////////////////////////////////////////////////////////////////////////////////////////
//
//   	    }
//	    cvDestroyAllWindows();
//	    cvReleaseCapture(&cap);
//	    	return 0;
//
//
//
//
//
//
//
//
////	    for(;;)
////	    {
////	    	Mat frame;
////			cap >> frame;
////			frameBuf.push_back(frame);
////
////			if(frameBuf.size() == 1)
////				frameBuf.push_back(frame);
////	    	if(frameBuf.size() > buf_size){
////	    		frameBuf.erase(frameBuf.begin());
////	    	}
//
////	    	 	goodFeaturesToTrack(prev_el,features,n_features,1,1,mask,3);
//
//
////	        calcOpticalFlowPyrLK(cur_el,
////	        					 prev_el,
////	        					 features,
////	        					 next_features,
////	        					 status_features,
////	        					 err_features);
//
////			cvCalcOpticalFlowBM(prev_el,cur_el,10,1,cvSize(5,5),4);
//
//
//
////			calcOpticalFlowFarneback(prev_el,cur_el,flow,0.5,1,3,1,5,1.1,OPTFLOW_FARNEBACK_GAUSSIAN );
//
//
////	        for(int i=0; i < n_features; i++){
////	        	if(status_features[i] == 1)
////	        		line(frameBuf[cur_elem],features[i],next_features[i],line_color,1,CV_AA,0);
////	        }
//
////	        imshow("Origin", frameBuf[cur_elem]);
////	        imshow("Optical Flow", flow);
////	        if(waitKey(30) >= 0) break;
////	    }
//
//
//	    return 0;
//	////////////////////////////////////////////////////////////////////////////////////////
/*
    IplImage* src;
    IplImage* imageInGray;
    IplImage* outra;
    IplImage* imgRoi;
    IplImage* sobelImg;
    IplImage* dst;

    char* srcImage = "C:\\Users\\Poruchik-bI\\workspace\\VideoStab\\sources\\stabilization\\src\\TestImage.bmp" ;

    // Loads source image from file
    src=cvLoadImage(srcImage, 1) ;
    if( src == 0 ) {
       cout << "Image cannot be loaded!" << endl ;
       getch();
       return -1;
    }

    // Show image properties
    cout << "Image height: " << src->height << " pixels" << endl ;
    cout << "Image width : " << src->width << " pixels" << endl ;

    //��������������������������������������������������������������������
    // Show original image
    //��������������������������������������������������������������������
    cvNamedWindow( "1.Original Image", 1 ) ;
    cvShowImage( "1.Original Image", src );

    //��������������������������������������������������������������������
    // Transform source image to gray level
    //��������������������������������������������������������������������
    imageInGray = cvCreateImage(cvGetSize(src),IPL_DEPTH_8U,1);
    cvCvtColor( src, imageInGray, CV_RGB2GRAY );
    src = imageInGray ;
    cvNamedWindow( "2.Gray Level", 1 ) ;
    cvShowImage( "2.Gray Level", src );

    //��������������������������������������������������������������������
    // Aply limiarization, with T = 1
    //��������������������������������������������������������������������
    //binarize( src );   //  32 seconds when executing 100.000 times
    //binarize2( src ) ; // 121 seconds when executing 100.000 times
    cvThreshold( src, src, 1, 255, CV_THRESH_BINARY );  // 6 seconds when executing 100.000 times
    cvNamedWindow( "3.Limiarization", 1 );
    cvShowImage( "3.Limiarization", src );

    //��������������������������������������������������������������������
    // Invert the gray level value of the pixels in a defined window
    //��������������������������������������������������������������������
    imgRoi = cvCloneImage(imageInGray) ;
    cvSetImageROI( imgRoi, cvRect( 50, 50, 100, 100 ));
    cvNot( imgRoi, imgRoi );
    cvResetImageROI(imgRoi);
    cvNamedWindow( "4.ROI", 1 );
    cvShowImage( "4.ROI", imgRoi );

    //��������������������������������������������������������������������
    // Aply sobel
    //��������������������������������������������������������������������
    outra = cvCreateImage( cvGetSize(src), IPL_DEPTH_16S, 1  );
    cvSobel( src, outra, 1, 1 );
    sobelImg = cvCreateImage( cvGetSize(src), IPL_DEPTH_8U, 1  );
    cvConvertScale( outra, sobelImg );
    cvNamedWindow( "5.Sobel", 1 );
    cvShowImage( "5.Sobel", sobelImg );

    //��������������������������������������������������������������������
    // Obtain the connected components
    //��������������������������������������������������������������������
    // Create the destination image, putting your pixels with zero value
    dst = cvCreateImage( cvGetSize(src), 8, 3 );
    cvZero( dst );

    // Get the connected components
    CvSeq* contour = 0;
    CvMemStorage* storage = cvCreateMemStorage(0);
    cvFindContours( src, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

    // Paint each connected component using a different color (random)
    srand( time(0) );
    for( ; contour != 0; contour = contour->h_next ) {
            CvScalar color = CV_RGB( rand()%256, rand()%256, rand()%256 );
            // replace CV_FILLED with 1 to see the outlines
            cvDrawContours( dst, contour, color, color, -1, CV_FILLED, 8 );
    }
    cvNamedWindow( "6.Connected Components", 1 );
    cvShowImage( "6.Connected Components", dst );

    cvWaitKey(0) ;

    //��������������������������������������������������������������������
    // Release memory
    //��������������������������������������������������������������������
    cvReleaseImage( &src );
    cvReleaseImage( &imageInGray );
    cvReleaseImage( &outra );
    cvReleaseImage( &sobelImg );
    cvReleaseImage( &dst );
    cvReleaseImage( &imgRoi );*/
//}


//int main(int argc, char** argv)
//{
//	setlocale(LC_ALL, "ru_RU.UTF-8");
//
//	VideoLib::Frame fr;
//
//	fr.foo();
//
////	ConfigData config;
////	if(!parseParams(argc,argv,config)){
////		return 0;
////	}
//
////	AppServer app(0,NULL);
//
////	app.init(config);
////	app.start();
////
//	std::cout << "Server closed." << std::endl;
//
//	return 0;
//}
