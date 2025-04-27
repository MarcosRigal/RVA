// This is the header file for the RVA functions
// (c) Realidad Virtual y Aumentada - Universidad de Cordoba - Manuel J. Marin-Jimenez

#ifndef _RVA_H
#define _RVA_H

#include <opencv2/opencv.hpp>

cv::Mat rva_compute_homography(std::vector<cv::Point2f> points_image1, std::vector<cv::Point2f> points_image2);

void rva_draw_contour(cv::Mat image, std::vector<cv::Point2f> points, cv::Scalar color, int thickness);

void rva_deform_image(const cv::Mat &im_input, cv::Mat &im_output, cv::Mat homography);

void rva_calculaKPsDesc(const cv::Mat &img,
                        std::vector<cv::KeyPoint> &kps,
                        cv::Mat &desc,
                        const cv::Ptr<cv::Feature2D> &detector = cv::ORB::create());

void rva_matchDesc(const cv::Mat &desc1,
                   const cv::Mat &desc2,
                   std::vector<cv::DMatch> &matches,
                   const cv::Ptr<cv::DescriptorMatcher> &matcher =
                       cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE_HAMMING));

cv::Mat rva_dibujaMatches(const cv::Mat &img1,
                          const std::vector<cv::KeyPoint> &kps1,
                          const cv::Mat &img2,
                          const std::vector<cv::KeyPoint> &kps2,
                          const std::vector<cv::DMatch> &matches,
                          bool dibujaSoloBuenos = true,
                          float maxRatio = 0.85f);

bool rva_localizaObj(const cv::Mat &img_model,
                     const cv::Mat &img_scene,
                     std::vector<cv::Point2f> &corners_out,
                     cv::Mat &out_vis);

#endif