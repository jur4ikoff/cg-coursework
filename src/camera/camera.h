#include "vec3.h"

#include <memory>
#include <exception>

class Camera
{
public:
    Camera(double fov, point3 &from, point3 &at, Vec3 &_vup) : vfov(fov), lookfrom(from), lookat(at), vup(_vup)
    {
    }
    Camera() = default;

public:
    double vfov = 90;                  // Vertical view angle (field of view)
    point3 lookfrom = point3(0, 0, 0); // Point Render is looking from
    point3 lookat = point3(0, 0, -1);  // Point Render is looking at
    Vec3 vup = Vec3(0, 1, 0);          // Camera-relative "up" direction

    double defocus_angle = 0; // Variation angle of rays through each pixel
    double focus_dist = 10;   // Distance from Render lookfrom point to plane of perfect focus
};

class CameraList
{
    CameraList()
    {
    }

    void add_camera(shared_ptr<Camera> camera)
    {
        _camera_list.push_back(camera);
    }

    shared_ptr<Camera> get_camera(size_t index)
    {
        _check_index(index);
        return _camera_list[index];
    }

    shared_ptr<Camera> get_active_camera()
    {
        _check_index(_active_camera);
        return _camera_list[_active_camera];
    }

    void set_active_camera(size_t index)
    {
        _active_camera = index;
    }

private:
    std::vector<shared_ptr<Camera>> _camera_list;
    size_t _active_camera = 0; // Индекс активной камеры

    void _check_index(int index)
    {
        if (index >= _camera_list.size())
        {
            throw std::invalid_argument("Неверно перадан размер");
        }
    }
};