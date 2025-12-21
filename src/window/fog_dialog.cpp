#include "fog_dialog.h"
#include <QColorDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>

FogDialog::FogDialog(const std::vector<size_t> &objectIds, QWidget *parent)
    : QDialog(parent), objectIds_(objectIds),
      currentColor(210, 210, 210) // пример: светло-серый/голубой
{
  setWindowTitle("Add Fog");
  setupUi();
  updateParametersVisibility();
}

void FogDialog::setupUi() {
  auto *mainLayout = new QVBoxLayout(this);

  // Fog type
  auto *fogTypeLayout = new QHBoxLayout();
  fogTypeLayout->addWidget(new QLabel("Fog Type:"));
  fogTypeCombo = new QComboBox();
  fogTypeCombo->addItems(
      {"Постоянный туман", "Неравномерный туман", "Наземный туман"});
  fogTypeLayout->addWidget(fogTypeCombo);
  mainLayout->addLayout(fogTypeLayout);

  // Object ID
  auto *objectIdLayout = new QHBoxLayout();
  objectIdLayout->addWidget(new QLabel("Boundary Object ID:"));
  objectIdCombo = new QComboBox();
  for (size_t id : objectIds_) {
    objectIdCombo->addItem(QString::number(id));
  }
  objectIdLayout->addWidget(objectIdCombo);
  mainLayout->addLayout(objectIdLayout);

  // Color
  auto *colorLayout = new QHBoxLayout();
  colorLayout->addWidget(new QLabel("Fog Color:"));
  colorButton = new QPushButton();
  colorButton->setStyleSheet(
      QString("background-color: %1;").arg(currentColor.name()));
  colorButton->setFixedSize(50, 25);
  colorLayout->addWidget(colorButton);
  mainLayout->addLayout(colorLayout);

  // Parameters group
  parametersGroup = new QGroupBox("Parameters");
  parametersLayout = new QFormLayout(parametersGroup);

  // Density
  densitySpin = new QDoubleSpinBox();
  densitySpin->setRange(0.001, 20.0);
  densitySpin->setSingleStep(0.1);
  densitySpin->setValue(0.9);
  densitySpin->setDecimals(3);
  parametersLayout->addRow("Плотность:", densitySpin);

  // Scale
  scaleSpin = new QDoubleSpinBox();
  scaleSpin->setRange(0.00001, 4.0);
  scaleSpin->setSingleStep(0.001);
  scaleSpin->setValue(0.0025);
  scaleSpin->setDecimals(4);
  parametersLayout->addRow("Масштаб шума:", scaleSpin);

  // Height Falloff
  heightFalloffSpin = new QDoubleSpinBox();
  heightFalloffSpin->setRange(0.0, 0.3);
  heightFalloffSpin->setSingleStep(0.0001);
  heightFalloffSpin->setValue(0.001);
  heightFalloffSpin->setDecimals(5);
  parametersLayout->addRow("Высотный фактор:", heightFalloffSpin);

  mainLayout->addWidget(parametersGroup);

  // Buttons
  auto *buttonBox = new QHBoxLayout();
  auto *okButton = new QPushButton("OK");
  auto *cancelButton = new QPushButton("Cancel");
  okButton->setDefault(true);
  buttonBox->addStretch();
  buttonBox->addWidget(okButton);
  buttonBox->addWidget(cancelButton);
  mainLayout->addLayout(buttonBox);

  // Connections
  connect(fogTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &FogDialog::onFogTypeChanged);
  connect(colorButton, &QPushButton::clicked, this,
          &FogDialog::onColorButtonClicked);
  connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
  connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void FogDialog::onFogTypeChanged(int /*index*/) {
  updateParametersVisibility();
}

void FogDialog::updateParametersVisibility() {
  FogType type = selectedFogType();

  // Показываем/скрываем виджеты
  scaleSpin->setVisible(type == Random || type == GroundHugging);
  heightFalloffSpin->setVisible(type == GroundHugging);

  // Обновляем отображение в QFormLayout (Qt автоматически скрывает метки тоже)
}

void FogDialog::onColorButtonClicked() {
  QColor newColor =
      QColorDialog::getColor(currentColor, this, "Выберите цвет тумана");
  if (newColor.isValid()) {
    currentColor = newColor;
    colorButton->setStyleSheet(
        QString("background-color: %1;").arg(currentColor.name()));
  }
}

FogDialog::FogType FogDialog::selectedFogType() const {
  return static_cast<FogType>(fogTypeCombo->currentIndex());
}

double FogDialog::density() const { return densitySpin->value(); }

double FogDialog::scale() const { return scaleSpin->value(); }

double FogDialog::heightFalloff() const { return heightFalloffSpin->value(); }

size_t FogDialog::selectedObjectId() const {
  int index = objectIdCombo->currentIndex();
  if (index < 0 || static_cast<size_t>(index) >= objectIds_.size()) {
    return 0;
  }
  return objectIds_[index];
}

QColor FogDialog::fogColor() const { return currentColor; }