#include <QApplication>
#include <QWidget>

#include "globals.h"

int main(int argc, char *argv[])
{
  QApplication app (argc, argv);

  QWidget window;
  window.setWindowTitle ("FilterCV");
  window.resize (400, 300);
  window.show ();

  return app.exec ();
}
