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

class SolidColor : public Texture
{
public:
  SolidColor(const color &albedo) : albedo(albedo) {}
  SolidColor(double red, double green, double blue) : SolidColor(color(red, green, blue)) {}

  color value(double u, double v, const point3 &p) const override
  {
    return albedo;
  }

private:
  color albedo;
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

