#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class RotateObjectDialog;
}
QT_END_NAMESPACE

class RotateObjectDialog : public QDialog {
  Q_OBJECT

public:
  explicit RotateObjectDialog(QWidget *parent = nullptr);
  ~RotateObjectDialog();

  double angleX() const;
  double angleY() const;
  double angleZ() const;

private:
  Ui::RotateObjectDialog *ui;
};