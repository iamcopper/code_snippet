#include <qapplication.h>
#include <qlabel.h>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QLabel *label = new QLabel("Hello Qt!", 0);
	app.setActiveWindow(label);

	/* 800x600 */
	label->resize(800, 600);
	label->show();

	/* full screen */
	//label->showFullScreen();

	/* hide cursor */
	//QCursor cursor(Qt::BlankCursor);
	//QApplication::setOverrideCursor(cursor);
	//QApplication::changeOverrideCursor(cursor);

	return app.exec();
}
