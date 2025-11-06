#ifndef BLUR_H
#define BLUR_H

#include "filters/filter.h"

namespace filters
{
  
class blur : public filter
{
public:
  const char *id () const override final { return "blur"; }

  bool is_enabled () const override final { return enabled; }
  void set_enabled (bool on) override final { enabled = on; }

  void set_ksize (int k)
  {
    if (k <= 1) 
      {
        ksize = 0;
        enabled = false; 
      }
    else 
      { 
        ksize = (k % 2 == 0 ? k + 1 : k); 
        enabled = true; 
      }
  }

  int get_ksize () const { return ksize; }

  void apply (const cv::Mat &src_bgr, cv::Mat &dst_bgr) override final
  {
    if (!enabled || ksize <= 1) 
      { 
        dst_bgr = src_bgr; 
        return; 
      }
    cv::GaussianBlur (src_bgr, dst_bgr, cv::Size (ksize, ksize), 0);
  }

private:
  bool enabled = false;
  int  ksize   = 0;
};

}


#endif