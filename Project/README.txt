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
1) Imagen + texto
   ./armuseum ../data/model.jpg ../data/scene3.jpg 

2) Imagen + parche estático
   ./armuseum ../data/model.jpg ../data/scene3.jpg --patch=../data/scene1.jpeg

3) Imagen + vídeo virtual
   ./armuseum ../data/model.jpg ../data/scene3.jpg --video=../data/scene8.mp4

4) Imagen + Cámara web como contenido virtual (índice 0)
   ./armuseum ../data/model.jpg ../data/scene3.jpg  --video=0

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