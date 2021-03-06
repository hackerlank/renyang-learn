#include <qpainter.h>
#include <qpixmap.h>

#include "axbouncer.h"
#include "objectsafetyimpl.h"

AxBouncer::AxBouncer(QWidget *parent, const char *name)
    : QWidget(parent, name, WNoAutoErase)
{
    ballColor = blue;
    ballSpeed = Normal;
    ballRadius = 15;
    myTimerId = 0;
    x = 20;
    delta = 2;
}

void AxBouncer::setColor(const QColor &newColor)
{
    if (newColor != ballColor && requestPropertyChange("color")) {
        ballColor = newColor;
        update();
        propertyChanged("color");
    }
}

void AxBouncer::setSpeed(Speed newSpeed)
{
    if (newSpeed != ballSpeed && requestPropertyChange("speed")) {
        ballSpeed = newSpeed;

        if (isRunning()) {
            killTimer(myTimerId);
            myTimerId = startTimer(intervalInMilliseconds());
        }
        propertyChanged("speed");
    }
}

int AxBouncer::intervalInMilliseconds() const
{
    switch (ballSpeed) {
    case Slow:
        return 50;
    case Fast:
        return 10;
    default:
        return 30;
    }
}

void AxBouncer::setRadius(int newRadius)
{
    if (newRadius != ballRadius && requestPropertyChange("radius")) {
        ballRadius = newRadius;
        update();
        updateGeometry();
        propertyChanged("radius");
    }
}

QSize AxBouncer::sizeHint() const
{
    return QSize(250, 5 * radius());
}

QAxAggregated *AxBouncer::createAggregate()
{
    return new ObjectSafetyImpl;
}

void AxBouncer::start()
{
    if (!isRunning() && requestPropertyChange("running")) {
        myTimerId = startTimer(intervalInMilliseconds());
        propertyChanged("running");
    }
}

void AxBouncer::stop()
{
    if (isRunning() && requestPropertyChange("running")) {
        killTimer(myTimerId);
        myTimerId = 0;
        propertyChanged("running");
    }
}

void AxBouncer::paintEvent(QPaintEvent *event)
{
    static QPixmap pixmap;
    QRect rect = event->rect();

    QSize newSize = rect.size().expandedTo(pixmap.size());
    pixmap.resize(newSize);
    pixmap.fill(this, rect.topLeft());

    QPainter painter(&pixmap, this);
    painter.translate(-rect.x(), -rect.y());
    painter.setPen(ballColor);
    painter.setBrush(ballColor);
    painter.drawEllipse(x - radius(), height() / 2 - radius(),
                        2 * radius(), 2 * radius());
    bitBlt(this, rect.x(), rect.y(), &pixmap, 0, 0,
           rect.width(), rect.height());
}

void AxBouncer::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == myTimerId) {
        if ((delta > 0 && x >= width() - radius())
                || (delta < 0 && x <= radius())) {
            delta = -delta;
            emit bouncing();
        }
        x += delta;
        update();
    } else {
        QWidget::timerEvent(event);
    }
}
