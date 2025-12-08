#include "rotate_object_dialog.h"
#include "ui_rotate_object_dialog.h"

RotateObjectDialog::RotateObjectDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::RotateObjectDialog)
{
    ui->setupUi(this);
}

RotateObjectDialog::~RotateObjectDialog()
{
    delete ui;
}

double RotateObjectDialog::angleX() const { return ui->xSpinBox->value(); }
double RotateObjectDialog::angleY() const { return ui->ySpinBox->value(); }
double RotateObjectDialog::angleZ() const { return ui->zSpinBox->value(); }