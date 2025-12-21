#pragma once

#include "vec3.h" // для Point3
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class MoveObjectDialog;
}
QT_END_NAMESPACE

class MoveObjectDialog : public QDialog {
  Q_OBJECT

public:
  explicit MoveObjectDialog(QWidget *parent = nullptr);
  ~MoveObjectDialog();

  Point3 offset() const;

private:
  Ui::MoveObjectDialog *ui;
};