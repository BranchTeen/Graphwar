#include "app/GraphwarApp.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    GraphwarApp gapp;
    gapp.show_main_window();
    return app.exec();
}
