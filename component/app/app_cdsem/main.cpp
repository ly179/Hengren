#include <QApplication>
#include "main_prepared.h"
int main(int argc, char* argv[]) {
    // system("chcp 65001");
    QApplication a(argc, argv);
    int r = run(a);
    qInfo() <<  "The app exit with code " << r;
    return r;
}
