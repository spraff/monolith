#include <ColourPlane.hpp>
#include <Formula.hpp>
#include <MainWindow.hpp>
#include <ParameterList.hpp>
#include <PointTransform.hpp>
#include <TextureMorph.hpp>
#include <Trajectory.hpp>

#include <QAction>
#include <QComboBox>
#include <QDialog>
#include <QFile>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QInputDialog>
#include <QLabel>
#include <QMenuBar>
#include <QPushButton>
#include <QSpacerItem>
#include <QTabWidget>
#include <QTextEdit>



namespace
{
	const QString SAVE
	= ":/trolltech/styles/commonstyle/images/floppy-16.png";

	const QString DELETE
	= ":/trolltech/styles/commonstyle/images/standardbutton-cancel-16.png";

	const QString EXAMPLES
	= ":/trolltech/styles/commonstyle/images/viewdetailed-16.png";
}



MainWindow :: MainWindow ()
{
	auto w = new QWidget ();

	setCentralWidget (w);

	auto help = new QAction (tr ("Help"), this);
	{
		connect (help, SIGNAL (triggered ()), this, SLOT (help ()));

		help -> setShortcut (QKeySequence :: HelpContents);

		menuBar () -> addAction (help);
	}

	auto saves = new QWidget ();
	{
		auto l = new QHBoxLayout (saves);

		l -> addWidget (new QLabel (tr ("Saved Formulae")));

		m_saved = new QComboBox;

		connect (m_saved,
			 SIGNAL (currentIndexChanged (const QString &)),
			 this,
			 SLOT (select_formula (const QString &)));

		m_saved -> setSizePolicy (
			QSizePolicy :: MinimumExpanding,
			QSizePolicy :: Maximum);

		l -> addWidget (m_saved);

		auto add = [&]
		(
			const QString & icon,
			const QString & tip,
			const char * slot
		)
		{
			auto b = new QPushButton (QIcon (icon), QString ());

			l -> addWidget (b);

			connect (b, SIGNAL (clicked ()), this, slot);

			b -> setSizePolicy (
				QSizePolicy :: Maximum,
				QSizePolicy :: Maximum);

			b -> setToolTip (tip);
		};

		add (SAVE, tr ("Save"), SLOT (save_formula ()));
		add (DELETE, tr ("Delete"), SLOT (delete_formula ()));
		add (EXAMPLES, tr ("Examples"), SLOT (take_example ()));
	}

	m_formula = new Formula ();
	{
		((QWidget *) m_formula) -> setSizePolicy ({
			QSizePolicy :: Expanding,
			QSizePolicy :: Fixed});
	}

	MathExpression :: Variable * X = nullptr;

	auto plg = new QGroupBox (tr ("Parameters"));
	{
		auto l = new QGridLayout (plg);

		auto pl = new ParameterList ();

		pl -> create ("A");
		pl -> create ("B");
		pl -> create ("C");

		X = pl -> get ("X");

		l -> addWidget (pl, 0, 0);

		m_formula -> set_variables (pl);

		connect (pl, SIGNAL (changed ()),
			 (QWidget *) m_formula, SLOT (parse ()));
	}

	auto t = new QTabWidget ();
	{
		auto add = [&] (ScaledCanvas * w, const QString & s)
		{
			w -> init ();

			w -> set_variable (X);

			connect ((QWidget *) m_formula,
				 SIGNAL (set_term (
					const MathExpression :: Term *)),
				 w,
				 SLOT (set_term (
					const MathExpression :: Term *)));

			t -> addTab (w, s);
		};
		
		add (new PointTransform, tr ("Point Transform"));
		add (new Trajectory,     tr ("Trajectory"));
		add (new ColourPlane,    tr ("Colour Plane"));
		add (new TextureMorph,   tr ("Texture Morph"));
	}

	auto l = new QGridLayout (w);

	l -> addWidget (saves, 0, 0, 1, 2);
	l -> addWidget (new QLabel ("f(X)="), 1, 0);
	l -> addWidget ((QWidget *) m_formula, 1, 1);
	l -> addWidget (plg, 2, 0, 1, 2);
	
	l -> addItem (
		new QSpacerItem (
			0,
			0,
			QSizePolicy :: Minimum,
			QSizePolicy :: Maximum),
		3,
		0);

	l -> addWidget (t, 0, 2, 4, 1);

	show ();
}



void
MainWindow :: save_formula ()
{
	auto t = m_formula -> text ();

	if (m_saved -> findText (t) >= 0)
		return;

	if (0 == t .size ())
		return;

	m_saved -> addItem (t);
}



void
MainWindow :: delete_formula ()
{
	m_saved -> removeItem (m_saved -> currentIndex ());
}



void
MainWindow :: select_formula (const QString & s)
{
	m_formula -> setText (s);
}



void
MainWindow :: take_example ()
{
	bool ok = false;

	auto s = QInputDialog :: getItem (
		this,
		tr ("Example Formulae"),
		QString (),
		QStringList ()
			<< "ln(X)^2"
			<< "atan(X)/X^2"
			<< "tan(ln(X)^2*e^(Ai))"
			<< "X^(1+1/X)"
			<< "pi^(i*real(X^2)+imag(cos(X)))",
		0,
		false,
		& ok);

	if (false == ok)
		return;

	m_formula -> setText (s);
}



void
MainWindow :: help ()
{
	QDialog d;

	QGridLayout l (& d);

	QTextEdit e;

	QPushButton b (tr ("Close"));

	connect (& b, SIGNAL (clicked ()), & d, SLOT (close ()));

	l .addWidget (& e, 0, 0, 1, 3);
	l .addWidget (& b, 1, 1);

	d .setWindowTitle (tr ("Help"));
	e .setReadOnly (true);

	QFile text (":/help.txt");

	if (text .open (QIODevice :: ReadOnly | QIODevice :: Text))
		e .setText (text .readAll ());
	else
		e .setText (tr ("Error"));

	d .exec ();
}
