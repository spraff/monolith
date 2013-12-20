#include <Term.ParseError.h>



MathExpression :: Term :: ParseError :: ParseError (const char * pos)
: m_what (new std :: ostringstream)
, m_position (pos)
{
}



const char *
MathExpression :: Term :: ParseError :: position ()
const
{
	return m_position;
}



const char *
MathExpression :: Term :: ParseError :: what ()
const noexcept
{
	return m_what -> str () .c_str ();
}
