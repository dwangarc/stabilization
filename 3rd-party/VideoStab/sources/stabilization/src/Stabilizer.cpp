#include <stabilization/Stabilizer.h>

#include <iostream>
#include <ios>
#include <iomanip>
#include <fstream>

namespace VideoLib {

const size_t Stabilizer::DEFAULT_CONERS_COUNT  = 200;
const Size Stabilizer::DEFAULT_WND_SIZE = Size(5,5);

Stabilizer::Stabilizer(size_t buf_capacity, Size orig_im_size = Size(0,0), Size gray_im_size = Size(0,0)) {
   //m_enable = !(orig_im_size == Size(0,0) || gray_im_size == Size(0,0));
   m_orig_im_size = orig_im_size;
   m_gray_im_size = gray_im_size;
   //m_buf_capacity = buf_capacity

   m_stabilized_framebuf = new FrameBuffer(buf_capacity);
   m_framebuf = new FrameBuffer(buf_capacity);

   m_opt_dist = 2;
   m_min_dist = 0.5;

   //stab_model = OPTIMISTIC_MODEL;
   //stab_func = POLYNOMIAL_2;

   m_max_normal_transform = Mat(Matx33d( 1.09, 0.10, 6
                                       , 0.10, 1.09, 6
                                       , 0.01, 0.01, 1));
   m_max_transform = Mat(Matx33d( 1.0, 0.7, 50
                                , 0.7, 1.0, 50
                                , 1.0, 1.0, 1.));
   m_min_transform = Mat(Matx33d( 1.0009, 0.0005, 0.09
                                , 0.0005, 1.0009, 0.09
                                , 0.0001, 0.0001, 1.00));
   e = Mat(Matx33d( 1, 0, 0
                  , 0, 1, 0
                  , 0, 0, 1));

//   support_homo_vec.push_back(Point2d(0,0))
//                   .push_back(Point2d(m_orig_im_size.width,0))
//                   .push_back(Point2d(0,m_orig_im_size.height))
//                   .push_back(Point2d(m_orig_im_size.width,m_orig_im_size.height));

   //homo_count = 0;
   //homo_ejections = 0;
   optimistic_k = 3;
}

Stabilizer::~Stabilizer() { }

bool Stabilizer::captureFrame(){
   FramePtr cap_frame(new Frame());
   FramePtr frame(new Frame());
   m_capture->read(*cap_frame);

   resize(cap_frame->mat(),frame->mat(),m_orig_im_size);
   resize(frame->mat(),frame->matGray(),m_gray_im_size);
   cvtColor(frame->matGray(),frame->matGray(), CV_BGR2GRAY);
   //Not entirely sure why do they reallocate
   frame->matGray().create(m_gray_im_size,CV_8UC1);

   m_framebuf->push(frame);

   if(!m_stabilized_framebuf->size()){ m_stabilized_framebuf->push(frame); }
   return true;
}

void Stabilizer::stabilize(){
   FramePtr frame = m_framebuf->getLast();
   FramePtr frame_delay = m_framebuf->getDelayed(1,true);
   FramePtr stab_frame(new Frame(Mat(m_orig_im_size,CV_8UC1),false));// = m_stabilized_framebuf->getLast();
   FramePtr stab_frame_delay = m_stabilized_framebuf->getLast();

///////////////////////////////////////////// Optical Flow LK ///////////////////////////////
   findFeatures(frame_delay);

   size_t n_f = frame_delay->featuresLK().size();

   Frame::FeaturesLK next_features(n_f);
   std::vector<float> err_features(n_f);
   std::vector<uchar> status_features(n_f);

   calcOpticalFlowPyrLK( frame_delay->matGray(), frame->matGray()
                       , frame_delay->featuresLK(), next_features
                       , status_features, err_features
                       , Size(15,15), 2
                       , TermCriteria(TermCriteria::COUNT+TermCriteria::EPS,20,0.01)
                       , 0.5, 0, 0.001);

   frame->featuresLK() = next_features;

   Mat last_transform, opt_transform;
   last_transform = findTransformLK(frame_delay->featuresLK(),frame->featuresLK(),status_features);

   int res = transformEstimation(frame, frame_delay, stab_frame, stab_frame_delay, last_transform, opt_transform);

   if(res >= 0) {
      warpPerspective(frame->mat(), stab_frame->mat(), opt_transform, m_orig_im_size, INTER_LINEAR);
      //cv::warpPerspective(frame->mat(),stab_frame->mat(),fundamMat,m_orig_im_size,cv::INTER_LINEAR);
   } else {
      frame->setEnjection(true);
      stab_frame->setEnjection(true);
      stab_frame->mat() = frame->mat();
   }

   m_stabilized_framebuf->push(stab_frame);
}

void Stabilizer::findFeatures(const FramePtr& frame) {
   Frame::FeaturesLK &features = frame->featuresLK();
   if(frame->isEnjection() || features.empty() || features.size() < DEFAULT_CONERS_COUNT * 0.4) {
      goodFeaturesToTrack(frame->matGray(), features, DEFAULT_CONERS_COUNT, 0.05, 1.0, noArray(),5, 0, 0.04);
   }
   cornerSubPix( frame->matGray(), features, DEFAULT_WND_SIZE,Size(-1,-1)
               , TermCriteria(TermCriteria::COUNT+TermCriteria::EPS,30,0.03));
}

Mat Stabilizer::findTransformLK(Frame::FeaturesLK& ft1, Frame::FeaturesLK& ft2, vector<uchar>& status) {
   if(ft1.size() != ft2.size()) {
      cout << " ft1.size() != ft2.size()" << endl;
      return e;
   }

   for(int i = 0; i < ft1.size(); i++) {
      if(!status[i]) {
         ft1.erase(ft1.begin()+i);
         ft2.erase(ft2.begin()+i);
         status.erase(status.begin()+i);
         i--;
      }
   }

   cout << "Features count: " << ft1.size() << endl;

   if(ft1.size() < 4) { return e; }

   return findHomography(ft2,ft1,CV_RANSAC);
}

/*Если не нужно стабилизировать, то -1
 * Если не нужно вообще сдвигать кадр, то 0
 * Иначе использовать opt_transform
 * в которую запишется оптимальное стабилизирующее преобразование*/
int Stabilizer::transformEstimation( const FramePtr& last_orig, const FramePtr& prev_orig
                                   , const FramePtr& last_stab, const FramePtr& prev_stab
                                   , Mat& last_transform, Mat& opt_transform) {
   double orig_dist = 1, orig_stab_dist = 1;
   Mat transf_vals = last_transform - e;

   if(checkEjection(transf_vals)) { return -1; }

   if(calcDistanseTo(transf_vals, m_min_transform) < 1) {
      last_orig->setNumOfStatic(prev_orig->numOfStatic() + 1);
   } else {
      last_orig->transformToPrev() = last_transform;
   }

   Mat max_norm_transform = m_max_normal_transform * m_min_dist;

   orig_dist = calcDistanseTo(transf_vals, max_norm_transform);
//////////////////////////
   Mat orig_stab_tr = last_orig->transformToPrev() * prev_stab->transformToOrig();
   Mat stab_transf_vals = orig_stab_tr - e;

   if(calcDistanseTo(stab_transf_vals, m_min_transform) < 1){
      cout << "Stab frames Less then min transform" << endl;
      cout << stab_transf_vals << endl;
      return -1;
   }

   orig_stab_dist = calcDistanseTo(stab_transf_vals, max_norm_transform);

   double func_val = 1;

   if(orig_stab_dist > 1){
      func_val = 1. / pow(log(orig_stab_dist)+1, 1./m_opt_dist);
   }

   Mat opt_stab_tr = stab_transf_vals * func_val + e;
   Mat stab_stab_tr = stab_transf_vals * (1 - func_val) + e;

   cout << "original dist: " << orig_dist << endl;
   cout << "original -> stab dist: " << orig_stab_dist << endl;

   if((orig_stab_dist) < 1.){
      if(last_orig->numOfStatic()-2 > optimistic_k) {
         opt_transform = stab_transf_vals * 0.8 + e;
      } else {
         opt_transform = orig_stab_tr;
      }

      last_stab->transformToOrig() = opt_transform;

      return 1;
   } else {
      if((prev_stab->numOfStatic()) > optimistic_k) {
         last_stab->transformToOrig() = opt_stab_tr;
         last_stab->transformToPrev() = stab_stab_tr;
         opt_transform = opt_stab_tr;
      } else {
         last_stab->setNumOfStatic(prev_stab->numOfStatic() + 1);
         opt_transform = stab_transf_vals * 0.9 + e;
         last_stab->transformToOrig() = opt_transform;
         return 1;
      }
   }

   m_transform_history.push_back(last_transform);

   return 0;
}

double Stabilizer::calcDistanseTo(const Mat& mat, const Mat& src_mat) {
   double dist = 0.0 , tmp_dist = 0.0;

   for(int i = 0; i < 3; i++) {
      for(int j = 0; j < 3; j++) {
         tmp_dist = abs(mat.at<double>(i,j)) /  src_mat.at<double>(i,j);
         if(tmp_dist > dist){ dist = tmp_dist; }
      }
   }
   return dist;
}

// Проверка на выброс
bool Stabilizer::checkEjection(const Mat& mat) {
   //homo_count++;
   for(int i = 0; i < 3; i++) {
      for(int j = 0; j < 3; j++) {
         if(abs(mat.at<double>(i,j)) > m_max_transform.at<double>(i,j)) {
            //homo_ejections++;
            return true;
         }
      }
   }
   return false;
}

}
