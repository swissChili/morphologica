#ifndef DATAENTRY_H
#define DATAENTRY_H

#include <QPixmap>
#include <QStringListModel>
#include <QJsonObject>
#include <optional>

#include "graphicsview.h"

class DataEntry : QObject
{
    Q_OBJECT

public:
    DataEntry(QString path, QString name, int numPoints);
    DataEntry(const DataEntry &&entry);
    DataEntry(QJsonObject object);
    ~DataEntry();

    QString getName() const
    {
        return name;
    }

    const QPixmap &getImage() const
    {
        return image;
    }

    const QVector<std::optional<QPointF>> &getPoints() const
    {
        return points;
    }

    void setNumPoints(int n)
    {
        numPoints = n;
        points.resize(n);
        updatePointList();
    }

    int getNumPoints() const
    {
        return numPoints;
    }

    int getSelectedPoint() const
    {
        return selectedPoint;
    }

    void setSelectedPoint(int p)
    {
        selectedPoint = p;
    }

    QStringListModel *getPointModel() const
    {
        return pointModel;
    }

    QString formatToString();

    void toJsonObject(QJsonObject &json);

public slots:
    void clickedAt(QPointF normalized);

private:
    QPixmap image;
    QString name;
    QString imagePath;
    QVector<std::optional<QPointF>> points;

    int numPoints;
    int selectedPoint = 0;

    QStringListModel *pointModel;

    void updatePointList();
};

#endif // DATAENTRY_H
