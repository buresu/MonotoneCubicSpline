#pragma once

#include <Eigen/Core>
#include <Eigen/StdVector>

class MonotoneCubicSpline {
public:
  typedef double Control;
  typedef std::vector<Control, Eigen::aligned_allocator<Control>> Controls;

  MonotoneCubicSpline();
  explicit MonotoneCubicSpline(size_t size);
  explicit MonotoneCubicSpline(const Controls &controls);
  ~MonotoneCubicSpline();

  bool isValid() const;

  size_t size() const;

  Control control(size_t index) const;
  void setControl(size_t index, const Control &control);

  Controls controls() const;
  void setControls(const Controls &controls);

  void reshape(size_t size);

  Control interpolate(double t) const;

protected:
  Controls _controls;
};
