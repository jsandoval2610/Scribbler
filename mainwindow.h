#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

#include "myscribbler.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    myScribbler *scribbler;
    QTabWidget *tabs;
    QString lastDir;

    QAction *saveAction;
    QAction *loadAction;
    QAction *resetAction;

    QAction *startAction;
    QAction *endAction;

    QAction *lineAction;
    QAction *dotsAction;

    int pageCount;
    QList<QList<MouseEvent>> allEvents;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void makeTable(QList<MouseEvent> ev, bool appendToAllEvents);
    void saveFile();
    void openFile();
    void resetAll();
    void fadeOtherTabs(int currPage);
    void tableRowSelected();
};

#endif // MAINWINDOW_H
