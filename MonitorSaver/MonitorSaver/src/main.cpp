#include "mainwindow.h"
#include <QApplication>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Ethan's Epic Monitor Position Remberer aka MonitorSaver V0.01");
   w.setWindowIcon(QIcon(":/ui/monitorSaver.ico"));
    w.show();
    return a.exec();

}
