#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include "cache_gui.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("Cache Simulator");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("IITG");
    app.setOrganizationDomain("iitg.ac.in");
    
    // Set a modern style if available
    QStringList availableStyles = QStyleFactory::keys();
    if (availableStyles.contains("Fusion")) {
        app.setStyle("Fusion");
    }
    
    // Create and show main window
    CacheGUI window;
    window.show();
    
    return app.exec();
}
