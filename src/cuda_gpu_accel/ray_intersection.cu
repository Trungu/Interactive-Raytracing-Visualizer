
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <random>
#include <vector>
#include <chrono>

using std::abs, std::cout;
constexpr double tolerance = 0.000001;

struct Vec3 
{
    double x;
    double y;
    double z;

    __host__ __device__
    Vec3 operator+(const Vec3 &other_vec) const 
    {
        Vec3 new_vec = (*this);

        new_vec.x += other_vec.x;
        new_vec.y += other_vec.y;
        new_vec.z += other_vec.z;

        return new_vec;
    }

    __host__ __device__
    Vec3 operator-(const Vec3 &other_vec) const 
    {
        Vec3 new_vec = (*this);

        new_vec.x -= other_vec.x;
        new_vec.y -= other_vec.y;
        new_vec.z -= other_vec.z;

        return new_vec;
    }

    __host__ __device__
    Vec3 operator*(double s) const 
    {
        Vec3 new_vec = (*this);

        new_vec.x *= s;
        new_vec.y *= s;
        new_vec.z *= s;

        return new_vec;
    }
};

struct Ray
{
    Vec3 origin;
    Vec3 direction;
};

struct Plane
{
    Vec3 normal;
    Vec3 ref; // A point on the plane
};

struct Wall
{
    Plane plane;
    Vec3 minCorner;
    Vec3 maxCorner;

    __host__ __device__
    bool containsPoint(const Vec3 &point) const;
};

struct Intersection
{
    double t;
    Vec3 position;
    Vec3 normal;
    bool hit;
};

__host__ __device__
double dot(const Vec3 &a, const Vec3 &b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

__host__ __device__
bool Wall::containsPoint(const Vec3 &point) const
{
    if (std::abs(dot(plane.normal, point - plane.ref)) > tolerance)
        return false;

    if (!(minCorner.x - tolerance <= point.x && point.x <= maxCorner.x + tolerance) ||
        !(minCorner.y - tolerance <= point.y && point.y <= maxCorner.y + tolerance) ||
        !(minCorner.z - tolerance <= point.z && point.z <= maxCorner.z + tolerance))
        return false;

    return true;
}

__host__ __device__
Intersection intersect_plane(const Ray &ray, const Plane &plane)
{
    double n_d = dot(plane.normal, ray.direction);

    // for this we check if n_d is within a certain tolerance (i.e some value close to 0)
    if (abs(n_d) < tolerance || !isfinite(n_d))
        return Intersection{0, {0, 0, 0}, {0, 0, 0}, false};

    double t = (dot(plane.normal, plane.ref - ray.origin)) / n_d;

    // an intersection is invalid if t is not strictly greater than tolerance
    if (t <= tolerance || !isfinite(t))
        return Intersection{0, {0, 0, 0}, {0, 0, 0}, false};

    Vec3 pos = ray.origin + (ray.direction * t);

    return Intersection{t, pos, plane.normal, true};
}

__host__ __device__
Intersection intersect_wall(const Ray &ray, const Wall &wall)
{
    auto intersection = intersect_plane(ray, wall.plane);

    if (intersection.hit)
    {
        Vec3 point = intersection.position;

        if (!wall.containsPoint(point))
            intersection.hit = false;
    }

    return intersection;
};

__host__ __device__
Intersection find_closest_hit(const Ray &incoming, const Wall *walls, int wall_count)
{
    Intersection closest_hit{0, {0, 0, 0}, {0, 0, 0}, false};

    for (int i = 0; i < wall_count; ++i)
    {
        Intersection intersection = intersect_wall(incoming, walls[i]);

        if (!intersection.hit)
            continue;

        if (!closest_hit.hit)
        {
            closest_hit = intersection;
        }
        else if (intersection.t < closest_hit.t)
        {
            closest_hit = intersection;
        }
    }

    return closest_hit;
}

// CUDA kernel
__global__
void intersect_ray_with_wall(
    Ray *rays, int ray_count, const Wall *walls, int wall_count, Intersection *results) 
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    
    for (int i = index; i < ray_count; i += blockDim.x * gridDim.x) 
    {
        results[i] = find_closest_hit(rays[i], walls, wall_count);
    }
    
}

int main() {
    constexpr int wall_count = 2;
    constexpr int trial_count = 30;

    std::vector<double> cpu_timings = {};
    std::vector<double> gpu_timings = {};
    cpu_timings.reserve(trial_count);
    gpu_timings.reserve(trial_count);

    Wall test_walls[wall_count]{
        { // Farther wall first: closest-hit selection should replace this hit.
            {{1, 0, 0}, {8, 0, 0}},
            {8, -2, -2},
            {8, 2, 2}
        },
        { // Closer wall: ray 0 should hit here at t = 5.
            {{1, 0, 0}, {5, 0, 0}},
            {5, -2, -2},
            {5, 2, 2}
        }
    };

    constexpr int ray_count = 100000;
    // Keep the four known cases, then fill the remaining slots randomly.
    std::vector<Ray> test_rays{
        {{0, 0, 0}, {1, 0, 0}},  
        {{0, 3, 0}, {1, 0, 0}},  
        {{0, 0, 0}, {0, 1, 0}},  
        {{0, 0, 0}, {-1, 0, 0}}  
    };

    test_rays.reserve(ray_count);
    std::mt19937 generator(42); // Fixed seed for repeatable test data.
    std::uniform_real_distribution<double> origin_x(-2.0, 10.0);
    std::uniform_real_distribution<double> origin_yz(-4.0, 4.0);
    std::uniform_real_distribution<double> random01(0.0, 1.0);
    const double pi = std::acos(-1.0);

    while (test_rays.size() < ray_count)
    {
        Vec3 origin{origin_x(generator), origin_yz(generator), origin_yz(generator)};

        // Sample a unit direction on the sphere, with Z as the vertical axis.
        double z = 2.0 * random01(generator) - 1.0;
        double angle = 2.0 * pi * random01(generator);
        double radius = std::sqrt(std::max(0.0, 1.0 - z * z));
        Vec3 direction{radius * std::cos(angle), radius * std::sin(angle), z};

        test_rays.push_back(Ray{origin, direction});
    }

    Ray *rays = nullptr;
    Intersection *results = nullptr;
    Wall *walls = nullptr;

    cudaError_t err = cudaMallocManaged(&rays, ray_count * sizeof(Ray));
    if (err != cudaSuccess) 
    {
        cout << "CUDA Allocation Error: " << cudaGetErrorString(err) << "\n";
        return -1;
    }

    err = cudaMallocManaged(&results, ray_count * sizeof(Intersection));
    if (err != cudaSuccess)
    {
        cout << "CUDA Allocation Error" << cudaGetErrorString(err) << "\n";

        cudaFree(rays);
        return -1;
    }

    err = cudaMallocManaged(&walls, wall_count * sizeof(Wall));
    if (err != cudaSuccess)
    {
        cout << "CUDA Allocation Error" << cudaGetErrorString(err) << "\n";

        cudaFree(rays);
        cudaFree(results);

        return -1;
    }

    for (size_t i = 0; i < wall_count; ++i) 
    {
        walls[i] = test_walls[i];
    }

    for (size_t i = 0; i < ray_count; ++i) 
    {
        rays[i] = test_rays[i];
    }


    // CPU results for comparison
    
    // warm-up run
    std::vector<Intersection> cpu_results(ray_count);

    for (size_t i = 0; i < ray_count; ++i)
    {
        cpu_results[i] = find_closest_hit(rays[i], walls, wall_count);
    }
    
    // timed run
    for (size_t i = 0; i < trial_count; ++i) 
    {
        auto start = std::chrono::steady_clock::now();
    
        for (size_t i = 0; i < ray_count; ++i)
        {
            cpu_results[i] = find_closest_hit(rays[i], walls, wall_count);
        }
    
        auto end = std::chrono::steady_clock::now();
    
        double elapsed_ms_cpu =
            std::chrono::duration<double, std::milli>(end - start).count();

        cpu_timings.push_back(elapsed_ms_cpu);
    }

    std::sort(cpu_timings.begin(), cpu_timings.end());
    double elapsed_ms_med = (cpu_timings[14] + cpu_timings[15]) / 2;

    cout << "Benchmarking for " << ray_count << " rays:\n";
    cout << "CPU Median Elapsed Time: " << elapsed_ms_med << " ms" << "\n";

    // GPU results
    constexpr int threads_per_block = 256;
    const int block_count = (ray_count + threads_per_block - 1) / threads_per_block;

    cudaEvent_t gpu_start = nullptr;
    cudaEvent_t gpu_stop = nullptr;

    cudaError_t err_start = cudaEventCreate(&gpu_start);
    cudaError_t err_stop = cudaEventCreate(&gpu_stop);

    if (err_start != cudaSuccess || err_stop != cudaSuccess) {
        if (err_start != cudaSuccess)
            cout << "CUDA Start Event Creation Error: " << cudaGetErrorString(err_start) << '\n';
        else
            cudaEventDestroy(gpu_start);

        if (err_stop != cudaSuccess)
            cout << "CUDA Stop Event Creation Error: " << cudaGetErrorString(err_stop) << '\n';
        else
            cudaEventDestroy(gpu_stop);

        cudaFree(rays);
        cudaFree(results);
        cudaFree(walls);
        return -1;
    }

    // Untimed warm-up; keep managed arrays on the GPU until the timed run finishes.
    intersect_ray_with_wall<<<block_count, threads_per_block>>>(
        rays, ray_count, walls, wall_count, results);

    err = cudaGetLastError();
    if (err != cudaSuccess)
    {
        cout << "CUDA Warm-up Launch Error: " << cudaGetErrorString(err) << '\n';
        cudaEventDestroy(gpu_start);
        cudaEventDestroy(gpu_stop);
        cudaFree(rays);
        cudaFree(results);
        cudaFree(walls);
        return -1;
    }

    err = cudaDeviceSynchronize();
    if (err != cudaSuccess)
    {
        cout << "CUDA Warm-up Sync Error: " << cudaGetErrorString(err) << '\n';
        cudaEventDestroy(gpu_start);
        cudaEventDestroy(gpu_stop);
        cudaFree(rays);
        cudaFree(results);
        cudaFree(walls);
        return -1;
    }

    for (int trial = 0; trial < trial_count; ++trial)
    {
        err = cudaEventRecord(gpu_start, 0);
        if (err != cudaSuccess)
        {
            cout << "CUDA Start Event Record Error: " << cudaGetErrorString(err) << '\n';

            cudaEventDestroy(gpu_start);
            cudaEventDestroy(gpu_stop);
            cudaFree(rays);
            cudaFree(results);
            cudaFree(walls);
            return -1;
        }
        intersect_ray_with_wall<<<block_count, threads_per_block>>>(
            rays, 
            ray_count, 
            walls,
            wall_count, 
            results
        );

        err = cudaGetLastError();
        if (err != cudaSuccess) 
        {
            cout << "CUDA Launch Error: " << cudaGetErrorString(err) << '\n';

            cudaEventDestroy(gpu_start);
            cudaEventDestroy(gpu_stop);
            cudaFree(rays);
            cudaFree(results);
            cudaFree(walls);

            return -1;
        }

        err = cudaEventRecord(gpu_stop, 0);
        if (err != cudaSuccess)
        {
            cout << "CUDA Stop Event Record Error: " << cudaGetErrorString(err) << '\n';

            cudaEventDestroy(gpu_start);
            cudaEventDestroy(gpu_stop);
            cudaFree(rays);
            cudaFree(results);
            cudaFree(walls);
            return -1;
        }

        err = cudaDeviceSynchronize();
        if (err != cudaSuccess) 
        {
            cout << "CUDA Sync Error: " << cudaGetErrorString(err) << '\n';

            cudaEventDestroy(gpu_start);
            cudaEventDestroy(gpu_stop);
            cudaFree(rays);
            cudaFree(results);
            cudaFree(walls);

            return -1;
        }

        float elapsed_ms_gpu = 0;
        err = cudaEventElapsedTime(&elapsed_ms_gpu, gpu_start, gpu_stop);
        if (err != cudaSuccess)
        {
            cout << "CUDA Event Timing Error: " << cudaGetErrorString(err) << '\n';

            cudaEventDestroy(gpu_start);
            cudaEventDestroy(gpu_stop);
            cudaFree(rays);
            cudaFree(results);
            cudaFree(walls);
            return -1;
        }

        gpu_timings.push_back(elapsed_ms_gpu);
    }

    err_start = cudaEventDestroy(gpu_start);
    err_stop = cudaEventDestroy(gpu_stop);
    if (err_start != cudaSuccess || err_stop != cudaSuccess)
    {
        if (err_start != cudaSuccess)
            cout << "CUDA Start Event Cleanup Error: " << cudaGetErrorString(err_start) << '\n';
        if (err_stop != cudaSuccess)
            cout << "CUDA Stop Event Cleanup Error: " << cudaGetErrorString(err_stop) << '\n';

        cudaFree(rays);
        cudaFree(results);
        cudaFree(walls);
        return -1;
    }

    std::sort(gpu_timings.begin(), gpu_timings.end());
    const size_t middle = gpu_timings.size() / 2;
    const double gpu_median_ms = (gpu_timings.size() % 2 == 0)
        ? (gpu_timings[middle - 1] + gpu_timings[middle]) / 2.0
        : gpu_timings[middle];
    cout << "GPU Median Kernel Execution Time: " << gpu_median_ms << " ms" << "\n";

    int mismatch_count = 0;
    for (size_t i = 0; i < ray_count; ++i)
    {
        const Intersection &cpu = cpu_results[i];
        const Intersection &gpu = results[i];
        bool matches = (cpu.hit == gpu.hit);

        if (cpu.hit && gpu.hit)
        {
            matches = std::abs(cpu.t - gpu.t) <= tolerance &&
                      std::abs(cpu.position.x - gpu.position.x) <= tolerance &&
                      std::abs(cpu.position.y - gpu.position.y) <= tolerance &&
                      std::abs(cpu.position.z - gpu.position.z) <= tolerance &&
                      std::abs(cpu.normal.x - gpu.normal.x) <= tolerance &&
                      std::abs(cpu.normal.y - gpu.normal.y) <= tolerance &&
                      std::abs(cpu.normal.z - gpu.normal.z) <= tolerance;
        }

        if (!matches)
        {
            ++mismatch_count;
            cout << "CPU/GPU mismatch for ray " << i << '\n';
        }
    }
    cout << "CPU/GPU comparison: " << ray_count << " rays checked, "
         << mismatch_count << " mismatches\n";

    cudaFree(rays);
    cudaFree(results);
    cudaFree(walls);

}
