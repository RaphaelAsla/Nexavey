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
> This project is currently being re-written in Vulkan, code will be updated once I clean it up a bit.

### Sneak peak of Vulkan
<p align="center">
  <img src="images/vulkan.png" width="500" />
</p>
