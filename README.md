# Nexavey

<p align="center">
  <img src="images/image_0.png" width="500" />
</p>

## Compilation

### (Linux)
To compile you need GLFW and CMake installed on your system.
<br>
If above requirements are fulfilled, just run the following
```bash
mkdir build && cd build
CXX=clang++ cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
ninja
./Nexavey
```
<br>
You can also move around with WASD/CTRL/SPACE and look around with the mouse !
