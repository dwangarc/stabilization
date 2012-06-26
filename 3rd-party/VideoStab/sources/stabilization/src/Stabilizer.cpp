/*
 * Stabilizer.cpp
 *
 *  Created on: 25.04.2012
 *      Author: Poruchik-bI
 */

#include <stabilization/Stabilizer.h>

#include <iostream>
#include <ios>
#include <iomanip>
#include <fstream>

namespace VideoLib{

const size_t Stabilizer::DEFAULT_CONERS_COUNT  = 200;
const cv::Size Stabilizer::DEFAULT_WND_SIZE = cv::Size(5,5);

Stabilizer::Stabilizer(size_t const& buf_capacity)
:m_enable(false)
,m_buf_capacity(buf_capacity)
,m_orig_im_size(0,0)
,m_gray_im_size(0,0)
,m_framebuf(new FrameBuffer(buf_capacity))
,m_stabilized_framebuf(new FrameBuffer(buf_capacity))
{
	init();
}
Stabilizer::Stabilizer(size_t const& buf_capacity, cv::Size const& orig_im_size, cv::Size const& gray_im_size)
:m_enable(true)
,m_buf_capacity(buf_capacity)
,m_orig_im_size(orig_im_size)
,m_gray_im_size(gray_im_size)
,m_framebuf(new FrameBuffer(buf_capacity))
,m_stabilized_framebuf(new FrameBuffer(buf_capacity))
{
	init();
}
void Stabilizer::init(){

	m_opt_dist = 2;
	m_min_dist = 0.5;

	stab_model = OPTIMISTIC_MODEL;
	stab_func = POLYNOMIAL_2;

	m_max_normal_transform = cv::Mat(cv::Matx33d(1.09,    0.10,   6,
												 0.10,    1.09,   6,
												 0.01,    0.01,   1 ));
	m_max_transform = cv::Mat(cv::Matx33d(1,   0.7,   50,
										  0.7,   1,   50,
										  1,   1,     1));

	m_min_transform = cv::Mat(cv::Matx33d(1.0009,   0.0005,   0.09,
										  0.0005,   1.0009,   0.09,
										  0.0001,   0.0001,   1    ));
	e = E;

	support_homo_vec.push_back(cv::Point2d(0,0))
					.push_back(cv::Point2d(m_orig_im_size.width,0))
					.push_back(cv::Point2d(0,m_orig_im_size.height))
					.push_back(cv::Point2d(m_orig_im_size.width,m_orig_im_size.height));

	homo_count = 0;
	homo_ejections = 0;
	optimistic_k = 3;
}
Stabilizer::~Stabilizer() {
	saveHistory();
}

bool Stabilizer::captureFrame(){
	FramePtr cap_frame(new Frame());
	FramePtr frame(new Frame());

	m_capture->read(*cap_frame);

	cv::resize(cap_frame->mat(),frame->mat(),m_orig_im_size);

	cv::resize(frame->mat(),frame->matGray(),m_gray_im_size);
	cv::cvtColor(frame->matGray(),frame->matGray(), CV_BGR2GRAY);


	frame->matGray().create(m_gray_im_size,CV_8UC1);

	m_framebuf->push(frame);

	if(!m_stabilized_framebuf->size()){
		m_stabilized_framebuf->push(frame);
	}
	return true;
}
// Stabilizer::getNextFrame(){
//	m_framebuf->setNext();
//	return m_stabilized_framebuf->getNext();
//}

void Stabilizer::stabilize(){
	FramePtr frame = m_framebuf->getLast();
	FramePtr frame_delay = m_framebuf->getDelayed(1,true);
	FramePtr stab_frame(new Frame(cv::Mat(m_orig_im_size,CV_8UC1),false));// = m_stabilized_framebuf->getLast();
	FramePtr stab_frame_delay = m_stabilized_framebuf->getLast();

	int x=0,y=0;

	int64_t n1 = 0, n2 = 0;

	n1=GetTickCount();
	/*вызов функции*/

///////////////////////////////////////////// Optical Flow LK ///////////////////////////////
	findFeatures(frame_delay);

	size_t n_f = frame_delay->featuresLK().size();

	Frame::FeaturesLK next_features(n_f);
	std::vector<float> err_features(n_f);
	std::vector<uchar> status_features(n_f);

//    int type = CV_MAT_TYPE(type);
//
//    int type0 = CV_MAT_TYPE(cv::OutputArray(next_features));

	double minEigThreshold=0.001;
    cv::calcOpticalFlowPyrLK(frame_delay->matGray(), frame->matGray(),
    						 frame_delay->featuresLK(),next_features,status_features,err_features,
    						 cv::Size(15,15),2, cv::TermCriteria(
    	                               cv::TermCriteria::COUNT+cv::TermCriteria::EPS,
    	                               20, 0.01),
						     0.5,0,minEigThreshold);

	frame->featuresLK() = next_features;

	cv::Mat last_transform, opt_transform;
	last_transform = findTransformLK(frame_delay->featuresLK(),frame->featuresLK(),status_features);
///////////////////////////////////////////////////////
//	cv::Mat mask;
//	cv::Mat fundamMat = cv::findFundamentalMat(frame->featuresLK(),frame_delay->featuresLK(),CV_FM_8POINT,3.,0.99,mask);

//	std::cout << "Fundamentel Mat"<< std::endl << fundamMat << std::endl << std::endl;
//	std::cout << "Fundamentel Mask"<< std::endl << mask << std::endl << std::endl;

//////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////// Optical Flow  Farneback ///////////////////////////////
//	cv::Mat  flowFp(m_gray_im_size,CV_32FC2); //// FloatingPoint Image
//
//	cv::calcOpticalFlowFarneback(frameDelay->matGray(), frame->matGray(),flowFp,0.5,2,15,2,5,1.1,cv::OPTFLOW_FARNEBACK_GAUSSIAN);
//	cv::cvtColor(frame->matGray(), stab_frame->mat(), CV_GRAY2BGR);
//	stab_frame->mat() = frame->matGray();

//	frame->transform() = findTransform(frame->matGray(),flowFp,1);
//frame->features() = flowFp;
//////////////////////////////////////////////////////////////////////////////////////////////////

	n2=GetTickCount();
	int64_t ticks=n2 - n1;
	std::cout << "time = " << ticks << std::endl;



	int res = transformEstimation(frame,frame_delay,stab_frame,stab_frame_delay,last_transform,opt_transform);


	//std::cout << "Original frames transform"<< std::endl << frame->transformToPrev() << std::endl << std::endl;

	if(res >= 0){
		std::cout << "Final Warp transform"<< std::endl << opt_transform << std::endl << std::endl;
		cv::warpPerspective(frame->mat(),stab_frame->mat(),opt_transform,m_orig_im_size,cv::INTER_LINEAR);
		//cv::warpPerspective(frame->mat(),stab_frame->mat(),fundamMat,m_orig_im_size,cv::INTER_LINEAR);
	}else{
		frame->setEnjection(true);
		stab_frame->setEnjection(true);
		stab_frame->mat() = frame->mat();
	}

	m_stabilized_framebuf->push(stab_frame);

}
void Stabilizer::findFeatures(FramePtr const& frame){

	Frame::FeaturesLK & features = frame->featuresLK();
	size_t sz = features.size();
	if(frame->isEnjection() || features.empty() || features.size() < DEFAULT_CONERS_COUNT * 0.4){
		cv::goodFeaturesToTrack( frame->matGray(), features, DEFAULT_CONERS_COUNT, 0.05, 1.0, cv::noArray(),5, 0, 0.04 );
	}

	cv::cornerSubPix( frame->matGray(), features, DEFAULT_WND_SIZE,
		    		cvSize( -1, -1 ), cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 30, 0.03 ) );


}
cv::Mat Stabilizer::findTransform(cv::Mat const& fr,cv::Mat const& features, int const& step){


	std::vector<cv::Point2f> fr1, fr2;

	size_t num_pts = fr.rows/step * fr.cols/step;

	fr1.reserve(num_pts);
	fr2.reserve(num_pts);

	for(int i=0;i< fr.rows;i+=step){
		for(int j=0;j< fr.cols;j+=step){
			cv::Point2f const& feature_pt = features.at<cv::Point2f>(i,j);

			fr1.push_back(cv::Point2f(i,j));
			fr2.push_back(cv::Point2f(i+feature_pt.x,j+feature_pt.y));
		}
	}


	return cv::findHomography(fr2,fr1,CV_RANSAC);
}
cv::Mat Stabilizer::findTransformLK(Frame::FeaturesLK & ft1,Frame::FeaturesLK & ft2, std::vector<uchar> & status){
	if(ft1.size() != ft2.size() ){
					std::cout << " ft1.size() != ft2.size()" << std::endl;
		return e;
	}

	for(int i = 0 ; i < ft1.size(); i++){
		if(!status[i]){
			ft1.erase(ft1.begin()+i);
			ft2.erase(ft2.begin()+i);
			status.erase(status.begin()+i);

			i--;
		}
	}
			std::cout << "Features count: " <<  ft1.size() << std::endl;
	if(ft1.size() < 4){
		return e;
	}


	return cv::findHomography(ft2,ft1,CV_RANSAC);
}

/*Если не нужно стабилизировать, то -1
 * Если не нужно вообще сдвигать кадр, то 0
 * Иначе использовать opt_transform
 * в которую запишется оптимальное стабилизирующее преобразование*/
int Stabilizer::transformEstimation(FramePtr const& last_orig,
									FramePtr const& prev_orig,
									FramePtr const& last_stab,
									FramePtr const& prev_stab,
									cv::Mat & last_transform,
									cv::Mat & opt_transform){

	double orig_dist = 1, orig_stab_dist = 1;
	cv::Mat transf_vals = last_transform - e;

	if(checkEjection(transf_vals)){
		return -1;
	}
	if(calcDistanseTo(transf_vals,m_min_transform) < 1){
		last_orig->setNumOfStatic(prev_orig->numOfStatic()+1);
	}else{
		last_orig->transformToPrev() = last_transform;
	}

	cv::Mat max_norm_transform = m_max_normal_transform* m_min_dist;

	orig_dist = calcDistanseTo(transf_vals,max_norm_transform);
//////////////////////////
	cv::Mat orig_stab_tr = last_orig->transformToPrev() * prev_stab->transformToOrig();
	cv::Mat stab_transf_vals = orig_stab_tr - e;

	if(calcDistanseTo(stab_transf_vals,m_min_transform) < 1){
		std::cout << "Stab frames Less then min transform" << std::endl;
		std::cout << stab_transf_vals << std::endl;
		return -1;
	}

	orig_stab_dist = calcDistanseTo(stab_transf_vals,max_norm_transform);

	double func_val = 1;

	if(orig_stab_dist > 1){
		func_val = 1./pow((log(orig_stab_dist)+1),1./m_opt_dist);
	}

	cv::Mat opt_stab_tr = stab_transf_vals * func_val + e;

	cv::Mat stab_stab_tr = stab_transf_vals * (1 - func_val) + e;


	std::cout << "original dist: " << orig_dist << std::endl;
	std::cout << "original -> stab dist: " << orig_stab_dist << std::endl;


	std::cout << "Last transform" << std::endl
			  << last_transform << std::endl << std::endl;
	std::cout << "Orig -> Stab transform" << std::endl
			  << orig_stab_tr << std::endl << std::endl;
	std::cout << "Optimal Origin -> Stab transform" << std::endl
			  << opt_stab_tr  << std::endl << std::endl;
	std::cout << "Stab -> PrevStab transform" << std::endl
			  << stab_stab_tr << std::endl << std::endl;


	if((orig_stab_dist) < 1.){
		if(last_orig->numOfStatic()-2 > optimistic_k){
			opt_transform = stab_transf_vals * 0.8 + e;
		}else{
			opt_transform = orig_stab_tr;
		}

		last_stab->transformToOrig() = opt_transform;

		return 1;
	}else{
		if((prev_stab->numOfStatic()) > optimistic_k){

			last_stab->transformToOrig() = opt_stab_tr ;
			last_stab->transformToPrev() = stab_stab_tr;

			opt_transform = opt_stab_tr;

		}else{
			last_stab->setNumOfStatic(prev_stab->numOfStatic() + 1);

			opt_transform = stab_transf_vals * 0.9 + e;
			last_stab->transformToOrig() = opt_transform;

			return 1;
		}
	}

	m_transform_history.push_back(last_transform);

	return 0;

}
void Stabilizer::fillEdges(FramePtr const& last_orig,
					  	   FramePtr const& prev_orig,
					  	   FramePtr const& last_stab,
					  	   FramePtr const& prev_stab){

}
void Stabilizer::findVoidAreas(cv::Mat & transform){
	HomoVec dst_homo_vec;
	cv::perspectiveTransform(support_homo_vec,dst_homo_vec,transform);


	HomoVec & A = support_homo_vec;
	HomoVec & B = dst_homo_vec;

	Area::Points pts;
	Area::UnitVecs vecs;
	double num,den,r;

	Areas areas;
	cv::Point2d P,P1;
	bool res,res1;
	int num_p = support_homo_vec.size();

	for(i = 0; i < num_p; i++){
		res = Lines::findIntersec(A[i], A[(i+1)%num_p], B[i], B[(i+1)%num_p], P);

		if(res){
			Common::Area area(3);
			if(B[i].y <  A[i].y){
				res1 = Lines::findIntersec( A[(i+1)%num_p],  A[(i+2)%num_p], B[i], B[(i+1)%num_p], P1);
				if(res1){
					area.points().push_back(P).push_back( A[(i+1)%num_p]).push_back(P1);
					area.calcUnitVecs();
					areas.push_back(area);
				}

			}else{
				res1 = Lines::findIntersec( A[i],  A[((i-1) + num_p)%num_p], B[i], B[(i+1)%num_p], P1);
				if(res1){
					area.points().push_back(A[i]).push_back(P).push_back(P1);
					area.calcUnitVecs();
					areas.push_back(area);
				}
			}
		}else{

		}
	}



//	res = Lines::findIntersec(A[0], A[1], B[0], B[1], P);
//
//	if(res){
//		Common::Area area(3);
//		if(B[0].y <  A[0].y){
//			res1 = Lines::findIntersec( A[1],  A[2], B[0], B[1], P1);
//			if(res1){
//				area.points().push_back(P).push_back( A[1]).push_back(P1);
//				area.calcUnitVecs();
//				areas.push_back(area);
//			}
//
//		}else{
//			res1 = Lines::findIntersec( A[0],  A[3], B[0], B[1], P1);
//			if(res1){
//				area.points().push_back(A[0]).push_back(P).push_back(P1);
//				area.calcUnitVecs();
//				areas.push_back(area);
//			}
//		}
//	}


}
/* Возвращает коэффициент между элементами матриц,
 * (входной и матрицей нормального преобразования m_max_normal_transform)
 * которые наиболее отличаются
 * */
double Stabilizer::calcDistanseTo(cv::Mat const& mat, cv::Mat const& src_mat){

	double dist = 0.0 , tmp_dist = 0.0;

	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			tmp_dist = abs(mat.at<double>(i,j)) /  src_mat.at<double>(i,j);

			if(tmp_dist > dist){
				dist = tmp_dist;
			}
		}
	}
	return dist;
}
/*Проверка на выброс
 * */
bool Stabilizer::checkEjection(cv::Mat const& mat){

	homo_count++;
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){

			if(abs(mat.at<double>(i,j)) >  m_max_transform.at<double>(i,j)){
				homo_ejections++;
				std::cout << "Enjection!!!" << std::endl << mat << std::endl;
				return true;
			}
		}
	}
	return false;
}

//bool Stabilizer::checkDistanse(cv::Mat const& mat1, cv::Mat const& mat1){
//	for(int i = 0; i < 3; i++){
//			for(int j = 0; j < 3; j++){
//				if(transf_vals.at<double>(i,j) >  (m_max_normal_transform.at<double>(i,j) * m_min_dist)){
//					is_normal_tr = false;
//					break;
//				}
//			}
//	}
//}
/* Произвести сглаживание
 * */
void Stabilizer::smooth(){

}
void Stabilizer::generateSubFrame(){

}
void Stabilizer::saveHistory(){
	std::ofstream tr_file;
		tr_file.open ("transforms.txt", std::ios::out | std::ios::app );
		tr_file << std::endl << "New matrixes "<< std::endl;

		cv::Mat avarage_tr(cv::Matx33d(1., 0., 0.,
									   0., 1., 0.,
									   0., 0., 1.));

		cv::Mat max_tr(cv::Matx33d(1., 0., 0.,
								   0., 1., 0.,
								   0., 0., 1.));
		double el;
		for(std::vector<cv::Mat>::iterator it=m_transform_history.begin(); it != m_transform_history.end(); it++){

			tr_file << std::setprecision(10);

			cv::Mat absmat = cv::abs(*it);

			for(int i = 0; i < 3; i++){
				for(int j = 0; j < 3; j++){
					tr_file << std::setw(20) << it->at<double>(i,j);

					el = absmat.at<double>(i,j);

					avarage_tr.at<double>(i,j) += el;
					max_tr.at<double>(i,j) = (max_tr.at<double>(i,j) < el )  ? el : max_tr.at<double>(i,j) ;
				}
			}

			tr_file	<< std::endl;

		}

		avarage_tr = avarage_tr / m_transform_history.size();

		tr_file << std::endl << avarage_tr << std::endl;
		tr_file << std::endl << max_tr << std::endl;

		tr_file << " Num homo: " << homo_count << std::endl;
		tr_file << " Num enjections: " << homo_ejections << std::endl;

		tr_file.close();
}

} /*VideoLib*/
