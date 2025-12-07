// #pragma once

// #include <QDialog>
// #include "vec3.h"

// QT_BEGIN_NAMESPACE
// namespace Ui { class AddObjectDialog; }
// QT_END_NAMESPACE

// class AddObjectDialog : public QDialog
// {
//     Q_OBJECT

// public:
//     enum ObjectType {
//         Sphere,
//         Cylinder,
//         Cone,
//         QuadPyramid,
//         TriPyramid,
//         Box
//     };

//     explicit AddObjectDialog(QWidget *parent = nullptr);
//     ~AddObjectDialog();

//     ObjectType selectedType() const;

//     // Sphere
//     point3 sphereCenter() const;
//     double sphereRadius() const;

//     // Cylinder
//     point3 cylinderAxisCenter() const; // x, z — центр оси, y игнорируется
//     double cylinderY0() const;
//     double cylinderY1() const;
//     double cylinderRadius() const;

//     // Cone
//     point3 coneBaseCenter() const;
//     double coneRadius() const;
//     double coneHeight() const;

//     // Quad Pyramid
//     point3 quadPyramidBaseCenter() const;
//     double quadPyramidHalfWidth() const;
//     double quadPyramidHeight() const;

//     // Tri Pyramid
//     point3 triPyramidV0() const;
//     point3 triPyramidV1() const;
//     point3 triPyramidV2() const;
//     point3 triPyramidApex() const;

//     // Box
//     point3 boxPointA() const;
//     point3 boxPointB() const;

// private slots:
//     void on_typeComboBox_currentIndexChanged(int index);

// private:
//     Ui::AddObjectDialog *ui;
//     void updateFields();
// };