#ifndef MYSCRIBBLER_H
#define MYSCRIBBLER_H

#include <QGraphicsView>
#include <QtWidgets>

class MouseEvent {
public:
    int action;
    QPointF pos;
    quint64 time;

    enum {
        Press,
        Move,
        Release
    };

    MouseEvent(int _action, QPointF _pos, quint64 time);
    MouseEvent();
    MouseEvent(const MouseEvent &me);

    friend QDataStream &operator<<(QDataStream &out, const MouseEvent &evt);
    friend QDataStream &operator>>(QDataStream &out, MouseEvent &evt);

};


class myScribbler : public QGraphicsView
{

    QGraphicsScene scene;
    double lineWidth;
    QPointF lastPoint;

    bool capture;


    QList<MouseEvent> events;

    Q_OBJECT

public:
    myScribbler();

public slots:
    void saveDrawing();
    void loadDrawing();
    void resetDrawing();
    void startCapture();
    void endCapture();
    void lineSegments();
    void dotsOnly();

signals:
    void emitMySignal(QList<MouseEvent> events);


protected:
    void mouseMoveEvent(QMouseEvent *evt) override;
    void mousePressEvent(QMouseEvent *evt) override;
    void mouseReleaseEvent(QMouseEvent *evt) override;

};

#endif // MYSCRIBBLER_H
