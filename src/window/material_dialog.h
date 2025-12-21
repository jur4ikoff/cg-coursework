#pragma once

#include "material_struct.h"
#include <QColor>
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class MaterialDialog;
}
QT_END_NAMESPACE

class MaterialDialog : public QDialog {
  Q_OBJECT

public:
  explicit MaterialDialog(QWidget *parent = nullptr);
  ~MaterialDialog();

  MaterialType selectedType() const;
  QColor lambertianColor() const;
  QColor metalColor() const;
  double fuzz() const;
  double refractionIndex() const;

private slots:
  void on_typeComboBox_currentIndexChanged(int index);
  void on_lambertianColorButton_clicked();
  void on_metalColorButton_clicked();

private:
  Ui::MaterialDialog *ui;
  void updateFields();
  QColor _lambertianColor;
  QColor _metalColor;
};