#ifndef TEXTURE_H
#define TEXTURE_H

#include "perlin.h"
#include "color.h"

class Texture
{
public:
  virtual ~Texture() = default;

  virtual color value(double u, double v, const point3 &p) const = 0;
};

class solid_color : public Texture
{
public:
  solid_color(const color &albedo) : albedo(albedo) {}
  solid_color(double red, double green, double blue) : solid_color(color(red, green, blue)) {}

  color value(double u, double v, const point3 &p) const override
  {
    return albedo;
  }

private:
  color albedo;
};

class CheckerTexture : public Texture
{
public:
  CheckerTexture(double scale, shared_ptr<Texture> even, shared_ptr<Texture> odd)
      : inv_scale(1.0 / scale), even(even), odd(odd) {}

  CheckerTexture(double scale, const color &c1, const color &c2)
      : CheckerTexture(scale, make_shared<solid_color>(c1), make_shared<solid_color>(c2)) {}

  color value(double u, double v, const point3 &p) const override
  {
    auto xInteger = int(std::floor(inv_scale * p.x()));
    auto yInteger = int(std::floor(inv_scale * p.y()));
    auto zInteger = int(std::floor(inv_scale * p.z()));

    bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

    return isEven ? even->value(u, v, p) : odd->value(u, v, p);
  }

private:
  double inv_scale;
  shared_ptr<Texture> even;
  shared_ptr<Texture> odd;
};


class NoiseTexture : public Texture
{
public:
  NoiseTexture(double scale) : scale(scale) {}

  color value(double u, double v, const point3 &p) const override
  {
    return color(.5, .5, .5) * (1 + std::sin(scale * p.z() + 10 * noise.turb(p, 7)));
  }

private:
  Perlin noise;
  double scale;
};

#endif
