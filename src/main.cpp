#include "main_window.h"
#include "research.h"

#include <QApplication>

int main(int argc, char **argv) {
  QApplication application(argc, argv);

  MainWindow window;
  window.show();

  return application.exec();
}

// int main()
// {
// run_sample_count_exp(10, 100, 10);
// run_depth_exp(1, 15, 2);
// }