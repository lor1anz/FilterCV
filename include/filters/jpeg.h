#ifndef JPEG_H
#define JPEG_H

#include "filters/filter.h"

namespace filters
{

class jpeg : public filter
{
public:
  const char *id () const override { return "jpeg"; }

  bool is_enabled () const override { return enabled; }
  void set_enabled (bool on) override { enabled = on; }

  void set_quality (int q)
  {
    quality = std::clamp (q, 0, 100);
  }

  int get_quality () const { return quality; }

  void apply (const cv::Mat& src_bgr, cv::Mat& dst_bgr) override
{
  if (!enabled) 
    { 
      dst_bgr = src_bgr; 
      return;
    }

  cv::Mat bgr;
  if (src_bgr.channels () == 3)        
    bgr = src_bgr;
  else if (src_bgr.channels () == 4)   
    cv::cvtColor (src_bgr, bgr, cv::COLOR_BGRA2BGR);
  else if (src_bgr.channels () == 1)   
    cv::cvtColor (src_bgr, bgr, cv::COLOR_GRAY2BGR);
  else                                 
    src_bgr.copyTo (bgr);

  std::vector<uchar> buf;
  std::vector<int> params { cv::IMWRITE_JPEG_QUALITY, quality };
  cv::imencode (".jpg", bgr, buf, params);
  dst_bgr = cv::imdecode (buf, cv::IMREAD_COLOR);
}

private:
  bool enabled = false;
  int quality = 80;
};

}

#endif