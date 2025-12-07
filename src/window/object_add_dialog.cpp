
#include "object_add_dialog.h"
#include "ui_object_add_dialog.h"

AddObjectDialog::AddObjectDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::object_add_dialog)
{
    ui->setupUi(this);
}

AddObjectDialog::~AddObjectDialog()
{
    delete ui;
}

// Сфера
Point3 AddObjectDialog::sphereCenter() const
{
    return Point3(ui->sphereCenterX->value(), ui->sphereCenterY->value(), ui->sphereCenterZ->value());
}
double AddObjectDialog::sphereRadius() const
{
    return ui->sphereRadius->value();
}

// Коробка
Point3 AddObjectDialog::boxPointA() const
{
    return Point3(ui->boxAX->value(), ui->boxAY->value(), ui->boxAZ->value());
}
Point3 AddObjectDialog::boxPointB() const
{
    return Point3(ui->boxBX->value(), ui->boxBY->value(), ui->boxBZ->value());
}

// Цилиндр
Point3 AddObjectDialog::cylinderAxisCenter() const
{
    return Point3(ui->cylinderAxisX->value(), 0.0, ui->cylinderAxisZ->value());
}
double AddObjectDialog::cylinderY0() const { return ui->cylinderY0->value(); }
double AddObjectDialog::cylinderY1() const { return ui->cylinderY1->value(); }
double AddObjectDialog::cylinderRadius() const { return ui->cylinderRadius->value(); }

// Конус
Point3 AddObjectDialog::coneBaseCenter() const
{
    return Point3(ui->coneBaseX->value(), ui->coneBaseY->value(), ui->coneBaseZ->value());
}
double AddObjectDialog::coneRadius() const { return ui->coneRadius->value(); }
double AddObjectDialog::coneHeight() const { return ui->coneHeight->value(); }

// 4-уг. пирамида
Point3 AddObjectDialog::quadPyramidBaseCenter() const
{
    return Point3(ui->quadPyramidCenterX->value(), ui->quadPyramidCenterY->value(), ui->quadPyramidCenterZ->value());
}
double AddObjectDialog::quadPyramidHalfWidth() const { return ui->quadPyramidHalfWidth->value(); }
double AddObjectDialog::quadPyramidHeight() const { return ui->quadPyramidHeight->value(); }

// 3-уг. пирамида
Point3 AddObjectDialog::triPyramidV0() const
{
    return Point3(ui->triV0X->value(), ui->triV0Y->value(), ui->triV0Z->value());
}
Point3 AddObjectDialog::triPyramidV1() const
{
    return Point3(ui->triV1X->value(), ui->triV1Y->value(), ui->triV1Z->value());
}
Point3 AddObjectDialog::triPyramidV2() const
{
    return Point3(ui->triV2X->value(), ui->triV2Y->value(), ui->triV2Z->value());
}
Point3 AddObjectDialog::triPyramidApex() const
{
    return Point3(ui->triApexX->value(), ui->triApexY->value(), ui->triApexZ->value());
}

// Quad (четырёхугольник)
Point3 AddObjectDialog::quadQ() const
{
    return Point3(ui->quadQX->value(), ui->quadQY->value(), ui->quadQZ->value());
}
// Для Quad: u = P1 - Q, v = P2 - Q, но в UI вы вводите компоненты U и V напрямую
// Однако ваш UI вводит именно векторы U и V (подписано как "U X", "V X" и т.д.)
// Поэтому:
Vec3 AddObjectDialog::quadU() const
{
    return Vec3(ui->quadP1X->value(), ui->quadP1Y->value(), ui->quadP1Z->value());
}
Vec3 AddObjectDialog::quadV() const
{
    return Vec3(ui->quadP2X->value(), ui->quadP2Y->value(), ui->quadP2Z->value());
}

// Triangle
Point3 AddObjectDialog::triA() const
{
    return Point3(ui->triAX->value(), ui->triAY->value(), ui->triAZ->value());
}
Point3 AddObjectDialog::triB() const
{
    return Point3(ui->triBX->value(), ui->triBY->value(), ui->triBZ->value());
}
Point3 AddObjectDialog::triC() const
{
    return Point3(ui->triCX->value(), ui->triCY->value(), ui->triCZ->value());
}

// Тип по индексу (если нужно)
AddObjectDialog::ObjectType AddObjectDialog::selectedType() const
{
    // Сопоставление индекса с enum
    static const ObjectType mapping[] = {
        Sphere, Cylinder, Cone, QuadPyramid, TriPyramid, Box, Quad, Triangle};
    int idx = ui->typeComboBox->currentIndex();
    if (idx >= 0 && idx < static_cast<int>(sizeof(mapping) / sizeof(mapping[0])))
    {
        return mapping[idx];
    }
    return Sphere;
}