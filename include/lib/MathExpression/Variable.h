#ifndef MATHEXPRESSION_VARIABLE_H
#define MATHEXPRESSION_VARIABLE_H

#include <complex>
#include <string>



namespace MathExpression
{
	class Variable;
}



/**
 * @brief
 *
 * @details
 **/
class MathExpression :: Variable
{
public:

	typedef std :: complex <long double> Value;

	static const Value NaN;

	class ValueMap;

	Variable (std :: string && name);

	const std :: string & name () const;

	Value value () const;

	void set_value (Value);

private:

	std :: string m_name;
	Value m_value;
};




#endif

