#pragma once

#include "vec3.h"
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class CameraAddDialog;
}
QT_END_NAMESPACE

class CameraAddDialog : public QDialog {
  Q_OBJECT

public:
  explicit CameraAddDialog(QWidget *parent = nullptr);
  CameraAddDialog(Point3 lookfrom, Point3 lookat, double vfov,
                  double defocus_angle, double focus_dist,
                  QWidget *parent = nullptr);

  ~CameraAddDialog();

  double vfov() const;
  Point3 lookfrom() const;
  Point3 lookat() const;
  // Vec3 vup() const;
  double defocus_angle() const;
  double focus_dist() const;

private slots:
  void on_buttonBox_accepted();

private:
  Ui::CameraAddDialog *ui;
  bool validateInputs() const;
};
