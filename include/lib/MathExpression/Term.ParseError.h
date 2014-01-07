// Copyright 2013 Bernie Greenwood (spraff@gmail.com)
// Licenced under GPL Version 2

#ifndef MATHEXPRESSION_TERM_PARSEERROR_H
#define MATHEXPRESSION_TERM_PARSEERROR_H

#include <MathExpression/Term.h>

#include <memory>
#include <sstream>
#include <exception>



/**
 * @brief
 *
 * @details
 **/
class MathExpression :: Term :: ParseError : public std :: exception
{
public:

	ParseError (const char * position);

	const char * position () const;

	const char * what () const noexcept;

	template <typename T>
	ParseError & operator << (const T &);

private:

	std :: shared_ptr <std :: ostringstream> m_what;

	const char * m_position;
};



template <typename T>
MathExpression :: Term :: ParseError &
MathExpression :: Term :: ParseError :: operator << (const T & x)
{
	(*m_what) << x;
	return *this;
}



#endif
