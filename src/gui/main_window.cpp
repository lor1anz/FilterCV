#include "gui/main_window.h"

#include <QWidget>
#include <QVBoxLayout>

#include <opencv2/opencv.hpp>

#include "globals.h"
#include "system/screen.h"
#include "gui/image_widget.h"
#include "gui/utils.h"

namespace gui
{

main_window::main_window (QWidget *parent) : QMainWindow (parent)
{
  build_ui ();
  show_test_image ();
}

void main_window::build_ui ()
{
  auto *central = new QWidget (this);
  auto *layout = new QVBoxLayout (central);
  layout->setContentsMargins (0, 0, 0, 0);

  viewport = new image_widget (central);
  
  system_utils::screen screen;
  int width = screen.get_width () / 1.5;
  int height = screen.get_height () / 1.5;

  resize (width, height);
  setWindowTitle (WINDOW_NAME);
}

void main_window::show_test_image ()
{
  cv::Mat mat (480, 640, CV_8UC3, cv::Scalar (0, 0, 0));
  QImage img = cvmat_to_qimage (mat);
  viewport->set_image (img);
}

}
