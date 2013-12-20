#ifndef PIXELMAP_HPP
#define PIXELMAP_HPP

#include <ScaledCanvas/Graph.hpp>

#include <QRgb>



class PixelMap : public ScaledCanvas :: Graph
{
Q_OBJECT

public:

	PixelMap ();

protected:

	virtual QRgb colour (MathExpression :: Variable :: Value) const = 0;

private:

	void paintEvent (QPaintEvent *) override;
};



#endif
