#ifndef QTUTIL_PARAMETERSLIDER_HPP
#define QTUTIL_PARAMETERSLIDER_HPP

#include <QWidget>

class QDoubleSpinBox;
class QSlider;



namespace QtUtil
{
	class ParameterSlider;
}



class QtUtil :: ParameterSlider : public QWidget
{
Q_OBJECT

public:

	ParameterSlider ();

	double value () const;

	void fix_minimum (double);

public slots:

	void set_minimum (double);

	void set_maximum (double);

	void set_value (double);

signals:

	void value_changed (double);

private:

	QDoubleSpinBox * m_min;
	QDoubleSpinBox * m_max;
	QSlider * m_slider;

	double m_value;

private slots:

	void range_changed (double);

	void slider_changed (int);
};



#endif
