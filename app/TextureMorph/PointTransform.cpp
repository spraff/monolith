#include <PointTransform.hpp>

#include <MathExpression/Term.h>
#include <MathExpression/Variable.h>

#include <QPainter>



ScaledCanvas :: Graph *
PointTransform :: create_graph ()
{
	return new Graph ();
}



PointTransform :: Graph :: Graph ()
{
	setMouseTracking (true);
}



void
PointTransform :: Graph :: mousePressEvent (QMouseEvent *)
{
	auto t = term ();
	auto v = variable ();

	if (t && v)
	{
		v -> set_value (at ());

		auto x = t -> value ();

		x = x;
	}
}



void
PointTransform :: Graph :: paintEvent (QPaintEvent *)
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

	p .setBrush (Qt :: blue);

	p .drawLine (width () / 2, 0, width () / 2, height () - 1);
	p .drawLine (0, height () / 2, width () - 1, height () / 2);

	p .setBrush (Qt :: red);

	v -> set_value (at ());
	
	p .drawLine (value_to_screen (at ()), value_to_screen (t -> value ()));
}
