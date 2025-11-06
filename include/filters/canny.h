#ifndef CANNY_H
#define CANNY_H

#include "filters/filter.h"

namespace filters
{
  
class canny : public filter
{
public:
  const char *id () const override final { return "canny"; }

  bool is_enabled () const override final { return enabled; }
  void set_enabled (bool on) override final { enabled = on; }

  void set_thresholds (double l, double h)
  {
    if (l < 0.0) 
      l = 0.0;
    if (h < 0.0) 
      h = 0.0;
    if (h < l)
      std::swap (l, h);
    low = l; 
    high = h;
  }

  double get_low ()  const { return low; }
  double get_high () const { return high; }

  void apply (const cv::Mat& src_bgr, cv::Mat& dst_bgr) override final
  {
    if (!enabled) 
      { 
        dst_bgr = src_bgr; 
        return; 
      }

    cv::Mat gray, edges;
    if (src_bgr.channels () == 1)
      gray = src_bgr;
    else
      cv::cvtColor (src_bgr, gray, cv::COLOR_BGR2GRAY);

    cv::Canny (gray, edges, low, high);
    cv::cvtColor (edges, dst_bgr, cv::COLOR_GRAY2BGR);
  }

private:
  bool enabled = false;
  double low = 50.0;
  double high = 150.0;
};

}


#endif