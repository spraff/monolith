// Copyright 2013 Bernie Greenwood (spraff@gmail.com)
// Licenced under GPL Version 2

#ifndef SCALEDCANVAS_HPP
#define SCALEDCANVAS_HPP

#include <QWidget>

namespace MathExpression
{
	class Term;
	class Variable;
}



class ScaledCanvas : public QWidget
{
Q_OBJECT

public:

	ScaledCanvas ();

	void init ();

	class Graph;

public slots:

	void set_term (const MathExpression :: Term *);

	void set_variable (MathExpression :: Variable *);

protected:

	Graph * m_graph;

	virtual Graph * create_graph () = 0;
};



#endif
