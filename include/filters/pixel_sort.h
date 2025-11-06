#ifndef PIXEL_SORT_H
#define PIxEL_SORT_H

#include "filters/filter.h"

namespace filters
{
  
class pixel_sort final : public filter
{
public:
  enum class axis_t { horizontal, vertical };

  const char *id () const override final { return "pixel_sort"; }

  bool is_enabled () const override final { return enabled; }
  void set_enabled (bool on) override final { enabled = on; }

  void set_axis (axis_t a) { axis = a; }
  axis_t get_axis () const { return axis; }

  void set_chunk (int value) { chunk = std::max (1, value); }
  int get_chunk () const { return chunk; }

  void set_reverse (bool on) { reverse = on; }
  bool get_reverse () const { return reverse; }

  void set_stride (int s) { stride = std::max (1, s); }
  int get_stride () const { return stride; }

  void apply (const cv::Mat &src_bgr, cv::Mat &dst_bgr) override final
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
    else                               
      cv::cvtColor (src_bgr, bgr, cv::COLOR_GRAY2BGR);

    dst_bgr = bgr.clone ();

    if (axis == axis_t::horizontal)
      sort_rows (dst_bgr);
    else
      sort_cols (dst_bgr);
  }

private:
  static inline int luma_key (const cv::Vec3b &pix)
  {
    const int b = pix[0], g = pix[1], r = pix[2];
    return 29 * b + 150 * g + 77 * r;
  }

  void sort_rows (cv::Mat &img)
  {
    const int rows = img.rows, cols = img.cols;
    std::vector<std::pair<int, cv::Vec3b>> buf;
    buf.reserve (chunk);

    for (int y = 0; y < rows; y += stride)
      {
        cv::Vec3b *row = img.ptr<cv::Vec3b> (y);
        for (int x = 0; x < cols; x += chunk)
          {
            const int w = std::min (chunk, cols - x);
            buf.resize (w);
            for (int i = 0; i < w; ++i)
              buf[i] = { luma_key (row[x + i]), row[x + i] };

            if (!reverse)
              std::sort (buf.begin (), buf.end (), [] (auto &a, auto &b) { return a.first < b.first; });
            else
              std::sort (buf.begin (), buf.end (), [] (auto &a, auto &b) { return a.first > b.first; });

            for (int i = 0; i < w; ++i)
              row[x + i] = buf[i].second;
          }
      }
  }

  void sort_cols (cv::Mat &img)
  {
    const int rows = img.rows, cols = img.cols;
    std::vector<std::pair<int, cv::Vec3b>> buf;
    buf.reserve (chunk);

    for (int x = 0; x < cols; x += stride)
      {
        for (int y0 = 0; y0 < rows; y0 += chunk)
          {
            const int h = std::min (chunk, rows - y0);
            buf.resize (h);

            for (int i = 0; i < h; ++i)
              {
                const cv::Vec3b &pix = img.at<cv::Vec3b> (y0 + i, x);
                buf[i] = { luma_key (pix), pix };
              }

            if (!reverse)
              std::sort (buf.begin (), buf.end (), [] (auto &a, auto &b) { return a.first < b.first; });
            else
              std::sort (buf.begin (), buf.end (), [] (auto &a, auto &b) { return a.first > b.first; });

            for (int i = 0; i < h; ++i)
              img.at<cv::Vec3b> (y0 + i, x) = buf[i].second;
          }
      }
  }

private:
  bool enabled = false;
  axis_t axis = axis_t::vertical;
  int chunk = 32;
  int stride = 1;
  bool reverse = false;
};

}


#endif