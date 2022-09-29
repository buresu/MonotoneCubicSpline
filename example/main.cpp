#include <MonotoneCubicSpline.hpp>
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QRandomGenerator>
#include <QWidget>
#include <algorithm>

class SplineWidget : public QWidget {
public:
  explicit SplineWidget(QWidget *parent = nullptr);
  ~SplineWidget() {}

protected:
  virtual void paintEvent(QPaintEvent *e) override;
  virtual void mousePressEvent(QMouseEvent *e) override;
  virtual void mouseMoveEvent(QMouseEvent *e) override;
  virtual void mouseReleaseEvent(QMouseEvent *e) override;

private:
  QList<QPair<double, double>> _controls;
  int _selectIndex = -1;
};

SplineWidget::SplineWidget(QWidget *parent) : QWidget(parent) {

  setAutoFillBackground(true);
  setPalette(Qt::white);

  _controls.append({0, 43.76931});
  _controls.append({0.25, 19.131026});
  _controls.append({0.5, 10.58543});
  _controls.append({0.75, 5.7298435});
  _controls.append({1.0, 2.627706});
}

void SplineWidget::paintEvent(QPaintEvent *) {

  QPainter p(this);

  p.setRenderHint(QPainter::Antialiasing);

  // draw spline
  MonotoneCubicSpline::Controls controls;

  for (auto &c : _controls)
    controls.push_back(c);

  MonotoneCubicSpline cs(controls);
  QPolygonF path;

  for (int i = 0; i <= 1000; ++i) {
    double t = double(i) / 1000.0;
    auto y = cs.interpolate(t);
    path.append(QPointF(double(width()) * t, y));
  }

  p.setPen(Qt::red);
  p.drawPolyline(path);

  // draw controls
  for (int i = 0; i < _controls.size(); ++i) {
    _selectIndex == i ? p.setPen(Qt::magenta) : p.setPen(Qt::black);
    p.drawEllipse(
        QPointF(double(width()) * _controls[i].first, _controls[i].second), 3,
        3);
  }
}

void SplineWidget::mousePressEvent(QMouseEvent *e) {

  if (e->button() == Qt::LeftButton) {
    for (int i = 0; i < _controls.size(); ++i) {
      auto d = e->position() - QPointF(_controls[i].first * double(width()),
                                       _controls[i].second);
      if (d.x() * d.x() + d.y() * d.y() < 6 * 6) {
        _selectIndex = i;
        break;
      }
    }
  }

  e->ignore();
  update();
}

void SplineWidget::mouseMoveEvent(QMouseEvent *e) {

  if (_selectIndex == 0 || _controls.size() - 1 == _selectIndex) {
    double y = std::clamp(e->position().y(), 0.0, double(height()));
    _controls[_selectIndex] = {_controls[_selectIndex].first, y};
  } else if (_selectIndex >= 0) {
    double t0 = _controls[_selectIndex - 1].first * double(width());
    double t1 = _controls[_selectIndex + 1].first * double(width());

    double x = std::clamp(e->position().x(), t0 + (t1 - t0) * 0.01,
                          t0 + (t1 - t0) * 0.99) /
               double(width());
    double y = std::clamp(e->position().y(), 0.0, double(height()));
    _controls[_selectIndex] = {x, y};
  }

  e->ignore();
  update();
}

void SplineWidget::mouseReleaseEvent(QMouseEvent *e) {

  _selectIndex = -1;

  e->ignore();
  update();
}

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  SplineWidget w;
  w.show();
  return a.exec();
}
