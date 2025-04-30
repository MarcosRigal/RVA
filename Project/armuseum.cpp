// This program reads a model image, a video and a patch image, and overlays the patch on the model object in the video frames.

// (c) Realidad Virtual y Aumentada - Universidad de Cordoba - Manuel J. Marin-Jimenez

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>

#include "rva.h"

using namespace std;

// Helper: is this string all digits?
static bool isNumber(const string &s) {
    return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
}

const cv::String keys =
"{help h usage ? |      | print this message   }"
"{@model         |<none>| Path to image model.}"
"{@scene         |<none>| Path to image scene.}"
"{patch         |<none>| Path to image patch.}"
"{video         |<none>| Path to playable video.}"
;

// Main function
int main(int argc, char ** argv) {

    // Get the arguments: model, video and patch using OpenCv parser
    cv::CommandLineParser parser(argc, argv, keys);
    if (parser.has("help")) {
        parser.printMessage();
        return 0;
    }
    string model_path = parser.get<string>(0);
    string scene_path = parser.get<string>(1);
    
    // Patch argument is available?
    string patch_path = parser.get<string>("patch");

    // Second video argument is available?
    string video_path = parser.get<string>("video");

    // Video2 has priority over patch
    bool use_video = !video_path.empty();
    bool use_patch = !patch_path.empty() && !use_video;

    bool use_info_text = !use_video && !use_patch;
    // *** TODO: define tu texto aqui ***
    vector<cv::String> info_text;
    if (use_info_text) {
        info_text.push_back("Titulo: La escuela de Atenas");
        info_text.push_back("Autor: Rafael Sanzio");
        info_text.push_back("Fecha: 1509-1511");
    }

    // Load the images in color
    cv::Mat img_model = cv::imread(model_path, cv::IMREAD_COLOR);
    if (img_model.empty()) {
        cerr << "Error: modelo no cargado: " << model_path << endl;
        return -1;
    }

    cv::Mat img_patch;
    if (use_patch) {
        img_patch = cv::imread(patch_path, cv::IMREAD_COLOR);

    // Check if the images are loaded
    // *** TODO: COMPLETAR ***
        if (img_patch.empty()) {
            cerr << "Error: patch no cargado: " << patch_path << endl;
            return -1;
        }
    }

    // Change resolution of the image model to half
    cv::resize(img_model, img_model, cv::Size(), 0.5, 0.5);

    // Resize the patch to the size of the model
    if (use_patch)
        cv::resize(img_patch, img_patch, img_model.size());

    // Load the scene image
    // *** TODO : COMPLETAR ***
    cv::Mat img_scene = cv::imread(scene_path, cv::IMREAD_COLOR);
    if (img_scene.empty()) {
        cerr << "Error: escena no cargada: " << scene_path << endl;
        return -1;
    }

    // If use video, create the video capture
    cv::VideoCapture cap2;
    if (use_video) {
            // Check if video_path is a the webcam index and open it
            // *** TODO: COMPLETAR ***
        if (isNumber(video_path))
            cap2.open(stoi(video_path));
        else    
            // Open video file to display
            // *** TODO: COMPLETAR ***
            cap2.open(video_path);
        
        if (!cap2.isOpened()) {
            cout << "Error: video not loaded: " + video_path << endl;
            return -1;
        }
    }

    // Pre-compute keypoints and descriptors for the model image
    std::vector<cv::KeyPoint> keypoints_model;
    cv::Mat descriptors_model;
    rva_calculaKPsDesc(img_model, keypoints_model, descriptors_model);

    // For the scene image, detect the object and overlay the patch    

    // To speed up processing, resize the image to half
    cv::resize(img_scene, img_scene, cv::Size(), 0.5, 0.5);

    // Compute keypoints and descriptors for the scene image
    std::vector<cv::KeyPoint> keypoints_scene;
    cv::Mat descriptors_scene;
    rva_calculaKPsDesc(img_scene, keypoints_scene, descriptors_scene);

    // Match the descriptors
    std::vector<cv::DMatch> matches;
    rva_matchDesc(descriptors_model, descriptors_scene, matches);

	// Filter out outliers
	// *** TODO: COMPLETAR ***
    vector<cv::DMatch> good_matches;
    rva_filterMatches(matches, keypoints_model, keypoints_scene, good_matches);

    // Compute the bounding-box of the model in the scene
    cv::Mat H;
    std::vector<cv::Point2f> pts_obj_in_scene;
    rva_localizaObj(img_model, img_scene, keypoints_model, keypoints_scene, good_matches, H, pts_obj_in_scene);

    // Draw the bounding-box 
    if (use_info_text) {
        rva_draw_contour(img_scene, pts_obj_in_scene, cv::Scalar(0,255,0), 4);
        rva_mostrarInfo(img_scene, info_text, pts_obj_in_scene);
   
        // Show the result
        cv::imshow("AugmentedReality-Info", img_scene);

        // Check pressed keys to take action
        // *** TODO: COMPLETAR ***
        char k = (char)cv::waitKey(0);
        if (k == 's')
            cv::imwrite("captura.png", img_scene);
    }
    else if (use_patch)
    {
        if (!H.empty())
            rva_dibujaPatch(img_scene, img_patch, H, img_scene);

        // Show the result
        cv::imshow("AugmentedReality-Patch", img_scene);

        // Check pressed keys to take action
        // *** TODO: COMPLETAR ***
        char k = (char)cv::waitKey(0);
        if (k == 's')
            cv::imwrite("captura.png", img_scene);
    }
    else {
        // Grab frames from the video and overlay the frame on the object region
        cv::Mat frame;
        while (cap2.read(frame)) {
            // Warp the read frame to the object region and overlay it
            // *** TODO: COMPLETAR ***
            cv::resize(frame, frame, img_model.size());

            cv::Mat out;
            if (!H.empty())
                rva_dibujaPatch(img_scene, frame, H, out);
            else
                img_scene.copyTo(out);

            // Show the result
            // *** TODO: COMPLETAR ***
            cv::imshow("AugmentedReality-Video", out);
            char k = (char)cv::waitKey(30);
            if (k == 27 || k == 'q')
                break;
            if (k == 's')
                cv::imwrite("captura.png", out);
        }
    }
        
    // The camera will be deinitialized automatically in VideoCapture destructor
	cv::destroyAllWindows();
    if (cap2.isOpened())
        cap2.release();
	
    return 0;
}
