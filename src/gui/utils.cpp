#include "gui/utils.h"

#include <QImage>

#include <opencv2/opencv.hpp>

namespace gui
{

QImage cvmat_to_qimage (const cv::Mat &cvimg)
{
  cv::Mat cvrgb;
  cv::cvtColor (cvimg, cvrgb, cv::COLOR_BGR2RGB);
  QImage qtimg (reinterpret_cast<const uchar*> (cvrgb.data), cvrgb.cols, cvrgb.rows, static_cast<int> (cvrgb.step), QImage::Format_RGB888);
  return qtimg.copy ();
}

}