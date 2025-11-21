#include "color_matrix.h"

#include "gtest/gtest.h"
#include <iostream>

TEST(size, size_1) {
  auto color_matrix = std::make_shared<ColorMatrix>(6, 5);

  EXPECT_EQ(color_matrix->size().x(), 6);
  EXPECT_EQ(color_matrix->size().y(), 5);
}
