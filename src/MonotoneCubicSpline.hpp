#pragma once

#include <stddef.h>
#include <vector>

class MonotoneCubicSpline {
public:
  typedef double Type;
  typedef std::pair<Type, Type> Control;
  typedef std::vector<Control> Controls;

  MonotoneCubicSpline();
  explicit MonotoneCubicSpline(size_t size);
  explicit MonotoneCubicSpline(const Controls &controls);
  ~MonotoneCubicSpline();

  bool isValid() const;

  size_t size() const;

  std::vector<Type> controlKeys() const;

  Control control(size_t index) const;
  void setControl(size_t index, const Control &control);

  void addControl(const Control &control);
  void removeControl(size_t index);
  void removeControl(const Control &control);

  Controls controls() const;
  void setControls(const Controls &controls);

  void reshape(size_t size);

  Type interpolate(Type t) const;

protected:
  Controls _controls;
};
