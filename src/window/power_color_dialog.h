#pragma once

#include <QColor>
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class PowerColorDialog;
}
QT_END_NAMESPACE

class PowerColorDialog : public QDialog {
  Q_OBJECT

public:
  explicit PowerColorDialog(QWidget *parent = nullptr);
  ~PowerColorDialog();

  double power() const;
  QColor color() const;

private slots:
  void on_chooseColorButton_clicked();

private:
  Ui::PowerColorDialog *ui;
  QColor _selectedColor;
};