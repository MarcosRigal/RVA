ARBlockMuseum – Realidad Aumentada sin marcadores
=================================================

1. Requisitos
-------------
* OpenCV ≥ 4.5
* CMake ≥ 3.0
* C++11

2. Compilación
--------------
cd src && mkdir build && cd build
cmake ..
make

Genera el ejecutable `armuseum`.

3. Uso básico
-------------
./armuseum <model> <scene> [--patch=<img>] [--video=<vid|camIndex>]

Ejemplos:
1) Imagen + texto
   ./armuseum ../../data/model.jpg ../../data/scene3.jpg 

2) Imagen + parche estático
   ./armuseum ../../data/model.jpg ../../data/scene3.jpg --patch=../../data/scene1.jpeg

3) Imagen + vídeo virtual
   ./armuseum ../../data/model.jpg ../../data/scene3.jpg --video=../../data/scene8.mp4

4) Imagen + Cámara web como contenido virtual (índice 0)
   ./armuseum ../../data/model.jpg ../../data/scene3.jpg  --video=0

Teclas en ejecución
-------------------
 s     Guardar captura (captura.png)
 q/Esc Salir

4. Estructura del código
------------------------
.
├── data/                  Modelo y escenas de ejemplo.
│   ├── model.jpg
│   ├── scene10.png
│   ├── scene11.jpg
│   ├── scene11.png
│   ├── scene1.jpeg
│   ├── scene2.jpg
│   ├── scene3.jpg
│   ├── scene4.jpg
│   ├── scene5.png
│   ├── scene6.png
│   ├── scene7.png
│   ├── scene8.mp4
│   └── scene9.png
├── README.txt             Documentacion del código.
├── report.pdf             Memoria de la práctica.
└── src
    ├── armuseum.cpp       Interfaz de usuario y lógica.
    ├── CMakeLists.txt     Configuración de CMake.
    ├── rva.cpp            Implementación de la biblioteca de utilidades de RVA
    └── rva.h              Biblioteca de utilidades de RVA
