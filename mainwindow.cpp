#include "mainwindow.h"
#include<QtWidgets>
#include "myscribbler.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    // Setup the UI
    QWidget *center = new QWidget(this);
    QHBoxLayout *splitter = new QHBoxLayout(center);

    scribbler = new myScribbler();
    tabs = new QTabWidget();

    splitter->addWidget(scribbler);
    splitter->addWidget(tabs);

    setCentralWidget(center);
    resize(800, 600);


    // FILE MENU
    // Save action
    saveAction = new QAction("Save Drawing", this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, scribbler, &myScribbler::saveDrawing);

    // Load action
    loadAction = new QAction("Load Drawing", this);
    loadAction->setShortcut(QKeySequence::Open);
    connect(loadAction, &QAction::triggered, scribbler, &myScribbler::loadDrawing);

    // Reset action
    resetAction = new QAction("Reset Drawing", this);
    resetAction->setShortcut(QKeySequence::Undo);
    connect(resetAction, &QAction::triggered, scribbler, &myScribbler::resetDrawing);

    QMenu *fileMenu = new QMenu("&File", this);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(loadAction);
    fileMenu->addAction(resetAction);

    // CAPTURE MENU
    // Start action
    startAction = new QAction("Begin recording", this);
    startAction->setShortcut(QKeySequence("Ctrl+B"));
    connect(saveAction, &QAction::triggered, scribbler, &myScribbler::startCapture);

    // End action
    endAction = new QAction("End recording", this);
    endAction->setShortcut(QKeySequence("Ctrl+E"));
    connect(loadAction, &QAction::triggered, scribbler, &myScribbler::endCapture);

    QMenu *captureMenu = new QMenu("&Capture", this);
    captureMenu->addAction(startAction);
    captureMenu->addAction(endAction);

    // VIEW MENU
    // Line action
    lineAction = new QAction("?", this);
    // startAction->setShortcut(QKeySequence::Save);
    connect(lineAction, &QAction::triggered, scribbler, &myScribbler::lineSegments);

    // Dots action
    dotsAction = new QAction("?", this);
    // endAction->setShortcut(QKeySequence::Open);
    connect(dotsAction, &QAction::triggered, scribbler, &myScribbler::dotsOnly);

    connect(scribbler, &myScribbler::emitMySignal, this, &MainWindow::makeTable);

    QMenu *viewMenu = new QMenu("&View", this);
    viewMenu->addAction(lineAction);
    viewMenu->addAction(dotsAction);



    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(captureMenu);
    menuBar()->addMenu(viewMenu);


    pageCount = 0;
}

MainWindow::~MainWindow() {}

void MainWindow::makeTable(QList<MouseEvent> ev) {

    QTableWidget myTable;
    for(int i = 0; i < ev.size(); ++i) {
        // myTable.setItem(i, 0, ev[i]);
        qDebug() << i;
    }
    // tabs->addTab(pageCount, "Current Tab");
    ++pageCount;
}

