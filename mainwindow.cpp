#include "mainwindow.h"
#include <QtWidgets>
#include <QSettings>
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

    // Setting up last directory
    QSettings settings("jsandoval", "Scribbler");
    lastDir = settings.value("lastDir", "").toString();

    // FILE MENU
    // Save action
    saveAction = new QAction("Save Drawing", this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);

    // Load action
    loadAction = new QAction("Load Drawing", this);
    loadAction->setShortcut(QKeySequence::Open);
    connect(loadAction, &QAction::triggered, this, &MainWindow::openFile);

    // Reset action
    resetAction = new QAction("Reset Drawing", this);
    resetAction->setShortcut(QKeySequence::Undo);
    connect(resetAction, &QAction::triggered, this, &MainWindow::resetAll);

    QMenu *fileMenu = new QMenu("&File", this);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(loadAction);
    fileMenu->addAction(resetAction);

    // CAPTURE MENU
    // Start action
    startAction = new QAction("Begin Recording", this);
    startAction->setShortcut(QKeySequence("Ctrl+B"));
    connect(startAction, &QAction::triggered, scribbler, &myScribbler::startCapture);

    // End action
    endAction = new QAction("End Recording", this);
    endAction->setShortcut(QKeySequence("Ctrl+E"));
    connect(endAction, &QAction::triggered, scribbler, &myScribbler::endCapture);

    QMenu *captureMenu = new QMenu("&Capture", this);
    captureMenu->addAction(startAction);
    captureMenu->addAction(endAction);

    // VIEW MENU
    // Line action
    lineAction = new QAction("Line Segments", this);
    lineAction->setShortcut(QKeySequence("Ctrl+L"));
    connect(lineAction, &QAction::triggered, scribbler, &myScribbler::lineSegments);

    // Dots action
    dotsAction = new QAction("Only Dots", this);
    dotsAction->setShortcut(QKeySequence("Ctrl+D"));
    connect(dotsAction, &QAction::triggered, scribbler, &myScribbler::dotsOnly);

    QMenu *viewMenu = new QMenu("&View", this);
    viewMenu->addAction(lineAction);
    viewMenu->addAction(dotsAction);

    // Other connections
    connect(scribbler, &myScribbler::emitMySignal, this, &MainWindow::makeTable);
    connect(tabs, &QTabWidget::currentChanged, this, &MainWindow::fadeOtherTabs);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(captureMenu);
    menuBar()->addMenu(viewMenu);

    pageCount = 0;
    tabs->hide();
}

MainWindow::~MainWindow() {
    // Save last directory to settings
    QSettings settings("jsandoval", "Scribbler");
    settings.setValue("lastDir", lastDir);
}

void MainWindow::makeTable(QList<MouseEvent> ev, bool appendToAllEvents) {
    if (ev.isEmpty())
        return;

    QTableWidget *eventsTable = new QTableWidget();
    eventsTable->setRowCount(ev.size());
    eventsTable->setColumnCount(4);

    QStringList headers;
    headers << "Action" << "Position" << "Time (s)" << "Distance (px)";
    eventsTable->setHorizontalHeaderLabels(headers);

    quint64 firstEventTime = ev.first().time;

    for (int i = 0; i < ev.size(); ++i) {
        const MouseEvent &e = ev[i];

        // Action
        QString actionStr;
        switch (e.action) {
        case MouseEvent::Press:
            actionStr = "Press";
            break;
        case MouseEvent::Move:
            actionStr = "Move";
            break;
        case MouseEvent::Release:
            actionStr = "Release";
            break;
        default:
            actionStr = "?";
            break;
        }
        eventsTable->setItem(i, 0, new QTableWidgetItem(actionStr));

        // Position
        QString posStr = QString("(%1, %2)").arg(e.pos.x()).arg(e.pos.y());
        eventsTable->setItem(i, 1, new QTableWidgetItem(posStr));

        // Time
        double timeInSeconds = (e.time - firstEventTime) / 1000.0;
        QString timeStr = QString::number(timeInSeconds, 'f', 3);
        eventsTable->setItem(i, 2, new QTableWidgetItem(timeStr));

        // Distance
        QString distanceStr = QString::number(e.distance, 'f', 2);
        eventsTable->setItem(i, 3, new QTableWidgetItem(distanceStr));
    }

    connect(eventsTable, &QTableWidget::itemSelectionChanged, this, &MainWindow::tableRowSelected);

    QString tabName = QString("Capture %1").arg(pageCount++);
    tabs->addTab(eventsTable, tabName);
    tabs->show();

    if (appendToAllEvents) {
        allEvents.append(ev);
    }

    fadeOtherTabs(tabs->currentIndex());
}

void MainWindow::resetAll() {
    scribbler->resetDrawing();
    tabs->clear();
    tabs->hide();
    pageCount = 0;
    allEvents.clear();
}

void MainWindow::saveFile() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save File", lastDir, "Drawing Files (*.drw)");
    if (!fileName.isEmpty()) {
        // Directory memory
        lastDir = QFileInfo(fileName).absolutePath();
        QSettings settings("jsandoval", "Scribbler");
        settings.setValue("lastDir", lastDir);

        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            QDataStream out(&file);

            out << allEvents;
            file.close();
        }
    }
}

void MainWindow::openFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", lastDir, "Drawing Files (*.drw)");
    if (!fileName.isEmpty()) {
        // Directory memory
        lastDir = QFileInfo(fileName).absolutePath();
        QSettings settings("jsandoval", "Scribbler");
        settings.setValue("lastDir", lastDir);

        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            QDataStream in(&file);

            resetAll();

            in >> allEvents;
            file.close();

            // Reconstruct the tables and drawings
            for (const QList<MouseEvent> &events : allEvents) {
                makeTable(events, false);
            }

            scribbler->setAllEvents(allEvents);
            fadeOtherTabs(tabs->currentIndex());
        }
    }
}

void MainWindow::fadeOtherTabs(int currPage) {
    scribbler->setCaptureOpacity(currPage, 0.25);
}

void MainWindow::tableRowSelected() {

    // Get the current table
    QTableWidget *table = qobject_cast<QTableWidget *>(tabs->currentWidget());
    if (!table)
        return;

    // Get selected rows
    QList<QTableWidgetItem*> selectedItems = table->selectedItems();
    QSet<int> selectedRows;
    for (QTableWidgetItem* item : selectedItems) {
        selectedRows.insert(item->row());
    }

    QList<int> eventIndices = selectedRows.values();

    int currTabIndex = tabs->currentIndex();
    scribbler->highlightEvents(currTabIndex, eventIndices);
}
