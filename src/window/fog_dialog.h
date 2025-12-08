#pragma once

#include <QDialog>
#include <QColor>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class FogDialog;
}
QT_END_NAMESPACE

class FogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FogDialog(const std::vector<size_t> &object_ids, QWidget *parent = nullptr);
    ~FogDialog();

    size_t selectedObjectId() const;
    double density() const;
    QColor fogColor() const;

private slots:
    void on_chooseColorButton_clicked();

private:
    Ui::FogDialog *ui;
    QColor _fogColor;
    std::vector<size_t> _objectIds;
};