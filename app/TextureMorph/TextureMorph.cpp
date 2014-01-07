// Copyright 2013 Bernie Greenwood (spraff@gmail.com)
// Licenced under GPL Version 2

#include <TextureMorph.hpp>

#include <QContextMenuEvent>
#include <QFileDialog>
#include <QMenu>

#include <cassert>



namespace
{
	const QString DEFAULT_IMAGE
	= ":/trolltech/qmessagebox/images/qtlogo-64.png";
}



ScaledCanvas :: Graph *
TextureMorph :: create_graph ()
{
	return new Graph ();
}



TextureMorph :: Graph :: Graph ()
: m_image (DEFAULT_IMAGE)
{
}



QRgb
TextureMorph :: Graph :: colour (MathExpression :: Variable :: Value v)
const
{
	if (0 == m_image .width () || 0 == m_image .height ())
		return Qt :: black;

	if (false == std :: isnormal (v .real())
	 || false == std :: isnormal (v .imag ()))
		return Qt :: black;

	auto x = 1 + v .real () - std :: floor (v .real ());
	auto y = 1 + v .imag () - std :: floor (v .imag ());

	int i = ((long long int) (x * m_image .width  ())) % m_image .width ();
	int j = ((long long int) (y * m_image .height ())) % m_image .height ();

	assert (i >= 0);
	assert (j >= 0);
	assert (i < m_image .width ());
	assert (j < m_image .height ());

	return m_image .pixel (i, m_image .height () - 1 - j);
}



void
TextureMorph :: Graph :: contextMenuEvent (QContextMenuEvent * e)
{
	QMenu m;

	m .addAction (tr ("Open Image"), this, SLOT (choose_image ()));

	m .exec (e -> globalPos ());
}



void
TextureMorph :: Graph :: choose_image ()
{
	auto n = QFileDialog :: getOpenFileName (
		this,
		tr ("Open File"),
		QString (),
		tr ("Images (*.png *.jpg *.jpeg)"));

	if (0 == n .size ())
		return;

	QImage im;

	if (im .load (n))
	{
		m_image = im;
		update ();
	}
}
