#include "material_dialog.h"
#include "ui_material_dialog.h"
#include <QColorDialog>

MaterialDialog::MaterialDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MaterialDialog)
{
    ui->setupUi(this);

    _lambertianColor = Qt::white;
    _metalColor = Qt::lightGray;

    ui->lambertianColorPreview->setStyleSheet("background-color: white;");
    ui->metalColorPreview->setStyleSheet("background-color: lightgray;");

    connect(ui->typeComboBox, &QComboBox::currentIndexChanged,
            this, &MaterialDialog::on_typeComboBox_currentIndexChanged);
    connect(ui->lambertianColorButton, &QPushButton::clicked,
            this, &MaterialDialog::on_lambertianColorButton_clicked);
    connect(ui->metalColorButton, &QPushButton::clicked,
            this, &MaterialDialog::on_metalColorButton_clicked);

    updateFields();
}

MaterialDialog::~MaterialDialog()
{
    delete ui;
}

void MaterialDialog::on_typeComboBox_currentIndexChanged(int)
{
    updateFields();
}

void MaterialDialog::updateFields()
{
    int idx = ui->typeComboBox->currentIndex();
    ui->lambertianGroup->setVisible(idx == 0);
    ui->metalGroup->setVisible(idx == 1);
    ui->transparentGroup->setVisible(idx == 2);
    adjustSize();
}

void MaterialDialog::on_lambertianColorButton_clicked()
{
    QColor c = QColorDialog::getColor(_lambertianColor, this, "Цвет матового материала");
    if (c.isValid()) {
        _lambertianColor = c;
        ui->lambertianColorPreview->setStyleSheet(QString("background-color: %1;").arg(c.name()));
    }
}

void MaterialDialog::on_metalColorButton_clicked()
{
    QColor c = QColorDialog::getColor(_metalColor, this, "Цвет металла");
    if (c.isValid()) {
        _metalColor = c;
        ui->metalColorPreview->setStyleSheet(QString("background-color: %1;").arg(c.name()));
    }
}

MaterialType MaterialDialog::selectedType() const
{
    return static_cast<MaterialType>(ui->typeComboBox->currentIndex());
}

QColor MaterialDialog::lambertianColor() const { return _lambertianColor; }
QColor MaterialDialog::metalColor() const { return _metalColor; }
double MaterialDialog::fuzz() const { return ui->fuzzSpinBox->value(); }
double MaterialDialog::refractionIndex() const { return ui->iorSpinBox->value(); }