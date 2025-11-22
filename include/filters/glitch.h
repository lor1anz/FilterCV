#ifndef GLITCH_H
#define GLITCH_H

#include "filters/jpeg.h"

namespace filters
{

class glitch final : public jpeg
{
public:
  const char *id () const override final { return "glitch"; }

  void set_strength (int s) { strength = std::clamp (s, 1, 30); }

  int get_strength () const { return strength; }

  void apply (const cv::Mat &src_bgr, cv::Mat &dst_bgr) override final
  {
    if (!is_enabled ())
      {
        dst_bgr = src_bgr;
        return;
      }

    set_quality (1);
    cv::Mat cur = src_bgr.clone ();
    jpeg::apply (cur, dst_bgr);

    // ==========================
    // PHASE 2 — CHANNEL SHIFT
    // ==========================

    {
      std::vector<cv::Mat> ch;
      cv::split (cur, ch);

      int s = strength;

      cv::Mat M1 = (cv::Mat_<double>(2,3) << 1,0,  0, 0,1, s);
      cv::warpAffine (ch[0], ch[0], M1, ch[0].size(), cv::INTER_LINEAR, cv::BORDER_REFLECT);

      cv::Mat M2 = (cv::Mat_<double>(2,3) << 1,0, s, 0,1, -s);
      cv::warpAffine (ch[2], ch[2], M2, ch[2].size(), cv::INTER_LINEAR, cv::BORDER_REFLECT);

      cv::merge (ch, cur);
    }

    // ==========================
    // PHASE 3 — BLOCK SHIFT
    // ==========================

    {
      int H = cur.rows;
      int block = 32 + strength * 2;

      for (int y = 0; y < H; y += block)
        {
          int shift = (y / block) % (strength * 2 + 1);
          shift -= strength;

          if (abs (shift) > 0)
            {
              cv::Mat row = cur.rowRange (y, std::min(y + block, H));
              cv::Mat shifted;
              cv::Mat M = (cv::Mat_<double>(2,3) << 1,0, shift*2, 0,1,0);
              cv::warpAffine (row, shifted, M, row.size(),
                              cv::INTER_LINEAR, cv::BORDER_REFLECT);
              shifted.copyTo (row);
            }
        }
    }

    // ==========================
    // PHASE 4 — SATURATION BOOST
    // ==========================

    {
      cv::Mat hsv;
      cv::cvtColor (cur, hsv, cv::COLOR_BGR2HSV);

      std::vector<cv::Mat> ch;
      cv::split (hsv, ch);

      ch[1] += strength * 3;
      ch[1] = cv::min (ch[1], 255);

      cv::merge (ch, hsv);
      cv::cvtColor (hsv, cur, cv::COLOR_HSV2BGR);
    }

    // ==========================
    // PHASE 5 — NOISE
    // ==========================

    {
      cv::Mat noise(cur.size (), CV_16SC3);
      for (int y = 0; y < noise.rows; ++y)
        {
          auto *p = noise.ptr<cv::Vec3s> (y);
          for (int x = 0; x < noise.cols; ++x)
            {
              p[x][0] = (x * 7 + y * 13) % (strength * 2);
              p[x][1] = (x * 11 + y * 5) % (strength * 2);
              p[x][2] = (x * 3 + y * 17) % (strength * 2);
            }
        }

      cv::Mat tmp;
      cur.convertTo (tmp, CV_16SC3);
      tmp += noise;
      tmp.convertTo (cur, CV_8UC3);
    }

    dst_bgr = cur;
  }

private:
  int strength = 50;
};


}

#endif
