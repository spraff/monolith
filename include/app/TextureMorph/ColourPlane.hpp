// Copyright 2013 Bernie Greenwood (spraff@gmail.com)
// Licenced under GPL Version 2

#ifndef COLOURPLANE_HPP
#define COLOURPLANE_HPP

#include <PixelMap.hpp>
#include <ScaledCanvas.hpp>

#include <memory>

namespace MathExpression
{
	class Term;
}



class ColourPlane : public ScaledCanvas
{
private:

	class Graph;

	ScaledCanvas :: Graph * create_graph () override;
};



class ColourPlane :: Graph : public PixelMap
{
Q_OBJECT

public:

	Graph ();

	~ Graph ();

	QWidget * create_controls () override;

	QRgb colour (MathExpression :: Variable :: Value) const override;

private:

	class XVariable;

	std :: unique_ptr <XVariable> m_x;

	const MathExpression :: Term * m_r;
	const MathExpression :: Term * m_g;
	const MathExpression :: Term * m_b;

private slots:

	void set_r (const MathExpression :: Term *);
	void set_g (const MathExpression :: Term *);
	void set_b (const MathExpression :: Term *);
};



#endif
