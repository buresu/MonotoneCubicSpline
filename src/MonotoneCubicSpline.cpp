#include "MonotoneCubicSpline.hpp"

MonotoneCubicSpline::MonotoneCubicSpline() {}

MonotoneCubicSpline::MonotoneCubicSpline(size_t size) {
  _controls.resize(size);
}

MonotoneCubicSpline::MonotoneCubicSpline(const Controls &controls)
    : _controls(controls) {}

MonotoneCubicSpline::~MonotoneCubicSpline() {}

bool MonotoneCubicSpline::isValid() const { return _controls.size() >= 2; }

size_t MonotoneCubicSpline::size() const { return _controls.size(); }

MonotoneCubicSpline::Control MonotoneCubicSpline::control(size_t index) const {
  return _controls[index];
}

void MonotoneCubicSpline::setControl(size_t index, const Control &control) {
  _controls[index] = control;
}

MonotoneCubicSpline::Controls MonotoneCubicSpline::controls() const {
  return _controls;
}

void MonotoneCubicSpline::setControls(const Controls &controls) {
  _controls = controls;
}

void MonotoneCubicSpline::reshape(size_t size) {

  if (size == _controls.size())
    return;

  Controls controls(size);

  const double step = 1.0 / double(size - 1);

  for (size_t i = 0; i < size; ++i)
    controls[i] = interpolate(step * double(i));

  _controls = controls;
}

MonotoneCubicSpline::Control MonotoneCubicSpline::interpolate(double t) const {

  const double step = 1.0 / double(_controls.size() - 1);
  double index = 0;
  const double tp = std::modf(t / step, &index);
  const size_t i = size_t(index);

  if (index >= _controls.size() - 1)
    return _controls[i];

  Control m0, m1;

  static auto delta = [](const Control &p1, const Control &p2, double step) {
    return (p2 - p1) / step;
  };

  static auto m = [&](const Control &p0, const Control &p1, const Control &p2,
                      double step) {
    auto d0 = delta(p0, p1, step);
    auto d1 = delta(p1, p2, step);
    return d0 * d1 < 0.0 ? 0.0 : (d0 + d1) * 0.5;
  };

  auto d = delta(_controls[i], _controls[i + 1], step);

  if (i == 0) {
    m0 = delta(_controls[i], _controls[i + 1], step);
    m1 = m(_controls[i], _controls[i + 1], _controls[i + 2], step);
  } else if (i == _controls.size() - 2) {
    m0 = m(_controls[i - 1], _controls[i], _controls[i + 1], step);
    m1 = delta(_controls[i], _controls[i + 1], step);
  } else {
    m0 = m(_controls[i - 1], _controls[i], _controls[i + 1], step);
    m1 = m(_controls[i], _controls[i + 1], _controls[i + 2], step);
  }

  if (_controls[i] == _controls[i + 1])
    m0 = m1 = 0.0;

  auto a = m0 / d;
  auto b = m1 / d;

  m0 = a < 0.0 || b < 0.0 ? 0.0 : m0;
  if (a * a + b * b > 9) {
    double g = 3.0 / std::sqrt(a * a + b * b);
    m0 = g * a * d;
    m1 = g * b * d;
  }

  auto h00 = 2.0 * tp * tp * tp - 3.0 * tp * tp + 1.0;
  auto h10 = tp * tp * tp - 2.0 * tp * tp + tp;
  auto h01 = -2.0 * tp * tp * tp + 3.0 * tp * tp;
  auto h11 = tp * tp * tp - tp * tp;

  return _controls[i] * h00 + step * m0 * h10 + _controls[i + 1] * h01 +
         step * m1 * h11;
}
