#include <Variable.h>

#include <limits>


	int foo = 1;

namespace
{

	const auto real_NaN = std :: numeric_limits
	<
		MathExpression :: Variable :: Value :: value_type
	> :: quiet_NaN ();
}



const MathExpression :: Variable :: Value
MathExpression :: Variable :: NaN (real_NaN, real_NaN);



MathExpression :: Variable :: Variable (std :: string && name)
: m_name (name)
, m_value (NaN)
{
}



const std :: string &
MathExpression :: Variable :: name ()
const
{
	return m_name;
}



MathExpression :: Variable :: Value
MathExpression :: Variable :: value ()
const
{
	return m_value;
}



void
MathExpression :: Variable :: set_value (Value v)
{
	m_value = v;
}
