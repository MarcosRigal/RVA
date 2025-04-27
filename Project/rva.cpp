// (c) Realidad Virtual y Aumentada - Universidad de Cordoba - Manuel J. Marin-Jimenez

#include <iostream>
#include <opencv2/opencv.hpp>

#include "rva.h"


// TASK 1

cv::Mat rva_compute_homography(std::vector<cv::Point2f> points_image1, std::vector<cv::Point2f> points_image2) {
;
}

void rva_draw_contour(cv::Mat image, std::vector<cv::Point2f> points, cv::Scalar color, int thickness) {
;
}

void rva_deform_image(const cv::Mat& im_input, cv::Mat & im_output, cv::Mat homography) {
 
 ;
}


// TASK 2

void rva_calculaKPsDesc(const cv::Mat &img, std::vector<cv::KeyPoint> &keypoints, cv::Mat &descriptors)
{
;
}

void rva_matchDesc(const cv::Mat &descriptors1, const cv::Mat &descriptors2, std::vector<cv::DMatch> &matches)
{
; 
}

void rva_dibujaMatches(cv::Mat &img1, cv::Mat &img2, std::vector<cv::KeyPoint> &keypoints1, std::vector<cv::KeyPoint> &keypoints2, std::vector<cv::DMatch> &matches, cv::Mat &img_matches)
{
 ;
}

// TASK 3

void rva_localizaObj(const cv::Mat &img1, const cv::Mat &img2, const std::vector<cv::KeyPoint> &keypoints1, const std::vector<cv::KeyPoint> &keypoints2, const std::vector<cv::DMatch> &matches, cv::Mat &homography, std::vector<cv::Point2f> &pts_im2)
{
;   
}

// TASK 4

void rva_filterMatches(const std::vector<cv::DMatch> & matches, const std::vector<cv::KeyPoint> & keypoints_model, const std::vector<cv::KeyPoint> & keypoints_scene, std::vector<cv::DMatch> & good_matches)
{
;
}

void rva_dibujaPatch(const cv::Mat & scene, const cv::Mat & patch, const cv::Mat & H, cv::Mat & output)
{
;	
}

void rva_mostrarInfo(cv::Mat& image, const std::vector<cv::String>& info, const std::vector<cv::Point2f>& vertices)
{
;	
}
