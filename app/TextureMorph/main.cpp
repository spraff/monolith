#include <MathExpression/Term.h>
#include <MainWindow.hpp>

#include <QApplication>



int
main (int argc, char ** argv)
{
	QApplication app (argc, argv);

	MainWindow mw;

	return app .exec ();
}