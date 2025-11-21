#include "base_matrix.h"

BaseMatrix::~BaseMatrix() = default;

bool BaseMatrix::is_empty() const noexcept {
  return _rows_count == 0 || _columns_count == 0;
}

BaseMatrix::size_type BaseMatrix::rows_count() const noexcept {
  return _rows_count;
}

BaseMatrix::size_type BaseMatrix::columns_count() const noexcept {
  return _columns_count;
}