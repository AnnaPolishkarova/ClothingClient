#include <QtTest/QtTest>

// Подключаем все тестовые классы
class TestAll : public QObject {
    Q_OBJECT
private slots:
    void dummy() {}
};

#include "test_apiclient.cpp"
#include "test_clothingitemmodel.cpp"
#include "test_mainwindow.cpp"
#include "test_roleselectiondialog.cpp"

QTEST_MAIN(TestAll)
#include "all_tests.moc"