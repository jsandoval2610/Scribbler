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

    QAction *saveAction;
    QAction *loadAction;
    QAction *resetAction;

    QAction *startAction;
    QAction *endAction;

    QAction *lineAction;
    QAction *dotsAction;

    int pageCount;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void makeTable(QList<MouseEvent> ev);
};
#endif // MAINWINDOW_H
