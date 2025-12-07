#include "main_window.h"
#include "ui_main_window.h"

#include "camera_add.h"
#include "color.h"
#include "start_simple_render_command.h"

#include <QCloseEvent>
#include <QDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QtConcurrent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)

{
    setWindowTitle("Рейтрейсинг");

    ui->setupUi(this);
    ui->graphicsView->viewport()->setMinimumSize(MIN_SCENE_WIDTH,
                                                 MIN_SCENE_HEIGHT);
    ui->graphicsView->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff); // Отключаем скроллбары
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QSize size = ui->graphicsView->viewport()->size();
    _qt_scene = std::make_shared<QGraphicsScene>(this);
    _pixmap = std::make_shared<QPixmap>(size);

    _drawer = std::make_shared<Drawer>(*_pixmap.get());
    _render_drawer = std::make_shared<Drawer>(*_pixmap.get());
    _scene = std::make_shared<Scene>();
    _render = std::make_shared<Render>();
    _facade = std::make_shared<Facade>();

    init_camera();
    init_scene();

    connect(&_futureWatcher, &QFutureWatcher<void>::finished, this,
            &MainWindow::tile_render_finished_slot);

    set_scene();
}

void MainWindow::init_camera()
{
    auto camera = std::make_shared<Camera>();
    camera->id = 0;
    camera->vfov = 40;
    camera->lookfrom = Point3(278, 278, -800);
    camera->lookat = Point3(278, 278, 0);
    camera->vup = Vec3(0, 1, 0);
    camera->defocus_angle = 0;
    camera->focus_dist = 10;
    camera_list.add_camera(camera);

    update_camera_list();
}

void MainWindow::init_scene()
{
    _world = _scene->make_default_scene();
    // _scene->make_default_scene();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (!_scene || !ui->graphicsView->scene() ||
        ui->graphicsView->scene()->items().isEmpty())
        return;

    _livetime_render();
}

void MainWindow::tile_render_finished_slot()
{
    if (!cancel_running)
    {
        _render_drawer->draw(*_render_color_matrix);
        update_render_scene();
    }
}

void MainWindow::set_scene()
{
    _qt_scene->setSceneRect(_pixmap.get()->rect());
    ui->graphicsView->setScene(_qt_scene.get());

    QGraphicsPixmapItem *pixmapItem = _qt_scene->addPixmap(*_pixmap.get());
}

void MainWindow::on_renderButton_clicked()
{
    cancel_running = false;

    _popup = new QWidget();
    _popup->setAttribute(Qt::WA_DeleteOnClose);
    connect(_popup, &QWidget::destroyed, this, &MainWindow::pop_up_closed_slot);

    ui->renderButton->setEnabled(false);
    QSize size = ui->graphicsView->viewport()->size();

    _render_color_matrix =
        std::make_shared<ColorMatrix>(size.height(), size.width());
    _render_pixmap = std::make_shared<QPixmap>(size);
    _render_drawer = std::make_shared<Drawer>(*_render_pixmap);
    _render_pixmap->fill(Qt::black);

    final_render_settings.samples_per_pixel = ui->renderSampleCount->value();
    final_render_settings.max_depth = ui->renderDepthCount->value();

    try
    {
        _render->set_camera(*camera_list.get_active_camera());
    }
    catch (const std::exception &e)
    {
        show_error("Ошибка", "Нет активной камеры");
        ui->renderButton->setEnabled(true);
        return;
    }
    _render->set_render_settings(final_render_settings);
    _futureWatcher.setFuture(QtConcurrent::run([this, size]() { _render->render(_world,
    *_render_color_matrix,
    cancel_running,
    16,
    [this]() { QMetaObject::invokeMethod(this, "tile_render_finished_slot",
                                                                                                                                                                       Qt::QueuedConnection); }); }));

    _render_label = new QLabel(_popup);
    update_render_scene();
    _render_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QVBoxLayout *layout = new QVBoxLayout(_popup);
    layout->addWidget(_render_label, 0, Qt::AlignCenter);
    layout->setContentsMargins(0, 0, 0, 0);

    _popup->resize(size);
    _popup->show();
}

void MainWindow::on_changeWorldColor_clicked()
{
    QColor selectedColor = QColorDialog::getColor(Qt::white, this, "Выберите цвет");

    if (selectedColor.isValid())
    {
        QString style = QString("background-Color: rgb(%1, %2, %3);")
                            .arg(selectedColor.red())
                            .arg(selectedColor.green())
                            .arg(selectedColor.blue());
        ui->changeWorldColor->setStyleSheet(style);

        float r = selectedColor.redF();
        float g = selectedColor.greenF();
        float b = selectedColor.blueF();
        float a = selectedColor.alphaF();

        final_render_settings.background = Color{ r, g, b };
        live_render_settings.background = Color{ r, g, b };

        _livetime_render();
    }
}

void MainWindow::on_cameraDeleteButton_clicked()
{
    auto cams = get_selected_camera();
    if (cams.size() == 0)
    {
        show_error("Ошибка", "Нужно выбрать хотя бы одну камеру");
        return;
    }
    for (size_t id : cams)
    {
        camera_list.delete_camera(id);
    }
    update_camera_list();
}

void MainWindow::on_CameraAddDialogButton_clicked()
{
    CameraAddDialog dialog(this);
    Point3 vup = Point3{ 0, 1, 0 };

    if (dialog.exec() == QDialog::Accepted)
    {
        try
        {
            auto camera = std::make_shared<Camera>(dialog.vfov(), dialog.lookfrom(), dialog.lookat(), vup);
            camera->defocus_angle = dialog.defocus_angle();
            camera->focus_dist = dialog.focus_dist();
            camera->id = camera_list.get_max_id() + 1;

            camera_list.add_camera(camera);
        }
        catch (const std::exception &e)
        {
            QMessageBox::critical(this, "Ошибка", "Невозможно добавить камеру.");
        }
        update_camera_list();
    }
}

void MainWindow::on_cameraEditButton_clicked()
{
    auto cams = get_selected_camera();
    if (cams.size() != 1)
    {
        show_error("Ошибка", "Для изменения необходимо выбрать ОДНУ камеру");
        return;
    }

    size_t id = cams[0];
    auto camera = camera_list.get_camera(id);

    CameraAddDialog dialog(
        camera->lookfrom, camera->lookat,
        camera->vfov, camera->defocus_angle,
        camera->focus_dist, this);
    Point3 vup = Point3{ 0, 1, 0 };

    if (dialog.exec() == QDialog::Accepted)
    {
        try
        {
            camera->lookat = dialog.lookat();
            camera->lookfrom = dialog.lookfrom();
            camera->vfov = dialog.vfov();
            camera->defocus_angle = dialog.defocus_angle();
            camera->focus_dist = dialog.focus_dist();
        }
        catch (const std::exception &e)
        {
            QMessageBox::critical(this, "Ошибка", "Невозможно отредактировать камеру.");
        }
        _livetime_render();
    }
}

void MainWindow::on_cameraSetButton_clicked()
{
    auto cams = get_selected_camera();
    if (cams.size() != 1)
    {
        show_error("Ошибка", "Выбрать активной можно только одну камеру");
        return;
    }

    size_t id = cams[0];
    camera_list.set_active_camera(id);
    _livetime_render();
}

void MainWindow::pop_up_closed_slot()
{
    cancel_running = true;
    ui->renderButton->setEnabled(true);
    qDebug() << "destroy";
}

void MainWindow::update_render_scene()
{
    _render_label->setPixmap(*_render_pixmap.get());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!is_mw_closed)
    {
        is_mw_closed = true;
        event->ignore();

        qDebug() << "MainWindow Destroy";

        cancel_running = true;
        QTimer::singleShot(50, this, [this]() {
            QTimer::singleShot(100, this, &MainWindow::close);
            QMainWindow::close(); // вызов closeEvent
        });
        return;
    }
    event->accept();
}

void MainWindow::_livetime_render()
{
    QSize new_size = ui->graphicsView->viewport()->size();

    _pixmap = std::make_shared<QPixmap>(new_size);
    _drawer = std::make_shared<Drawer>(*_pixmap);
    _color_matrix =
        std::make_shared<ColorMatrix>(new_size.height(), new_size.width());

    cancel_live_view_running = false;

    Camera camera;
    try
    {
        camera = *camera_list.get_active_camera();
    }
    catch (const std::exception &e)
    {
        show_error("Ошибка", "Нет активной камеры");
        ui->renderButton->setEnabled(true);
        return;
    }
    StartSimpleRenderCommand command{ live_render_settings, camera, _world, *_color_matrix, cancel_live_view_running };
    _facade->execute(command);

    _drawer->draw(*_color_matrix);

    set_scene();
}

void MainWindow::update_camera_list()
{
    ui->cameraListWidget->clear();
    for (size_t id : camera_list.get_camera_ids())
    {
        ui->cameraListWidget->addItem(QString::number(id));
    }
}

std::vector<size_t> MainWindow::get_selected_camera()
{
    std::vector<size_t> ids;
    for (int i = 0; i < ui->cameraListWidget->count(); i++)
    {
        if (ui->cameraListWidget->item(i)->isSelected())
        {
            ids.push_back(ui->cameraListWidget->item(i)->text().toInt());
        }
    }
    return ids;
}

std::vector<size_t> MainWindow::get_selected_object()
{
    std::vector<size_t> ids;
    for (int i = 0; i < ui->objectListWidget->count(); i++)
    {
        if (ui->objectListWidget->item(i)->isSelected())
        {
            ids.push_back(ui->objectListWidget->item(i)->text().toInt());
        }
    }
    return ids;
}

void MainWindow::show_error(const std::string label, const std::string message)
{
    QMessageBox::critical(
        this,
        QString::fromStdString(label),
        QString::fromStdString(message));
}

MainWindow::~MainWindow()
{
    delete ui;
}

// #include "main_window.hpp"

// #include <QPushButton>
// #include <QMessageBox>
// #include <QFileDialog>
// #include <QFileInfo>
// #include <cmath>

// #include "MatrixLoadCommand.h"
// #include "ListLoadCommand.h"
// #include "SqliteLoadCommandDecorator.h"
// #include "PostgresqlLoadCommandDecorator.h"
// #include "TxtLoadCommandDecorator.h"
// #include "DrawSceneQtCommand.h"
// #include "GetCameraIDsSceneCommand.h"
// #include "GetObjectIDsSceneCommand.h"
// #include "AddCameraCommand.h"
// #include "SetCameraCommand.h"
// #include "RemoveCameraCommand.h"
// #include "ShiftObjectCommand.h"
// #include "RotateObjectCommand.h"
// #include "ScaleObjectCommand.h"
// #include "CompositeObjectCommand.h"
// #include "RemoveObjectCommand.h"
// #include "Point.h"
// #include "baseexception.h"
// #include "BaseHistoryCommand.h"
// #include "ToggleFaceCullingCommand.h"

// double DegToRad(double angle) { return angle / 180.0 * M_PI; }

// MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
// {
//     ui->setupUi(this);
//     QGraphicsScene *scene = new QGraphicsScene(this);
//     ui->graphicsView->setScene(scene);
//     scene->setBackgroundBrush(Qt::white);
//     ui->graphicsView->scene()->setSceneRect(ui->graphicsView->sceneRect());
// }

// void MainWindow::on_fileSelectButton_clicked()
// {
//     QString fileName = QFileDialog::getOpenFileName(this,
//                                                     tr("Выберите файл"), "",
//                                                     tr("Все файлы (*);;Текстовые файлы (*.txt);;База данных SQLite (*.db *.sqlite)"));

//     if (!fileName.isEmpty())
//     {
//         _selected_file_path = fileName;
//         ui->fileSelectButton->setText(QFileInfo(fileName).fileName());
//     }
// }

// void MainWindow::on_loadPushButton_clicked()
// {
//     // Обработчик нажатия на кнопку "загрузка файла"
//     if (_selected_file_path.isEmpty())
//     {
//         QMessageBox::critical(nullptr, "Ошибка", "Сначала выберите файл.");
//         return;
//     }

//     std::string str = _selected_file_path.toStdString();
//     const char *fname = str.c_str();

//     std::shared_ptr<BaseLoadCommand> command;

//     // Выбираем лоадеры
//     if (ui->listRadioButton->isChecked())
//         command = std::make_shared<ListLoadCommand>();
//     else
//         command = std::make_shared<MatrixLoadCommand>();

//     // Использование паттерна декоратор
//     std::shared_ptr<BaseCommand> decorator;
//     if (ui->sqliteRadioButton->isChecked())
//         decorator = std::make_shared<SqliteLoadCommandDecorator>(*command, fname);
//     else if (ui->postgresqlRadioButton->isChecked())
//         decorator = std::make_shared<PostgresqlLoadCommandDecorator>(*command, fname);
//     else
//         decorator = std::make_shared<TxtLoadCommandDecorator>(*command, fname);
//     try
//     {
//         // Фасад просто вызывает метод .Execute у команды
//         _facade.Execute(*decorator);
//     }
//     catch (BaseException &exc)
//     {
//         QMessageBox::critical(nullptr, "Ошибка", exc.what());
//         return;
//     }

//     drawScene();
//     updateObjectList();
// }

// void MainWindow::on_CameraAddDialogPushbutton_clicked()
// {
//     double x = ui->cameraXSpin->value();
//     double y = ui->cameraYSpin->value();
//     double z = ui->cameraZSpin->value();
//     Point pos(x, y, z);
//     AddCameraCommand command(pos);
//     _facade.Execute(command);
//     updateCameraList();
//     updateObjectList();
// }

// void MainWindow::on_cameraSetPushbutton_clicked()
// {
//     auto cams = getSelectedCameraIds();
//     if (cams.size() != 1)
//     {
//         QMessageBox::critical(nullptr, "Ошибка", "Нужно выбрать ровно одну камеру.");
//         return;
//     }
//     SetCameraCommand command(cams[0]);
//     _facade.Execute(command);
//     drawScene();
// }

// void MainWindow::updateCameraList()
// {
//     ui->cameraListWidget->clear();
//     GetCameraIDsSceneCommand ids;
//     _facade.Execute(ids);
//     std::vector<size_t> cameraIds = ids.GetIDs();
//     for (size_t id : cameraIds)
//     {
//         ui->cameraListWidget->addItem(QString::number(id));
//     }
// }

// void MainWindow::updateObjectList()
// {
//     ui->objectListWidget->clear();
//     GetObjectIDsSceneCommand ids;
//     _facade.Execute(ids);
//     std::vector<size_t> objectIds = ids.GetIDs();
//     for (size_t id : objectIds)
//     {
//         ui->objectListWidget->addItem(QString::number(id));
//     }
// }

// void MainWindow::drawScene()
// {
//     ui->graphicsView->scene()->clear();
//     ui->graphicsView->scene()->setSceneRect(ui->graphicsView->sceneRect());
//     DrawSceneQtCommand drawcommand(ui->graphicsView->scene());
//     _facade.Execute(drawcommand);
// }

// void MainWindow::on_objectMovePushbutton_clicked()
// {
//     auto objs = getSelectedObjectIds();
//     if (objs.size() == 0)
//     {
//         QMessageBox::critical(nullptr, "Ошибка", "Нужно выбрать хотя бы один объект.");
//         return;
//     }

//     saveStateBeforeTransform(objs);

//     double x = ui->obkectMoveXSpin->value();
//     double y = ui->objectMoveYSpin->value();
//     double z = ui->objectMoveZSpin->value();
//     for (size_t id : objs)
//     {
//         ShiftObjectCommand command(id, x, y, z);
//         _facade.Execute(command);
//     }
//     drawScene();
// }

// void MainWindow::on_objectRotatePushbutton_clicked()
// {
//     auto objs = getSelectedObjectIds();
//     if (objs.size() == 0)
//     {
//         QMessageBox::critical(nullptr, "Ошибка", "Нужно выбрать хотя бы один объект.");
//         return;
//     }

//     saveStateBeforeTransform(objs);

//     double x = DegToRad(ui->objectRotateXSpin->value());
//     double y = DegToRad(ui->objectRotateYSpin->value());
//     double z = DegToRad(ui->objectRotateZSpin->value());
//     for (size_t id : objs)
//     {
//         RotateObjectCommand command(id, x, y, z);
//         _facade.Execute(command);
//     }
//     drawScene();
// }

// void MainWindow::on_objectScalePushbutton_clicked()
// {
//     auto objs = getSelectedObjectIds();
//     if (objs.size() == 0)
//     {
//         QMessageBox::critical(nullptr, "Ошибка", "Нужно выбрать хотя бы один объект.");
//         return;
//     }

//     saveStateBeforeTransform(objs);

//     double x = ui->objectScaleXSpin->value();
//     double y = ui->objectScaleYSpin->value();
//     double z = ui->objectScaleZSpin->value();
//     for (size_t id : objs)
//     {
//         ScaleObjectCommand command(id, x, y, z);
//         _facade.Execute(command);
//     }
//     drawScene();
// }

// void MainWindow::on_objectDeletePushbutton_clicked()
// {
//     auto objs = getSelectedObjectIds();
//     if (objs.size() == 0)
//     {
//         QMessageBox::critical(nullptr, "Ошибка", "Нужно выбрать хотя бы один объект.");
//         return;
//     }
//     for (auto &id : objs)
//     {
//         RemoveObjectCommand command(id);
//         _facade.Execute(command);

//         RemoveCameraCommand command_del_camera(id);
//         _facade.Execute(command_del_camera);
//     }

//     drawScene();
//     updateCameraList();
//     updateObjectList();
// }

// void MainWindow::on_objectCompositePushbutton_clicked()
// {
//     auto objs = getSelectedObjectIds();
//     if (objs.size() == 0)
//     {
//         QMessageBox::critical(nullptr, "Ошибка", "Нужно выбрать хотя бы один объект.");
//         return;
//     }
//     CompositeObjectCommand command(objs);
//     _facade.Execute(command);

//     drawScene();
//     updateObjectList();
// }

// void MainWindow::saveStateBeforeTransform(const std::vector<size_t> &objectIds)
// {
//     for (size_t id : objectIds)
//     {
//         SaveStateCommand saveCommand(id);
//         _facade.Execute(saveCommand);
//     }
// }

// void MainWindow::on_undoLastTransformPushbutton_clicked()
// {
//     auto objs = getSelectedObjectIds();
//     if (objs.size() == 0)
//     {
//         QMessageBox::critical(nullptr, "Ошибка", "Нужно выбрать хотя бы один объект.");
//         return;
//     }

//     for (size_t id : objs)
//     {
//         RestoreStateCommand restoreCommand(id);
//         _facade.Execute(restoreCommand);
//     }

//     drawScene();
// }

// void MainWindow::on_toggleFaceCullingPushbutton_clicked()
// {
//     ToggleFaceCullingCommand toggleCommand;
//     _facade.Execute(toggleCommand);

//     drawScene();
// }
