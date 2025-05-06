#include "../include/MainWindow.h"
#include "../include/RoleSelectionDialog.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>


MainWindow::MainWindow(bool isAdminMode, QWidget *parent)
    : QWidget(parent), m_isAdminMode(isAdminMode)

{
    m_networkThread = new QThread(this);
    // m_apiClient = std::make_unique<ApiClient>();
    m_apiClient = new ApiClient();
    m_itemModel = new ClothingItemModel(this);


    // Инициализация интерфейсов
    m_stackedWidget = new QStackedWidget(this);

    if (m_isAdminMode)
    {
        setupAdminInterface();
    } else
    {
        setupUserInterface();
    }


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_stackedWidget);

    // настройка сети
    m_apiClient->moveToThread(m_networkThread);
    m_networkThread->start();

    // Общие соединения сигналов и слотов
    connect(m_apiClient, &ApiClient::matchesFound, this, &MainWindow::handleMatchesFound);
    connect(m_apiClient, &ApiClient::operationCompleted, this, &MainWindow::handleOperationCompleted);
    connect(m_apiClient, &ApiClient::errorOccurred, this, &MainWindow::handleError);
    connect(m_apiClient, &ApiClient::itemsLoaded, this, &MainWindow::handleItemsLoaded);
}



void MainWindow::setupUserInterface()
{
    m_userWidget = new QWidget();

    // Элементы для пользователя
    m_waistSpinBox = new QDoubleSpinBox(m_userWidget);
    m_hipSpinBox = new QDoubleSpinBox(m_userWidget);
    m_lengthSpinBox = new QDoubleSpinBox(m_userWidget);
    m_findMatchesButton = new QPushButton(tr("Find Matches"), m_userWidget);
    m_resultsTextEdit = new QTextEdit(m_userWidget);
    m_resultsTextEdit->setReadOnly(true);

    // Layout для пользовательского интерфейса
    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow(tr("Waist:"), m_waistSpinBox);
    formLayout->addRow(tr("Hip:"), m_hipSpinBox);
    formLayout->addRow(tr("Length:"), m_lengthSpinBox);

    QVBoxLayout *layout = new QVBoxLayout(m_userWidget);
    layout->addLayout(formLayout);
    layout->addWidget(m_findMatchesButton);
    layout->addWidget(new QLabel(tr("Results:"), m_userWidget));
    layout->addWidget(m_resultsTextEdit);

    connect(m_findMatchesButton, &QPushButton::clicked, this, &MainWindow::onFindMatches);

    m_stackedWidget->addWidget(m_userWidget);
}

void MainWindow::setupAdminInterface()
{
    m_adminWidget = new QWidget();

    // Общие элементы
    m_waistSpinBox = new QDoubleSpinBox(m_adminWidget);
    m_hipSpinBox = new QDoubleSpinBox(m_adminWidget);
    m_lengthSpinBox = new QDoubleSpinBox(m_adminWidget);
    m_findMatchesButton = new QPushButton(tr("Find Matches"), m_adminWidget);
    m_resultsTextEdit = new QTextEdit(m_adminWidget);
    m_resultsTextEdit->setReadOnly(true);

    // Элементы только для админа
    m_addItemButton = new QPushButton(tr("Add Item"), m_adminWidget);
    m_editItemButton = new QPushButton(tr("Edit Item"), m_adminWidget);
    m_deleteItemButton = new QPushButton(tr("Delete Item"), m_adminWidget);
    m_itemsTableView = new QTableView(m_adminWidget);
    m_itemsTableView->setModel(m_itemModel);
    m_itemsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_itemsTableView->setSelectionMode(QAbstractItemView::SingleSelection);

    // Layout для админского интерфейса
    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow(tr("Waist:"), m_waistSpinBox);
    formLayout->addRow(tr("Hip:"), m_hipSpinBox);
    formLayout->addRow(tr("Length:"), m_lengthSpinBox);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_findMatchesButton);
    buttonLayout->addWidget(m_addItemButton);
    buttonLayout->addWidget(m_editItemButton);
    buttonLayout->addWidget(m_deleteItemButton);

    QVBoxLayout *layout = new QVBoxLayout(m_adminWidget);
    layout->addLayout(formLayout);
    layout->addLayout(buttonLayout);
    layout->addWidget(m_itemsTableView);
    layout->addWidget(new QLabel(tr("Results:"), m_adminWidget));
    layout->addWidget(m_resultsTextEdit);

    QPushButton* m_refreshButton = new QPushButton(tr("Refresh"), m_adminWidget);
    buttonLayout->addWidget(m_refreshButton);

    // Подключение сигналов для админских функций
    connect(m_findMatchesButton, &QPushButton::clicked, this, &MainWindow::onFindMatches);
    connect(m_addItemButton, &QPushButton::clicked, this, &MainWindow::onAddItem);
    connect(m_editItemButton, &QPushButton::clicked, this, &MainWindow::onEditItem);
    connect(m_deleteItemButton, &QPushButton::clicked, this, &MainWindow::onDeleteItem);
    connect(m_refreshButton, &QPushButton::clicked, [this]() {
    QMetaObject::invokeMethod(m_apiClient, "loadItems");
});

    m_stackedWidget->addWidget(m_adminWidget);

    if (m_isAdminMode) {
        QMetaObject::invokeMethod(m_apiClient, "loadItems");
    }
}


MainWindow::~MainWindow()
{
    // Отменить все pending-запросы
    QMetaObject::invokeMethod(m_apiClient, [this]()
    {
        for (auto reply : m_apiClient->findChildren<QNetworkReply*>())
        {
            reply->abort();
        }
    }, Qt::BlockingQueuedConnection);

    // Остановить поток
    m_networkThread->quit();

    // Удалить ApiClient в потоке
    QMetaObject::invokeMethod(m_apiClient, &QObject::deleteLater, Qt::QueuedConnection);

    // неблокирующее ожидание завершения
    QEventLoop loop;
    connect(m_networkThread, &QThread::finished, &loop, &QEventLoop::quit);
    loop.exec(QEventLoop::ExcludeUserInputEvents);

    // удалить сам поток (уже остановленный)
    delete m_networkThread;

    qDebug() << "MainWindow destroyed, thread safe:" << QThread::currentThread();
}


void MainWindow::onFindMatches()
{
    double waist = m_waistSpinBox->value();
    double hip = m_hipSpinBox->value();
    double length = m_lengthSpinBox->value();

    // QMetaObject::invokeMethod(m_apiClient.get(), "findMatches",
    //     Q_ARG(double, waist),
    //     Q_ARG(double, hip),
    //     Q_ARG(double, length));
    QMetaObject::invokeMethod(m_apiClient,
        [this, waist, hip, length] { m_apiClient->findMatches(waist, hip, length); },
        Qt::QueuedConnection);
}

void MainWindow::onAddItem()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Add Item"), tr("Name:"), QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    double waist = QInputDialog::getDouble(this, tr("Add Item"), tr("Waist:"), 0, 0, 1000, 1, &ok);
    if (!ok) return;

    double hip = QInputDialog::getDouble(this, tr("Add Item"), tr("Hip:"), 0, 0, 1000, 1, &ok);
    if (!ok) return;

    double length = QInputDialog::getDouble(this, tr("Add Item"), tr("Length:"), 0, 0, 1000, 1, &ok);
    if (!ok) return;

    ClothingItem item{waist, hip, length, name};
    // QMetaObject::invokeMethod(m_apiClient, "addItem", Q_ARG(ClothingItem, item));
    QMetaObject::invokeMethod(m_apiClient,
        [this, item] { m_apiClient->addItem(item); },
        Qt::QueuedConnection);
}

void MainWindow::onEditItem()
{
    int row = m_itemsTableView->currentIndex().row();
    if (row < 0) {
        QMessageBox::critical(this, tr("Error"), tr("Please select an item to edit"));
        return;
    }

    ClothingItem oldItem = m_itemModel->getItem(row);

    bool ok;
    QString newName = QInputDialog::getText(this, tr("Edit Item"), tr("Name:"), QLineEdit::Normal, oldItem.name, &ok);
    if (!ok || newName.isEmpty()) return;

    double waist = QInputDialog::getDouble(this, tr("Edit Item"), tr("Waist:"), oldItem.waist, 0, 1000, 1, &ok);
    if (!ok) return;

    double hip = QInputDialog::getDouble(this, tr("Edit Item"), tr("Hip:"), oldItem.hip, 0, 1000, 1, &ok);
    if (!ok) return;

    double length = QInputDialog::getDouble(this, tr("Edit Item"), tr("Length:"), oldItem.length, 0, 1000, 1, &ok);
    if (!ok) return;

    ClothingItem newItem{waist, hip, length, newName};
    // QMetaObject::invokeMethod(m_apiClient, "editItem",
    //     Q_ARG(QString, oldItem.name),
    //     Q_ARG(ClothingItem, newItem));
    QMetaObject::invokeMethod(m_apiClient,
        [this, oldItem, newItem] { m_apiClient->editItem(oldItem.name, newItem); },
        Qt::QueuedConnection);
}

void MainWindow::onDeleteItem()
{
    int row = m_itemsTableView->currentIndex().row();
    if (row < 0) {
        QMessageBox::critical(this, tr("Error"), tr("Please select an item to delete"));
        return;
    }

    ClothingItem item = m_itemModel->getItem(row);
    // QMetaObject::invokeMethod(m_apiClient, "deleteItem", Q_ARG(QString, item.name));
    QMetaObject::invokeMethod(m_apiClient,
        [this, item] { m_apiClient->deleteItem(item.name); },
        Qt::QueuedConnection);
}

void MainWindow::handleMatchesFound(const QList<QPair<QString, double>> &matches)
{
    m_resultsTextEdit->clear();
    if (matches.isEmpty()) {
        m_resultsTextEdit->append(tr("No matches found"));
        return;
    }

    for (const auto &match : matches) {
        m_resultsTextEdit->append(QString("%1: %2%").arg(match.first).arg(match.second * 100, 0, 'f', 2));
    }
}

void MainWindow::handleOperationCompleted(bool success, const QString &message)
{
    if (success) {
        QMessageBox::information(this, tr("Success"), message);
    } else {
        QMessageBox::critical(this, tr("Error"), message);
    }
}

void MainWindow::handleError(const QString &error)
{
    QMessageBox::critical(this, tr("Error"), error);
}

void MainWindow::handleItemsLoaded(const QJsonArray &items)
{
    m_itemModel->removeAllItems();

    for (const QJsonValue &value : items) {
        QJsonObject obj = value.toObject();
        ClothingItem item;
        item.name = obj["name"].toString();
        item.waist = obj["waist"].toDouble();
        item.hip = obj["hip"].toDouble();
        item.length = obj["length"].toDouble();
        m_itemModel->addItem(item);
    }
}

