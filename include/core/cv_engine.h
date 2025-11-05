#ifndef CV_ENGINE_H
#define CV_ENGINE_H

#include <QImage>

#include <opencv2/opencv.hpp>

#include "filters/filter.h"

namespace core
{

class cv_engine
{
public:
  void set_source_image (const cv::Mat &bgr);
  void clear_filters ();
  void add_filter (std::shared_ptr<filters::filter> filter);
  std::shared_ptr<filters::filter> find_filter (const char *id);

  QImage process ();

private:
  cv::Mat source_bgr;
  std::vector<std::shared_ptr<filters::filter>> pipeline;
};

}


#endif