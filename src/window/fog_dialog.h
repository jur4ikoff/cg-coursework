// #pragma once

// #include <QDialog>
// #include <QColor>

// QT_BEGIN_NAMESPACE
// namespace Ui { class FogDialog; }
// QT_END_NAMESPACE

// class FogDialog : public QDialog
// {
//     Q_OBJECT

// public:
//     enum FogType {
//         Basic,
//         Noisy,
//         GroundHugging
//     };

//     explicit FogDialog(QWidget* parent = nullptr);
//     ~FogDialog();

//     FogType fogType() const;
//     double density() const;
//     QColor fogColor() const;
//     double scale() const;          // для Noisy и GroundHugging
//     double groundFactor() const;   // только для GroundHugging

// private slots:
//     void on_typeComboBox_currentIndexChanged(int index);
//     void on_chooseColorButton_clicked();

// private:
//     void updateVisibility();

//     Ui::FogDialog* ui;
//     QColor _fogColor;
// };