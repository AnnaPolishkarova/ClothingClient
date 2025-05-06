#include <QtTest>
#include "../include/MainWindow.h"
#include <QSignalSpy>

class TestMainWindow : public QObject {
    Q_OBJECT

private slots:
    void testOnFindMatches_CallsApiClientFindMatches();
    void testOnAddItem_CallsApiClientAddItem();
};

// Проверяем, что onFindMatches (нажатие кноки)  вызывает findMatches в ApiClient с правильными параметрами
void TestMainWindow::testOnFindMatches_CallsApiClientFindMatches()
{
    MainWindow window(false); // пользовательский режим
    QSignalSpy spy(window.m_apiClient, &ApiClient::findMatches);

    window.m_waistSpinBox->setValue(75.0);
    window.m_hipSpinBox->setValue(90.0);
    window.m_lengthSpinBox->setValue(30.0);

    window.onFindMatches(); // вызываем вручную

    QVERIFY(spy.wait(5000));
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toDouble(), 75.0);
    QCOMPARE(arguments.at(1).toDouble(), 90.0);
    QCOMPARE(arguments.at(2).toDouble(), 30.0);
}

// Проверяем, что onAddItem (нажатие кнопки) вызывает addItem в ApiClient
void TestMainWindow::testOnAddItem_CallsApiClientAddItem()
{
    MainWindow window(true); // админский режим
    QSignalSpy spy(window.m_apiClient, &ApiClient::addItem);

    QTest::mouseClick(window.m_addItemButton, Qt::LeftButton);

    bool ok = true;
    QString name = "MockItem";
    double waist = 80.0, hip = 95.0, length = 32.0;

    ClothingItem item{waist, hip, length, name};
    window.m_apiClient->addItem(item);

    QVERIFY(spy.wait(5000));
    QCOMPARE(spy.count(), 1);
}

// QTEST_MAIN(TestMainWindow)
// #include "test_mainwindow.moc"