#ifndef AFFINE_H
#define AFFINE_H

#include "filters/filter.h"
#include <opencv2/opencv.hpp>
#include <algorithm>

namespace filters
{

class affine final : public filter
{
public:
  const char *id () const override final { return "affine"; }

  bool is_enabled () const override final { return enabled; }
  void set_enabled (bool on) override final { enabled = on; }

  void set_angle (double v) { angle = std::clamp (v, -180.0, 180.0); }
  double get_angle () const { return angle; }

  void set_scale (double v) { scale = std::clamp (v, 0.1, 3.0); }
  double get_scale () const { return scale; }

  void set_tx (int v) { tx = v; }
  int get_tx () const { return tx; }

  void set_ty (int v) { ty = v; }
  int get_ty () const { return ty; }

  void apply (const cv::Mat &src_bgr, cv::Mat &dst_bgr) override final
  {
    if (!enabled)
    {
      dst_bgr = src_bgr;
      return;
    }

    const cv::Point2f center (
      static_cast<float>(src_bgr.cols) / 2.0f,
      static_cast<float>(src_bgr.rows) / 2.0f
    );

    cv::Mat M = cv::getRotationMatrix2D (center, angle, scale);

    // добавляем сдвиг
    M.at<double>(0, 2) += tx;
    M.at<double>(1, 2) += ty;

    cv::warpAffine (
      src_bgr, dst_bgr, M,
      src_bgr.size (),
      cv::INTER_LINEAR,
      cv::BORDER_CONSTANT,
      cv::Scalar (0, 0, 0)
    );
  }

private:
  bool enabled = false;
  double angle = 0.0;
  double scale = 1.0;
  int tx = 0;
  int ty = 0;
};

}

#endif
