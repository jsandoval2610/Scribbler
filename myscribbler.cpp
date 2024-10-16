#include "myscribbler.h"
#include <QtWidgets>

MouseEvent::MouseEvent(int _action, QPointF _pos, quint64 _time) : action(_action), pos(_pos), time(_time) { }

MouseEvent::MouseEvent(){}

MouseEvent::MouseEvent(const MouseEvent &me) {
    *this = me;
}

QDataStream &operator<<(QDataStream &out, const MouseEvent &evt) {
    return out << evt.action << evt.pos << evt.time;
}

QDataStream &operator>>(QDataStream &in, MouseEvent &evt) {
    return in >> evt.action >> evt.pos >> evt.time;
}


// -----------------------------------------------


myScribbler::myScribbler() : lineWidth(4.0){

    setScene(&scene);
    setSceneRect(QRectF(0.0, 0.0, 800.0, 600.0));
    setMinimumSize(QSize(800, 600));
    setRenderHint(QPainter::Antialiasing, true);

    scene.addRect(sceneRect());

    capture = false;

}

void myScribbler::mouseMoveEvent(QMouseEvent *evt) {

    QGraphicsView::mouseMoveEvent(evt);

    QPointF p = mapToScene(evt->pos());
    scene.addLine(QLineF(lastPoint, p), QPen(Qt::black, lineWidth, Qt::SolidLine, Qt::FlatCap));
    scene.addEllipse(QRectF(p - QPointF(0.5*lineWidth, 0.5*lineWidth), QSizeF(lineWidth, lineWidth)), Qt::NoPen, Qt::black);
    lastPoint = p;

    if(capture) {
    events << MouseEvent(MouseEvent::Move, p, evt->timestamp());
    }
}

void myScribbler::mousePressEvent(QMouseEvent *evt) {

    QGraphicsView::mousePressEvent(evt);

    QPointF p = mapToScene(evt->pos());
    lastPoint = p;
    scene.addEllipse(QRectF(p - QPointF(0.5*lineWidth, 0.5*lineWidth), QSizeF(lineWidth, lineWidth)), Qt::NoPen, Qt::black);

    if(capture) {
    events << MouseEvent(MouseEvent::Press, p, evt->timestamp());
    }

}

void myScribbler::mouseReleaseEvent(QMouseEvent *evt) {

    QGraphicsView::mouseReleaseEvent(evt);

    QPointF p = mapToScene(evt->pos());
    // scene.addEllipse(QRectF(p - QPointF(0.5*lineWidth, 0.5*lineWidth), QSizeF(lineWidth, lineWidth)), Qt::NoPen, Qt::black);

    if(capture) {
    events << MouseEvent(MouseEvent::Release, p, evt->timestamp());
    // qDebug() << events;
    }

}

void myScribbler::saveDrawing() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Drawing", "", "Drawing Files (*.drw)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            QDataStream out(&file);
            out << events;
            file.close();
        }
    }
}

void myScribbler::loadDrawing() {

    QString fileName = QFileDialog::getOpenFileName(this, "Load Drawing", "", "Drawing Files (*.drw)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            QDataStream in(&file);
            QList<MouseEvent> loadedEvents;
            in >> loadedEvents;
            file.close();

            // Clear the current scene and events
            events.clear();
        }
    }

}

void myScribbler::resetDrawing() {
    scene.clear();
    scene.addRect(sceneRect());
}

void myScribbler::startCapture() {
    capture = true;
}

void myScribbler::endCapture() {

    // emitSignal
    emit emitMySignal(events);
    capture = false;
}

void myScribbler::lineSegments() {

}

void myScribbler::dotsOnly() {

}
