#include "gui/main_window.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QDockWidget>
#include <QFormLayout>
#include <QCheckBox>
#include <QRadioButton>
#include <QSlider>
#include <QLabel>

#include <opencv2/opencv.hpp>

#include "globals.h"
#include "system/screen.h"
#include "gui/image_widget.h"
#include "gui/utils.h"
#include "filters/grayscale.h"
#include "filters/blur.h"
#include "filters/canny.h"

namespace gui
{

main_window::main_window (QWidget *parent) : QMainWindow (parent), engine (std::make_unique<core::cv_engine>())
{
  build_ui ();
  build_dock ();
  show_test_image ();

  engine->add_filter (std::make_shared<filters::grayscale> ());
  engine->add_filter (std::make_shared<filters::blur> ());
  engine->add_filter (std::make_shared<filters::canny> ());

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
  dock->setFeatures (QDockWidget::NoDockWidgetFeatures);
  dock->setAllowedAreas (Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

  auto *panel = new QWidget (dock);
  auto *v = new QVBoxLayout (panel);
  v->setContentsMargins(8, 8, 8, 8);

  add_grayscale_filter (v, panel);
  add_blur_filter (v, panel);
  add_canny_filter (v, panel);

  v->addStretch (1);

  panel->setLayout (v);
  dock->setWidget (panel);
  addDockWidget (Qt::RightDockWidgetArea, dock);
}

void main_window::add_grayscale_filter (QVBoxLayout *v, QWidget *panel)
{
  cb_grayscale = new QCheckBox (tr ("Grayscale"), panel);
  v->addWidget (cb_grayscale);

  connect (cb_grayscale, &QCheckBox::toggled, this, [this] (bool on) {
    if (auto f = engine->find_filter ("grayscale")) 
      f->set_enabled (on);
  });
}

void main_window::add_blur_filter (QVBoxLayout *v, QWidget *panel)
{
  auto *form = new QFormLayout ();
  form->setContentsMargins (0, 0, 0, 0);
  form->setSpacing (6);

  sl_blur_ksize = new QSlider (Qt::Horizontal, panel);
  sl_blur_ksize->setRange (0, 99);
  sl_blur_ksize->setSingleStep (2);
  sl_blur_ksize->setPageStep (2);
  sl_blur_ksize->setTickPosition (QSlider::TicksBelow);
  sl_blur_ksize->setTickInterval (4);
  sl_blur_ksize->setValue (0);

  lb_blur_value = new QLabel (QString::number (0), panel);
  lb_blur_value->setMinimumWidth (30);
  lb_blur_value->setAlignment (Qt::AlignRight | Qt::AlignVCenter);

  auto *h = new QHBoxLayout ();
  h->setContentsMargins (0, 0, 0, 0);
  h->setSpacing (6);
  h->addWidget (sl_blur_ksize, /*stretch*/ 1);
  h->addWidget (lb_blur_value);

  form->addRow (tr ("Blur"), h);
  v->addLayout (form);

  connect (sl_blur_ksize, &QSlider::valueChanged, this, [this] (int k) {
    if (k > 0 && (k % 2 == 0))
      {
        sl_blur_ksize->blockSignals (true);
        sl_blur_ksize->setValue (k + 1);
        sl_blur_ksize->blockSignals (false);
        k = k + 1;
      }

    lb_blur_value->setText (QString::number (k));

    auto base = engine->find_filter ("blur");
    if (!base) return;
    auto f = std::dynamic_pointer_cast<filters::blur> (base);
    if (!f) return;

    f->set_ksize (k);
  });
}

void main_window::add_canny_filter (QVBoxLayout *v, QWidget *panel)
{
  cb_canny = new QCheckBox (tr ("Canny (Edges)"), panel);
  v->addWidget (cb_canny);

  auto *form = new QFormLayout ();
  form->setContentsMargins (0, 0, 0, 0);
  form->setSpacing (6);

  // Low
  sl_canny_lo = new QSlider (Qt::Horizontal, panel);
  sl_canny_lo->setRange (0, 255);
  sl_canny_lo->setSingleStep (1);
  sl_canny_lo->setPageStep (5);
  sl_canny_lo->setTickPosition (QSlider::TicksBelow);
  sl_canny_lo->setTickInterval (10);
  sl_canny_lo->setValue (50);

  lb_canny_lo = new QLabel (QString::number (50), panel);
  lb_canny_lo->setMinimumWidth (30);
  lb_canny_lo->setAlignment (Qt::AlignRight | Qt::AlignVCenter);

  auto *hLo = new QHBoxLayout ();
  hLo->setContentsMargins (0, 0, 0, 0);
  hLo->setSpacing (6);
  hLo->addWidget (sl_canny_lo, 1);
  hLo->addWidget (lb_canny_lo);
  form->addRow (tr ("Low"), hLo);

  // High
  sl_canny_hi = new QSlider (Qt::Horizontal, panel);
  sl_canny_hi->setRange (0, 255);
  sl_canny_hi->setSingleStep (1);
  sl_canny_hi->setPageStep (5);
  sl_canny_hi->setTickPosition (QSlider::TicksBelow);
  sl_canny_hi->setTickInterval (10);
  sl_canny_hi->setValue (150);

  lb_canny_hi = new QLabel (QString::number (150), panel);
  lb_canny_hi->setMinimumWidth (30);
  lb_canny_hi->setAlignment (Qt::AlignRight | Qt::AlignVCenter);

  auto *hHi = new QHBoxLayout ();
  hHi->setContentsMargins (0, 0, 0, 0);
  hHi->setSpacing (6);
  hHi->addWidget (sl_canny_hi, 1);
  hHi->addWidget (lb_canny_hi);
  form->addRow (tr ("High"), hHi);

  v->addLayout (form);

  connect (cb_canny, &QCheckBox::toggled, this, [this] (bool on) {
    if (auto base = engine->find_filter ("canny"))
      base->set_enabled (on);
  });

  connect (sl_canny_lo, &QSlider::valueChanged, this, [this] (int lo) {
    lb_canny_lo->setText (QString::number (lo));
    auto base = engine->find_filter ("canny");
    if (!base) return;
    auto f = std::dynamic_pointer_cast<filters::canny> (base);
    if (!f) return;

    int hi = sl_canny_hi->value ();
    if (hi < lo)
      {
        sl_canny_hi->blockSignals (true);
        sl_canny_hi->setValue (lo);
        sl_canny_hi->blockSignals (false);
        hi = lo;
        lb_canny_hi->setText (QString::number (hi));
      }
    f->set_thresholds (lo, hi);
  });

  connect (sl_canny_hi, &QSlider::valueChanged, this, [this] (int hi) {
    lb_canny_hi->setText (QString::number (hi));
    auto base = engine->find_filter ("canny");
    if (!base) return;
    auto f = std::dynamic_pointer_cast<filters::canny> (base);
    if (!f) return;

    int lo = sl_canny_lo->value ();
    if (hi < lo)
      {
        sl_canny_lo->blockSignals (true);
        sl_canny_lo->setValue (hi);
        sl_canny_lo->blockSignals (false);
        lo = hi;
        lb_canny_lo->setText (QString::number (lo));
      }
    f->set_thresholds (lo, hi);
  });
}


void main_window::build_source_dock ()
{
  auto *dock = new QDockWidget (tr ("Source"), this);
  dock->setFeatures (QDockWidget::NoDockWidgetFeatures);
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
