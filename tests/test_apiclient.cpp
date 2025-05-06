#include <QtTest>
#include "../include/ApiClient.h"
#include <QSignalSpy>

class TestApiClient : public QObject {
    Q_OBJECT

private slots:
    void testFindMatches_EmitsSignal();
    void testAddItem_SendsRequestAndEmitsSignal();
};

// Проверка испускания сигнала matchesFound
void TestApiClient::testFindMatches_EmitsSignal()
{
    ApiClient client;
    QSignalSpy spy(&client, &ApiClient::matchesFound);

    client.findMatches(75.0, 90.0, 30.0); // Вызов метода

    QVERIFY(spy.wait(5000)); // Ждем сигнал
    QCOMPARE(spy.count(), 1); // Проверяем, что сигнал был испущен
}

// Проверка испускания сигнала operationCompleted с корректными параметрами
void TestApiClient::testAddItem_SendsRequestAndEmitsSignal()
{
    ApiClient client;
    QSignalSpy spyOperation(&client, &ApiClient::operationCompleted);

    ClothingItem item{75.0, 90.0, 30.0, "Jeans", };
    client.addItem(item);

    QVERIFY(spyOperation.wait(5000));
    auto args = spyOperation.takeFirst();
    QVERIFY(args.at(0).toBool());
}

// QTEST_MAIN(TestApiClient)
// #include "test_apiclient.moc"