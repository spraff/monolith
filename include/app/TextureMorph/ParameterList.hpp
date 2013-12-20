#ifndef PARAMETERLIST_HPP
#define PARAMETERLIST_HPP

#include <MathExpression/Variable.ValueMap.h>

#include <QMap>
#include <QWidget>

class NamedParameter;

class QLineEdit;
class QPushButton;



class ParameterList
: public QWidget
, public MathExpression :: Variable :: ValueMap
{
Q_OBJECT

public:

	ParameterList ();

	MathExpression :: Variable * get (const char * n, const char * end = 0);

public slots:

	void create (const QString &);

signals:

	void changed ();

private:

	QLineEdit * m_new_name;

	QPushButton * m_create_button;

	QMap <QString, MathExpression :: Variable *> m_vars;

private slots:

	void new_name_changed (const QString &);

	void create ();

	void kill (const QString &);

	void update (const QString &, double);
};



#endif
