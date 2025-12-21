#pragma once

#include "vec3.h"
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class object_add_dialog;
}
QT_END_NAMESPACE

class AddObjectDialog : public QDialog {
  Q_OBJECT

public:
  enum ObjectType {
    Sphere,
    Cylinder,
    Cone,
    QuadPyramid,
    TriPyramid,
    Box,
    Quad,
    Triangle
  };

  explicit AddObjectDialog(QWidget *parent = nullptr);
  ~AddObjectDialog();

  ObjectType selectedType() const;

  // Sphere
  Point3 sphereCenter() const;
  double sphereRadius() const;

  // Cylinder
  Point3 cylinderAxisCenter() const;
  double cylinderY0() const;
  double cylinderY1() const;
  double cylinderRadius() const;

  // Cone
  Point3 coneBaseCenter() const;
  double coneRadius() const;
  double coneHeight() const;

  // Quad Pyramid
  Point3 quadPyramidBaseCenter() const;
  double quadPyramidHalfWidth() const;
  double quadPyramidHeight() const;

  // Tri Pyramid
  Point3 triPyramidV0() const;
  Point3 triPyramidV1() const;
  Point3 triPyramidV2() const;
  Point3 triPyramidApex() const;

  // Box
  Point3 boxPointA() const;
  Point3 boxPointB() const;

  // Quad
  Point3 quadQ() const;
  Vec3 quadU() const;
  Vec3 quadV() const;

  // Triangle
  Point3 triA() const;
  Point3 triB() const;
  Point3 triC() const;

private:
  Ui::object_add_dialog *ui;
};