#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

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

  void build_ui ();
  void show_test_image ();
};

}

#endif