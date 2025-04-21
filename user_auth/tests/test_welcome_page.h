#ifndef TEST_WELCOME_PAGE_H
#define TEST_WELCOME_PAGE_H

#include <QTest>
#include <QSignalSpy>
#include "../src/welcome_page.h"

class TestWelcomePage : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testSetUsername();
    void testLogoutButton();

private:
    WelcomePage* m_welcomePage;
};

#endif