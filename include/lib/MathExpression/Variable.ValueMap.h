// Copyright 2013 Bernie Greenwood (spraff@gmail.com)
// Licenced under GPL Version 2

#ifndef MATHEXPRESSION_VARIABLE_VALUEMAP_H
#define MATHEXPRESSION_VARIABLE_VALUEMAP_H

#include <MathExpression/Variable.h>

#include <map>
#include <string>



/**
 * @brief
 *
 * @details
 **/
class MathExpression :: Variable :: ValueMap
{
public:

	//! Return the variable, creating it if it is new, or null if forbidden.
	virtual Variable * get (const char * name, const char * end = nullptr);

	virtual ~ ValueMap ();

protected:

	Variable * find (const char * name, const char * name_end) const;

	Variable * find (const std :: string &) const;

	//! Throws if a variable with this name already exists.
	void add (Variable *);

	void remove (Variable *);

private:

	std :: map <std :: string, Variable *> m_vars;
};




#endif

