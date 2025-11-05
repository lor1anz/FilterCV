#include "core/cv_engine.h"

#include "gui/utils.h"

namespace core
{

void cv_engine::set_source_image (const cv::Mat &bgr) 
{
  source_bgr = bgr.clone();
}

void cv_engine::clear_filters ()
{ 
  pipeline.clear(); 
}

void cv_engine::add_filter (std::shared_ptr<filters::filter> filter)
{ 
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
  if (source_bgr.empty ()) 
    return {};

  cv::Mat src = source_bgr;
  cv::Mat dst;
  for (auto &filter : pipeline) 
    {
      filter->apply (src, dst);
      if (dst.data != src.data) 
        src = dst;
    }

  return gui::cvmat_to_qimage (src);
}

}
