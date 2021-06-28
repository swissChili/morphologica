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

    int numPoints = 5;
    DataEntry *currentEntry = nullptr;

    QColor defaultColor = QColor(0, 0, 255, 255);
    QPen defaultPen = QPen(defaultColor);
    QBrush defaultBrush = QBrush(defaultColor);

    DataEntryListModel *listModel;

private slots:
    void viewClicked(QPointF denormalized);
    void on_pointListView_activated(const QModelIndex &index);
    void on_pointListView_clicked(const QModelIndex &index);
    void on_addSourceButton_clicked();
    void on_sourcesListView_clicked(const QModelIndex &index);
    void on_pushButton_clicked();
};
#endif // MAINWINDOW_H
