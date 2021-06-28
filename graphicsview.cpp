#include "graphicsview.h"

#include <QDebug>
#include <QMouseEvent>

GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView(parent)
{

}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    QPointF remapped = mapToScene(event->pos());

    qDebug() << remapped;

    emit clickedAt(remapped);
}
