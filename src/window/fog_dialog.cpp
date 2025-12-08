#include "fog_dialog.h"
#include "ui_fog_dialog.h"
#include <QColorDialog>

FogDialog::FogDialog(const std::vector<size_t> &object_ids, QWidget *parent)
    : QDialog(parent), ui(new Ui::FogDialog), _objectIds(object_ids)
{
    ui->setupUi(this);

    // Заполняем выпадающий список ID объектов
    for (size_t id : _objectIds)
    {
        ui->objectComboBox->addItem(QString::number(id), static_cast<qulonglong>(id));
    }

    // Цвет по умолчанию — белый
    _fogColor = Qt::white;
    ui->colorPreview->setStyleSheet("background-color: white;");

    connect(ui->chooseColorButton, &QPushButton::clicked,
            this, &FogDialog::on_chooseColorButton_clicked);
}

FogDialog::~FogDialog()
{
    delete ui;
}

void FogDialog::on_chooseColorButton_clicked()
{
    QColor color = QColorDialog::getColor(_fogColor, this, "Цвет тумана");
    if (color.isValid())
    {
        _fogColor = color;
        ui->colorPreview->setStyleSheet(QString("background-color: %1;").arg(color.name()));
    }
}

size_t FogDialog::selectedObjectId() const
{
    int idx = ui->objectComboBox->currentIndex();
    if (idx < 0 || idx >= static_cast<int>(_objectIds.size()))
    {
        throw std::runtime_error("No object selected");
    }
    return _objectIds[idx];

    // Альтернатива, если используете setData:
    // return ui->objectComboBox->currentData().toULongLong();
}

double FogDialog::density() const
{
    return ui->densitySpinBox->value();
}

QColor FogDialog::fogColor() const
{
    return _fogColor;
}