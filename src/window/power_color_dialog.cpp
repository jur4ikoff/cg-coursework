#include "power_color_dialog.h"
#include "ui_power_color_dialog.h"
#include <QColorDialog>
#include <QPalette>

PowerColorDialog::PowerColorDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::PowerColorDialog) {
  ui->setupUi(this);
  _selectedColor = Qt::white;
  ui->colorPreview->setStyleSheet("background-color: white;");
}

PowerColorDialog::~PowerColorDialog() { delete ui; }

void PowerColorDialog::on_chooseColorButton_clicked() {
  QColor color = QColorDialog::getColor(_selectedColor, this, "Выберите цвет");
  if (color.isValid()) {
    _selectedColor = color;
    ui->colorPreview->setStyleSheet(
        QString("background-color: %1;").arg(color.name()));
  }
}

double PowerColorDialog::power() const { return ui->powerSpinBox->value(); }

QColor PowerColorDialog::color() const { return _selectedColor; }