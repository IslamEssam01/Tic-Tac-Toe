#ifndef TEST_MAIN_WINDOW_H
#define TEST_MAIN_WINDOW_H

#include <QTest>
#include "../src/main_window.h"
#include "../src/login_page.h"
#include "../src/welcome_page.h"

class TestMainWindow : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testPageTransition();

private:
    MainWindow* m_mainWindow;
};

#endif