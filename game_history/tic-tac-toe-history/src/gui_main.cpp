#include <QApplication>
#include "game_history.h"
#include "game_history_gui.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Initialize the game history
    GameHistory history("tictactoe.db");
    
    // Create the GUI
    GameHistoryGUI gui(&history);
    gui.show();
    
    return app.exec();
}