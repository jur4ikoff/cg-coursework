#pragma once

#include "vec3.h"

#include <memory>
#include <exception>

class Camera
{
public:
    Camera(double fov, const Point3 &from, const Point3 &at, const Vec3 &_vup) : vfov(fov), lookfrom(from), lookat(at), vup(_vup)
    {
    }
    Camera() = default;

public:
    double vfov = 90;                  // Vertical view angle (field of view)
    Point3 lookfrom = Point3(0, 0, 0); // Point Render is looking from
    Point3 lookat = Point3(0, 0, -1);  // Point Render is looking at
    Vec3 vup = Vec3(0, 1, 0);          // Camera-relative "up" direction

    double defocus_angle = 0; // Variation angle of rays through each pixel
    double focus_dist = 10;   // Distance from Render lookfrom point to plane of perfect focus

    size_t id;
};

class CameraList
{
public:
    CameraList() = default;
    CameraList(std::shared_ptr<Camera> &camera)
    {
        add_camera(camera);
    }

    void add_camera(shared_ptr<Camera> camera)
    {
        camera->id = _max_id;
        _max_id++;

        if (_camera_list.size() == 0)
            _active_camera = camera->id;

        _camera_list.push_back(camera);
    }

    void delete_camera(size_t id)
    {
        if (id == _active_camera)
            _active_camera = -1;

        auto it = std::find_if(_camera_list.begin(), _camera_list.end(),
                               [id](const auto &camera)
                               {
                                   return camera->id == id;
                               });

        if (it != _camera_list.end())
        {
            _camera_list.erase(it);
        }
    }

    shared_ptr<Camera> &get_camera(int camera_id)
    {
        int index = _find_element(camera_id);
        _check_index(index);
        return _camera_list[index];
    }

    int _find_element(int camera_id)
    {
        for (size_t i = 0; i < _camera_list.size(); i++)
        {
            if (_camera_list[i]->id == camera_id)
            {
                return i;
            }
        }
        return -1;
    }

    shared_ptr<Camera> &get_active_camera()
    {
        int index = _find_element(_active_camera);
        _check_index(index);
        return _camera_list[index];
    }

    void set_active_camera(int index)
    {
        _active_camera = index;
    }

    const std::vector<shared_ptr<Camera>> &get_camera_list()
    {
        return _camera_list;
    }

    std::vector<size_t> get_camera_ids()
    {
        std::vector<size_t> ids;

        for (auto camera : _camera_list)
        {
            ids.push_back(camera->id);
        }

        return ids;
    }

    size_t get_max_id()
    {
        return _max_id;
    }

private:
    std::vector<shared_ptr<Camera>> _camera_list;
    int _active_camera = 0; // Индекс активной камеры
    size_t _max_id = 0;

    void _check_index(int index)
    {
        if (index >= _camera_list.size() || index < 0)
        {
            throw std::invalid_argument("Такой камеры не существует");
        }
    }
};