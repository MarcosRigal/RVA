#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>
#include <algorithm>
#include "rva.h"

using namespace std;

static bool isNumber(const string &s)
{
    return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
}

static bool hasExtension(const string &s, const vector<string> &exts)
{
    string lower = s;
    transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    for (const auto &e : exts)
        if (lower.size() >= e.size() && lower.compare(lower.size() - e.size(), e.size(), e) == 0)
            return true;
    return false;
}

const cv::String keys =
    "{help h usage ? |      | muestra esta ayuda }"
    "{@model         |<none>| Ruta de la imagen modelo }"
    "{@scene         |<none>| Ruta de la escena (imagen, vídeo o índice de cámara) }"
    "{patch          |      | (opcional) Imagen patch a incrustar }"
    "{video          |      | (opcional) Segundo vídeo o índice de cámara }";

int main(int argc, char **argv)
{
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about("ARBlockMuseum 2025");
    if (parser.has("help") || argc < 3)
    {
        parser.printMessage();
        cerr << "\nEjemplos:\n"
             << "  ./armuseum cuadro.jpg escena.mp4 --patch=logo.png\n"
             << "  ./armuseum cuadro.jpg escena.mp4 0\n";
        return 0;
    }

    string model_path = parser.get<string>(0);
    string scene_path = parser.get<string>(1);
    string patch_path = parser.get<string>("patch");
    string video_path = parser.get<string>("video");

    bool use_video = !video_path.empty();
    bool use_patch = !patch_path.empty() && !use_video;
    bool use_info = !use_patch && !use_video;

    cv::Mat img_model = cv::imread(model_path, cv::IMREAD_COLOR);
    if (img_model.empty())
    {
        cerr << "Error: modelo no cargado: " << model_path << endl;
        return -1;
    }
    cv::resize(img_model, img_model, cv::Size(), 0.5, 0.5);

    cv::Mat img_patch;
    if (use_patch)
    {
        img_patch = cv::imread(patch_path, cv::IMREAD_COLOR);
        if (img_patch.empty())
        {
            cerr << "Error: patch no cargado: " << patch_path << endl;
            return -1;
        }
        cv::resize(img_patch, img_patch, img_model.size());
    }

    cv::VideoCapture cap2;
    if (use_video)
    {
        if (isNumber(video_path))
            cap2.open(stoi(video_path));
        else
            cap2.open(video_path);
        if (!cap2.isOpened())
        {
            cerr << "Error: vídeo2 no cargado: " << video_path << endl;
            return -1;
        }
    }

    cv::VideoCapture cap1;
    cv::Mat img_scene;
    bool sceneIsVideo = false;

    if (isNumber(scene_path))
    {
        cap1.open(stoi(scene_path));
        sceneIsVideo = cap1.isOpened();
    }
    else
    {
        string lower_path = scene_path;
        transform(lower_path.begin(), lower_path.end(), lower_path.begin(), ::tolower);

        bool isVideoFile = (lower_path.find(".mp4") != string::npos ||
                            lower_path.find(".avi") != string::npos ||
                            lower_path.find(".mov") != string::npos ||
                            lower_path.find(".mkv") != string::npos);

        if (isVideoFile)
        {
            cap1.open(scene_path);
            sceneIsVideo = cap1.isOpened();
        }
    }

    if (!sceneIsVideo)
    {
        img_scene = cv::imread(scene_path, cv::IMREAD_COLOR);
        if (img_scene.empty())
        {
            cerr << "Error: escena no cargada: " << scene_path << endl;
            return -1;
        }
        cv::resize(img_scene, img_scene, cv::Size(), 0.5, 0.5);
    }

    vector<cv::KeyPoint> kps_model;
    cv::Mat desc_model;
    rva_calculaKPsDesc(img_model, kps_model, desc_model);

    vector<cv::String> info_text = {
        "Título: Ejemplo de cuadro",
        "Autor: Desconocido",
        "Año: 1889"};

    if (sceneIsVideo)
    {
        cv::Mat frame;
        while (cap1.read(frame))
        {
            if (frame.empty())
                break;
            cv::Mat frame_small;
            cv::resize(frame, frame_small, cv::Size(), 0.5, 0.5);

            vector<cv::KeyPoint> kps_scene;
            cv::Mat desc_scene;
            rva_calculaKPsDesc(frame_small, kps_scene, desc_scene);

            vector<cv::DMatch> matches;
            rva_matchDesc(desc_model, desc_scene, matches);
            vector<cv::DMatch> good;
            rva_filterMatches(matches, kps_model, kps_scene, good);

            cv::Mat H;
            vector<cv::Point2f> objCorners;
            rva_localizaObj(img_model, frame_small, kps_model, kps_scene, good, H, objCorners);

            cv::Mat frame_out;
            if (!H.empty())
            {
                if (use_video)
                {
                    cv::Mat frame2;
                    if (!cap2.read(frame2))
                    {
                        cap2.set(cv::CAP_PROP_POS_FRAMES, 0);
                        cap2.read(frame2);
                    }
                    if (!frame2.empty())
                    {
                        cv::resize(frame2, frame2, img_model.size());
                        rva_dibujaPatch(frame_small, frame2, H, frame_out);
                    }
                    else
                        frame_small.copyTo(frame_out);
                }
                else if (use_patch)
                {
                    rva_dibujaPatch(frame_small, img_patch, H, frame_out);
                }
                else if (use_info)
                {
                    rva_draw_contour(frame_small, objCorners, cv::Scalar(0, 255, 0), 4);
                    rva_mostrarInfo(frame_small, info_text, objCorners);
                    frame_small.copyTo(frame_out);
                }
            }
            else
            {
                frame_small.copyTo(frame_out);
            }

            cv::imshow("AR-Scene", frame_out);
            char k = (char)cv::waitKey(10);
            if (k == 27 || k == 'q')
                break;
            if (k == 's')
                cv::imwrite("captura.png", frame_out);
        }
        cv::destroyAllWindows();
        return 0;
    }

    vector<cv::KeyPoint> kps_scene;
    cv::Mat desc_scene;
    rva_calculaKPsDesc(img_scene, kps_scene, desc_scene);

    vector<cv::DMatch> matches;
    rva_matchDesc(desc_model, desc_scene, matches);
    vector<cv::DMatch> good;
    rva_filterMatches(matches, kps_model, kps_scene, good);

    cv::Mat H;
    vector<cv::Point2f> objCorners;
    rva_localizaObj(img_model, img_scene, kps_model, kps_scene, good, H, objCorners);

    cv::Mat img_out;
    img_scene.copyTo(img_out);

    if (use_video)
    {
        while (true)
        {
            cv::Mat frame2;
            if (!cap2.read(frame2))
            {
                cap2.set(cv::CAP_PROP_POS_FRAMES, 0);
                cap2.read(frame2);
            }
            if (frame2.empty())
                break;

            cv::resize(frame2, frame2, img_model.size());

            if (!H.empty())
                rva_dibujaPatch(img_scene, frame2, H, img_out);

            cv::imshow("AR-Static+Video", img_out);
            char k = (char)cv::waitKey(10);
            if (k == 27 || k == 'q')
                break;
            if (k == 's')
                cv::imwrite("captura.png", img_out);
        }
    }
    else if (use_patch)
    {
        if (!H.empty())
            rva_dibujaPatch(img_scene, img_patch, H, img_out);
        else
            cout << "Advertencia: No se encontró homografía. Mostrando imagen original.\n";

        cv::imshow("AugmentedReality-Patch", img_out);
        while (true)
        {
            char k = (char)cv::waitKey(0);
            if (k == 27 || k == 'q')
                break;
            if (k == 's')
                cv::imwrite("captura.png", img_out);
        }
    }
    else if (use_info)
    {
        if (!H.empty())
        {
            rva_draw_contour(img_out, objCorners, cv::Scalar(0, 255, 0), 4);
            rva_mostrarInfo(img_out, info_text, objCorners);
        }
        else
            cout << "Advertencia: No se encontró homografía. Mostrando imagen original.\n";

        cv::imshow("AugmentedReality-Info", img_out);
        while (true)
        {
            char k = (char)cv::waitKey(0);
            if (k == 27 || k == 'q')
                break;
            if (k == 's')
                cv::imwrite("captura.png", img_out);
        }
    }

    cv::destroyAllWindows();
    return 0;
}
