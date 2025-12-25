#ifndef THRESHOLD_H
#define THRESHOLD_H

#include "filters/filter.h"
#include <opencv2/opencv.hpp>
#include <algorithm>

namespace filters
{

class threshold final : public filter
{
public:
  enum class mode_t
  {
    binary,
    adaptive_mean,
    adaptive_gaussian
  };

  const char *id () const override final { return "threshold"; }

  bool is_enabled () const override final { return enabled; }
  void set_enabled (bool on) override final { enabled = on; }

  void set_mode (mode_t m) { mode = m; }
  mode_t get_mode () const { return mode; }

  void set_thresh (int v) { thresh = std::clamp (v, 0, 255); }
  int get_thresh () const { return thresh; }

  void set_block_size (int v)
  {
    v = std::max (3, v);
    if ((v % 2) == 0) ++v;
    block_size = v;
  }
  int get_block_size () const { return block_size; }

  void set_c (int v) { c = std::clamp (v, -50, 50); }
  int get_c () const { return c; }

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

    switch (mode)
    {
      case mode_t::binary:
        cv::threshold (gray, bin, thresh, 255, cv::THRESH_BINARY);
        break;

      case mode_t::adaptive_mean:
        cv::adaptiveThreshold (
          gray, bin, 255,
          cv::ADAPTIVE_THRESH_MEAN_C,
          cv::THRESH_BINARY,
          block_size, c
        );
        break;

      case mode_t::adaptive_gaussian:
        cv::adaptiveThreshold (
          gray, bin, 255,
          cv::ADAPTIVE_THRESH_GAUSSIAN_C,
          cv::THRESH_BINARY,
          block_size, c
        );
        break;
    }

    cv::cvtColor (bin, dst_bgr, cv::COLOR_GRAY2BGR);
  }

private:
  bool enabled = false;
  mode_t mode = mode_t::binary;
  int thresh = 128;
  int block_size = 11;
  int c = 2;
};

}

#endif
