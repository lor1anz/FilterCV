#ifndef IMAGE_WIDGET_H
#define IMAGE_WIDGET_H

#include <QWidget>
#include <QImage>

namespace gui
{

class image_widget : public QWidget {
  Q_OBJECT
public:
  explicit image_widget (QWidget *parent = nullptr);

  void set_image(const QImage &img);

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  QImage image;
};

}

#endif