// (c) Realidad Virtual y Aumentada - Universidad de Cordoba - Manuel J. Marin-Jimenez

#include <iostream>
#include <opencv2/opencv.hpp>

#include "rva.h"

// -------------------- Práctica 1 --------------------
cv::Mat rva_compute_homography(std::vector<cv::Point2f> points_image1, std::vector<cv::Point2f> points_image2)
{
    return cv::findHomography(points_image1, points_image2);
}

void rva_draw_contour(cv::Mat image, std::vector<cv::Point2f> points, cv::Scalar color, int thickness)
{
    for (size_t i = 0; i < points.size(); i++)
    {
        cv::line(image, points[i], points[(i + 1) % points.size()], color, thickness);
    }
}

void rva_deform_image(const cv::Mat &im_input, cv::Mat &im_output, cv::Mat homography)
{
    cv::warpPerspective(im_input, im_output, homography, im_output.size());
}

// -------------------- Práctica 2 --------------------
void rva_calculaKPsDesc(const cv::Mat &img,
                        std::vector<cv::KeyPoint> &kps,
                        cv::Mat &desc,
                        const cv::Ptr<cv::Feature2D> &detector)
{
    CV_Assert(!img.empty());
    detector->detectAndCompute(img, cv::noArray(), kps, desc);
}

void rva_matchDesc(const cv::Mat &desc1,
                   const cv::Mat &desc2,
                   std::vector<cv::DMatch> &matches,
                   const cv::Ptr<cv::DescriptorMatcher> &matcher)
{
    CV_Assert(!desc1.empty() && !desc2.empty());
    std::vector<cv::DMatch> allMatches;
    matcher->match(desc1, desc2, allMatches);

    double minDist = std::numeric_limits<double>::max();
    for (const auto &m : allMatches)
        minDist = std::min(minDist, (double)m.distance);

    const double thresh = std::max(2.0 * minDist, 60.0);
    for (const auto &m : allMatches)
        if (m.distance < thresh)
            matches.push_back(m);
}

cv::Mat rva_dibujaMatches(const cv::Mat &img1,
                          const std::vector<cv::KeyPoint> &kps1,
                          const cv::Mat &img2,
                          const std::vector<cv::KeyPoint> &kps2,
                          const std::vector<cv::DMatch> &matches,
                          bool dibujaSoloBuenos,
                          float maxRatio)
{
    std::vector<cv::DMatch> good = matches;
    if (dibujaSoloBuenos)
    {

        std::vector<std::vector<cv::DMatch>> knn;
        cv::Ptr<cv::DescriptorMatcher> matcher =
            cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE_HAMMING);
        matcher->knnMatch(kps1.empty() ? cv::Mat() : cv::Mat(), cv::Mat(), knn, 2);

        good.clear();
        for (const auto &m : matches)
            if (m.distance < maxRatio * matches[0].distance)
                good.push_back(m);
    }

    cv::Mat salida;
    cv::drawMatches(img1, kps1, img2, kps2, good, salida,
                    cv::Scalar::all(-1), cv::Scalar::all(-1),
                    std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
    return salida;
}

// -------------------- Práctica 3 --------------------
bool rva_localizaObj(const cv::Mat &img_model,
                     const cv::Mat &img_scene,
                     std::vector<cv::Point2f> &corners_out,
                     cv::Mat &out_vis)
{
    if (img_model.empty() || img_scene.empty())
        return false;

    std::vector<cv::KeyPoint> kps_model, kps_scene;
    cv::Mat desc_model, desc_scene;
    auto detector = cv::ORB::create(3500); // más puntos que en P2
    rva_calculaKPsDesc(img_model, kps_model, desc_model, detector);
    rva_calculaKPsDesc(img_scene, kps_scene, desc_scene, detector);

    cv::Ptr<cv::DescriptorMatcher> matcher =
        cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE_HAMMING);

    std::vector<std::vector<cv::DMatch>> knn;
    matcher->knnMatch(desc_model, desc_scene, knn, 2);

    std::vector<cv::DMatch> good;
    for (auto &m : knn)
        if (m[0].distance < 0.80f * m[1].distance)
            good.push_back(m[0]);

    if (good.size() < 10)
        return false;

    std::vector<cv::Point2f> pts_model, pts_scene;
    for (auto &m : good)
    {
        pts_model.push_back(kps_model[m.queryIdx].pt);
        pts_scene.push_back(kps_scene[m.trainIdx].pt);
    }

    cv::Mat maskInliers;

    cv::Mat H = cv::findHomography(pts_model, pts_scene,
                                   cv::RANSAC, 3.0, maskInliers);

    if (H.empty())
        return false;

    std::vector<cv::Point2f> model_corners = {
        {0, 0},
        {float(img_model.cols), 0},
        {float(img_model.cols), float(img_model.rows)},
        {0, float(img_model.rows)}};
    cv::perspectiveTransform(model_corners, corners_out, H);

    cv::drawMatches(img_model, kps_model,
                    img_scene, kps_scene,
                    good, out_vis,
                    cv::Scalar::all(-1), cv::Scalar::all(-1),
                    maskInliers,
                    cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    for (int i = 0; i < 4; ++i)
        cv::line(out_vis,
                 corners_out[i] + cv::Point2f(float(img_model.cols), 0),
                 corners_out[(i + 1) % 4] + cv::Point2f(float(img_model.cols), 0),
                 cv::Scalar(0, 255, 0), 4);

    return true;
}
