// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "custom_types.h"
#include "math.h"

Vec2 eqRectProjection(const LatLng& target, const LatLng& ref) {
  const float R = 6371000.0;
  float lat0_r = ref.lat_ * M_PI / 180;
  float lng0_r = ref.lng_ * M_PI / 180;
  float lat1_r = target.lat_ * M_PI / 180;
  float lng1_r = target.lng_ * M_PI / 180;
  float lat0_cos = cos((lat0_r + lat1_r) * 0.5f);


  Vec2 res;

  res.x_ = (lng1_r - lng0_r) * lat0_cos * R;
  res.y_ = (lat1_r - lat0_r) * R;
  return res;
}

LatLng eqRectInverse(const Vec2& point, const LatLng& ref) {
    const float R = 6371000.0f;

    float lat0 = ref.lat_ * M_PI / 180.0f;
    float lng0 = ref.lng_ * M_PI / 180.0f;

    float cos_lat0 = cos(lat0);

    // Recover latitude
    float lat = lat0 + (point.y_ / R);

    // Recover longitude
    float lng = lng0 + (point.x_ / (R * cos_lat0));

    LatLng res;
    res.lat_ = lat * 180.0f / M_PI;
    res.lng_ = lng * 180.0f / M_PI;

    return res;
}

Vec2 vec2Midpoint(const Vec2& A, const Vec2& B) {
  Vec2 res;
  res.x_ = (A.x_ + B.x_) * 0.5f;
  res.y_ = (A.y_ + B.y_) * 0.5f;
  return res;
}

Vec2 vec2Sum(const Vec2& A, const Vec2& B) {
  Vec2 res;
  res.x_ = A.x_ + B.x_;
  res.y_ = A.y_ + B.y_;
  return res;
}

float vecMod(const Vec2& in) {
  return sqrtf(in.x_*in.x_ + in.y_*in.y_);
}

float vec2SqDist(const Vec2& A, const Vec2& B) {
  float deltaX = B.x_ - A.x_;
  float deltaY = B.y_ - A.y_;
  return deltaX * deltaX + deltaY * deltaY;
}


float vec2Cross(const Vec2& B, const Vec2& M) {
  return B.x_ * M.y_ - B.y_ * M.x_;
}