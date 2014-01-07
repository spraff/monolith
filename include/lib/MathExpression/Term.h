// Copyright 2013 Bernie Greenwood (spraff@gmail.com)
// Licenced under GPL Version 2

#ifndef MATHEXPRESSION_TERM_H
#define MATHEXPRESSION_TERM_H

#include <MathExpression/Variable.h>

#include <iosfwd>
#include <memory>
#include <vector>



namespace MathExpression
{
	class Term;
}



/**
 * @brief
 *
 * @details
 **/
class MathExpression :: Term
{
public:

	typedef MathExpression :: Variable :: Value Constant;

	typedef std :: unique_ptr <Term> Ptr;

	typedef Constant (* UnaryFunction)  (const Constant &);

	typedef Constant (* BinaryFunction) (const Constant &, const Constant&);

	enum class Type
	{
		SUM,
		PRODUCT,
		CONSTANT,
		VARIABLE,
		UNARY_FUNCTION,
		BINARY_FUNCTION
	};

	//! Type is SUM
	Term ();

	//! Type is CONSTANT
	Term (Constant);

	//! Type is VARIABLE. Variable must outlive the term.
	Term (const MathExpression :: Variable *);

	//! Type is UNARY_FUNCTION
	Term (UnaryFunction, Ptr &&);

	//! Type is BINARY_FUNCTION
	Term (BinaryFunction, Ptr &&, Ptr &&);

	//! Calls variables->establish each time a variable is found.
	static Term parse (
		const char * expression,
		MathExpression :: Variable :: ValueMap * variables);

	class ParseError;

	Constant value () const;

	Term (Term &&)      = default;
	Term (const Term &) = delete;

	Term & operator = (Term &&)      = default;
	Term & operator = (const Term &) = delete;

	//! Eliminates redundant terms, leaving the value unchanged.
	void simplify ();

	void print_formula (std :: ostream &) const;

	void print_tree (
		std :: ostream &,
		const char * tab = nullptr,
		unsigned depth = 0)
	const;

	typedef void (* ListCallback) (const char *, void *);

	static void list_constants (ListCallback, void * argument);

	static void list_unary_functions (ListCallback, void * argument);

private:

	Type m_type;

	//! Constant for CONSTANT, coefficient for BRACKET
	Constant m_value;

	//! Used with this term is within a SUM or PRODUCT
	bool m_invert_in_group = false;

	//! Subterms for SUM and PRODUCT, arguments to *ARY_FUNCTION.
	std :: vector <Ptr> m_terms;

	//! For power
	const MathExpression :: Variable * m_var = nullptr;

	UnaryFunction m_unary_function = nullptr;

	BinaryFunction m_binary_function = nullptr;

	static Term parse_sum (
		const char * &,
		MathExpression :: Variable :: ValueMap *,
		unsigned depth);

	static Term parse_product (
		const char * &,
		MathExpression :: Variable :: ValueMap *,
		unsigned depth);

	static Term parse_basic (
		const char * &,
		MathExpression :: Variable :: ValueMap *,
		unsigned depth);

	static Term parse_single (
		const char * &,
		MathExpression :: Variable :: ValueMap *,
		unsigned depth);

	static Term start_group (Type, Term && first_term);
	
	void print_formula (std :: ostream &, bool first) const;
};



std :: ostream &
operator << (std :: ostream &, const MathExpression :: Term &);



#endif

