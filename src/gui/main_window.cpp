#include "gui/main_window.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QDockWidget>
#include <QFormLayout>

#include <opencv2/opencv.hpp>

#include "globals.h"
#include "system/screen.h"
#include "gui/image_widget.h"
#include "gui/utils.h"
#include "filters/grayscale.h"

namespace gui
{

main_window::main_window (QWidget *parent) : QMainWindow (parent), engine (std::make_unique<core::cv_engine>())
{
  build_ui ();
  build_dock ();
  show_test_image ();

  engine->add_filter (std::make_shared<filters::grayscale> ());
  timer.setInterval (33); // ~ 30 fps
  connect (&timer, &QTimer::timeout, this, &main_window::onTick);
  timer.start ();
}

void main_window::build_ui ()
{
  auto *central = new QWidget (this);
  auto *layout = new QVBoxLayout (central);
  layout->setContentsMargins (0, 0, 0, 0);
  layout->setSpacing (0);

  viewport = new image_widget (central);
  
  system_utils::screen screen;
  int width = screen.get_width () / 1.5;
  int height = screen.get_height () / 1.5;

  layout->addWidget (viewport, /*stretch*/ 1);
  central->setLayout (layout);
  setCentralWidget (central);

  resize (width, height);
  setWindowTitle (WINDOW_NAME);
}

void main_window::build_dock ()
{
  build_source_dock ();

  auto *dock = new QDockWidget (tr ("Settings"), this);
  dock->setAllowedAreas (Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

  auto *panel = new QWidget (dock);
  auto *v = new QVBoxLayout (panel);
  v->setContentsMargins(8, 8, 8, 8);

  cb_grayscale = new QCheckBox (tr ("Grayscale"), panel);
  v->addWidget (cb_grayscale);
  v->addStretch (1);

  panel->setLayout (v);
  dock->setWidget (panel);
  addDockWidget (Qt::RightDockWidgetArea, dock);

  connect (cb_grayscale, &QCheckBox::toggled, this, [this] (bool on) {
    if (auto f = engine->find_filter ("grayscale")) 
      f->set_enabled (on);
  });
}

void main_window::build_source_dock ()
{
  auto *dock = new QDockWidget (tr ("Source"), this);
  dock->setAllowedAreas (Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

  auto *panel = new QWidget (dock);
  auto *v = new QVBoxLayout (panel);
  v->setContentsMargins (8, 8, 8, 8);
  v->setSpacing (8);

  rb_image  = new QRadioButton (tr ("Test Image"), panel);
  rb_video  = new QRadioButton (tr ("Test Video"), panel);
  rb_camera = new QRadioButton (tr ("Camera"), panel);
  rb_image->setChecked(true);

  sb_camera_index = new QSpinBox (panel);
  sb_camera_index->setRange (0, 10);
  sb_camera_index->setValue (0);
  sb_camera_index->setEnabled (false);

  v->addWidget (rb_image);
  v->addWidget (rb_video);
  v->addWidget (rb_camera);
  auto *form = new QFormLayout ();
  form->addRow (tr ("Camera Index"), sb_camera_index);
  v->addLayout (form);
  v->addStretch (1);

  panel->setLayout (v);
  dock->setWidget (panel);
  addDockWidget (Qt::LeftDockWidgetArea, dock);

  connect(rb_image, &QRadioButton::toggled, this, [this] (bool on) {
    if (!on) 
      return;
    engine->set_source (core::cv_engine::source::image);
    sb_camera_index->setEnabled (false);
    engine->close();
  });

  connect(rb_video, &QRadioButton::toggled, this, [this] (bool on) {
    if (!on) 
      return;
    engine->set_source (core::cv_engine::source::video);
    sb_camera_index->setEnabled (false);

    engine->set_test_video_file ("../resources/rickroll.mp4");
    if (!engine->open ())
      {
        qWarning() << "Cannot open video";
      }
  });

  connect(rb_camera, &QRadioButton::toggled, this, [this] (bool on) {
    if (!on) 
      return;
    engine->set_source (core::cv_engine::source::camera);
    sb_camera_index->setEnabled (true);
    engine->set_camera_index (sb_camera_index->value ());
    engine->open ();
  });

  connect (sb_camera_index, qOverload<int> (&QSpinBox::valueChanged), this, [this] (int idx) {
    if (!rb_camera->isChecked ()) 
      return;
    engine->set_camera_index (idx);
    engine->open ();
  });
}

void main_window::show_test_image ()
{
  cv::Mat mat = cv::imread ("../resources/shrek.jpg", cv::IMREAD_UNCHANGED);
  engine->set_test_image (mat);
  engine->set_source (core::cv_engine::source::image);
}

void main_window::onTick () 
{
  if (!engine) 
    return;
  if (!engine->grab()) 
    return;

  const QImage img = engine->process();
  if (!img.isNull ()) 
    viewport->set_image (img);
}

}
