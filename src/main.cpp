#include <QApplication>
#include <QIcon>
#include "view/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/app.ico"));
    app.setStyle("Fusion");

    MainWindow window;
    window.show();

    return app.exec();
}
