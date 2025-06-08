#include "application.h"
#include "main_window.h"

Application::Application(int argc, char *argv[]) {
    m_qapp = std::make_unique<QApplication>(argc, argv);
    m_mainWindow = std::make_unique<MainWindow>();
}

Application::~Application() = default;

int Application::run() {
    m_mainWindow->show();
    return m_qapp->exec();
}