#include "myscribbler.h"
#include <QtWidgets>

MouseEvent::MouseEvent(int _action, QPointF _pos, quint64 _time, double _distance)
    : action(_action), pos(_pos), time(_time), distance(_distance) { }

MouseEvent::MouseEvent() { }

MouseEvent::MouseEvent(const MouseEvent &me) {
    *this = me;
}

QDataStream &operator<<(QDataStream &out, const MouseEvent &evt) {
    return out << evt.action << evt.pos << evt.time << evt.distance;
}

QDataStream &operator>>(QDataStream &in, MouseEvent &evt) {
    return in >> evt.action >> evt.pos >> evt.time >> evt.distance;
}

// -----------------------------------------------------------------

myScribbler::myScribbler() : lineWidth(4.0), showLines(true) {
    setScene(&scene);
    setSceneRect(QRectF(0.0, 0.0, 800.0, 600.0));
    setMinimumSize(QSize(800, 600));
    setRenderHint(QPainter::Antialiasing, true);

    scene.addRect(sceneRect());
    capture = false;
}

void myScribbler::mouseMoveEvent(QMouseEvent *evt) {

    QPointF p = mapToScene(evt->pos());
    double distance = std::hypot(p.x() - lastPoint.x(), p.y() - lastPoint.y());

    QGraphicsLineItem* lineItem = scene.addLine(QLineF(lastPoint, p),
                                                QPen(Qt::darkGreen, lineWidth, Qt::SolidLine, Qt::FlatCap));
    lineItem->setVisible(showLines);
    lines.append(lineItem);

    QGraphicsEllipseItem* dotItem = scene.addEllipse(
        QRectF(p - QPointF(0.5 * lineWidth, 0.5 * lineWidth),
               QSizeF(lineWidth, lineWidth)), Qt::NoPen, Qt::darkGreen);
    dots.append(dotItem);

    lastPoint = p;
    MouseEvent me(MouseEvent::Move, p, QDateTime::currentMSecsSinceEpoch(), distance);

    // Store items if capturing
    if (capture) {
        capturedEvents.append(me);
        currentCaptureItems.append(lineItem);
        currentCaptureItems.append(dotItem);
    }
}

void myScribbler::mousePressEvent(QMouseEvent *evt) {

    QPointF p = mapToScene(evt->pos());
    lastPoint = p;

    QGraphicsEllipseItem* dotItem = scene.addEllipse(
        QRectF(p - QPointF(0.5 * lineWidth, 0.5 * lineWidth),
               QSizeF(lineWidth, lineWidth)), Qt::NoPen, Qt::darkGreen);
    dots.append(dotItem);

    MouseEvent me(MouseEvent::Press, p, QDateTime::currentMSecsSinceEpoch(), 0);

    // Store items if capturing
    if (capture) {
        capturedEvents.append(me);
        currentCaptureItems.append(dotItem);
    }
}

void myScribbler::mouseReleaseEvent(QMouseEvent *evt) {

    QPointF p = mapToScene(evt->pos());

    MouseEvent me(MouseEvent::Release, p, QDateTime::currentMSecsSinceEpoch(), 0);

    if (capture) {
        capturedEvents.append(me);
    }
}

void myScribbler::resetDrawing() {

    scene.clear();
    scene.addRect(sceneRect());
    lines.clear();
    dots.clear();
    captures.clear();
    currentCaptureItems.clear();
    allEvents.clear();

    dotCaptureIndices.clear();
    lineCaptureIndices.clear();
}

void myScribbler::startCapture() {
    capture = true;
    capturedEvents.clear();
    currentCaptureItems.clear();

    dotCaptureIndices.append(dots.size());
    lineCaptureIndices.append(lines.size());
}

void myScribbler::endCapture() {
    // Group current capture items
    QGraphicsItemGroup* group = new QGraphicsItemGroup();
    for (QGraphicsItem* item : currentCaptureItems) {
        group->addToGroup(item);
    }
    scene.addItem(group);
    captures.append(group);
    currentCaptureItems.clear();

    // Store the captured events
    allEvents.append(capturedEvents);

    emit emitMySignal(capturedEvents, true);
    capture = false;
    capturedEvents.clear();
}

void myScribbler::lineSegments() {

    showLines = true;
    for (QGraphicsLineItem* line : lines) {
        line->setVisible(true);
    }
}

void myScribbler::dotsOnly() {

    showLines = false;
    for (QGraphicsLineItem* line : lines) {
        line->setVisible(false);
    }
}

void myScribbler::setCaptureOpacity(int captureIndex, double opacity) {

    for (int i = 0; i < captures.size(); ++i) {
        if (i != captureIndex) {
            captures[i]->setOpacity(opacity);
        } else {
            captures[i]->setOpacity(1);
        }
    }
}

void myScribbler::highlightEvents(int captureIndex, const QList<int> &eventIndices) {

    resetHighlighting(captureIndex);

    if (captureIndex < 0 || captureIndex >= captures.size())
        return;

    int dotStartIndex = dotCaptureIndices[captureIndex];
    int dotEndIndex = (captureIndex + 1 < dotCaptureIndices.size()) ? dotCaptureIndices[captureIndex + 1] : dots.size();

    int lineStartIndex = lineCaptureIndices[captureIndex];
    int lineEndIndex = (captureIndex + 1 < lineCaptureIndices.size()) ? lineCaptureIndices[captureIndex + 1] : lines.size();

    for (int index : eventIndices) {
        int dotIndex = dotStartIndex + index;
        int lineIndex = lineStartIndex + index;

        if (dotIndex >= dotStartIndex && dotIndex < dotEndIndex) {
            QGraphicsEllipseItem* dot = dots[dotIndex];
            dot->setBrush(Qt::yellow);
        }
        if (lineIndex >= lineStartIndex && lineIndex < lineEndIndex) {
            QGraphicsLineItem* line = lines[lineIndex];
            QPen pen = line->pen();
            pen.setColor(Qt::yellow);
            line->setPen(pen);
        }
    }
}

void myScribbler::resetHighlighting(int captureIndex) {

    if (captureIndex < 0 || captureIndex >= captures.size())
        return;

    int dotStartIndex = dotCaptureIndices[captureIndex];
    int dotEndIndex = (captureIndex + 1 < dotCaptureIndices.size()) ? dotCaptureIndices[captureIndex + 1] : dots.size();

    int lineStartIndex = lineCaptureIndices[captureIndex];
    int lineEndIndex = (captureIndex + 1 < lineCaptureIndices.size()) ? lineCaptureIndices[captureIndex + 1] : lines.size();

    for (int i = dotStartIndex; i < dotEndIndex; ++i) {
        QGraphicsEllipseItem* dot = dots[i];
        dot->setBrush(Qt::darkGreen);
    }
    for (int i = lineStartIndex; i < lineEndIndex; ++i) {
        QGraphicsLineItem* line = lines[i];
        QPen pen = line->pen();
        pen.setColor(Qt::darkGreen);
        line->setPen(pen);
    }
}


void myScribbler::setAllEvents(const QList<QList<MouseEvent>> &eventsList) {

    resetDrawing();

    allEvents = eventsList;
    captures.clear();
    dotCaptureIndices.clear();
    lineCaptureIndices.clear();

    // Iterate over all captures
    for (const QList<MouseEvent> &eventList : allEvents) {

        currentCaptureItems.clear();
        dotCaptureIndices.append(dots.size());
        lineCaptureIndices.append(lines.size());

        // Start a new group for each capture
        QGraphicsItemGroup* group = new QGraphicsItemGroup();
        QPointF lastPoint;
        bool firstPoint = true;

        // Iterate over events in the capture
        for (const MouseEvent &ev : eventList) {
            QPointF p = ev.pos;
            switch (ev.action) {
            case MouseEvent::Press:
                lastPoint = p;
                // Draw dot
                {
                    QGraphicsEllipseItem* dotItem = scene.addEllipse(
                        QRectF(p - QPointF(0.5 * lineWidth, 0.5 * lineWidth), QSizeF(lineWidth, lineWidth)), Qt::NoPen, Qt::darkGreen);
                    dots.append(dotItem);
                    currentCaptureItems.append(dotItem);
                }
                firstPoint = false;
                break;

            case MouseEvent::Move:
                if (!firstPoint) {
                    QGraphicsLineItem* lineItem = scene.addLine(QLineF(lastPoint, p), QPen(Qt::darkGreen, lineWidth, Qt::SolidLine, Qt::FlatCap));
                    lineItem->setVisible(showLines);
                    lines.append(lineItem);
                    currentCaptureItems.append(lineItem);
                }
                {
                    // Draw dot
                    QGraphicsEllipseItem* dotItem = scene.addEllipse(QRectF(p - QPointF(0.5 * lineWidth, 0.5 * lineWidth), QSizeF(lineWidth, lineWidth)), Qt::NoPen, Qt::darkGreen);
                    dots.append(dotItem);
                    currentCaptureItems.append(dotItem);
                    lastPoint = p;
                    break;
                }

            case MouseEvent::Release:
                break;
            }
        }

        // Add current capture items to the group
        for (QGraphicsItem* item : currentCaptureItems) {
            group->addToGroup(item);
        }
        scene.addItem(group);
        captures.append(group);
    }
}
