// Copyright 2013 Bernie Greenwood (spraff@gmail.com)
// Licenced under GPL Version 2

#include <MathExpression/Variable.ValueMap.h>

#include <cassert>

#include <stdexcept>



MathExpression :: Variable :: ValueMap :: ~ ValueMap ()
{
	for (auto & i : m_vars)
		delete i .second;
}



MathExpression :: Variable *
MathExpression :: Variable :: ValueMap :: find (const char * s, const char * e)
const
{
	return find ({s, e});
}



MathExpression :: Variable *
MathExpression :: Variable :: ValueMap :: find (const std :: string & name)
const
{
	auto i = m_vars .find (name);

	if (m_vars .end () == i)
		return nullptr;

	assert (nullptr != i -> second);

	return i -> second;
}



void
MathExpression :: Variable :: ValueMap :: add (Variable * v)
{
	if (nullptr == v)
		throw std :: invalid_argument (__PRETTY_FUNCTION__);

	if (find (v -> name ()))
		throw std :: logic_error (__PRETTY_FUNCTION__);

	m_vars [v -> name ()] = v;
}



MathExpression :: Variable *
MathExpression :: Variable :: ValueMap :: get (const char * s, const char * e)
{
	if (nullptr == e)
	{
		e = s;

		while (*e)
			++e;
	}

	auto v = find (s, e);

	if (nullptr == v)
	{
		v = new Variable ({s, e});
		add (v);
	}

	return v;
}
