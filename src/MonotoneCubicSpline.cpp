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

  static auto delta = [](const Control &p1, const Control &p2, double step) {
    return (p2 - p1) / step;
  };

  static auto m = [&](const Control &p0, const Control &p1, const Control &p2,
                      double step) {
    auto d0 = delta(p0, p1, step);
    auto d1 = delta(p1, p2, step);
    return d0 * d1 < 0.0 ? 0.0 : (d0 + d1) * 0.5;
  };

  // Step 1
  auto dk = delta(_controls[i], _controls[i + 1], step);

  // Step 2
  Control mk0, mk1;

  if (i == 0) {
    mk0 = delta(_controls[i], _controls[i + 1], step);
    mk1 = m(_controls[i], _controls[i + 1], _controls[i + 2], step);
  } else if (i == _controls.size() - 2) {
    mk0 = m(_controls[i - 1], _controls[i], _controls[i + 1], step);
    mk1 = delta(_controls[i], _controls[i + 1], step);
  } else {
    mk0 = m(_controls[i - 1], _controls[i], _controls[i + 1], step);
    mk1 = m(_controls[i], _controls[i + 1], _controls[i + 2], step);
  }

  // Step 3
  if (std::abs(_controls[i] - _controls[i + 1]) <
      std::numeric_limits<Control>::epsilon()) {
    mk0 = mk1 = 0.0;
  } else {

    // Step 4
    auto ak = mk0 / dk;
    auto bk = mk1 / dk;

    mk0 = ak < 0.0 ? 0.0 : mk0;
    mk1 = bk < 0.0 ? 0.0 : mk1;

    // Step 5
    if (ak * ak + bk * bk > 9) {
      double gk = 3.0 / std::sqrt(ak * ak + bk * bk);
      mk0 = gk * ak * dk;
      mk1 = gk * bk * dk;
    }
  }

  auto h00 = 2.0 * tp * tp * tp - 3.0 * tp * tp + 1.0;
  auto h10 = tp * tp * tp - 2.0 * tp * tp + tp;
  auto h01 = -2.0 * tp * tp * tp + 3.0 * tp * tp;
  auto h11 = tp * tp * tp - tp * tp;

  return _controls[i] * h00 + step * mk0 * h10 + _controls[i + 1] * h01 +
         step * mk1 * h11;
}
