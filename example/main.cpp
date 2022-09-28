#include <MonotoneCubicSpline.hpp>
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QRandomGenerator>
#include <QWidget>

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
  QList<QPointF> _controls;
  int _selectIndex = -1;
};

SplineWidget::SplineWidget(QWidget *parent) : QWidget(parent) {

  /*for (int i = 0; i < 6; ++i) {
    auto x = QRandomGenerator::global()->generateDouble() * width();
    auto y = QRandomGenerator::global()->generateDouble() * height();
    x = qMin(qMax(10.0, x), double(width() - 10));
    y = qMin(qMax(10.0, y), double(width() - 10));
    mControls.append(QPointF(x, y));
  }*/

  // std::sort(mControls.begin(), mControls.end(),
  //          [](auto const &lhs, auto const &rhs) { return lhs.x() > rhs.x();
  //          });

  _controls.append({100 + 0, 100 - 43.76931});
  _controls.append({100 + 25, 100 - 19.131026});
  _controls.append({100 + 50, 100 - 10.58543});
  _controls.append({100 + 75, 100 - 5.7298435});
  _controls.append({100 + 100, 100 - 2.627706});
}

void SplineWidget::paintEvent(QPaintEvent *) {
  /*
    QPainter p(this);

    p.setRenderHint(QPainter::Antialiasing);

    p.fillRect(rect(), Qt::white);

    // draw spline
    MonotoneCubicSpline::Controls controls;

    for (auto &c : _controls) {
      controls.push_back(MonotoneCubicSpline::Control(c.x(), c.y()));
    }

    MonotoneCubicSpline cs(controls);
    QPolygonF path;

    for (int i = 0; i <= 100; ++i) {
      auto pt = cs.interpolate(double(i) / 100.0);
      path.append(QPointF(pt.x(), pt.y()));
    }

    p.setPen(Qt::red);
    p.drawPolyline(path);

    // draw controls
    for (int i = 0; i < _controls.size(); ++i) {
      _selectIndex == i ? p.setPen(Qt::magenta) : p.setPen(Qt::black);
      p.drawEllipse(_controls[i], 3, 3);
    }*/
}

void SplineWidget::mousePressEvent(QMouseEvent *e) {

  if (e->button() == Qt::LeftButton) {
    for (int i = 0; i < _controls.size(); ++i) {
      auto d = e->position() - _controls[i];
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

  if (_selectIndex >= 0)
    _controls[_selectIndex] = e->position();

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
