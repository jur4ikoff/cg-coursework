#pragma once

#include <QDialog>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QColor>
#include <vector>
#include <cstddef>

class FogDialog : public QDialog
{
    Q_OBJECT

public:
    enum FogType {
        Constant,
        Random,
        GroundHugging
    };

    explicit FogDialog(const std::vector<size_t>& objectIds, QWidget* parent = nullptr);

    FogType selectedFogType() const;
    double density() const;
    double scale() const;
    double heightFalloff() const;
    size_t selectedObjectId() const;
    QColor fogColor() const;

private slots:
    void onFogTypeChanged(int index);
    void onColorButtonClicked();

private:
    void setupUi();
    void updateParametersVisibility();

    QComboBox* fogTypeCombo;
    QComboBox* objectIdCombo;
    QPushButton* colorButton;

    // Все спинбоксы существуют всегда
    QDoubleSpinBox* densitySpin;
    QDoubleSpinBox* scaleSpin;
    QDoubleSpinBox* heightFalloffSpin;

    QGroupBox* parametersGroup;
    QFormLayout* parametersLayout; // используем QFormLayout для удобства

    std::vector<size_t> objectIds_;
    QColor currentColor;
};