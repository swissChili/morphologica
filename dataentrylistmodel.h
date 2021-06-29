#ifndef DATAENTRYLISTMODEL_H
#define DATAENTRYLISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QDebug>

#include "dataentry.h"

class DataEntryListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit DataEntryListModel(QObject *parent = nullptr);
    ~DataEntryListModel();

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    DataEntry *entryAt(int index) const
    {
        return entries[index];
    }

    void addEntry(DataEntry &&entry)
    {
        int i = entries.size();
        qDebug() << "Inserting at" << i;

        beginInsertRows(QModelIndex(), i, i);
        entries.append(new DataEntry(std::move(entry)));
        endInsertRows();
    }

    QList<DataEntry *> getEntries() const
    {
        return entries;
    }

    void clear();
    void removeAt(int index);

private:
    QList<DataEntry *> entries;
};

#endif // DATAENTRYLISTMODEL_H
