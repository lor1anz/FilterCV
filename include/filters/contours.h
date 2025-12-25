#ifndef CONTOURS_H
#define CONTOURS_H

#include "filters/filter.h"
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <vector>

namespace filters
{

class contours final : public filter
{
public:
  const char *id () const override final { return "contours"; }

  bool is_enabled () const override final { return enabled; }
  void set_enabled (bool on) override final { enabled = on; }

  void set_epsilon (double v)
  {
    epsilon = std::clamp (v, 0.001, 0.2);
  }
  double get_epsilon () const { return epsilon; }

  void set_min_area (double v)
  {
    min_area = std::max (0.0, v);
  }
  double get_min_area () const { return min_area; }

  void set_draw_approx (bool on) { draw_approx = on; }
  bool get_draw_approx () const { return draw_approx; }

  void apply (const cv::Mat &src_bgr, cv::Mat &dst_bgr) override final
  {
    if (!enabled)
    {
      dst_bgr = src_bgr;
      return;
    }

    cv::Mat gray;
    if (src_bgr.channels () == 1)
      gray = src_bgr;
    else
      cv::cvtColor (src_bgr, gray, cv::COLOR_BGR2GRAY);

    cv::Mat bin;
    cv::threshold (gray, bin, 128, 255, cv::THRESH_BINARY);

    std::vector<std::vector<cv::Point>> found;
    cv::findContours (bin, found, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    dst_bgr = src_bgr.clone ();

    for (const auto &cnt : found)
    {
      const double area = cv::contourArea (cnt);
      if (area < min_area)
        continue;

      if (draw_approx)
      {
        std::vector<cv::Point> approx;
        const double eps = epsilon * cv::arcLength (cnt, true);
        cv::approxPolyDP (cnt, approx, eps, true);

        cv::polylines (dst_bgr, approx, true,
                        cv::Scalar (0, 255, 0), 2);
      }
      else
      {
        cv::drawContours (dst_bgr,
                          std::vector<std::vector<cv::Point>>{cnt},
                          -1,
                          cv::Scalar (255, 0, 0), 2);
      }
    }
  }

private:
  bool enabled = false;
  double epsilon = 0.02;
  double min_area = 100.0;
  bool draw_approx = true;
};

}

#endif
