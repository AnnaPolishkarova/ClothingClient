#include <QtTest>
#include "../include/ClothingItemModel.h"
#include <QSignalSpy>

class TestClothingItemModel : public QObject {
    Q_OBJECT

private slots:
    void testAddItem_IncreasesRowCount();
    void testRemoveItem_DecreasesRowCount();
    void testData_ReturnsCorrectValues();
};

// Проверка увеличения количества строк после добавления элемента
void TestClothingItemModel::testAddItem_IncreasesRowCount()
{
    ClothingItemModel model;
    int initialCount = model.rowCount();

    ClothingItem item{80.0, 95.0, 32.0, "Shirt"};
    model.addItem(item);

    QCOMPARE(model.rowCount(), initialCount + 1);
}

// Проверка уменьшения количества строк после удаления элемента
void TestClothingItemModel::testRemoveItem_DecreasesRowCount()
{
    ClothingItemModel model;
    ClothingItem item{ 75.0, 90.0, 30.0, "Pants",};
    model.addItem(item);

    QSignalSpy rowsRemovedSpy(&model, &ClothingItemModel::rowsRemoved);
    model.removeItem(0);

    QCOMPARE(model.rowCount(), 0);
    QVERIFY(rowsRemovedSpy.count() > 0);
}

// Проверка, что данные возвращаются корректно
void TestClothingItemModel::testData_ReturnsCorrectValues()
{
    ClothingItemModel model;
    ClothingItem item{85.0, 95.0, 34.0, "Coat", };
    model.addItem(item);

    QModelIndex index = model.index(0, 0);
    QVERIFY(index.isValid());
    QCOMPARE(model.data(index, Qt::DisplayRole).toString(), QString("Coat"));

    index = model.index(0, 1);
    QCOMPARE(model.data(index, Qt::DisplayRole).toDouble(), 85.0);
}

// QTEST_MAIN(TestClothingItemModel)
// #include "test_clothingitemmodel.moc"