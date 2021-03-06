#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>
#include <QInputDialog>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMimeData>
#include <algorithm>
#include <QPixmap>
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

    setCentralWidget(ui->centralSplitter);

    connect(graphicsView, &GraphicsView::clickedAt, this, &MainWindow::viewClicked);
    graphicsView->setScene(scene);
    thisIsStupid->addWidget(graphicsView);
    ui->graphicsView->setLayout(thisIsStupid);

    ui->graphicsView->setContentsMargins(0, 0, 0, 0);
    thisIsStupid->setContentsMargins(0, 0, 0, 0);

    ui->centralSplitter->setContentsMargins(8, 8, 8, 8);

    ui->sourcesListView->setModel(listModel);

    connect(listModel, &QAbstractListModel::rowsInserted, [](const auto &, int first, int last)
    {
        qDebug() << "Inserted!" << first << last;
    });

    ui->pointSpinBox->setValue(numPoints);

    connect(ui->sourcesListView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &MainWindow::sourceSelectionChanged);

    ui->zoomSlider->setValue(50);

    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete scene;
    delete graphicsView;
    delete thisIsStupid;
    delete listModel;
}

void MainWindow::displayEntry(DataEntry &entry)
{
    scene->clear();
    currentEntry = &entry;

    if (ui->pointListView->selectionModel())
        disconnect(ui->pointListView->selectionModel(), &QItemSelectionModel::currentChanged,
                   this, &MainWindow::pointSelectionChanged);
    ui->pointListView->setModel(entry.getPointModel());
    connect(ui->pointListView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &MainWindow::pointSelectionChanged);
    qDebug() << "Model" << entry.getPointModel()->rowCount();

    scene->addPixmap(entry.getImage());

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
                pen = QPen(focusedColor);
                brush = QBrush(focusedColor);
            }
            QPoint denormalized((double)entry.getImage().width() * point.x() - 5,
                                (double)entry.getImage().height() * point.y() - 5);

            scene->addEllipse(denormalized.x(), denormalized.y(), 10, 10,
                              pen, brush);
        }
    }
}

void MainWindow::exportJsonTo(QString fileName)
{
    QJsonObject json;
    json["numPoints"] = numPoints;

    QJsonArray sources;

    for (auto *entry : listModel->getEntries())
    {
        QJsonObject entryObject;
        entry->toJsonObject(entryObject);
        sources.append(entryObject);
    }

    json["sources"] = sources;

    QJsonDocument doc{json};

    QFile of{fileName};
    of.open(QIODevice::WriteOnly | QIODevice::Text);

    of.write(doc.toJson());

    of.close();
}

void MainWindow::loadFromJson(QString fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray text = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(text);

    numPoints = doc["numPoints"].toInt(5);

    qDebug() << "numPoints" << numPoints;

    qDebug() << doc["sources"];

    QJsonArray sources = doc["sources"].toArray();

    listModel->clear();

    for (auto s : sources)
    {
        qDebug() << "Loaded entry";
        DataEntry entry(s.toObject());
        qDebug() << "--- adding entry named" << entry.getName();
        listModel->addEntry(std::move(entry));
    }
}

void MainWindow::zoomChanged(int decimalPos)
{
    qreal scale = (qreal)decimalPos / 99.0 + 0.2;
    qreal factor = scale / currentScale;

    qDebug() << "Scaling to" << decimalPos << scale;

    graphicsView->scale(factor, factor);
    currentScale = scale;
}

void MainWindow::updatePointSelectionModel()
{
    ui->pointListView->selectionModel()->select(
                currentEntry->getPointModel()->index(currentEntry->getSelectedPoint(), 0),
                QItemSelectionModel::ClearAndSelect);

    displayEntry(*currentEntry);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasImage())
    {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QPixmap pixmap(event->mimeData()->imageData().toByteArray());

    bool ok;

    QString name = QInputDialog::getText(this, "Add data source",
                                         "Data source name", QLineEdit::Normal,
                                         "", &ok);

    if (!ok)
        return;

    DataEntry entry(pixmap, name, numPoints);
    listModel->addEntry(std::move(entry));

    statusBar()->showMessage("Added source " + name, defaultTimeout);
}

void MainWindow::viewClicked(QPointF denormalized)
{
    if (!currentEntry)
        return;

    // TODO: what?
    QPointF size(denormalized.x() / (double)currentEntry->getImage().width(),
                 denormalized.y() / (double)currentEntry->getImage().height());

    qDebug() << size;

    currentEntry->clickedAt(size);
    displayEntry(*currentEntry);
}

void MainWindow::pointSelectionChanged(const QModelIndex &current, const QModelIndex &)
{
    qDebug() << "Point selection changed" << current;
    currentEntry->setSelectedPoint(current.row());

    displayEntry(*currentEntry);
}

void MainWindow::sourceSelectionChanged(const QModelIndex &current, const QModelIndex &)
{
    qDebug() << "Source selection changed";
    displayEntry(*listModel->entryAt(current.row()));

    // UNREADABLE
    // Selects the current point
    QModelIndex index = ui->pointListView->model()
            ->index(listModel->entryAt(current.row())->getSelectedPoint(), 0);
    ui->pointListView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
}

void MainWindow::addSource(QString fileName = "")
{
    bool ok;

    QString name = QInputDialog::getText(this, "Add data source",
                                         "Data source name", QLineEdit::Normal,
                                         "", &ok);
    if (!ok)
        return;

    if (fileName == "")
        fileName = QFileDialog::getOpenFileName(this, "Add data source",
                                                QDir::homePath(),
                                                "Image Files (*.png *.jpg *.jpeg *.bmp)");

    if (fileName == "")
        return;

    DataEntry entry(fileName, name, numPoints);
    listModel->addEntry(std::move(entry));

    qDebug() << "Added entry to list model";

    statusBar()->showMessage("Added source " + name, defaultTimeout);
}

void MainWindow::on_addSourceButton_clicked()
{
    addSource();
}

void MainWindow::on_pointSpinBox_valueChanged(int p)
{
    numPoints = p;
    for (auto e : listModel->getEntries())
    {
        e->setNumPoints(p);
    }

    if (currentEntry)
        displayEntry(*currentEntry);
}

void MainWindow::on_actionExport_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export data",
                                                    QDir::homePath(),
                                                    "TSV files (*.tsv);;Text files (*.txt)");

    QString ex;

    for (auto *e : listModel->getEntries())
    {
        ex += e->formatToString();
    }

    QFile f(fileName);
    f.open(QIODevice::WriteOnly | QIODevice::Text);
    f.write(ex.toUtf8());
    f.close();

    statusBar()->showMessage("Exported " + fileName, defaultTimeout);
}

void MainWindow::on_removeSourceButton_clicked()
{
    QModelIndex selected = ui->sourcesListView->selectionModel()->selectedRows()[0];

    listModel->removeAt(selected.row());

    // Select the previous item
    ui->sourcesListView->selectionModel()->select(
                listModel->index(selected.row() - 1, 0),
                QItemSelectionModel::ClearAndSelect);
}

void MainWindow::on_actionSave_project_triggered()
{
    if (saveProjectPath == "")
        saveProjectPath = QFileDialog::getSaveFileName(this, "Save project",
                                                       QDir::homePath(),
                                                       "Morphologica project (*.morph)");

    if (saveProjectPath == "")
        return; // hit cancel

    exportJsonTo(saveProjectPath);
}

void MainWindow::on_actionSave_project_as_triggered()
{
    saveProjectPath = QFileDialog::getSaveFileName(this, "Save project as",
                                                   QDir::homePath(),
                                                   "Morphologica project (*.morph)");

    if (saveProjectPath == "")
        return; // hit cancel

    exportJsonTo(saveProjectPath);
}

void MainWindow::on_actionOpen_project_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open project",
                                                    QDir::homePath(),
                                                    "Morphologica project (*.morph)");

    saveProjectPath = fileName;

    loadFromJson(fileName);
}

void MainWindow::on_zoomSlider_valueChanged(int value)
{
    qDebug() << "zoomSlider_valueChanged" << value;
    zoomChanged(value);
}

void MainWindow::on_actionZoom_in_triggered()
{
    ui->zoomSlider->setValue(std::min(ui->zoomSlider->value() + 5, 99));
}

void MainWindow::on_actionZoom_out_triggered()
{
    ui->zoomSlider->setValue(std::max(ui->zoomSlider->value() - 5, 0));
}

void MainWindow::on_actionNext_point_triggered()
{
    if (currentEntry)
    {
        currentEntry->nextPoint();
        updatePointSelectionModel();
    }
}

void MainWindow::on_actionPrevious_point_triggered()
{
    if (currentEntry)
    {
        currentEntry->previousPoint();
        updatePointSelectionModel();
    }
}
