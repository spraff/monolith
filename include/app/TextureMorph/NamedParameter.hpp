#ifndef NAMEDPARAMETER_HPP
#define NAMEDPARAMETER_HPP

#include <QWidget>

namespace QtUtil
{
	class ParameterSlider;
}

class QLabel;



class NamedParameter : public QWidget
{
Q_OBJECT

public:

	NamedParameter (const QString & name);

	double value () const;

signals:

	void killed (const QString & name);

	void value_changed (const QString & name, double value);

private:

	QLabel * m_name;
	
	QtUtil :: ParameterSlider * m_slider;

private slots:

	void kill ();

	void slider_changed (double);
};



#endif
