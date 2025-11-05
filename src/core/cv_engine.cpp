#include "core/cv_engine.h"

#include <QDebug>

#include <opencv2/videoio.hpp>

#include "gui/utils.h"

namespace core
{

void cv_engine::set_source (source s) { src = s; }
void cv_engine::set_test_image (const cv::Mat &bgr) { test_bgr = bgr.clone (); }
void cv_engine::set_test_video_file (const QString &path) { video_path = path; }
void cv_engine::set_camera_index (int index) { camera_index = index; }

bool cv_engine::open ()
{
  close ();
  if (src == source::video)
    {
      capture.open (video_path.toStdString ());
      if (!capture.isOpened ())
        {
          qWarning () << "Cannot open video:" << video_path; 
          return false;
        }
      return true;
    }
  else if (src == source::camera)
    {
      if (!capture.open (camera_index, cv::CAP_ANY)) 
        {
          qWarning () << "Cannot open camera" << camera_index;
          return false;
        }
      return true;
    }
  else
    {
      // image doesn't need to be open
    }
  return true;
}

void cv_engine::close ()
{
  if (capture.isOpened ())
    capture.release ();
}

bool cv_engine::grab ()
{
  switch (src)
    {
      case source::image:
        {
          if (test_bgr.empty ())
            return false;
          current_bgr = test_bgr.clone ();
          return true;
        }

      case source::video:
      case source::camera:
        {
          if (!capture.isOpened ())
            {
              if (!open ())
                return false;
            }

          cv::Mat frame;
          if (!capture.read (frame) || frame.empty ())
            {
              if (src == source::video)
                {
                  capture.set (cv::CAP_PROP_POS_FRAMES, 0);
                  if (!capture.read (frame) || frame.empty ())
                    {
                      const std::string path = video_path.toStdString ();
                      capture.release ();
                      if (!capture.open (path))
                        return false;
                      capture.set (cv::CAP_PROP_POS_FRAMES, 0);
                      if (!capture.read (frame) || frame.empty ())
                        return false;
                    }
                }
              else
                {
                  return false;
                }
            }

          current_bgr = std::move (frame);
          return true;
        }
    }

  return false;
}


void cv_engine::clear_filters ()
{ 
  pipeline.clear(); 
}

void cv_engine::add_filter (std::shared_ptr<filters::filter> filter)
{ 
  if (!filter)
    return;
  pipeline.push_back (std::move (filter));
}

std::shared_ptr<filters::filter> cv_engine::find_filter (const char *id) 
{
  for (auto &filter : pipeline) 
    {
      if (std::string (filter->id ()) == id) 
        return filter;
    }

  return {};
}

QImage cv_engine::process () 
{
  if (current_bgr.empty ()) 
    return {};

  cv::Mat src = current_bgr;
  cv::Mat dst;
  for (auto &filter : pipeline) 
    {
      if (!filter)
        continue;

      filter->apply (src, dst);
      if (dst.data != src.data) 
        src = dst;
    }

  return gui::cvmat_to_qimage (src);
}

}
