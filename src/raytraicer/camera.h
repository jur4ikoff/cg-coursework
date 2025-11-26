#include "vec3.h"

class Camera
{
public:
    Camera(double fov, point3 &from, point3 &at, vec3 &_vup) : vfov(fov), lookfrom(from), lookat(at), vup(_vup)
    {
    }
    Camera() = default;

public:
    double vfov = 90;                  // Vertical view angle (field of view)
    point3 lookfrom = point3(0, 0, 0); // Point Render is looking from
    point3 lookat = point3(0, 0, -1);  // Point Render is looking at
    vec3 vup = vec3(0, 1, 0);          // Camera-relative "up" direction

    double defocus_angle = 0; // Variation angle of rays through each pixel
    double focus_dist = 10;   // Distance from Render lookfrom point to plane of perfect focus
};