#ifndef MYSCRIBBLER_H
#define MYSCRIBBLER_H

#include <QGraphicsView>
#include <QtWidgets>

class MouseEvent {
public:
    int action;
    QPointF pos;
    quint64 time;
    double distance;

    enum {
        Press,
        Move,
        Release
    };

    MouseEvent(int _action, QPointF _pos, quint64 time, double _distance = 0);
    MouseEvent();
    MouseEvent(const MouseEvent &me);

    friend QDataStream &operator<<(QDataStream &out, const MouseEvent &evt);
    friend QDataStream &operator>>(QDataStream &in, MouseEvent &evt);
};

class myScribbler : public QGraphicsView
{
    Q_OBJECT

    QGraphicsScene scene;
    double lineWidth;
    QPointF lastPoint;

    bool capture;
    bool showLines;

    // Stores the items of the current capture
    QList<QGraphicsItem*> currentCaptureItems;

    // Stores the groups of items for each capture
    QList<QGraphicsItemGroup*> captures;

    QList<QGraphicsLineItem*> lines;
    QList<QGraphicsEllipseItem*> dots;

    QList<MouseEvent> capturedEvents;
    QList<QList<MouseEvent>> allEvents;

    // For highlighting
    QList<int> dotCaptureIndices;
    QList<int> lineCaptureIndices;

public:
    myScribbler();
    void setCaptureOpacity(int captureIndex, double opacity);
    void highlightEvents(int captureIndex, const QList<int> &eventIndices);
    void resetHighlighting(int captureIndex);

public slots:
    void resetDrawing();
    void startCapture();
    void endCapture();
    void lineSegments();
    void dotsOnly();
    void setAllEvents(const QList<QList<MouseEvent>> &eventsList);

signals:
    void emitMySignal(QList<MouseEvent> events, bool appendToAllEvents);

protected:
    void mouseMoveEvent(QMouseEvent *evt) override;
    void mousePressEvent(QMouseEvent *evt) override;
    void mouseReleaseEvent(QMouseEvent *evt) override;
};

#endif // MYSCRIBBLER_H
