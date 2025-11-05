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
  enum class source { image, video, camera };

  cv_engine () = default;
  ~cv_engine () { close (); }

  cv_engine (const cv_engine &) = delete;
  cv_engine &operator= (const cv_engine &) = delete;
  cv_engine (cv_engine &&) = delete;
  cv_engine &operator= (cv_engine &&) = delete;

  // sources
  void set_source (source s);
  void set_test_image (const cv::Mat &bgr);
  void set_test_video_file (const QString &path);
  void set_camera_index (int index);

  bool open ();
  void close ();
  bool grab ();

  void set_source_image (const cv::Mat &bgr);
  void clear_filters ();
  void add_filter (std::shared_ptr<filters::filter> filter);
  std::shared_ptr<filters::filter> find_filter (const char *id);

  QImage process ();

private:
  source src = source::image;
  cv::Mat test_bgr;
  QString video_path;
  int camera_index = 0;
  cv::VideoCapture capture;

  cv::Mat current_bgr;

  std::vector<std::shared_ptr<filters::filter>> pipeline;
};

}


#endif