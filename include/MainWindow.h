#pragma once

#include "ApiClient.h"
#include "ClothingItemModel.h"

#include <QWidget>
#include <QThread>
#include <memory>

#include <QDoubleSpinBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QStackedWidget>

class QLineEdit;
class QPushButton;
class QTextEdit;
class QTableView;
class QDoubleSpinBox;

class MainWindow : public QWidget
{
    Q_OBJECT
    friend class TestMainWindow;

public:
    explicit MainWindow(bool isAdminMode, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onFindMatches();
    void onAddItem();
    void onEditItem();
    void onDeleteItem();

    void handleMatchesFound(const QList<QPair<QString, double>> &matches);
    void handleOperationCompleted(bool success, const QString &message);
    void handleError(const QString &error);
    void handleItemsLoaded(const QJsonArray &items);

private:
    void setupUserInterface();
    void setupAdminInterface();

    QStackedWidget *m_stackedWidget;

    bool m_isAdminMode;

    QWidget *m_userWidget;
    QWidget *m_adminWidget;

    // Общие элементы
    QDoubleSpinBox* m_waistSpinBox;
    QDoubleSpinBox* m_hipSpinBox;
    QDoubleSpinBox* m_lengthSpinBox;
    QPushButton* m_findMatchesButton;
    QTextEdit* m_resultsTextEdit;

    // Только для админа
    QPushButton* m_addItemButton;
    QPushButton* m_editItemButton;
    QPushButton* m_deleteItemButton;
    QTableView* m_itemsTableView;

    QThread* m_networkThread;
    // std::unique_ptr<ApiClient> m_apiClient;
    ApiClient *m_apiClient;
    ClothingItemModel* m_itemModel;
};

