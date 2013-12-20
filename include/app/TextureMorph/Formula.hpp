#ifndef FORMULA_HPP
#define FORMULA_HPP

#include <MathExpression/Variable.h>

#include <QWidget>

#include <memory>

namespace MathExpression
{
	class Term;
}

class QLineEdit;
class QStyleOptionFrameV2;



class Formula : private QWidget
{
Q_OBJECT

public:

	Formula ();

	~ Formula ();

	QSize sizeHint () const override;

	void set_ok (bool);

	void set_error_at (unsigned);

	void set_error_text (const QString &);

	void set_variables (MathExpression :: Variable :: ValueMap *);

	operator QWidget * ();

	void setText (const QString &);

	QString text ();

signals:

	void set_term (const MathExpression :: Term *);

	void textChanged ();

public slots:

	void parse ();

private:

	MathExpression :: Variable :: ValueMap * m_variables;

	QLineEdit * m_text;

	std :: unique_ptr <MathExpression :: Term> m_term;

	void initStyleOption (QStyleOptionFrameV2 *) const;

	void contextMenuEvent (QContextMenuEvent *) override;

	QString caller_action_name ();

private slots:

	void textChanged (const QString &);

	void show_tree ();

	void simplify ();

	void add_constant ();

	void add_unary_function ();
};



#endif
