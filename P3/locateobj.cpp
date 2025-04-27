#include <iostream>
#include <opencv2/opencv.hpp>
#include "rva.h"

int main(int argc, char **argv)
{
    cv::CommandLineParser parser(argc, argv,
                                 "{@modelo | | imagen del objeto modelo}"
                                 "{@escena | | imagen de la escena}"
                                 "{h help  | | }");
    if (parser.has("help") || !parser.check())
    {
        parser.printMessage();
        return 0;
    }

    cv::Mat img_model = cv::imread(parser.get<cv::String>("@modelo"), cv::IMREAD_GRAYSCALE);
    cv::Mat img_scene = cv::imread(parser.get<cv::String>("@escena"), cv::IMREAD_GRAYSCALE);
    if (img_model.empty() || img_scene.empty())
    {
        std::cerr << "Error leyendo imágenes\n";
        return -1;
    }

    std::vector<cv::Point2f> corners;
    cv::Mat vis;
    if (!rva_localizaObj(img_model, img_scene, corners, vis))
    {
        std::cerr << "No se encontró homografía convincente.\n";
        return -1;
    }

    cv::imshow("Objeto localizado", vis);
    cv::imwrite("objeto_localizado.jpg", vis);
    cv::waitKey(0);
    return 0;
}
