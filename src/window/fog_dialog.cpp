// #include "fog_dialog.h"
// #include "ui_fog_dialog.h"
// #include <QColorDialog>

// FogDialog::FogDialog(QWidget* parent)
//     : QDialog(parent)
//     , ui(new Ui::FogDialog)
// {
//     ui->setupUi(this);

//     _fogColor = Qt::white;
//     ui->colorPreview->setStyleSheet("background-color: white;");

//     connect(ui->typeComboBox, &QComboBox::currentIndexChanged,
//             this, &FogDialog::on_typeComboBox_currentIndexChanged);
//     connect(ui->chooseColorButton, &QPushButton::clicked,
//             this, &FogDialog::on_chooseColorButton_clicked);

//     updateVisibility();
// }

// FogDialog::~FogDialog()
// {
//     delete ui;
// }

// void FogDialog::on_typeComboBox_currentIndexChanged(int)
// {
//     updateVisibility();
// }

// void FogDialog::updateVisibility()
// {
//     int type = ui->typeComboBox->currentIndex();

//     bool showScale = (type == Noisy || type == GroundHugging);
//     bool showGround = (type == GroundHugging);

//     ui->scaleGroup->setVisible(showScale);
//     ui->groundGroup->setVisible(showGround);

//     adjustSize();
// }

// void FogDialog::on_chooseColorButton_clicked()
// {
//     QColor c = QColorDialog::getColor(_fogColor, this, "Цвет тумана");
//     if (c.isValid()) {
//         _fogColor = c;
//         ui->colorPreview->setStyleSheet(QString("background-color: %1;").arg(c.name()));
//     }
// }

// FogDialog::FogType FogDialog::fogType() const
// {
//     return static_cast<FogType>(ui->typeComboBox->currentIndex());
// }

// double FogDialog::density() const { return ui->densitySpinBox->value(); }
// QColor FogDialog::fogColor() const { return _fogColor; }
// double FogDialog::scale() const { return ui->scaleSpinBox->value(); }
// double FogDialog::groundFactor() const { return ui->groundFactorSpinBox->value(); }