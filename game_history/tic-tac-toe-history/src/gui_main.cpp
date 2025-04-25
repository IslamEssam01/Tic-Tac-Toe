#include "game_history_gui.h"
#include <QApplication>
#include <QCommandLineParser>
#include <iostream>

int main(int argc, char *argv[]) {
    // Initialize Qt application
    QApplication app(argc, argv);
    
    // Set application info
    QApplication::setApplicationName("Tic-Tac-Toe History Viewer");
    QApplication::setApplicationVersion("1.0");
    
    // Parse command line arguments
    QCommandLineParser parser;
    parser.setApplicationDescription("A GUI application to view Tic-Tac-Toe game history");
    parser.addHelpOption();
    parser.addVersionOption();
    
    // Add database file option
    QCommandLineOption dbOption(QStringList() << "d" << "database", 
                               "SQLite database file path", "database", "tictactoe.db");
    parser.addOption(dbOption);
    
    // Process the command line arguments
    parser.process(app);
    
    // Get the database path
    QString dbPath = parser.value(dbOption);
    
    // Create and show the main window
    GameHistoryGUI window(dbPath.toStdString());
    window.show();
    
    // Execute the application
    return app.exec();
}