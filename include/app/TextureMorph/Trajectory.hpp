// Copyright 2013 Bernie Greenwood (spraff@gmail.com)
// Licenced under GPL Version 2

#ifndef TRAJECTORY_HPP
#define TRAJECTORY_HPP

#include <ScaledCanvas/Graph.hpp>



class Trajectory : public ScaledCanvas
{
private:

	class Graph;

	ScaledCanvas :: Graph * create_graph () override;
};



class Trajectory :: Graph : public ScaledCanvas :: Graph
{
Q_OBJECT

public:

	Graph ();

private:

	void paintEvent (QPaintEvent *) override;
};



#endif
