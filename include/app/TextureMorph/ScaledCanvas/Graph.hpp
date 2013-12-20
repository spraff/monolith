#ifndef SCALEDCANVAS_GRAPH_HPP
#define SCALEDCANVAS_GRAPH_HPP

#include <ScaledCanvas.hpp>

#include <MathExpression/Variable.h>



class ScaledCanvas :: Graph : public QWidget
{
Q_OBJECT

public:

	Graph ();

	double scale () const;

	const MathExpression :: Term * term () const;

	MathExpression :: Variable * variable () const;

	MathExpression :: Variable :: Value screen_to_value (QPoint) const;

	QPoint value_to_screen (MathExpression :: Variable :: Value) const;

	virtual QWidget * create_controls ();

	MathExpression :: Variable :: Value at () const;

public slots:

	virtual void set_scale (double);

	void set_term (const MathExpression :: Term *);

	void set_variable (MathExpression :: Variable *);

private:

	double m_scale;

	MathExpression :: Variable :: Value m_at;

	const MathExpression :: Term * m_term;

	MathExpression :: Variable * m_var;

	void mouseMoveEvent (QMouseEvent *);
};



#endif
