#include "gui/window.h"

#include <QString>
#include <QImage>
#include <QLabel>
#include <QPixmap>

#include <opencv2/opencv.hpp>

#include <string>

#include "gui/utils.h"

namespace gui
{

window::window (const QString &title, int width, int height)
{
  widget.setWindowTitle (title);
  widget.resize (width, height);

  cv::Mat cvimg (height, width, CV_8UC3, cv::Scalar (0, 0, 0));
  QImage qtimg = cvmat_to_qimage (cvimg);

  QLabel *image = new QLabel (&widget);
  image->setPixmap (QPixmap::fromImage (qtimg));
  image->show ();
}

void window::show ()
{
  widget.show ();
}

window::~window () {}

}
