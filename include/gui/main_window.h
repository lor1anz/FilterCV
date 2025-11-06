#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QLabel>

#include "core/cv_engine.h"

namespace gui
{

class image_widget;

class main_window : public QMainWindow
{
  Q_OBJECT
public:
  explicit main_window (QWidget *parent = nullptr);
  ~main_window () {};

private:
  image_widget *viewport = nullptr;
  std::unique_ptr<core::cv_engine> engine;
  QTimer timer;

  void build_ui ();
  void show_test_image ();

  QRadioButton *rb_image  = nullptr;
  QRadioButton *rb_video  = nullptr;
  QRadioButton *rb_camera = nullptr;
  QSpinBox     *sb_camera_index = nullptr;

  void build_dock ();
  void build_source_dock ();

  // grayscale
  QCheckBox *cb_grayscale = nullptr;
  // blur
  QSlider *sl_blur_ksize = nullptr;
  QLabel *lb_blur_value = nullptr;
  // canny
  QCheckBox *cb_canny = nullptr;
  QSlider *sl_canny_lo = nullptr;
  QSlider *sl_canny_hi = nullptr;
  QLabel *lb_canny_lo = nullptr;
  QLabel *lb_canny_hi = nullptr;
  // jpeg
  QCheckBox *cb_jpeg = nullptr;
  QSlider *sl_jpeg_quality = nullptr;
  QLabel *lb_jpeg_quality = nullptr;
  // Sharpen
  QCheckBox *cb_sharpen = nullptr;
  QSlider *sl_sharpen_amount = nullptr;
  QSlider *sl_sharpen_radius = nullptr;
  QSlider *sl_sharpen_threshold = nullptr;
  QLabel *lb_sharpen_amount = nullptr;
  QLabel *lb_sharpen_radius = nullptr;
  QLabel *lb_sharpen_threshold = nullptr;

  void add_grayscale_filter (QVBoxLayout *v, QWidget *panel);
  void add_blur_filter (QVBoxLayout *v, QWidget *panel);
  void add_canny_filter (QVBoxLayout *v, QWidget *panel);
  void add_jpeg_filter (QVBoxLayout *v, QWidget *panel);
  void add_sharpen_filter (QVBoxLayout *v, QWidget *panel);

private slots:
  void onTick ();
};

}

#endif