#include "gui/image_widget.h"

#include <QPainter>

namespace gui
{

image_widget::image_widget (QWidget *parent) : QWidget (parent)
{
  setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);
  setMinimumSize (1, 1);
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

  painter.setRenderHint (QPainter::SmoothPixmapTransform, true);
  painter.drawImage (image_rect, image);
}

}