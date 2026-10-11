# Interactive Ray Tracing Visualizer

A C++/OpenGL project for exploring how rays travel, reflect off walls, and intersect a receiver in three-dimensional environments. Animated ray trails and interactive controls make the underlying geometry visible.

The project is intended as a general foundation that can later be adapted to specific applications like audio tracing or graphics. Its current focus is on visualization and GPU acceleration.

## Demonstrations

## Features

- Multi-bounce ray tracing against bounded, axis-aligned rectangular walls.
- Three switchable environments: a box room, an L-shaped room, and an atrium with adjoining rooms, open doorways, and a central obstacle.
- Animated, fading ray trails with pause and restart controls.
- An orbit camera and a movable spherical receiver, with approaching rays highlighted when their paths intersect the receiver.
- A separate CUDA prototype for parallel closest-hit calculations, CPU/GPU result comparison, and performance benchmarking.

## Technology Stack

| Component             | Technology            |
| --------------------- | --------------------- |
| Language              | C++                   |
| Rendering and shaders | OpenGL                |
| GPU acceleration      | CUDA                  |
| GPU profiling         | NVIDIA Nsight Systems |

The desktop visualizer currently traces paths on the CPU and renders them with OpenGL. The CUDA implementation is a standalone benchmark and is not yet connected to the visualizer. CUDA is not required to build or run the desktop application.

## Build and Run

### Requirements

The desktop application is currently developed on macOS. It requires:

- A C++20-compatible compiler.
- CMake 3.20 or newer.
- GLFW and GLM.
- OpenGL 3.3 core support.

With the Xcode Command Line Tools and Homebrew installed, install the dependencies:

```sh
brew install cmake glfw glm
```

From the repository root:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build build --parallel
./build/Ray-Tracing-Visualizer
```

The application opens in the atrium scene. By default, it emits 100 rays in the horizontal plane and traces up to 8 reflections per ray. The tracing module also includes a generator for directions distributed over a sphere.

## Controls

| Key        | Action                                    |
| ---------- | ----------------------------------------- |
| `1`        | Load the box room                         |
| `2`        | Load the L-shaped room                    |
| `3`        | Load the atrium                           |
| `A` / `D`  | Orbit the camera horizontally             |
| `W` / `S`  | Tilt the camera up / down                 |
| `Q` / `E`  | Zoom in / out                             |
| Arrow keys | Move the receiver in the horizontal plane |
| `Space`    | Pause or resume the animation             |
| `R`        | Restart the animation                     |

Close the window to exit. Switching scenes resets the camera, source, receiver, and animation to that scene's defaults.

### Reading the Visualization

- **Gray outlines:** room walls and obstacles.
- **Green marker:** ray source.
- **Pink marker:** receiver center.
- **Blue trails:** traveling rays.
- **Orange trails:** rays on an approach that will intersect the receiver, before entry.

The receiver uses a sphere for intersection calculations but is displayed as a point marker; the marker does not show its physical radius. Rays continue reflecting after passing through the receiver.

The world uses **Z as the vertical axis**, with floors at `z = 0` and horizontal motion in the XY plane.

## Configuration

Settings are currently changed in code:

| Setting                                                            | Location                                                           |
| ------------------------------------------------------------------ | ------------------------------------------------------------------ |
| Ray count, maximum bounces, animation speed, trail length          | [src/main.cpp](src/main.cpp)                                       |
| Scene layouts, source and receiver positions, initial camera views | [src/scene/Scene.cpp](src/scene/Scene.cpp)                         |
| Box and L-shaped room construction                                 | [src/scene/Room.cpp](src/scene/Room.cpp)                           |
| Ray emission patterns                                              | [src/tracing/RayEmitter.cpp](src/tracing/RayEmitter.cpp)           |
| Shader source                                                      | [src/rendering/ShaderSources.hpp](src/rendering/ShaderSources.hpp) |

Rebuild the application after changing these settings.

## CUDA Prototype

The [CUDA notebook](src/cuda_gpu_accel/test.ipynb) evaluates the closest wall intersection for each ray in parallel. Each GPU thread processes rays independently, checking the available walls to find the nearest valid hit.

To run the experiment:

1. Open the notebook in Google Colab and select a GPU runtime.
2. Run the device-check and CUDA source-generation cells.
3. Run the compilation and benchmark cell. It uses `nvcc -O3` to compile the generated CUDA C++ source.
4. Inspect the CPU/GPU timing summary and mismatch count.

The later profiling cells use NVIDIA Nsight Systems and require its installation in the notebook environment. Profiling is separate from the ordinary benchmark run.

### Recorded Benchmark

The notebook contains the following results from an NVIDIA T4:

| Measurement                | Result                               |
| -------------------------- | ------------------------------------ |
| Workload                   | 100,000 rays against 2 bounded walls |
| Timed trials               | 30 per implementation, after warm-up |
| Single-threaded CPU median | 5.5248 ms                            |
| GPU kernel median          | 0.173696 ms                          |
| CPU / GPU timing ratio     | Approximately 32×                    |
| Result comparison          | 0 mismatches across 100,000 rays     |

The comparison checks hit status and, for successful hits, intersection distance, position, and normal with a numerical tolerance.

These measurements compare the CPU closest-hit loop with warmed GPU kernel execution. GPU timings exclude allocation, initial Unified Memory migration, and retrieving results on the CPU. The speedup applies to this closest-hit workload, rather than the complete visualizer or full multi-bounce tracing. Results will vary with hardware, scene complexity, and workload size.

## Tests

After building with `BUILD_TESTING=ON`, run:

```sh
ctest --test-dir build --output-on-failure
```

To display the individual test runner messages even when tests pass:

```sh
ctest --test-dir build -V
```

The registered test suite currently covers ray-plane intersection cases: straight-on hits, parallel rays, rays pointing away, rays starting on the plane, and angled hits. The CUDA notebook provides a separate CPU/GPU comparison for its benchmark workload.

## Project Structure

```text
src/
├── main.cpp              # Application setup, input, and render loop
├── math/                 # Vector operations
├── geometry/             # Rays, planes, bounded walls, intersections, receiver
├── tracing/              # Ray emission, closest hits, and reflected paths
├── animation/            # Playback state and visible ray trails
├── scene/                # Room geometry and scene presets
├── rendering/            # Camera, shaders, buffers, and drawing
└── cuda_gpu_accel/       # Standalone CUDA experiment and notebook
tests/                   # CPU geometry tests and test runner
```

## Current Limitations and Next Steps

- Geometry is limited to axis-aligned rectangular walls; arbitrary meshes are not supported.
- Exact corner and edge collisions need further handling.
- Receiver movement is not constrained by walls or room boundaries.
- GPU acceleration currently covers closest-hit queries in the standalone prototype.

Next steps include measuring GPU performance with transfer costs included, benchmarking larger wall counts, extending CUDA tracing to multiple bounces, and integrating it into the interactive visualizer.
