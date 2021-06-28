#include "dataentry.h"

DataEntry::DataEntry(QString path, QString name, int numPoints)
    : image(path)
    , name(name)
    , points(numPoints)
    , numPoints(numPoints)
    , pointModel(new QStringListModel)
{
    updatePointList();
}

DataEntry::DataEntry(const DataEntry &&entry)
    : image(entry.image)
    , name(entry.name)
    , points(entry.numPoints)
    , numPoints(entry.numPoints)
    , pointModel(new QStringListModel)
{
    updatePointList();
}

DataEntry::~DataEntry()
{
    delete pointModel;
}

QString DataEntry::formatToString()
{
    QString string = name;

    for (std::optional<QPointF> pointOpt : points)
    {
        if (pointOpt.has_value())
        {
            QPointF point = *pointOpt;
            string += "\t" + QString::number(point.x()) + "\t" + QString::number(point.y());
        }
    }

    return string + "\n";
}

void DataEntry::clickedAt(QPointF normalized)
{
    points[selectedPoint] = normalized;
}

void DataEntry::updatePointList()
{
    QStringList pts;

    for (int i = 1; i <= numPoints; i++)
    {
        pts.append(QString::number(i));
    }

    pointModel->setStringList(pts);
}
