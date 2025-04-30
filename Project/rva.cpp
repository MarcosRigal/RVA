// (c) Realidad Virtual y Aumentada - Universidad de Cordoba - Manuel J. Marin-Jimenez

#include <iostream>
#include <opencv2/opencv.hpp>

#include "rva.h"


// TASK 1

cv::Mat rva_compute_homography(std::vector<cv::Point2f> points_image1, std::vector<cv::Point2f> points_image2) {
    CV_Assert(points_image1.size() >= 4 && points_image1.size() == points_image2.size());
    return findHomography(points_image1, points_image2, cv::RANSAC, 3.0);
}

void rva_draw_contour(cv::Mat image, std::vector<cv::Point2f> points, cv::Scalar color, int thickness) {
    if (points.size() != 4)
        return;
    for (int i = 0; i < 4; ++i)
        line(image, points[i], points[(i + 1) % 4], color, thickness);
}

void rva_deform_image(const cv::Mat& im_input, cv::Mat & im_output, cv::Mat homography) {
    cv::Size s = im_output.empty() ? im_input.size() : im_output.size();
    warpPerspective(im_input, im_output, homography, s, cv::INTER_LINEAR, cv::BORDER_TRANSPARENT);
}


// TASK 2

void rva_calculaKPsDesc(const cv::Mat &img, std::vector<cv::KeyPoint> &keypoints, cv::Mat &descriptors)
{
    cv::Ptr<cv::Feature2D> det = cv::ORB::create(2000);
    det->detectAndCompute(img, cv::noArray(), keypoints, descriptors);
}

void rva_matchDesc(const cv::Mat &descriptors1, const cv::Mat &descriptors2, std::vector<cv::DMatch> &matches)
{
    if (descriptors1.empty() || descriptors2.empty())
        return;
    int norm = (descriptors1.depth() == CV_32F) ? cv::NORM_L2 : cv::NORM_HAMMING;
    cv::BFMatcher matcher(norm);

    std::vector<std::vector<cv::DMatch>> knn;
    matcher.knnMatch(descriptors1, descriptors2, knn, 2);
    matches.clear();
    const float ratio = 0.75f;
    for (auto &v : knn)
        if (v.size() == 2 && v[0].distance < ratio * v[1].distance)
            matches.push_back(v[0]);
}

void rva_dibujaMatches(cv::Mat &img1, cv::Mat &img2, std::vector<cv::KeyPoint> &keypoints1, std::vector<cv::KeyPoint> &keypoints2, std::vector<cv::DMatch> &matches, cv::Mat &img_matches)
{
    drawMatches(img1, keypoints1, img2, keypoints2, matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
}

// TASK 3

void rva_localizaObj(const cv::Mat &img1, const cv::Mat &img2, const std::vector<cv::KeyPoint> &keypoints1, const std::vector<cv::KeyPoint> &keypoints2, const std::vector<cv::DMatch> &matches, cv::Mat &homography, std::vector<cv::Point2f> &pts_im2)
{
    if (matches.size() < 4)
    {
        homography.release();
        return;
    }
    std::vector<cv::Point2f> p1, p2;
    p1.reserve(matches.size());
    p2.reserve(matches.size());
    for (auto &m : matches)
    {
        p1.push_back(keypoints1[m.queryIdx].pt);
        p2.push_back(keypoints2[m.trainIdx].pt);
    }
    homography = rva_compute_homography(p1, p2);

    std::vector<cv::Point2f> corners = {{0, 0}, {float(img1.cols), 0}, {float(img1.cols), float(img1.rows)}, {0, float(img1.rows)}};
    perspectiveTransform(corners, pts_im2, homography);
}

// TASK 4

void rva_filterMatches(const std::vector<cv::DMatch> & matches, const std::vector<cv::KeyPoint> & keypoints_model, const std::vector<cv::KeyPoint> & keypoints_scene, std::vector<cv::DMatch> & good_matches)
{
    good_matches.clear();
    if (matches.size() < 4)
        return;
    std::vector<cv::Point2f> pm, ps;
    for (auto &m : matches)
    {
        pm.push_back(keypoints_model[m.queryIdx].pt);
        ps.push_back(keypoints_scene[m.trainIdx].pt);
    }
    cv::Mat mask;
    findHomography(pm, ps, cv::RANSAC, 3.0, mask);
    for (size_t i = 0; i < matches.size(); ++i)
        if (mask.at<uchar>(int(i)))
            good_matches.push_back(matches[i]);
}

void rva_dibujaPatch(const cv::Mat & scene, const cv::Mat & patch, const cv::Mat & H, cv::Mat & output)
{
    if (H.empty())
    {
        scene.copyTo(output);
        return;
    }
    scene.copyTo(output);
    cv::Mat warped;
    warpPerspective(patch, warped, H, scene.size());
    cv::Mat mask;
    cvtColor(warped, mask, cv::COLOR_BGR2GRAY);
    threshold(mask, mask, 1, 255, cv::THRESH_BINARY);
    warped.copyTo(output, mask);
}

void rva_mostrarInfo(cv::Mat& image, const std::vector<cv::String>& info, const std::vector<cv::Point2f>& vertices)
{
    if (info.empty() || vertices.size() != 4)
        return;
    cv::Point org = vertices[0] + cv::Point2f(10, 20);
    int h = 25;
    for (size_t i = 0; i < info.size(); ++i)
        cv::putText(image, info[i], org + cv::Point(0, int(i * h)), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 255), 2);
}
