# Nexavey
<p align="center">
  <img src="images/image_0.png" width="500" />
</p>

## Compilation (linux only)
To compile you need GLFW, CMake and Ninja installed on your system.
<br>
If above requirements are fulfilled, just run the following
```bash
mkdir build && cd build
CXX=clang++ cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
ninja
./Nexavey
```
<br>

> **Note:**
> This project is currently on hold as I wanted to focus more on my studies and I also plan to re-write it using Vulkan after my exams.
