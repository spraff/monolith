#ifndef TEXTUREMORPH_HPP
#define TEXTUREMORPH_HPP

#include <PixelMap.hpp>
#include <ScaledCanvas.hpp>



class TextureMorph : public ScaledCanvas
{
private:

	class Graph;

	ScaledCanvas :: Graph * create_graph () override;
};



class TextureMorph :: Graph : public PixelMap
{
Q_OBJECT

public:

	Graph ();

	QRgb colour (MathExpression :: Variable :: Value) const override;

private:

	QImage m_image;

	void contextMenuEvent (QContextMenuEvent *);

private slots:

	void choose_image ();
};



#endif
