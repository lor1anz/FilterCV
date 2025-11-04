#ifndef WINDOW_H
#define WINDOW_H

#include <QImage>

#include <QWidget>

namespace gui
{

class window
{
public:
  window (const QString &title, int width, int height);

  void show ();

  ~window ();

private:
  QWidget widget;
};

}

#endif