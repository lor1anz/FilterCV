#include "gui/image_widget.h"

#include <QPainter>

#include "system/screen.h"

namespace gui
{

image_widget::image_widget (QWidget *parent) : QWidget (parent)
{
  setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);
  setMinimumSize (1, 1);

  system_utils::screen screen;
  hint_width = screen.get_width () / 2;
  hint_height = screen.get_height () / 2;
}

void image_widget::set_image(const QImage &img)
{
  image = img;
  update ();
}

void image_widget::paintEvent(QPaintEvent * /*event*/)
{
  QPainter painter (this);
  painter.fillRect (rect (), Qt::black);

  if (image.isNull ())
    return;

  const QSize target_size = image.size ().scaled (size (), Qt::KeepAspectRatio);
  QRect image_rect (QPoint (0, 0), target_size);
  image_rect.moveCenter (rect ().center ());

  QPixmap pixmap = QPixmap::fromImage (image);
  pixmap.setDevicePixelRatio (devicePixelRatioF ());

  painter.setRenderHint (QPainter::SmoothPixmapTransform, true);
  painter.drawPixmap (image_rect, pixmap);
}

}