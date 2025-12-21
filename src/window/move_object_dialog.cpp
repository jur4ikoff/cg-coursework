#include "move_object_dialog.h"
#include "ui_move_object_dialog.h"

MoveObjectDialog::MoveObjectDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::MoveObjectDialog) {
  ui->setupUi(this);
}

MoveObjectDialog::~MoveObjectDialog() { delete ui; }

Point3 MoveObjectDialog::offset() const {
  return Point3(ui->xSpinBox->value(), ui->ySpinBox->value(),
                ui->zSpinBox->value());
}