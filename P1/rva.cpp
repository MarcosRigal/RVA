// (c) Realidad Virtual y Aumentada - Universidad de Cordoba - Manuel J. Marin-Jimenez

#include <iostream>
#include <opencv2/opencv.hpp>

#include "rva.h"

// Función para calcular la homografía entre dos conjuntos de puntos
cv::Mat rva_compute_homography(std::vector<cv::Point2f> points_image1, std::vector<cv::Point2f> points_image2) {
    // Se utiliza la función findHomography para estimar la transformación
    return cv::findHomography(points_image1, points_image2);
}

// Función para dibujar el contorno (polígono) dado un conjunto de puntos
void rva_draw_contour(cv::Mat image, std::vector<cv::Point2f> points, cv::Scalar color, int thickness) {
    // Se recorre cada punto y se dibuja una línea hasta el siguiente (cierra el polígono con el primer punto)
    for (size_t i = 0; i < points.size(); i++) {
        cv::line(image, points[i], points[(i + 1) % points.size()], color, thickness);
    }
}

// Función para deformar la imagen aplicando la homografía calculada
void rva_deform_image(const cv::Mat& im_input, cv::Mat & im_output, cv::Mat homography) {
    // Se aplica warpPerspective para generar la imagen de salida con la transformación deseada
    cv::warpPerspective(im_input, im_output, homography, im_output.size());
}
