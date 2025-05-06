#pragma once

#include <QAbstractTableModel>
#include <QList>

struct ClothingItem {
    double waist;
    double hip;
    double length;
    QString name;
};

class ClothingItemModel : public QAbstractTableModel
{
    Q_OBJECT
    friend class TestClothingItemModel;

public:
    explicit ClothingItemModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void addItem(const ClothingItem &item);
    void updateItem(int row, const ClothingItem &item);
    void removeItem(int row);
    ClothingItem getItem(int row) const;
    void removeAllItems();

private:
    QList<ClothingItem> m_items;
};
