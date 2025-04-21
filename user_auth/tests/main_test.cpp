#include <QTest>
#include "test_login_page.h"
#include "test_welcome_page.h"
#include "test_main_window.h"

int main(int argc, char *argv[]) {
    // Create the application
    QApplication app(argc, argv);

    // Create and run the tests
    int status = 0;
    
    {
        TestLoginPage testLoginPage;
        status |= QTest::qExec(&testLoginPage, argc, argv);
    }
    
    {
        TestWelcomePage testWelcomePage;
        status |= QTest::qExec(&testWelcomePage, argc, argv);
    }
    
    {
        TestMainWindow testMainWindow;
        status |= QTest::qExec(&testMainWindow, argc, argv);
    }
    
    return status;
}