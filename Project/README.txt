ARBlockMuseum – Realidad Aumentada sin marcadores
=================================================

1. Requisitos
-------------
* OpenCV ≥ 4.5 (compilado con contrib si se desea usar SIFT)
* CMake ≥ 3.0
* C++11

2. Compilación
--------------
mkdir build && cd build
cmake ..
make

Genera el ejecutable `armuseum`.

3. Uso básico
-------------
./armuseum <model> <scene> [--patch=<img>] [--video=<vid|camIndex>]

Ejemplos:
1) Imagen + parche estático
   ./armuseum data/cuadro.jpg data/foto_escena.jpg --patch=data/logo.png

2) Vídeo escena + vídeo virtual
   ./armuseum data/cuadro.jpg data/galeria.mp4 --video=data/clip.mp4

3) Cámara web como escena y como contenido virtual (índice 0 y 1)
   ./armuseum data/cuadro.jpg 0 --video=1

Teclas en ejecución
-------------------
 s     Guardar captura (captura.png)
 q/Esc Salir

4. Estructura del código
------------------------
src/
 ├── armuseum.cpp   Interfaz de usuario y lógica.
 ├── rva.h/.cpp     Biblioteca de utilidades de RVA
 └── CMakeLists.txt