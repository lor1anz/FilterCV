#ifndef SHARPEN_H
#define SHARPEN_H

#include "filters/filter.h"

namespace filters
{

class sharpen final : public filter
{
public:
  const char *id () const override final { return "sharpen"; }

  bool is_enabled () const override final { return enabled; }
  void set_enabled (bool on) override final { enabled = on; }

  void set_amount (double value)
  {
    amount = std::clamp (value, 0.0, 3.0);
  }

  void set_radius (int value)
  {
    radius = std::clamp (value, 1, 15);
  }

  void set_threshold (int value)
  {
    threshold = std::clamp (value, 0, 255);
  }

  double get_amount () const { return amount; }
  int get_radius () const { return radius; }
  int get_threshold () const { return threshold; }

  void apply (const cv::Mat &src_bgr, cv::Mat &dst_bgr) override final
  {
    if (!enabled)
    {
      dst_bgr = src_bgr;
      return;
    }

    cv::Mat blurred, low_contrast_mask, sharpened;

    cv::GaussianBlur (src_bgr, blurred, cv::Size (radius * 2 + 1, radius * 2 + 1), 0);

    cv::addWeighted (src_bgr, 1.0 + amount, blurred, -amount, 0, sharpened);

    cv::Mat diff;
    cv::absdiff (src_bgr, blurred, diff);
    cv::cvtColor (diff, diff, cv::COLOR_BGR2GRAY);
    cv::threshold (diff, low_contrast_mask, threshold, 255, cv::THRESH_BINARY);

    dst_bgr = src_bgr.clone ();
    sharpened.copyTo (dst_bgr, low_contrast_mask);
  }

private:
  bool enabled = false;
  double amount = 1.0;
  int radius = 3;
  int threshold = 10;
};

}

#endif