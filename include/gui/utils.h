#ifndef UTILS_H
#define UTILS_H

#include <QImage>

#include <opencv2/opencv.hpp>

namespace gui
{

inline QImage cvmat_to_qimage (const cv::Mat &mat) {
  if (mat.empty ()) 
    return {};

  if (mat.type () == CV_8UC3) 
    {
      cv::Mat rgb; 
      cv::cvtColor (mat, rgb, cv::COLOR_BGR2RGB);
      return QImage (rgb.data, rgb.cols, rgb.rows, static_cast<int> (rgb.step),QImage::Format_RGB888).copy ();
    } 
  else if (mat.type () == CV_8UC1)
    {
      return QImage (mat.data, mat.cols, mat.rows, static_cast<int> (mat.step), QImage::Format_Grayscale8).copy ();
    } 
  else if (mat.type () == CV_8UC4) 
    {
      cv::Mat rgba; 
      cv::cvtColor (mat, rgba, cv::COLOR_BGRA2RGBA);
      return QImage (rgba.data, rgba.cols, rgba.rows, static_cast<int> (rgba.step), QImage::Format_RGBA8888).copy ();
    }

  return {};
}

}

#endif
