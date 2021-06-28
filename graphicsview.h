#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QObject>
#include <QWidget>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit GraphicsView(QWidget *parent = nullptr);

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;

signals:
    void clickedAt(QPointF denormalized);
};

#endif // GRAPHICSVIEW_H
