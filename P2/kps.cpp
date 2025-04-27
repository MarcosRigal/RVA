#include <iostream>
#include <opencv2/opencv.hpp>
#include "rva.h"

int main(int argc, char** argv)
{
    cv::CommandLineParser parser(argc, argv,
        "{@img1 |       | primera imagen}"
        "{@img2 |       | segunda imagen}"
        "{h help||}");
    if (parser.has("help") || !parser.check()) {
        parser.printMessage();
        return 0;
    }
    cv::Mat im1 = cv::imread(parser.get<cv::String>("@img1"), cv::IMREAD_GRAYSCALE);
    cv::Mat im2 = cv::imread(parser.get<cv::String>("@img2"), cv::IMREAD_GRAYSCALE);
    if (im1.empty() || im2.empty()) {
        std::cerr << "Error leyendo imágenes.\n";
        return -1;
    }

    std::vector<cv::KeyPoint> kps1, kps2;
    cv::Mat desc1, desc2;
    rva_calculaKPsDesc(im1, kps1, desc1);
    rva_calculaKPsDesc(im2, kps2, desc2);

    std::vector<cv::DMatch> matches;
    rva_matchDesc(desc1, desc2, matches);

    cv::Mat vis = rva_dibujaMatches(im1, kps1, im2, kps2, matches);
    cv::imshow("Keypoints – matches", vis);
    std::cout << "Matches finales: " << matches.size() << std::endl;
    cv::imwrite("matches.jpg", vis);

    cv::waitKey(0);
    return 0;
}
