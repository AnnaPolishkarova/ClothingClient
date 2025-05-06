#pragma once

#include <QDialog>
#include <QButtonGroup>

class RoleSelectionDialog : public QDialog
{
    Q_OBJECT
    friend class TestRoleSelectionDialog;
public:
    explicit RoleSelectionDialog(QWidget *parent = nullptr);
    bool isAdminSelected() const;

private:
    QButtonGroup *roleGroup;
};