#include "dataentry.h"

#include <QJsonArray>
#include <QDebug>
#include <QBuffer>

DataEntry::DataEntry(QString path, QString name, int numPoints)
    : DataEntry(QPixmap(path), name, numPoints)
{
    imagePath = path;
}

DataEntry::DataEntry(QPixmap pixmap, QString name, int numPoints)
    : image(pixmap)
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
    , imagePath(entry.imagePath)
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

void DataEntry::toJsonObject(QJsonObject &json)
{
    json["name"] = name;
    // json["imagePath"] = imagePath;
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "JPG");
    QString pixmapString(buffer.data().toBase64());
    json["pixmap"] = pixmapString;

    qDebug() << "pixmap string size is" << pixmapString.size();

    QJsonArray pointArray;

    for (auto p : points)
    {
        QJsonObject pointObject;

        if (p.has_value())
        {
            pointObject["x"] = p->x();
            pointObject["y"] = p->y();
        }
        else
        {
            pointObject["x"] = -1.0;
            pointObject["y"] = -1.0;
        }

        pointArray.append(pointObject);
    }

    json["points"] = pointArray;
}

void DataEntry::nextPoint()
{
    selectedPoint = (selectedPoint + 1) % numPoints;
}

void DataEntry::previousPoint()
{
    selectedPoint = (selectedPoint - 1) % numPoints;
}

DataEntry::DataEntry(QJsonObject object)
{
    name = object["name"].toString();
    qDebug() << "name is" << name;
    QByteArray pixmapData = QByteArray::fromBase64(object["pixmap"].toString().toUtf8());
    image.load(pixmapData);

    pointModel = new QStringListModel;

    QJsonArray pointArray = object["points"].toArray();

    points.resize(pointArray.size());
    numPoints = pointArray.size();

    for (int i = 0; i < pointArray.size(); i++)
    {
        qDebug() << "Adding source" << i;
        QJsonObject o = pointArray[i].toObject();

        if (o["x"].toDouble() > 0 && o["y"].toDouble() > 0)
        {
            QPointF p(o["x"].toDouble(), o["y"].toDouble());
            points[i] = std::optional(p);
        }
        else
            points[i] = std::optional<QPointF>();
    }

    updatePointList();
}

void DataEntry::clickedAt(QPointF normalized)
{
    points[selectedPoint] = normalized;

    pointModel->setData(pointModel->index(selectedPoint, 0), QString::number(selectedPoint + 1) + " ???", Qt::DisplayRole);
}

void DataEntry::updatePointList()
{
    QStringList pts;

    for (int i = 1; i <= numPoints; i++)
    {
        QString str = QString::number(i);

        if (points.size() > i && points[i].has_value())
            str += " ???"; // U+2714

        pts.append(str);
    }

    pointModel->setStringList(pts);
}
