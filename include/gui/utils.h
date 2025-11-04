#ifndef UTILS_H
#define UTILS_H

#include <QImage>

#include <opencv2/opencv.hpp>

namespace gui
{

QImage cvmat_to_qimage (const cv::Mat &cvimg);

}

#endif
