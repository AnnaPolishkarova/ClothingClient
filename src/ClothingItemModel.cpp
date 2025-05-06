#include "../include/ClothingItemModel.h"

ClothingItemModel::ClothingItemModel(QObject *parent)
    : QAbstractTableModel(parent) {}

int ClothingItemModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_items.size();
}

int ClothingItemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

QVariant ClothingItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    const ClothingItem &item = m_items[index.row()];
    switch (index.column()) {
    case 0: return item.name;
    case 1: return item.waist;
    case 2: return item.hip;
    case 3: return item.length;
    default: return QVariant();
    }
}

QVariant ClothingItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0: return "Name";
        case 1: return "Waist";
        case 2: return "Hip";
        case 3: return "Length";
        default: return QVariant();
        }
    }
    return QVariant();
}

void ClothingItemModel::addItem(const ClothingItem &item)
{
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items.append(item);
    endInsertRows();
}

void ClothingItemModel::updateItem(int row, const ClothingItem &item)
{
    if (row < 0 || row >= m_items.size())
        return;

    m_items[row] = item;
    emit dataChanged(index(row, 0), index(row, columnCount() - 1));
}

void ClothingItemModel::removeItem(int row)
{
    if (row < 0 || row >= m_items.size())
        return;

    beginRemoveRows(QModelIndex(), row, row);
    m_items.removeAt(row);
    endRemoveRows();
}

ClothingItem ClothingItemModel::getItem(int row) const
{
    if (row >= 0 && row < m_items.size())
        return m_items[row];
    return ClothingItem();
}

void ClothingItemModel::removeAllItems()
{
    beginResetModel();
    m_items.clear();
    endResetModel();
}
