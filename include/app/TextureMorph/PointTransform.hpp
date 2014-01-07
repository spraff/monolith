// Copyright 2013 Bernie Greenwood (spraff@gmail.com)
// Licenced under GPL Version 2

#ifndef POINTTRANSFORM_HPP
#define POINTTRANSFORM_HPP

#include <ScaledCanvas/Graph.hpp>



class PointTransform : public ScaledCanvas
{
private:

	class Graph;

	ScaledCanvas :: Graph * create_graph () override;
};



class PointTransform :: Graph : public ScaledCanvas :: Graph
{
Q_OBJECT

public:

	Graph ();

private:

	void mousePressEvent (QMouseEvent *) override;

	void paintEvent (QPaintEvent *) override;
};



#endif
