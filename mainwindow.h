#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QVBoxLayout>
#include <QPen>
#include <QBrush>
#include <QStringListModel>

#include "dataentry.h"
#include "dataentrylistmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    GraphicsView *graphicsView;
    QVBoxLayout *thisIsStupid;

    void displayEntry(DataEntry &entry);

    int numPoints = 5,
        defaultTimeout = 1500;
    DataEntry *currentEntry = nullptr;

    QColor defaultColor = QColor(36, 109, 244, 255),
        focusedColor = QColor(244, 36, 74, 255);
    QPen defaultPen = QPen(defaultColor);
    QBrush defaultBrush = QBrush(defaultColor);

    DataEntryListModel *listModel;

    QString saveProjectPath = "";

    qreal currentScale = 1.0;

    void exportJsonTo(QString fileName);
    void loadFromJson(QString fileName);

    void zoomChanged(int decimalPos);

    void updatePointSelectionModel();

private slots:
    void viewClicked(QPointF denormalized);
    void pointSelectionChanged(const QModelIndex &current, const QModelIndex &previous);
    void sourceSelectionChanged(const QModelIndex &current, const QModelIndex &previous);
    void addSource();

    void on_addSourceButton_clicked();
    void on_pointSpinBox_valueChanged(int arg1);
    void on_actionExport_triggered();
    void on_removeSourceButton_clicked();
    void on_actionSave_project_triggered();
    void on_actionSave_project_as_triggered();
    void on_actionOpen_project_triggered();
    void on_zoomSlider_valueChanged(int value);
    void on_actionZoom_in_triggered();
    void on_actionZoom_out_triggered();
    void on_actionNext_point_triggered();
    void on_actionPrevious_point_triggered();
};
#endif // MAINWINDOW_H
