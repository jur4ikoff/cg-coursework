#pragma once

#include <iostream>

class BaseMatrix {
public:
  using size_type = size_t;
  virtual ~BaseMatrix();

  bool is_empty() const noexcept;

  size_type rows_count() const noexcept;
  size_type columns_count() const noexcept;

protected:
  size_type _rows_count = 0;
  size_type _columns_count = 0;
};