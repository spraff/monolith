#include <ScaledCanvas/Graph.hpp>

#include <MathExpression/Term.h>

#include <QMouseEvent>
#include <QToolTip>



ScaledCanvas :: Graph :: Graph ()
: m_scale (1)
, m_at (0, 0)
, m_term (nullptr)
, m_var (nullptr)
{
	setSizePolicy (QSizePolicy :: Expanding, QSizePolicy :: Expanding);

	setMinimumSize (300, 300);
}



const MathExpression :: Term *
ScaledCanvas :: Graph :: term ()
const
{
	return m_term;
}



MathExpression :: Variable :: Value
ScaledCanvas :: Graph :: at ()
const
{
	return m_at;
}



void
ScaledCanvas :: Graph :: set_term (const MathExpression :: Term * t)
{
	m_term = t;

	update ();
}



double
ScaledCanvas :: Graph :: scale ()
const
{
	return m_scale;
}



void
ScaledCanvas :: Graph :: set_scale (double x)
{
	if (x < 0)
		return;

	m_scale = x;

	update ();
}



void
ScaledCanvas :: Graph :: set_variable (MathExpression :: Variable * v)
{
	m_var = v;
}



MathExpression :: Variable *
ScaledCanvas :: Graph :: variable ()
const
{
	return m_var;
}



MathExpression :: Variable :: Value
ScaledCanvas :: Graph :: screen_to_value (QPoint p)
const
{
	p = QPoint (p .x (), height () - 1 - p .y ());
	p = QPoint (p .x () - width () / 2, p .y () - height () / 2);

	auto s = 2 * scale () / qMin (width (), height ());

	return {p .x () * s, p .y () * s};
}



QPoint
ScaledCanvas :: Graph :: value_to_screen (MathExpression :: Variable :: Value v)
const
{
	auto s = 2 * scale () / qMin (width (), height ());

	QPoint p (
		v .real () / s + width () / 2,
		v .imag () / s + height () / 2);

	return {p .x (), height () - 1 - p .y ()};
}



QWidget *
ScaledCanvas :: Graph :: create_controls ()
{
	return nullptr;
}



void
ScaledCanvas :: Graph :: mouseMoveEvent (QMouseEvent * e)
{
	m_at = screen_to_value (e -> pos ());

	auto t = term ();
	auto v = variable ();

	if (t && v)
	{
		v -> set_value (m_at);

		auto x = t -> value ();

		QToolTip :: showText (
			e -> globalPos (),
			QString ("%1 + %2i => %3 + %4i")
				.arg ((float) m_at .real ())
				.arg ((float) m_at .imag ())
				.arg ((float) x .real ())
				.arg ((float) x .imag ()));
	}

	else
	{
		QToolTip :: showText (
			e -> globalPos (),
			QString ("%1 + %2i")
				.arg ((float) m_at .real ())
				.arg ((float) m_at .imag ()));
	}

	update ();
}
