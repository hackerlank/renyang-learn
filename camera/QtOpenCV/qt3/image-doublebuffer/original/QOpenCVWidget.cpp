
#include "QOpenCVWidget.h"
#include <qpainter.h>
#include <qpixmap.h>

// Constructor
QOpenCVWidget::QOpenCVWidget(QWidget *parent):QWidget(parent) {
	layout = new QVBoxLayout(this);
	imagelabel = new QLabel(this,"imagelabel");
	// 如此設定就不會在更新圖片前, 把舊的frame全部清空, 造成閃爍的感覺
	imagelabel->setBackgroundMode(Qt::NoBackground);
	layout->addWidget(imagelabel);
}

QOpenCVWidget::~QOpenCVWidget(void) {
	// do nothing
}

void QOpenCVWidget::putImage(IplImage *cvimage) {
	// 宣告一個pixmap來當作double-buffer
	QPixmap pix(imagelabel->size());
	int cvIndex,cvLineStart;
	// switch between bit depths
	switch (cvimage->depth) {
		case IPL_DEPTH_8U:
			switch (cvimage->nChannels) {
				case 3:
					if ((cvimage->width != image.width()) || (cvimage->height != image.height())) {
						QImage temp(cvimage->width,cvimage->height,32);
						image = temp;
					}
					cvIndex = 0; cvLineStart = 0;
					for (int y = 0;y<cvimage->height;y++) {
						unsigned char red,green,blue;
						cvIndex = cvLineStart;
						for (int x = 0;x < cvimage->width;x++) {
							red = cvimage->imageData[cvIndex+2];
							green = cvimage->imageData[cvIndex+1];
							blue = cvimage->imageData[cvIndex+0];

							image.setPixel(x,y,qRgb(red,green,blue));
							cvIndex+=3;
						}
						cvLineStart += cvimage->widthStep;
					}
					break;
				default:
					printf("This number of channels is not supported\n");
					break;
			}
			break;
		default:
			printf("This type of IplImage is not implemented in QOpenCVWidget\n");
			break;
	}
	pix.convertFromImage(image);
	imagelabel->setPixmap(pix);
}
