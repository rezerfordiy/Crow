#include <QApplication>
#include "MainWidget.h"

#include <QSettings>


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    
    QCoreApplication::setOrganizationName("YourCompany");
    QCoreApplication::setApplicationName("YourApp");
    
    
    MainWidget widget;
    widget.show();
    
    return app.exec();
}
