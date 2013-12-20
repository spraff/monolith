#include <NamedParameter.hpp>
#include <ParameterList.hpp>

#include <QLineEdit>
#include <QPushButton>
#include <QValidator>
#include <QVBoxLayout>

#include <cassert>



namespace
{
	class Validator : public QValidator
	{
	public:

		QValidator :: State
		validate (QString & input, int &) const override
		{
			if (0 == input .size () || "X" == input)
				return QValidator :: Intermediate;

			for (auto c : input)
			{
				if (c < 'A' || c > 'Z')
					return QValidator :: Invalid;
			}

			return QValidator :: Acceptable;
		}
	};
}



ParameterList :: ParameterList ()
: m_new_name (new QLineEdit)
, m_create_button (new QPushButton (tr ("Create")))
{
	m_vars ["X"] = new MathExpression :: Variable ("X");

	auto l = new QVBoxLayout (this);

	auto w = new QWidget;

	l -> addWidget (w);

	auto hl = new QHBoxLayout (w);

	hl -> addWidget (m_new_name);

	hl -> addWidget (m_create_button);

	m_new_name -> setValidator (new Validator);

	m_create_button -> setEnabled (false);

	connect (m_new_name, SIGNAL (textChanged (const QString &)),
		 this, SLOT (new_name_changed (const QString &)));

	connect (m_create_button, SIGNAL (clicked ()), this, SLOT (create ()));
}



void
ParameterList :: new_name_changed (const QString & s)
{
	m_create_button -> setEnabled (false == m_vars .contains (s));
}



void
ParameterList :: create ()
{
	create (m_new_name -> text ());
}



void
ParameterList :: create (const QString & n)
{
	if (m_vars .contains (n))
		return;

	auto v = new MathExpression :: Variable (n .toStdString ());
	auto p = new NamedParameter (n);

	m_vars [n] = v;

	v -> set_value (p -> value ());

	connect (p, SIGNAL (killed (const QString &)),
		 this, SLOT (kill (const QString &)));

	connect (p, SIGNAL (value_changed (const QString &, double)),
		 this, SLOT (update (const QString &, double)));

	dynamic_cast <QVBoxLayout *> (layout ()) -> insertWidget (0, p);

	m_create_button -> setEnabled (false);

	emit changed ();
}



void
ParameterList :: kill (const QString & n)
{
	assert (m_vars .contains (n));

	delete m_vars [n];

	m_vars .remove (n);

	emit changed ();
}



void
ParameterList :: update (const QString & n, double v)
{
	assert (m_vars .contains (n));

	m_vars [n] -> set_value (v);

	emit changed ();
}



MathExpression :: Variable *
ParameterList :: get (const char * s, const char * e)
{
	int i = 0;

	for (auto & x : m_vars)
		i += (x == x);

	if (nullptr == e)
		e = s + strlen (s);

	assert (e > s);

	auto v = m_vars .find (QString (QByteArray (s, e - s)));

	if (m_vars .end () == v)
		return nullptr;
	else
		return *v;
}
