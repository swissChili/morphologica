#include "dataentrylistmodel.h"

DataEntryListModel::DataEntryListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

DataEntryListModel::~DataEntryListModel()
{
    for (auto *e : entries)
    {
        delete e;
    }
}

QVariant DataEntryListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return "Source";
}

int DataEntryListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return entries.size();
}

QVariant DataEntryListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole)
        return entries[index.row()]->getName();

    return QVariant();
}
