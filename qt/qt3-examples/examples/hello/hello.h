/****************************************************************************
** $Id: qt/hello.h   3.3.7   edited Aug 31 2005 $
**
** Copyright (C) 1992-2005 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef HELLO_H
#define HELLO_H

#include <qwidget.h>


class Hello : public QWidget
{
    Q_OBJECT
public:
    Hello( const char *text, QWidget *parent=0, const char *name=0 );
signals:
    void clicked();
protected:
    void mouseReleaseEvent( QMouseEvent * );
    void paintEvent( QPaintEvent * );
private slots:
    void animate();
private:
    QString t;
    int     b;
};

#endif
