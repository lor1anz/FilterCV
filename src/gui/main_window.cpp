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
#include "filters/jpeg.h"
#include "filters/sharpen.h"
#include "filters/pixel_sort.h"

#include "filters/glitch.h"

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
  engine->add_filter (std::make_shared<filters::jpeg> ());
  engine->add_filter (std::make_shared<filters::sharpen> ());
  engine->add_filter (std::make_shared<filters::pixel_sort> ());

  engine->add_filter (std::make_shared<filters::glitch> ());

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
  add_jpeg_filter (v, panel);
  add_sharpen_filter (v, panel);
  add_pixel_sort_filter (v, panel);

  add_glitch_filter (v, panel);

  v->addStretch (1);

  panel->setLayout (v);
  dock->setWidget (panel);
  addDockWidget (Qt::RightDockWidgetArea, dock);
}

void main_window::add_glitch_filter (QVBoxLayout *v, QWidget *panel)
{
  cb_glitch = new QCheckBox (tr ("Glitch"), panel);
  v->addWidget (cb_glitch);

  connect (cb_glitch, &QCheckBox::toggled, this, [this] (bool on) {
    if (auto base = engine->find_filter ("glitch"))
      base->set_enabled (on);
  });
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

void main_window::add_jpeg_filter (QVBoxLayout *v, QWidget *panel)
{
  cb_jpeg = new QCheckBox (tr ("JPEG compress"), panel);
  v->addWidget (cb_jpeg);

  auto *form = new QFormLayout ();
  form->setContentsMargins (0, 0, 0, 0);
  form->setSpacing (6);

  sl_jpeg_quality = new QSlider (Qt::Horizontal, panel);
  sl_jpeg_quality->setRange (0, 100);
  sl_jpeg_quality->setSingleStep (5);
  sl_jpeg_quality->setPageStep (10);
  sl_jpeg_quality->setTickPosition (QSlider::TicksBelow);
  sl_jpeg_quality->setTickInterval (10);
  sl_jpeg_quality->setValue (80);
  sl_jpeg_quality->setEnabled (false);

  lb_jpeg_quality = new QLabel (QString::number (80), panel);
  lb_jpeg_quality->setMinimumWidth (30);
  lb_jpeg_quality->setAlignment (Qt::AlignRight | Qt::AlignVCenter);

  auto *h = new QHBoxLayout ();
  h->setContentsMargins (0, 0, 0, 0);
  h->setSpacing (6);
  h->addWidget (sl_jpeg_quality, 1);
  h->addWidget (lb_jpeg_quality);

  form->addRow (tr ("Quality"), h);
  v->addLayout (form);

  connect (cb_jpeg, &QCheckBox::toggled, this, [this] (bool on) {
    if (auto base = engine->find_filter ("jpeg"))
      base->set_enabled (on);
    sl_jpeg_quality->setEnabled (on);
  });

  connect (sl_jpeg_quality, &QSlider::valueChanged, this, [this] (int q) {
    lb_jpeg_quality->setText (QString::number (q));
    auto base = engine->find_filter ("jpeg");
    if (!base) return;
    auto f = std::dynamic_pointer_cast<filters::jpeg> (base);
    if (!f) return;
    f->set_quality (q);
  });
}

void main_window::add_sharpen_filter (QVBoxLayout *v, QWidget *panel)
{
  cb_sharpen = new QCheckBox (tr ("Sharpen"), panel);
  v->addWidget (cb_sharpen);

  auto *form = new QFormLayout ();
  form->setContentsMargins (0, 0, 0, 0);
  form->setSpacing (6);

  sl_sharpen_amount = new QSlider (Qt::Horizontal, panel);
  sl_sharpen_amount->setRange (0, 300);
  sl_sharpen_amount->setValue (100);
  lb_sharpen_amount = new QLabel (QString::number (1.00, 'f', 2), panel);

  auto *hA = new QHBoxLayout ();
  hA->addWidget (sl_sharpen_amount, 1);
  hA->addWidget (lb_sharpen_amount);
  form->addRow (tr ("Amount"), hA);

  sl_sharpen_radius = new QSlider (Qt::Horizontal, panel);
  sl_sharpen_radius->setRange (1, 15);
  sl_sharpen_radius->setValue (3);
  lb_sharpen_radius = new QLabel (QString::number (3), panel);

  auto *hR = new QHBoxLayout ();
  hR->addWidget (sl_sharpen_radius, 1);
  hR->addWidget (lb_sharpen_radius);
  form->addRow (tr ("Radius"), hR);

  sl_sharpen_threshold = new QSlider (Qt::Horizontal, panel);
  sl_sharpen_threshold->setRange (0, 255);
  sl_sharpen_threshold->setValue (10);
  lb_sharpen_threshold = new QLabel (QString::number (10), panel);

  auto *hT = new QHBoxLayout ();
  hT->addWidget (sl_sharpen_threshold, 1);
  hT->addWidget (lb_sharpen_threshold);
  form->addRow (tr ("Threshold"), hT);

  v->addLayout (form);

  connect (cb_sharpen, &QCheckBox::toggled, this, [this] (bool on) {
    if (auto base = engine->find_filter ("sharpen"))
      base->set_enabled (on);
  });

  connect (sl_sharpen_amount, &QSlider::valueChanged, this, [this] (int val) {
    double amount = val / 100.0;
    lb_sharpen_amount->setText (QString::number (amount, 'f', 2));

    auto base = engine->find_filter ("sharpen");
    if (auto f = std::dynamic_pointer_cast<filters::sharpen> (base))
      f->set_amount (amount);
  });

  connect (sl_sharpen_radius, &QSlider::valueChanged, this, [this] (int val) {
    lb_sharpen_radius->setText (QString::number (val));
    auto base = engine->find_filter ("sharpen");
    if (auto f = std::dynamic_pointer_cast<filters::sharpen> (base))
      f->set_radius (val);
  });

  connect (sl_sharpen_threshold, &QSlider::valueChanged, this, [this] (int val) {
    lb_sharpen_threshold->setText (QString::number (val));
    auto base = engine->find_filter ("sharpen");
    if (auto f = std::dynamic_pointer_cast<filters::sharpen> (base))
      f->set_threshold (val);
  });
}

void main_window::add_pixel_sort_filter (QVBoxLayout *v, QWidget *panel)
{
  cb_pixsort = new QCheckBox (tr ("Pixel sort"), panel);
  v->addWidget (cb_pixsort);

  cb_pixsort_vertical = new QCheckBox (tr ("Horizontal"), panel);
  cb_pixsort_reverse  = new QCheckBox (tr ("Reverse"), panel);

  auto *form = new QFormLayout ();
  form->setContentsMargins (0, 0, 0, 0);

  sl_pixsort_chunk = new QSlider (Qt::Horizontal, panel);
  sl_pixsort_chunk->setRange (4, 128);
  sl_pixsort_chunk->setSingleStep (4);
  sl_pixsort_chunk->setPageStep (8);
  sl_pixsort_chunk->setTickPosition (QSlider::TicksBelow);
  sl_pixsort_chunk->setTickInterval (8);
  sl_pixsort_chunk->setValue (32);

  lb_pixsort_chunk = new QLabel (QString::number (32), panel);
  lb_pixsort_chunk->setMinimumWidth (28);
  lb_pixsort_chunk->setAlignment (Qt::AlignRight | Qt::AlignVCenter);

  auto *h = new QHBoxLayout ();
  h->addWidget (sl_pixsort_chunk, 1);
  h->addWidget (lb_pixsort_chunk);

  form->addRow (tr ("Chunk"), h);
  v->addLayout (form);
  v->addWidget (cb_pixsort_vertical);
  v->addWidget (cb_pixsort_reverse);

  connect (cb_pixsort, &QCheckBox::toggled, this, [this] (bool on) {
    if (auto base = engine->find_filter ("pixel_sort"))
      base->set_enabled (on);
  });

  connect (sl_pixsort_chunk, &QSlider::valueChanged, this, [this] (int k) {
    lb_pixsort_chunk->setText (QString::number (k));
    auto base = engine->find_filter ("pixel_sort");
    if (!base) 
      return;
    auto f = std::dynamic_pointer_cast<filters::pixel_sort> (base);
    if (!f) 
      return;
    f->set_chunk (k);
  });

  connect (cb_pixsort_vertical, &QCheckBox::toggled, this, [this] (bool on) {
    auto base = engine->find_filter ("pixel_sort");
    if (!base) 
      return;
    auto f = std::dynamic_pointer_cast<filters::pixel_sort> (base);
    if (!f) 
      return;
    f->set_axis (on ? filters::pixel_sort::axis_t::vertical : filters::pixel_sort::axis_t::horizontal);
  });

  connect (cb_pixsort_reverse, &QCheckBox::toggled, this, [this] (bool on) {
    auto base = engine->find_filter ("pixel_sort");
    if (!base) 
      return;
    auto f = std::dynamic_pointer_cast<filters::pixel_sort> (base);
    if (!f) 
      return;
    f->set_reverse (on);
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
