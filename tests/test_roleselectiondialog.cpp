#include <QtTest>
#include "../include/RoleSelectionDialog.h"
#include <QAbstractButton>

class TestRoleSelectionDialog : public QObject {
    Q_OBJECT

private slots:
    void testUserSelected_IsAdminFalse();
    void testAdminSelected_IsAdminTrue();
};

// Проверка, что при выборе роли "Пользователь" isAdminSelected возвращает false
void TestRoleSelectionDialog::testUserSelected_IsAdminFalse()
{
    RoleSelectionDialog dialog;
    dialog.roleGroup->button(0)->setChecked(true); // user
    QCOMPARE(dialog.exec(), QDialog::Accepted);
    QVERIFY(!dialog.isAdminSelected());
}

// Проверка, что при выборе роли "Администратор" isAdminSelected возвращает true
void TestRoleSelectionDialog::testAdminSelected_IsAdminTrue()
{
    RoleSelectionDialog dialog;
    dialog.roleGroup->button(1)->setChecked(true); // admin
    QCOMPARE(dialog.exec(), QDialog::Accepted);
    QVERIFY(dialog.isAdminSelected());
}

// QTEST_MAIN(TestRoleSelectionDialog)
// #include "test_roleselectiondialog.moc"