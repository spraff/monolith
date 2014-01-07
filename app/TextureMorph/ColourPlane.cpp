// Copyright 2013 Bernie Greenwood (spraff@gmail.com)
// Licenced under GPL Version 2

#include <ColourPlane.hpp>
#include <Formula.hpp>
#include <ParameterList.hpp>

#include <MathExpression/Term.h>
#include <MathExpression/Variable.ValueMap.h>

#include <QGridLayout>
#include <QLabel>



class ColourPlane :: Graph :: XVariable
: public MathExpression :: Variable :: ValueMap
{
public:

	XVariable ();

	MathExpression :: Variable m_x;

	MathExpression :: Variable * get (const char *, const char *);

	ParameterList * m_params = nullptr;
};



ColourPlane :: Graph :: XVariable :: XVariable ()
: m_x ("X")
{
}



MathExpression :: Variable *
ColourPlane :: Graph :: XVariable :: get (const char * n, const char * e)
{
	if (!e)
		e = n + strlen (n);

	return (1 == (e-n) && 0 == strncmp ("X", n, 1))
		? & m_x
		: m_params ? m_params -> get (n, e) : nullptr;
}



ColourPlane :: Graph :: Graph ()
: m_x (new Graph :: XVariable ())
, m_r (nullptr)
, m_g (nullptr)
, m_b (nullptr)
{
	setMinimumSize (100, 100);
}



ScaledCanvas :: Graph *
ColourPlane :: create_graph ()
{
	return new Graph;
}



ColourPlane :: Graph :: ~ Graph ()
{
}



QWidget *
ColourPlane :: Graph :: create_controls ()
{
	auto w = new QWidget;

	auto l = new QGridLayout (w);

	l -> addWidget (new QLabel (tr ("r=")), 0, 0);
	l -> addWidget (new QLabel (tr ("g=")), 1, 0);
	l -> addWidget (new QLabel (tr ("b=")), 2, 0);

	auto r = new Formula ();
	auto g = new Formula ();
	auto b = new Formula ();

	m_x -> m_params = new ParameterList ();

	l -> addWidget ((QWidget *) r, 0, 1);
	l -> addWidget ((QWidget *) g, 1, 1);
	l -> addWidget ((QWidget *) b, 2, 1);
	
	l -> addWidget (m_x -> m_params, 3, 0, 1, 2);

	connect ((QWidget *) r,
		 SIGNAL (set_term (const MathExpression :: Term *)),
		 this,
		 SLOT (set_r (const MathExpression :: Term *)));

	connect ((QWidget *) g,
		 SIGNAL (set_term (const MathExpression :: Term *)),
		 this,
		 SLOT (set_g (const MathExpression :: Term *)));

	connect ((QWidget *) b,
		 SIGNAL (set_term (const MathExpression :: Term *)),
		 this,
		 SLOT (set_b (const MathExpression :: Term *)));

	connect (m_x -> m_params, SIGNAL (changed ()), this, SLOT (update ()));

	connect (m_x -> m_params, SIGNAL (changed ()),
		 (QWidget *) r, SLOT (parse ()));

	connect (m_x -> m_params, SIGNAL (changed ()),
		 (QWidget *) g, SLOT (parse ()));

	connect (m_x -> m_params, SIGNAL (changed ()),
		 (QWidget *) b, SLOT (parse ()));

	r -> set_variables (m_x .get ());
	g -> set_variables (m_x .get ());
	b -> set_variables (m_x .get ());

	r -> setText ("real(X)");
	g -> setText ("imag(X)");
	b -> setText ("0");

	return w;
}



void
ColourPlane :: Graph :: set_r (const MathExpression :: Term * t)
{
	m_r = t;
}



void
ColourPlane :: Graph :: set_g (const MathExpression :: Term * t)
{
	m_g = t;
}



void
ColourPlane :: Graph :: set_b (const MathExpression :: Term * t)
{
	m_b = t;
}



QRgb
ColourPlane :: Graph :: colour (const MathExpression :: Variable :: Value v)
const
{
	if (nullptr == m_r || nullptr == m_g || nullptr == m_b)
		return Qt :: black;

	m_x -> m_x .set_value (v);

	auto r = m_r -> value ();
	auto g = m_g -> value ();
	auto b = m_b -> value ();

	if (0 != r .imag () || 0 != g .imag () || 0 != b .imag ())
		return Qt :: black;

	auto clamp = [] (MathExpression :: Variable :: Value x) -> double
	{
		return x .real () < 0 ? 0 : x .real () > 1 ? 1 : x .real ();
	};

	return qRgb (
		int (255 * clamp (r)),
		int (255 * clamp (g)),
		int (255 * clamp (b)));
}
