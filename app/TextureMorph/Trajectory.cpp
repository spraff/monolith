// Copyright 2013 Bernie Greenwood (spraff@gmail.com)
// Licenced under GPL Version 2

#include <Trajectory.hpp>

#include <MathExpression/Term.h>

#include <QPainter>



ScaledCanvas :: Graph *
Trajectory :: create_graph ()
{
	return new Graph;
}



Trajectory :: Graph :: Graph ()
{
	setMouseTracking (true);
}



void
Trajectory :: Graph :: paintEvent (QPaintEvent *)
{
	QPainter p (this);

	auto t = term ();
	auto v = variable ();

	if (nullptr == t || nullptr == v)
	{
		p .fillRect (0, 0, width (), height (), Qt :: gray);

		return;
	}

	p .fillRect (0, 0, width (), height (), Qt :: white);

	p .drawLine (width () / 2, 0, width () / 2, height () - 1);
	p .drawLine (0, height () / 2, width () - 1, height () / 2);

	MathExpression :: Variable :: Value x = at ();

	QColor c = Qt :: blue;

	MathExpression :: Variable :: Value :: value_type
		step_size = scale () / qMin (width (), height ());

	for (int i = 0; i < 10000; ++i)
	{
		p .setPen (c);
		
		p .drawPoint (value_to_screen (x));

		v -> set_value (x);

		auto grad_x = t -> value ();

		x += grad_x * (step_size / std :: norm (grad_x));
	}
}
