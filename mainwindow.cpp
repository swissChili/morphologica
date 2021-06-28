#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>
#include <QInputDialog>
#include <QFileDialog>
#include "graphicsview.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , scene(new QGraphicsScene)
    , graphicsView(new GraphicsView)
    , thisIsStupid(new QVBoxLayout)
    , listModel(new DataEntryListModel)
{
    ui->setupUi(this);

    ui->centralwidget->setLayout(ui->centralLayout);

    connect(graphicsView, &GraphicsView::clickedAt, this, &MainWindow::viewClicked);
    graphicsView->setScene(scene);
    thisIsStupid->addWidget(graphicsView);
    ui->graphicsView->setLayout(thisIsStupid);

    ui->sourcesListView->setModel(listModel);

    connect(listModel, &QAbstractListModel::rowsInserted, [](const auto &parent, int first, int last)
    {
        qDebug() << "Inserted!" << first << last;
    });
}

MainWindow::~MainWindow()
{
    delete ui;
    delete scene;
    delete graphicsView;
    delete thisIsStupid;
}

void MainWindow::displayEntry(DataEntry &entry)
{
    scene->clear();
    currentEntry = &entry;

    ui->pointListView->setModel(entry.getPointModel());
    qDebug() << "Model" << entry.getPointModel()->rowCount();

    scene->addPixmap(entry.getImage());
    ui->entryName->setText(entry.getName());

    for (int i = 0; i < entry.getPoints().size(); i++)
    {
        auto pointOpt = entry.getPoints()[i];
        if (pointOpt.has_value())
        {
            QPointF point = *pointOpt;

            QPen pen = defaultPen;
            QBrush brush = defaultBrush;

            if (i == entry.getSelectedPoint())
            {
                pen = QPen(QColor(0, 255, 0, 255));
                brush = QBrush(QColor(0, 255, 0, 255));
            }
            QPoint denormalized((double)entry.getImage().width() * point.x(),
                                (double)entry.getImage().height() * point.y());

            scene->addEllipse(denormalized.x(), denormalized.y(), 10, 10,
                              pen, brush);
        }
    }
}

void MainWindow::viewClicked(QPointF denormalized)
{
    // TODO: what?
    QPointF size(denormalized.x() / (double)currentEntry->getImage().width(),
                 denormalized.y() / (double)currentEntry->getImage().height());

    qDebug() << size;

    currentEntry->clickedAt(size);
    displayEntry(*currentEntry);
}

void MainWindow::on_pointListView_activated(const QModelIndex &index)
{
    displayEntry(*currentEntry);
}

void MainWindow::on_pointListView_clicked(const QModelIndex &index)
{
    qDebug() << "Activated" << index;
    currentEntry->setSelectedPoint(index.row());

    displayEntry(*currentEntry);
}

void MainWindow::on_addSourceButton_clicked()
{
    QString name = QInputDialog::getText(this, "Add data source",
                                         "Data source name", QLineEdit::Normal);
    QString fileName = QFileDialog::getOpenFileName(this, "Add data source",
                                                    QDir::homePath(),
                                                    "Image Files (*.png *.jpg *.jpeg *.bmp)");

    DataEntry entry(fileName, name, numPoints);
    listModel->addEntry(std::move(entry));

    qDebug() << "Added entry to list model";
}

void MainWindow::on_sourcesListView_clicked(const QModelIndex &index)
{
    qDebug() << "clcked source";
    displayEntry(*listModel->entryAt(index.row()));
}

void MainWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export as TSV",
                                                    QDir::homePath(), "TSV files (*.tsv);; Text files (*.txt)");

    QString ex;

    for (auto *e : listModel->getEntries())
    {
        ex += e->formatToString();
    }

    QFile f(fileName);
    f.open(QIODevice::WriteOnly | QIODevice::Text);
    f.write(ex.toUtf8());
    f.close();
}
