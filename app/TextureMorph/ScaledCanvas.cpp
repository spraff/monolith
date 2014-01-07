// Copyright 2013 Bernie Greenwood (spraff@gmail.com)
// Licenced under GPL Version 2

#include <ScaledCanvas.hpp>
#include <ScaledCanvas/Graph.hpp>

#include <QtUtil/ParameterSlider.hpp>

#include <QVBoxLayout>



ScaledCanvas :: ScaledCanvas ()
: m_graph (nullptr)
{
}



void
ScaledCanvas :: init ()
{
	auto l = new QVBoxLayout (this);

	m_graph = create_graph ();

	auto s = new QtUtil :: ParameterSlider;

	s -> fix_minimum (0);
	s -> set_maximum (10);
	s -> set_value (1);

	connect (s, SIGNAL (value_changed (double)),
		 m_graph, SLOT (set_scale (double)));

	m_graph -> set_scale (1);

	l -> addWidget (m_graph);

	l -> addWidget (s);

	if (auto c = m_graph -> create_controls ())
		l -> addWidget (c);
}



void
ScaledCanvas :: set_term (const MathExpression :: Term * t)
{
	m_graph -> set_term (t);
}



void
ScaledCanvas :: set_variable (MathExpression :: Variable * v)
{
	m_graph -> set_variable (v);
}
