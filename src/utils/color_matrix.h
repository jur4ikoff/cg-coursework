#pragma once

#include "base_matrix.h"
#include "color.h"

#include <QImage>
#include <stdexcept>
#include <vector>

class ColorMatrix : public BaseMatrix {
private:
  std::vector<std::vector<Color>> _data;

public:
  // Конструкторы
  ColorMatrix() {
    _rows_count = 0;
    _columns_count = 0;
  }

  ColorMatrix(size_type rows_count, size_type columns_count) {
    _rows_count = rows_count;
    _columns_count = columns_count;

    _data.resize(rows_count, std::vector<Color>(columns_count));
  }

  ColorMatrix(size_type rows_count, size_type columns_count,
              const Color &default_color) {
    _rows_count = rows_count;
    _columns_count = columns_count;

    _data.resize(rows_count, std::vector<Color>(columns_count, default_color));
  }

  // Доступ к элементам
  Color &at(size_type row, size_type column) {
    if (row >= _rows_count || column >= _columns_count)
      throw std::out_of_range("Index out of range");
    return _data[row][column];
  }

  const Color &at(size_type row, size_type column) const {
    if (row >= _rows_count || column >= _columns_count)
      throw std::out_of_range("Index out of range");
    return _data[row][column];
  }

  // Оператор доступа
  Color &operator()(size_type row, size_type column) {
    return _data[row][column];
  }

  const Color &operator()(size_type row, size_type column) const {
    return _data[row][column];
  }

  Vec3 size() { return Vec3(_rows_count, _columns_count, 0); }

  // Изменение размера
  void resize(size_type new_rows, size_type new_columns,
              const Color &default_color = Color()) {
    _data.resize(new_rows);
    for (auto &row : _data)
      row.resize(new_columns, default_color);

    _rows_count = new_rows;
    _columns_count = new_columns;
  }

  // Заполнение всей матрицы одним цветом
  void fill(const Color &color) {
    for (auto &row : _data)
      for (auto &cell : row)
        cell = color;
  }

  // Очистка матрицы
  void clear() {
    _data.clear();
    _rows_count = 0;
    _columns_count = 0;
  }

  // Проверка на пустоту
  bool empty() const { return _data.empty(); }

  // Обмен содержимым с другой матрицей
  void swap(ColorMatrix &other) {
    std::swap(_data, other._data);
    std::swap(_rows_count, other._rows_count);
    std::swap(_columns_count, other._columns_count);
  }

  // Получение строки (для итерации)
  std::vector<Color> &operator[](size_type row) { return _data[row]; }

  const std::vector<Color> &operator[](size_type row) const {
    return _data[row];
  }

  QImage to_QImage() const {
    if (empty())
      return QImage();

    QImage image(_columns_count, _rows_count, QImage::Format_RGB32);

    for (size_type y = 0; y < _rows_count; ++y) {
      for (size_type x = 0; x < _columns_count; ++x) {
        const Color &color = _data[y][x];
        const QColor q_color = get_QColor(color);
        image.setPixelColor(x, y, q_color);
      }
    }

    return image;
  }

  // Преобразование в QPixmap
  QPixmap toQPixmap() const { return QPixmap::fromImage(to_QImage()); }

  // Итераторы для range-based for
  auto begin() { return _data.begin(); }
  auto end() { return _data.end(); }
  auto begin() const { return _data.begin(); }
  auto end() const { return _data.end(); }
  auto cbegin() const { return _data.cbegin(); }
  auto cend() const { return _data.cend(); }
};