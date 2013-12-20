#include <NamedParameter.hpp>

#include <QtUtil/ParameterSlider.hpp>

#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPushButton>



namespace
{
	const QString ICON_CLOSE =
	":/trolltech/styles/commonstyle/images/standardbutton-cancel-16.png";
}



NamedParameter :: NamedParameter (const QString & name)
: m_name (new QLabel (name))
, m_slider (new QtUtil :: ParameterSlider)
{
	auto l = new QHBoxLayout (this);

	auto b = new QPushButton (QIcon (ICON_CLOSE), {});

	b -> setToolTip (tr ("Remove this parameter."));

	l -> addWidget (m_name);
	l -> addWidget (m_slider);
	l -> addWidget (b);

	connect (b, SIGNAL (clicked ()), this, SLOT (kill ()));

	connect (m_slider, SIGNAL (value_changed (double)),
		 this, SLOT (slider_changed (double)));
}



void
NamedParameter :: kill ()
{
	emit killed (m_name -> text ());

	deleteLater ();
}



void
NamedParameter :: slider_changed (double x)
{
	emit value_changed (m_name -> text (), x);
}



double
NamedParameter :: value ()
const
{
	return m_slider -> value ();
}
