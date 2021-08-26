#pragma once
#include <liblava/lava.hpp>

#include <cmath>

namespace crow {

struct ray {
  glm::vec3 position;
  glm::vec3 normalized_angle;
  glm::vec3 normalized_angle_inverse;
};

struct axis_aligned_bounding_box {
  float x, y, z, width, height, length;  // NOLINT

  auto intersect_ray(crow::ray& ray) const -> bool {
    // Clip ray along X.
    float tx1 = (this->x - ray.position.x) * ray.normalized_angle_inverse.x;
    float tx2 = (this->x + this->width - ray.position.x) *
                ray.normalized_angle_inverse.x;
    float tmin = std::fmin(tx1, tx2);
    float tmax = std::fmax(tx1, tx2);

    // Clip ray along Y.
    float ty1 = (this->y - ray.position.y) * ray.normalized_angle_inverse.y;
    float ty2 = (this->y + this->height - ray.position.y) *
                ray.normalized_angle_inverse.y;
    tmin = std::fmax(tmin, std::fmin(ty1, ty2));
    tmax = std::fmin(tmax, std::fmax(ty1, ty2));

    // Clip ray along Z.
    float tz1 = (this->z - ray.position.z) * ray.normalized_angle_inverse.z;
    float tz2 = (this->z + this->length - ray.position.z) *
                ray.normalized_angle_inverse.z;
    tmin = std::fmax(tmin, std::fmin(tz1, tz2));
    tmax = std::fmin(tmax, std::fmax(tz1, tz2));

    return tmax >= tmin;
  }
};

}  // namespace crow
