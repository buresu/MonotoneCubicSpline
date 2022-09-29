#include "MonotoneCubicSpline.hpp"
#include <algorithm>
#include <cmath>

MonotoneCubicSpline::MonotoneCubicSpline() {}

MonotoneCubicSpline::MonotoneCubicSpline(size_t size) {
  Type step = 1.0 / Type(size - 1);
  for (size_t i = 0; i < size; ++i)
    _controls.push_back({step * Type(i), 0.0});
}

MonotoneCubicSpline::MonotoneCubicSpline(const Controls &controls) {
  setControls(controls);
}

MonotoneCubicSpline::~MonotoneCubicSpline() {}

bool MonotoneCubicSpline::isValid() const { return _controls.size() >= 2; }

size_t MonotoneCubicSpline::size() const { return _controls.size(); }

std::vector<MonotoneCubicSpline::Type>
MonotoneCubicSpline::controlKeys() const {
  std::vector<Type> keys;
  std::transform(_controls.begin(), _controls.end(), std::back_inserter(keys),
                 [](const auto &p) { return p.first; });
  return keys;
}

MonotoneCubicSpline::Control MonotoneCubicSpline::control(size_t index) const {
  return _controls[index];
}

void MonotoneCubicSpline::setControl(size_t index, const Control &control) {
  _controls[index] = control;
  std::sort(
      _controls.begin(), _controls.end(),
      [](const auto &lhs, const auto &rhs) { return lhs.first < rhs.first; });
}

void MonotoneCubicSpline::addControl(const Control &control) {
  _controls.push_back(control);
  std::sort(
      _controls.begin(), _controls.end(),
      [](const auto &lhs, const auto &rhs) { return lhs.first < rhs.first; });
}

void MonotoneCubicSpline::removeControl(size_t index) {
  _controls.erase(_controls.begin() + index);
}

void MonotoneCubicSpline::removeControl(const Control &control) {
  _controls.erase(std::remove(_controls.begin(), _controls.end(), control),
                  _controls.end());
}

MonotoneCubicSpline::Controls MonotoneCubicSpline::controls() const {
  return _controls;
}

void MonotoneCubicSpline::setControls(const Controls &controls) {
  _controls = controls;
  std::sort(
      _controls.begin(), _controls.end(),
      [](const auto &lhs, const auto &rhs) { return lhs.first < rhs.first; });
}

void MonotoneCubicSpline::reshape(size_t size) {

  if (size == _controls.size())
    return;

  Controls controls(size);

  const Type step = 1.0 / Type(size - 1);

  for (size_t i = 0; i < size; ++i)
    controls[i] = {step * Type(i), interpolate(step * Type(i))};

  setControls(controls);
}

MonotoneCubicSpline::Type MonotoneCubicSpline::interpolate(Type t) const {

  const auto keys = controlKeys();
  const auto min = *std::min_element(keys.begin(), keys.end());
  const auto max = *std::max_element(keys.begin(), keys.end());

  // Out of range
  if (t <= min) {
    return _controls.front().second;
  } else if (t >= max) {
    return _controls.back().second;
  }

  // Find index
  size_t idx;

  for (size_t i = 0; i < _controls.size() - 1; ++i) {
    if (_controls[i].first <= t && _controls[i + 1].first > t) {
      idx = i;
      break;
    }
  }

  // Step 1
  static auto delta = [](const Control &c0, const Control &c1) {
    return (c1.second - c0.second) / (c1.first - c0.first);
  };

  auto dk = delta(_controls[idx], _controls[idx + 1]);

  // Step 2
  static auto m = [&](const Control &c0, const Control &c1, const Control &c2) {
    auto d0 = delta(c0, c1);
    auto d1 = delta(c1, c2);
    return d0 * d1 < 0.0 ? 0.0 : (d0 + d1) * 0.5;
  };

  Type mk0, mk1;

  if (idx == 0) {
    mk0 = delta(_controls[idx], _controls[idx + 1]);
    mk1 = m(_controls[idx], _controls[idx + 1], _controls[idx + 2]);
  } else if (idx == _controls.size() - 2) {
    mk0 = m(_controls[idx - 1], _controls[idx], _controls[idx + 1]);
    mk1 = delta(_controls[idx], _controls[idx + 1]);
  } else {
    mk0 = m(_controls[idx - 1], _controls[idx], _controls[idx + 1]);
    mk1 = m(_controls[idx], _controls[idx + 1], _controls[idx + 2]);
  }

  // Step 3
  if (std::abs(_controls[idx].second - _controls[idx + 1].second) <
      std::numeric_limits<Type>::epsilon()) {
    mk0 = mk1 = 0.0;
  } else {

    // Step 4
    auto ak = mk0 / dk;
    auto bk = mk1 / dk;

    mk0 = ak < 0.0 ? 0.0 : mk0;
    mk1 = bk < 0.0 ? 0.0 : mk1;

    // Step 5
    if (ak * ak + bk * bk > 9) {
      Type gk = 3.0 / std::sqrt(ak * ak + bk * bk);
      mk0 = gk * ak * dk;
      mk1 = gk * bk * dk;
    }
  }

  const Type step = _controls[idx + 1].first - _controls[idx].first;
  const Type lt = (t - _controls[idx].first) / step;
  const auto h00 = 2.0 * lt * lt * lt - 3.0 * lt * lt + 1.0;
  const auto h10 = lt * lt * lt - 2.0 * lt * lt + lt;
  const auto h01 = -2.0 * lt * lt * lt + 3.0 * lt * lt;
  const auto h11 = lt * lt * lt - lt * lt;

  return _controls[idx].second * h00 + step * mk0 * h10 +
         _controls[idx + 1].second * h01 + step * mk1 * h11;
}
