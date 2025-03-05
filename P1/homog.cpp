// This program creates a sample homograpy matrix and apply it to an image
// (c) Realidad Virtual y Aumentada - Universidad de Cordoba - Manuel J. Marin-Jimenez

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>

#include "rva.h"

using namespace std;

// Variable global para almacenar los 4 puntos
std::vector<cv::Point2f> points;

// Función callback del mouse para recoger los 4 puntos
void mouse_callback(int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_LBUTTONDOWN) {

        // Obtenemos la imagen de entrada desde userdata
        cv::Mat im_input = *(cv::Mat*)userdata;

        // Verificamos que no se hayan seleccionado más de 4 puntos
        if (points.size() >= 4) {
            return;
        }

        // Añadimos el punto al vector
        points.push_back(cv::Point2f(x, y));
        std::cout << "Point " << points.size() << ": " << x << ", " << y << std::endl;

        // Dibuja un círculo en el punto seleccionado
        cv::circle(im_input, cv::Point(x, y), 5, cv::Scalar(0, 0, 255), cv::FILLED);

        // Muestra la imagen de entrada actualizada
        cv::imshow("Input Image", im_input);
    }
}

int main(int argc, char** argv)
{
    // Se lee la imagen de entrada desde la línea de comandos
    cv::CommandLineParser parser(argc, argv, "{@input | ../data/lena.jpg | input image}");
    cv::Mat im_input = cv::imread(parser.get<cv::String>("@input"), cv::IMREAD_COLOR);
    if (im_input.empty()) {
        std::cout << "Could not open or find the image!" << std::endl;
        return -1;
    }

    // Se redimensiona la imagen si es mayor que 1024x768 manteniendo la relación de aspecto
    if (im_input.cols > 1024 || im_input.rows > 768) {
        float scale = std::min(1024.0f / im_input.cols, 768.0f / im_input.rows);
        cv::resize(im_input, im_input, cv::Size(), scale, scale);
    }

    // Se crea la ventana para mostrar la imagen de entrada
    cv::namedWindow("Input Image", cv::WINDOW_AUTOSIZE);

    // Se establece el callback para recoger los 4 puntos
    cv::setMouseCallback("Input Image", mouse_callback, &im_input);
    
    // Se muestra la imagen de entrada
    cv::imshow("Input Image", im_input);

    // Espera hasta que el usuario presione la tecla ESC o se recojan los 4 puntos
    while (cv::waitKey(0) != 27) {
        if (points.size() >= 4) {
            break;
        }
    }
    
    // Se dibuja el contorno en la imagen de entrada
    rva_draw_contour(im_input, points, cv::Scalar(0, 255, 0), 2);

    // Se muestra la imagen con el contorno dibujado
    cv::imshow("Input Image", im_input);
    cv::waitKey(0);

    // Se crea la imagen de salida con tamaño 480x640
    cv::Mat im_output(640, 480, CV_8UC3);

    // Se definen los 4 puntos destino para el rectángulo resultante
    std::vector<cv::Point2f> points_dst = {
        cv::Point2f(0, 0),
        cv::Point2f(im_output.cols - 1, 0),
        cv::Point2f(im_output.cols - 1, im_output.rows - 1),
        cv::Point2f(0, im_output.rows - 1)
    };

    // Se calcula la homografía a partir de los puntos seleccionados y los puntos destino
    cv::Mat homography = rva_compute_homography(points, points_dst);
    
    // Se aplica la transformación para deformar la imagen
    rva_deform_image(im_input, im_output, homography);

    // Se muestra la imagen de salida
    cv::imshow("Output Image", im_output);
    cv::waitKey(0);

    // Se guarda la imagen de salida en disco
    cv::imwrite("output.jpg", im_output);
    
    return 0;
}
