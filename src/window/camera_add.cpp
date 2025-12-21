#include "camera_add.h"
#include "ui_camera_add.h"
#include <QDoubleValidator>
#include <QMessageBox>

CameraAddDialog::CameraAddDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::CameraAddDialog) {
  ui->setupUi(this);
}

CameraAddDialog::CameraAddDialog(Point3 lookfrom, Point3 lookat, double vfov,
                                 double defocus_angle, double focus_dist,
                                 QWidget *parent)
    : QDialog(parent), ui(new Ui::CameraAddDialog)

{
  ui->setupUi(this);

  ui->from_x->setValue(lookfrom.x());
  ui->from_y->setValue(lookfrom.y());
  ui->from_z->setValue(lookfrom.z());

  ui->look_at_x->setValue(lookat.x());
  ui->look_at_y->setValue(lookat.y());
  ui->look_at_z->setValue(lookat.z());

  ui->fov->setValue(vfov);
  ui->defocus_angle->setValue(defocus_angle);
  ui->focus_dist->setValue(focus_dist);
}

CameraAddDialog::~CameraAddDialog() { delete ui; }

double CameraAddDialog::vfov() const { return ui->fov->value(); }

Point3 CameraAddDialog::lookfrom() const {
  return Point3(ui->from_x->value(), ui->from_y->value(), ui->from_z->value());
}

Point3 CameraAddDialog::lookat() const {
  return Point3(ui->look_at_x->value(), ui->look_at_y->value(),
                ui->look_at_z->value());
}

double CameraAddDialog::defocus_angle() const {
  return ui->defocus_angle->value();
}

double CameraAddDialog::focus_dist() const { return ui->focus_dist->value(); }

void CameraAddDialog::on_buttonBox_accepted() {
  // Закрыть диалог с результатом Accepted
  accept();
}