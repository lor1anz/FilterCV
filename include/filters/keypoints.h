#ifndef KEYPOINTS_H
#define KEYPOINTS_H

#include "filters/filter.h"
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <algorithm>
#include <vector>

namespace filters
{

class keypoints final : public filter
{
public:
  enum class detector_t
  {
    fast,
    orb
  };

  const char *id () const override final { return "keypoints"; }

  bool is_enabled () const override final { return enabled; }
  void set_enabled (bool on) override final { enabled = on; }

  void set_detector (detector_t d) { detector = d; }
  detector_t get_detector () const { return detector; }

  void set_threshold (int v)
  {
    threshold = std::clamp (v, 1, 100);
  }
  int get_threshold () const { return threshold; }

  void set_max_features (int v)
  {
    max_features = std::clamp (v, 50, 5000);
  }
  int get_max_features () const { return max_features; }

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

    std::vector<cv::KeyPoint> kps;

    if (detector == detector_t::fast)
    {
      cv::FAST (gray, kps, threshold, true);
    }
    else
    {
      auto orb = cv::ORB::create (max_features);
      orb->detect (gray, kps);
    }

    dst_bgr = src_bgr.clone ();
    cv::drawKeypoints (
      dst_bgr, kps, dst_bgr,
      cv::Scalar (0, 255, 0),
      cv::DrawMatchesFlags::DRAW_OVER_OUTIMG
    );
  }

private:
  bool enabled = false;
  detector_t detector = detector_t::fast;
  int threshold = 20;
  int max_features = 500;
};

}

#endif
