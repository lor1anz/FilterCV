#ifndef MORPHOLOGY_H
#define MORPHOLOGY_H

#include "filters/filter.h"
#include <opencv2/opencv.hpp>
#include <algorithm>

namespace filters
{

class morphology final : public filter
{
public:
  enum class op_t
  {
    erode,
    dilate,
    open,
    close
  };

  const char *id () const override final { return "morphology"; }

  bool is_enabled () const override final { return enabled; }
  void set_enabled (bool on) override final { enabled = on; }

  void set_op (op_t o) { op = o; }
  op_t get_op () const { return op; }

  void set_kernel_size (int v)
  {
    v = std::max (1, v);
    if ((v % 2) == 0) ++v;
    kernel_size = v;
  }
  int get_kernel_size () const { return kernel_size; }

  void set_iterations (int v)
  {
    iterations = std::max (1, v);
  }
  int get_iterations () const { return iterations; }

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

    cv::Mat kernel = cv::getStructuringElement (
      cv::MORPH_RECT,
      cv::Size (kernel_size, kernel_size)
    );

    cv::Mat out;

    switch (op)
    {
      case op_t::erode:
        cv::erode (bin, out, kernel, cv::Point (-1, -1), iterations);
        break;

      case op_t::dilate:
        cv::dilate (bin, out, kernel, cv::Point (-1, -1), iterations);
        break;

      case op_t::open:
        cv::morphologyEx (bin, out, cv::MORPH_OPEN, kernel,
                          cv::Point (-1, -1), iterations);
        break;

      case op_t::close:
        cv::morphologyEx (bin, out, cv::MORPH_CLOSE, kernel,
                          cv::Point (-1, -1), iterations);
        break;
    }

    cv::cvtColor (out, dst_bgr, cv::COLOR_GRAY2BGR);
  }

private:
  bool enabled = false;
  op_t op = op_t::open;
  int kernel_size = 3;
  int iterations = 1;
};

}

#endif