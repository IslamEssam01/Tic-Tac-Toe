#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <memory>

class MainWindow;

class Application {
public:
    Application(int argc, char *argv[]);
    ~Application();
    
    int run();

private:
    std::unique_ptr<QApplication> m_qapp;
    std::unique_ptr<MainWindow> m_mainWindow;
};

#endif