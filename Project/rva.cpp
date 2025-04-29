// (c) Realidad Virtual y Aumentada – Universidad de Córdoba
#include "rva.h"

#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <algorithm>

using namespace cv;
using std::vector;

// TASK 1
Mat rva_compute_homography(vector<Point2f> pts1, vector<Point2f> pts2)
{
    CV_Assert(pts1.size() >= 4 && pts1.size() == pts2.size());
    return findHomography(pts1, pts2, RANSAC, 3.0);
}

void rva_draw_contour(Mat img, vector<Point2f> pts, Scalar color, int thickness)
{
    if (pts.size() != 4)
        return;
    for (int i = 0; i < 4; ++i)
        line(img, pts[i], pts[(i + 1) % 4], color, thickness);
}

void rva_deform_image(const Mat &in, Mat &out, Mat H)
{
    Size s = out.empty() ? in.size() : out.size();
    warpPerspective(in, out, H, s, INTER_LINEAR, BORDER_TRANSPARENT);
}

// TASK 2
void rva_calculaKPsDesc(const Mat &img, vector<KeyPoint> &kps, Mat &desc)
{
    Ptr<Feature2D> det = ORB::create(2000);
    det->detectAndCompute(img, noArray(), kps, desc);
}

void rva_matchDesc(const Mat &d1, const Mat &d2, vector<DMatch> &matches)
{
    if (d1.empty() || d2.empty())
        return;
    int norm = (d1.depth() == CV_32F) ? NORM_L2 : NORM_HAMMING;
    BFMatcher matcher(norm);

    vector<vector<DMatch>> knn;
    matcher.knnMatch(d1, d2, knn, 2);
    matches.clear();
    const float ratio = 0.75f;
    for (auto &v : knn)
        if (v.size() == 2 && v[0].distance < ratio * v[1].distance)
            matches.push_back(v[0]);
}

void rva_dibujaMatches(Mat &i1, Mat &i2, vector<KeyPoint> &k1, vector<KeyPoint> &k2, vector<DMatch> &m, Mat &out)
{
    drawMatches(i1, k1, i2, k2, m, out, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
}

// TASK 3
void rva_localizaObj(const Mat &img1, const Mat & /*img2*/, const vector<KeyPoint> &k1, const vector<KeyPoint> &k2, const vector<DMatch> &matches, Mat &H, vector<Point2f> &pts_in_scene)
{
    if (matches.size() < 4)
    {
        H.release();
        return;
    }
    vector<Point2f> p1, p2;
    p1.reserve(matches.size());
    p2.reserve(matches.size());
    for (auto &m : matches)
    {
        p1.push_back(k1[m.queryIdx].pt);
        p2.push_back(k2[m.trainIdx].pt);
    }
    H = rva_compute_homography(p1, p2);

    vector<Point2f> corners = {{0, 0}, {float(img1.cols), 0}, {float(img1.cols), float(img1.rows)}, {0, float(img1.rows)}};
    perspectiveTransform(corners, pts_in_scene, H);
}

// TASK 4
void rva_filterMatches(const vector<DMatch> &matches, const vector<KeyPoint> &kpsM, const vector<KeyPoint> &kpsS, vector<DMatch> &good)
{
    good.clear();
    if (matches.size() < 4)
        return;
    vector<Point2f> pm, ps;
    for (auto &m : matches)
    {
        pm.push_back(kpsM[m.queryIdx].pt);
        ps.push_back(kpsS[m.trainIdx].pt);
    }
    Mat mask;
    findHomography(pm, ps, RANSAC, 3.0, mask);
    for (size_t i = 0; i < matches.size(); ++i)
        if (mask.at<uchar>(int(i)))
            good.push_back(matches[i]);
}

void rva_dibujaPatch(const Mat &scene, const Mat &patch, const Mat &H, Mat &output)
{
    if (H.empty())
    {
        scene.copyTo(output);
        return;
    }
    scene.copyTo(output);
    Mat warped;
    warpPerspective(patch, warped, H, scene.size());
    Mat mask;
    cvtColor(warped, mask, COLOR_BGR2GRAY);
    threshold(mask, mask, 1, 255, THRESH_BINARY);
    warped.copyTo(output, mask);
}

void rva_mostrarInfo(Mat &img, const vector<String> &info, const vector<Point2f> &verts)
{
    if (info.empty() || verts.size() != 4)
        return;
    Point org = verts[0] + Point2f(10, 20);
    int h = 25;
    for (size_t i = 0; i < info.size(); ++i)
        putText(img, info[i], org + Point(0, int(i * h)), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 255), 2);
}
