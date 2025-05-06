#include <QApplication>
#include <QFile>
#include <QFontDatabase>
#include <QKeyEvent>
#include <QTranslator>
#include <QStyleFactory>
#include "../include/MainWindow.h"
#include "../include/AppEventFilter.h"
#include "../include/RoleSelectionDialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Загрузка шрифтов
    QFontDatabase::addApplicationFont(":/fonts/Montserrat-Regular.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Montserrat-SemiBold.ttf");

    // Установка шрифта по умолчанию
    QFont defaultFont("Montserrat", 10);
    app.setFont(defaultFont);

    QFile styleFile(":/styles/fashion.qss");
    if (styleFile.open(QFile::ReadOnly))
    {
        app.setStyleSheet(styleFile.readAll());
    }


    // диалог выбора роли
    RoleSelectionDialog roleDialog;
    if (roleDialog.exec() != QDialog::Accepted) {
        return 0; // Закрываем приложение, если пользователь отменил выбор
    }

    QApplication::setStyle(QStyleFactory::create("Fusion"));

    // главное окно с выбранным режимом
    MainWindow window(roleDialog.isAdminSelected());

    // Настраиваем фильтр событий
    AppEventFilter appFilter(&app);
    app.installEventFilter(&appFilter);

    // Показываем главное окно и запускаем приложение
    window.show();
    return app.exec();
}



// // Загрузка переводов
// QTranslator translator;
// if (translator.load(":/translations/ru.qm")) {
//     app.installTranslator(&translator);
// }
