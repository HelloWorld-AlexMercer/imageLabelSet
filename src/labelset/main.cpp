#include "labelsetmainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LabelSetMainWindow w;
    w.show();
    return a.exec();
}
