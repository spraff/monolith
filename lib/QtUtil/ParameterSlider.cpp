#include <QtUtil/ParameterSlider.hpp>

#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QSlider>



namespace
{
	double MAX = 1000;
	double MIN = -MAX;
	int TICKS = 10000;
}



QtUtil :: ParameterSlider :: ParameterSlider ()
: m_min (new QDoubleSpinBox)
, m_max (new QDoubleSpinBox)
, m_slider (new QSlider (Qt :: Horizontal))
{
	auto l = new QHBoxLayout (this);

	l -> addWidget (m_min);
	l -> addWidget (m_slider);
	l -> addWidget (m_max);

	m_min -> setRange (MIN, MAX);
	m_max -> setRange (MIN, MAX);
	m_slider -> setRange (0, TICKS);

	m_min -> setValue (-1);
	m_max -> setValue (1);
	m_slider -> setValue (TICKS / 2);

	m_slider -> setMinimumWidth (100);

	m_value = 0;

	connect (m_min, SIGNAL (valueChanged (double)),
		 this, SLOT (range_changed (double)));

	connect (m_max, SIGNAL (valueChanged (double)),
		 this, SLOT (range_changed (double)));

	connect (m_slider, SIGNAL (valueChanged (int)),
		 this, SLOT (slider_changed (int)));

	m_value = value ();
}



double
QtUtil :: ParameterSlider :: value ()
const
{
	return m_value;
}



void
QtUtil :: ParameterSlider :: range_changed (double)
{
	if (m_min -> value () == m_max -> value ())
		// Position of the slider is irrelevant.
		return;

	set_value (m_value);
}



void
QtUtil :: ParameterSlider :: slider_changed (int pos)
{
	m_value = m_min -> value ()
		+ ((m_max -> value () - m_min -> value ()) * pos) / TICKS;

	setToolTip (QString :: number (m_value));

	emit value_changed (m_value);
}



void
QtUtil :: ParameterSlider :: fix_minimum (double x)
{
	set_minimum (x);

	m_min -> setEnabled (false);
}



void
QtUtil :: ParameterSlider :: set_minimum (double x)
{
	m_min -> setValue (x);
}



void
QtUtil :: ParameterSlider :: set_maximum (double x)
{
	m_max -> setValue (x);
}



void
QtUtil :: ParameterSlider :: set_value (double x)
{
	if (x < m_min -> value ())
		x = m_min -> value ();

	if (x > m_max -> value ())
		x = m_max -> value ();

	m_value = x;

	int pos = TICKS * (m_value - m_min -> value ())
		/ (m_max -> value () - m_min -> value ());

	m_slider -> setValue (pos);
}
