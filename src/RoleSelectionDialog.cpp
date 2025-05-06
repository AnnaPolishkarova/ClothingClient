#include "../include/RoleSelectionDialog.h"
#include <QVBoxLayout>
#include <QRadioButton>
#include <QPushButton>

RoleSelectionDialog::RoleSelectionDialog(QWidget *parent)
    : QDialog(parent), roleGroup(new QButtonGroup(this))
{
    setWindowTitle(tr("Select Role"));
    setFixedSize(150, 150);

    QRadioButton *userBtn = new QRadioButton(tr("User"), this);
    QRadioButton *adminBtn = new QRadioButton(tr("Administrator"), this);

    roleGroup->addButton(userBtn, 0);
    roleGroup->addButton(adminBtn, 1);
    userBtn->setChecked(true);

    QPushButton *okBtn = new QPushButton(tr("OK"), this);
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(userBtn);
    layout->addWidget(adminBtn);
    layout->addWidget(okBtn);
}

bool RoleSelectionDialog::isAdminSelected() const
{
    return roleGroup->checkedId() == 1;
}
