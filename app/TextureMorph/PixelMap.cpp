// Copyright 2013 Bernie Greenwood (spraff@gmail.com)
// Licenced under GPL Version 2

#include <PixelMap.hpp>

#include <MathExpression/Term.h>

#include <QImage>
#include <QPainter>



PixelMap :: PixelMap ()
{
}



void
PixelMap :: paintEvent (QPaintEvent *)
{
	QPainter p (this);

	auto t = term ();
	auto v = variable ();

	if (nullptr == t || nullptr == v)
	{
		p .fillRect (0, 0, width (), height (), Qt :: gray);

		return;
	}

	const int w = width ();
	const int h = height ();

	QImage i (w, h, QImage :: Format_RGB32);

	for (int y = h-1; y >= 0; --y)
	{
		for (int x = 0; x < w; ++x)
		{
			v -> set_value (screen_to_value ({x, y}));

			i .setPixel (x, y, colour (t -> value ()));
		}
	}

	p .drawImage (0, 10, i);
}
