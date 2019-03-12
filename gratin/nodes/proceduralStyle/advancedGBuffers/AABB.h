#pragma once

struct AABB {
  float xmin;
  float ymin;
  float zmin;
  float xmax;
  float ymax;
  float zmax;

  float width() const { return xmax - xmin; }
  float height() const { return ymax - ymin; }
  float depth() const { return zmax - zmin; }

  AABB transform(const Matrix4f &m) const
  {
      auto xa = m.col(0) * xmin;
      auto xb = m.col(0) * xmax;

      auto ya = m.col(1) * ymin;
      auto yb = m.col(1) * ymax;

      auto za = m.col(2) * zmin;
      auto zb = m.col(2) * zmax;

      auto vmin = xa.cwiseMin(xb) + ya.cwiseMin(yb) + za.cwiseMin(zb) + m.col(3);
      auto vmax = xa.cwiseMax(xb) + ya.cwiseMax(yb) + za.cwiseMax(zb) + m.col(3);

      return AABB{ vmin.x(), vmin.y(), vmin.z(), vmax.x(), vmax.y(), vmax.z() };
  }
};

