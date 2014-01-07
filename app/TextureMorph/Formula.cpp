// Copyright 2013 Bernie Greenwood (spraff@gmail.com)
// Licenced under GPL Version 2

#include <Formula.hpp>

#include <MathExpression/Term.h>
#include <MathExpression/Term.ParseError.h>
#include <MathExpression/Variable.ValueMap.h>

#include <QApplication>
#include <QContextMenuEvent>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMenu>
#include <QStyleOptionFrameV2>
#include <QTextEdit>



Formula :: Formula ()
: m_variables (nullptr)
, m_text (new QLineEdit)
{
	auto l = new QHBoxLayout (this);

	l -> addWidget (m_text);

	connect (this, SIGNAL (textChanged ()), this, SLOT (parse ()));

	connect (m_text,
		 SIGNAL (textChanged (const QString &)),
		 this,
		 SLOT (textChanged (const QString &)));
}



Formula :: ~ Formula ()
{
}



Formula :: operator QWidget * ()
{
	return this;
}



void
Formula :: setText (const QString & t)
{
	m_text -> setText (t);
}



QString
Formula :: text ()
{
	return m_text -> text ();
}



void
Formula :: initStyleOption (QStyleOptionFrameV2 * opt)
const
{
	opt -> initFrom (this);

	opt -> rect         = contentsRect ();
	opt -> midLineWidth = 0;
	opt -> state       |= QStyle :: State_Sunken;
	opt -> features     = QStyleOptionFrameV2 :: None;

	opt -> lineWidth = style () -> pixelMetric (
		QStyle :: PM_DefaultFrameWidth,
		opt,
		this);

#ifdef QT_KEYPAD_NAVIGATION
	if (hasEditFocus ())
		opt -> state |= QStyle :: State_HasEditFocus;
#endif
}



QSize
Formula :: sizeHint ()
const
{
	QStyleOptionFrameV2 opt;
	
	initStyleOption (& opt);

	QSize size (0, QFontMetrics (font ()) .height ());

	size = size .expandedTo (QApplication :: globalStrut ());

	size = style () -> sizeFromContents (
		QStyle :: CT_LineEdit,
		& opt,
		size,
		this);

	return size;
}



void
Formula :: set_ok (bool ok)
{
	if (ok)
		setStyleSheet ("color: green");
	else
		setStyleSheet ("color: red");
}



void
Formula :: set_error_at (unsigned)
{
}



void
Formula :: set_error_text (const QString & msg)
{
	setToolTip (msg);
}



void
Formula :: parse ()
{
	const auto expression = text () .toLatin1 ();

	if (nullptr == m_variables)
	{
		set_ok (false);
		
		set_error_at (expression .size ());
		
		set_error_text (tr ("No variables assigned."));

		emit set_term (nullptr);

		return;
	}

	m_term .reset ();

	try
	{
		m_term .reset (new MathExpression :: Term (
			MathExpression :: Term :: parse (
				expression .constData (),
				m_variables)));

		emit set_term (m_term .get ());

		set_ok (true);
	}
	catch (const MathExpression :: Term :: ParseError & e)
	{
		set_ok (false);
		
		set_error_at (
			e .position () - expression .constData ());

		set_error_text (e .what ());

		emit set_term (nullptr);
	}
}



void
Formula :: set_variables (MathExpression :: Variable :: ValueMap * m)
{
	m_variables = m;

	parse ();
}



void
Formula :: contextMenuEvent (QContextMenuEvent * e)
{
	QMenu m;

	if (m_term)
	{
		m .addAction (
			tr ("Show Parse Tree"),
			this,
			SLOT (show_tree ()));

		m .addAction (
			tr ("Simplify"),
			this,
			SLOT (simplify ()));
	}

	struct Arg
	{
		QMenu * m;
		QWidget * self;
	};

	QMenu funcs (tr ("Insert Function"));
	QMenu consts (tr ("Insert Constant"));

	Arg c_arg = {& consts, this};
	Arg f_arg = {& funcs, this};

	MathExpression :: Term :: list_constants (
		[] (const char * name, void * a)
		{
			auto arg = static_cast <Arg *> (a);

			arg -> m -> addAction (
				name,
				arg -> self,
				SLOT (add_constant ()));
		},
		& c_arg);

	MathExpression :: Term :: list_unary_functions (
		[] (const char * name, void * a)
		{
			auto arg = static_cast <Arg *> (a);

			arg -> m -> addAction (
				name,
				arg -> self,
				SLOT (add_unary_function ()));
		},
		& f_arg);

	m .addMenu (& consts);
	m .addMenu (& funcs);

	m .exec (mapToGlobal (e -> pos ()));
}



QString
Formula :: caller_action_name ()
{
	if (nullptr == sender ())
		return {};

	auto a = dynamic_cast <QAction *> (sender ());

	if (nullptr == a)
		return {};

	return a -> text ();
}



void
Formula :: add_constant ()
{
	setText (text () + caller_action_name ());
}



void
Formula :: add_unary_function ()
{
	setText (text () + caller_action_name () + "()");
}



void
Formula :: simplify ()
{
	if (m_term)
		m_term -> simplify ();
}


void
Formula :: show_tree ()
{
	if (! m_term)
		return;

	std :: ostringstream s;
	s << *m_term;

	auto w = new QTextEdit ();

	w -> setPlainText (QString :: fromStdString (s .str ()));
	
	w -> show ();
}



void
Formula :: textChanged (const QString &)
{
	emit textChanged ();
}
