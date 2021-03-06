#ifndef VIEW_H
#define VIEW_H

#include <mydebug.h>

#include <qcanvas.h>

class View:public QCanvasView
{
Q_OBJECT
	public:
		View(QCanvas& canvas,QWidget *parent=0);
		static QCanvasPixmapArray ani,ani_x;
	public slots:
		void setCanPress(bool t);

	protected:
		void dragEnterEvent(QDragEnterEvent *e);
		void dropEvent(QDropEvent *e);
		void contentsMousePressEvent(QMouseEvent *e);
		void contentsMouseMoveEvent(QMouseEvent *e);
		void contentsMouseReleaseEvent(QMouseEvent *e);
	private:
		bool CanPress;
		QCanvasItem *moving;
		QPoint moving_start;
};

#endif // VIEW_H
