#include <QApplication>

#include "globals.h"
#include "gui/window.h"
#include "system/screen.h"

int main(int argc, char *argv[])
{
  QApplication app (argc, argv);
  QApplication::setApplicationName (WINDOW_NAME);

  system_utils::screen screen;
  int main_window_width = screen.get_width () / 1.5;
  int main_window_height = screen.get_height () / 1.5;

  gui::window main_window (WINDOW_NAME, main_window_width, main_window_height);
  main_window.show ();

  return app.exec ();
}
