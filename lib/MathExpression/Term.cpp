#include <Term.h>
#include <Term.ParseError.h>
#include <Variable.ValueMap.h>

#include <algorithm>
#include <array>
#include <stdexcept>

#include <cassert>
#include <cstring>



namespace
{
	namespace Math
	{
		typedef MathExpression :: Term :: Constant C;

#define FUNC(X) C X (const C & x) {return std :: X (x);}
		FUNC(real)
		FUNC(imag)
		FUNC(abs)
		FUNC(arg)
		FUNC(norm)
		FUNC(conj)
		FUNC(proj)
//		FUNC(polar)
		FUNC(acos)
		FUNC(asin)
		FUNC(atan)
		FUNC(acosh)
		FUNC(asinh)
		FUNC(atanh)
		FUNC(cos)
		FUNC(cosh)
		FUNC(exp)
//		FUNC(ln)
//		FUNC(log)
		FUNC(sin)
		FUNC(sqrt)
		FUNC(tan)
		FUNC(tanh)

		C polar (const C & x)
		{
			return std :: polar (x .real(), x .imag ());
		}

		C ln (const C & x)    {return std :: log (x);}
		C log (const C & x)   {return std :: log10 (x);}
#undef FUNC

		C pow (const C & a, const C & b) {return std :: pow (a, b);}

		C multiply (const C & a, const C & b) {return a * b;}
	}

	const std :: string ERROR_EARLY_END
		= "Ended too early.";

	const std :: string ERROR_BRACKET
		= "Extra ')' without '('.";

	const std :: string ERROR_UNEXPECTED
		= "Unexpected symbol.";

	const std :: string ERROR_NOT_A_NUMBER
		= "Not a number.";

	const std :: string ERROR_IMAGINARY
		= "Imaginary unit expected.";

	const std :: string ERROR_VARIABLE
		= " is not an allowed variable name.";

	const std :: string ERROR_NAME
		= " is not a function or constant.";

	const std :: string ERROR_FUNCTION_BRACKET
		= "Function expects '(' here.";
}



namespace
{
	void skip_whitespace (const char * & ptr)
	{
		while (isspace (*ptr))
			++ptr;
	}



	typedef MathExpression :: Term :: UnaryFunction UF;
	typedef MathExpression :: Term :: BinaryFunction BF;



	std :: array <std :: pair <const char *, UF>, 23> unary_functions
	{{
#define FUNC(X) std :: make_pair (#X, Math :: X)
		FUNC(real),
		FUNC(imag),
		FUNC(abs),
		FUNC(arg),
		FUNC(norm),
		FUNC(conj),
		FUNC(proj),
		FUNC(polar),
		FUNC(acos),
		FUNC(asin),
		FUNC(atan),
		FUNC(acosh),
		FUNC(asinh),
		FUNC(atanh),
		FUNC(cos),
		FUNC(cosh),
		FUNC(exp),
		FUNC(ln),
		FUNC(log),
		FUNC(sin),
		FUNC(sqrt),
		FUNC(tan),
		FUNC(tanh)
#undef FUNC
	}};



	std :: array <std :: pair <const char *, BF>, 2> binary_functions
	{{
		std :: make_pair ("pow", Math :: pow),
		std :: make_pair ("multiply", Math :: multiply)
	}};



	typedef MathExpression :: Term :: Constant C;



	std :: array <std :: pair <const char *, C>, 2> constants
	{{
#ifdef __USE_GNU
		std :: make_pair ("e",  C (M_El, 0)),
		std :: make_pair ("pi", C (M_PIl, 0))
#else
		std :: make_pair ("e",  C (M_E, 0)),
		std :: make_pair ("pi", C (M_PI, 0))
#endif
	}};



	bool
	str_equal (const char * s1, const char * s2_start, const char * s2_end)
	{
		assert (s2_end >= s2_start);

		return strlen (s1) == size_t (s2_end - s2_start)
		    && 0 == strncmp (s1, s2_start, s2_end - s2_start);
	}



	MathExpression :: Term :: UnaryFunction
	unary (const char * s, const char * e)
	{
		for (auto & f : unary_functions)
		{
			if (str_equal (f .first, s, e))
				return f .second;
		}

		return nullptr;
	}



	MathExpression :: Term :: Constant
	constant (const char * s, const char * e)
	{
		for (auto & c : constants)
		{
			if (str_equal (c .first, s, e))
				return c .second;
		}

		return {0, 0};
	}



	const char * unary_name (MathExpression :: Term :: UnaryFunction func)
	{
		for (auto & f : unary_functions)
		{
			if (f .second == func)
				return f .first;
		}

		return "UNKNOWN UNARY";
	}



	const char * binary_name (MathExpression :: Term :: BinaryFunction func)
	{
		for (auto & f : binary_functions)
		{
			if (f .second == func)
				return f .first;
		}

		return "UNKNOWN BINARY";
	}



	const char * const_name (MathExpression :: Term :: Constant c)
	{
		for (auto & f : constants)
		{
			if (c == f .second)
				return f .first;
		}

		return nullptr;
	}



	bool ends_term (char c)
	{
		return isspace (c) || '\0' == c || '^' == c
		 || '+' == c || '-' == c
		 || '*' == c || '/' == c
		 || '(' == c || ')' == c;
	}
}



void
MathExpression :: Term :: list_constants (ListCallback f, void * arg)
{
	if (! f)
		return;

	for (auto & c : constants)
		f (c .first, arg);
}



void
MathExpression :: Term :: list_unary_functions (ListCallback f, void * arg)
{
	if (! f)
		return;

	for (auto & c : unary_functions)
		f (c .first, arg);
}



MathExpression :: Term :: Term ()
: m_type (Type :: SUM)
, m_value (Variable :: NaN)
{
}



MathExpression :: Term :: Term (Constant c)
: m_type (Type :: CONSTANT)
, m_value (c)
{
}



MathExpression :: Term :: Term (const Variable * v)
: m_type (Type :: VARIABLE)
, m_value (Variable :: NaN)
, m_var (v)
{
	if (nullptr == v)
		throw std :: invalid_argument (__PRETTY_FUNCTION__);
}



MathExpression :: Term :: Term (UnaryFunction f, Ptr && p)
: m_type (Type :: UNARY_FUNCTION)
, m_value (Variable :: NaN)
{
	m_unary_function = f;

	m_terms .emplace_back (std :: move (p));
}



MathExpression :: Term :: Term (BinaryFunction f, Ptr && p1, Ptr && p2)
: m_type (Type :: BINARY_FUNCTION)
, m_value (Variable :: NaN)
{
	m_binary_function = f;

	m_terms .emplace_back (std :: move (p1));
	m_terms .emplace_back (std :: move (p2));
}



MathExpression :: Term :: Constant
MathExpression :: Term :: value ()
const
{
	switch (m_type)
	{
	case Type :: SUM:
		{
			Constant n = {0, 0};

			for (auto & t : m_terms)
			{
				if (t -> m_invert_in_group)
					n -= t -> value ();
				else
					n += t -> value ();
			}

			return n;
		}

	case Type :: PRODUCT:
		{
			Constant n = {1, 0};

			for (auto & t : m_terms)
			{
				if (t -> m_invert_in_group)
					n /= t -> value ();
				else
					n *= t -> value ();
			}

			return n;
		}

	case Type :: CONSTANT:
		return m_value;

	case Type :: VARIABLE:
		return m_var -> value ();

	case Type :: UNARY_FUNCTION:

		assert (m_unary_function);
		assert (1 == m_terms .size ());

		return m_unary_function (m_terms [0] -> value ());

	case Type :: BINARY_FUNCTION:

		assert (m_binary_function);
		assert (2 == m_terms .size ());

		return m_binary_function (
			m_terms [0] -> value (),
			m_terms [1] -> value ());
	};

	throw std :: logic_error (__PRETTY_FUNCTION__);
}



MathExpression :: Term
MathExpression :: Term :: parse (const char * e, Variable :: ValueMap * v)
{
	if (nullptr == v)
		throw std :: invalid_argument (__PRETTY_FUNCTION__);

	if (nullptr == e)
		return Term ();

	try
	{
		return parse_sum (e, v, 0);
	}
	catch (const ParseError & err)
	{
		throw;
	}
	catch (const std :: exception & err)
	{
		throw ParseError (e) << "External error: " << err .what ();
	}
	catch (...)
	{
		throw ParseError (e) << "Unknown error.";
	}
}



MathExpression :: Term
MathExpression :: Term :: start_group (Type type, Term && t)
{
	Term new_t;

	new_t .m_type = type;

	new_t .m_terms .emplace_back (Ptr (new Term (std :: move (t))));

	return new_t;
}



MathExpression :: Term
MathExpression :: Term :: parse_sum
(
	const char * & e,
	Variable :: ValueMap * v,
	unsigned depth
)
{
	auto skip = [&] ()
	{
		skip_whitespace (e);

		if ('\0' == *e)
			throw ParseError (e) << ERROR_EARLY_END;
	};

	skip ();

	Term t = parse_product (e, v, depth);

	skip_whitespace (e);

	// Stop at the end only if we're not within a bracket.
	if ('\0' == *e)
	{
		if (0 == depth)
			return t;
		else
			throw ParseError (e) << ERROR_EARLY_END;
	}

	// Stop at a bracket only if we're within one.
	if (')' == *e)
	{
		if (0 == depth)
			throw ParseError (e) << ERROR_BRACKET;
		else
		{
			++e;
			return t;
		}
	}

	if ('+' == *e || '-' == *e)
	{
		bool invert = ('-' == *e);

		++e;
		skip ();

		t = start_group (Type :: SUM, std :: move (t));

		while (true)
		{
			t .m_terms .emplace_back (
				Ptr (new Term (parse_product (e, v, depth))));

			t .m_terms .back () -> m_invert_in_group = invert;

			skip_whitespace (e);

			// Stop at the end only if we're not within a bracket.
			if ('\0' == *e)
			{
				if (0 == depth)
					return t;
				else
					throw ParseError (e) << ERROR_EARLY_END;
			}

			// Stop at a bracket only if we're within one.
			if (')' == *e)
			{
				if (0 == e)
					throw ParseError (e) << ERROR_BRACKET;
				else
				{
					++e;
					return t;
				}
			}

			if ('+' == *e || '-' == *e)
			{
				invert = ('-' == *e);

				++e;
				skip ();
			}
			else
				throw ParseError (e) << ERROR_UNEXPECTED;
		}
	}
	else
		throw ParseError (e) << ERROR_UNEXPECTED;
}



MathExpression :: Term
MathExpression :: Term :: parse_product
(
	const char * & e,
	Variable :: ValueMap * v,
	unsigned depth
)
{
	auto skip = [&] ()
	{
		skip_whitespace (e);

		if ('\0' == *e)
			throw ParseError (e) << ERROR_EARLY_END;
	};

	skip ();

	Term t = parse_basic (e, v, depth);

	skip_whitespace (e);

	// Stop at the end only if we're not within a bracket.
	if ('\0' == *e)
	{
		if (0 == depth)
			return t;
		else
			throw ParseError (e) << ERROR_EARLY_END;
	}

	// Stop at a bracket only if we're within one.
	if (')' == *e)
	{
		if (0 == depth)
			throw ParseError (e) << ERROR_BRACKET;
		else
			return t;
	}

	// Stop after the sequence of products within a sum
	if ('+' == *e || '-' == *e)
		return t;

	if ('*' == *e || '/' == *e)
	{
		bool invert = ('/' == *e);

		++e;
		skip ();

		t = start_group (Type :: PRODUCT, std :: move (t));

		while (true)
		{
			t .m_terms .emplace_back (
				Ptr (new Term (parse_basic (e, v, depth))));

			t .m_terms .back () -> m_invert_in_group = invert;

			skip_whitespace (e);

			// Stop at the end only if we're not within a bracket.
			if ('\0' == *e)
			{
				if (0 == depth)
					return t;
				else
					throw ParseError (e) << ERROR_EARLY_END;
			}

			// Stop at a bracket only if we're within one.
			if (')' == *e)
			{
				if (0 == e)
					throw ParseError (e) << ERROR_BRACKET;
				else
					return t;
			}

			// Stop after the sequence of products within a sum
			if ('+' == *e || '-' == *e)
				return t;

			if ('*' == *e || '/' == *e)
			{
				invert = ('/' == *e);

				++e;
				skip ();
			}
			else
				throw ParseError (e) << ERROR_UNEXPECTED;
		}
	}
	else
		throw ParseError (e) << ERROR_UNEXPECTED;
}



MathExpression :: Term
MathExpression :: Term :: parse_basic
(
	const char * & e,
	MathExpression :: Variable :: ValueMap * v,
	unsigned depth
)
{
	auto skip = [&] ()
	{
		skip_whitespace (e);

		if ('\0' == *e)
			throw ParseError (e) << ERROR_EARLY_END;
	};

	skip ();

	Term t = parse_single (e, v, depth);

	skip_whitespace (e);

	// Stop at the end only if we're not within a bracket.
	if ('\0' == *e)
	{
		if (0 == depth)
			return t;
		else
			throw ParseError (e) << ERROR_EARLY_END;
	}

	// Stop at a bracket only if we're within one.
	if (')' == *e)
	{
		if (0 == e)
			throw ParseError (e) << ERROR_BRACKET;
		else
			return t;
	}

	if ('^' == *e)
	{
		++e;
		skip ();

		return
		{
			Math :: pow,
			Ptr (new Term (std :: move (t))),
			Ptr (new Term (parse_single (e, v, depth)))
		};
	}
	else
		return t;
}



MathExpression :: Term
MathExpression :: Term :: parse_single
(
	const char * & e,
	MathExpression :: Variable :: ValueMap * v,
	unsigned depth
)
{
	Constant coefficient = ('-' == *e) ? -1 : 1;

	// If we don't return early with a complex expression, this must become
	// true or we didn't parse anything.
	bool number = false;

	if ('-' == *e)
	{
		++e;

		// Look ahead to see if this term is -i
		if ('i' == *e && ends_term (e[1]))
			number = true;
	}

	if ('.' == *e || isdigit (*e))
	{
		char * end = const_cast <char *> (e);

		errno = 0;

		coefficient *= strtold (e, & end);

		if (errno)
			throw ParseError (e) << ERROR_NOT_A_NUMBER;

		e = end;
		number = true;
	}

	auto check_i_suffix = [&] ()
	{
		if ('i' == *e)
		{
			++e;

			if (ends_term (*e))
				coefficient *= Constant (0, 1);
			else
				throw ParseError (e) << ERROR_IMAGINARY;
		}
	};

	auto finish = [&] (Term && t) -> Term
	{
		check_i_suffix ();

		if (coefficient == Constant (1))
			return std :: move (t);

		// This term is the coefficient multiplied by a following term.
		return
		{
			Math :: multiply,
			Ptr (new Term (std :: move (t))),
			Ptr (new Term (coefficient))
		};
	};

	if (isupper (*e))
	{
		const char * end = e;

		do
			++ end;
		while (isupper (*end));

		auto var = v -> get (e, end);

		if (nullptr == var)
		{
			throw ParseError (e)
				<< std :: string (e, end) << ERROR_VARIABLE;
		}

		e = end;

		return finish (var);
	}
	else if ('i' == *e && ends_term (e[1]))
	{
		check_i_suffix ();
		return {coefficient};
	}
	else if (islower (*e))
	{
		const char * end = e;

		do
			++ end;
		while (islower (*end));

		auto f = unary (e, end);
		auto c = constant (e, end);

		if (nullptr == f && Constant (0, 0) == c)
		{
			throw ParseError (e)
				<< std :: string (e, end) << ERROR_NAME;
		}

		if (f)
		{
			e = end;
			
			skip_whitespace (e);

			if ('(' != *e)
				throw ParseError (e) << ERROR_FUNCTION_BRACKET;

			++e;

			return {f, Ptr (new Term (parse_sum (e, v, depth+1)))};
		}
		else
		{
			if (false == ends_term (*end))
				throw ParseError (e) << ERROR_UNEXPECTED;

			e = end;

			return {c};
		}
	}
	else if ('(' == *e)
	{
		++e;

		return finish (parse_sum (e, v, depth + 1));
	}
	else if (number)
	{
		check_i_suffix ();

		return {coefficient};
	}
	else
		throw ParseError (e) << ERROR_UNEXPECTED;
}



void
MathExpression :: Term :: simplify ()
{
	for (auto & t : m_terms)
		t -> simplify ();

	switch (m_type)
	{
	case Type :: SUM:
	case Type :: PRODUCT:

		if (m_terms .size () < 2)
			return;

		// Constants at the end.

		std :: stable_sort (
			m_terms .begin (),
			m_terms .end (),
			[] (const Ptr & a, const Ptr & b)
			{
				return Type :: CONSTANT != a -> m_type
				    || Type :: CONSTANT == b -> m_type;
			});

		{
			// First constant.
			auto i = m_terms .end ();

			while (i > m_terms .begin ()
			       && Type :: CONSTANT == (*(i-1)) -> m_type)
				--i;

			// Let no constants be inverted.

			if (Type :: SUM == m_type)
			{
				for (auto t = i; t < m_terms .end (); ++t)
				{
					auto & x = *t;

					if (x -> m_invert_in_group)
					{
						x -> m_invert_in_group = false;
						x -> m_value = - x -> m_value;
					}
				}
			}
			else
			{
				for (auto t = i; t < m_terms .end (); ++t)
				{
					auto & x = *t;

					if (x -> m_invert_in_group)
					{
						x -> m_invert_in_group = false;

						x -> m_value = Constant (1)
							/ x -> m_value;
					}
				}
			}

			// everything from i+1 to end-1 can be accumulated into
			// i and then deleted.
			
			if (Type :: SUM == m_type)
			{
				for (auto at = i+1; at < m_terms .end (); ++at)
				{
					assert (! (*at) -> m_invert_in_group);
					(*i) -> m_value += (*at) -> m_value;
				}
			}
			else
			{
				for (auto at = i+1; at < m_terms .end (); ++at)
				{
					assert (! (*at) -> m_invert_in_group);
					(*i) -> m_value *= (*at) -> m_value;
				}
			}

			if (i < m_terms .end ())
				m_terms .erase (i + 1, m_terms .end ());
		}

		if (1 == m_terms .size ()
		 && Type :: CONSTANT == m_terms [0] -> m_type)
		{
			m_type = Type :: CONSTANT;

			m_value = m_terms [0] -> m_value;
			
			m_terms .clear ();
		}

		break;

	case Type :: CONSTANT:
	case Type :: VARIABLE:
		break;

	case Type :: UNARY_FUNCTION:
		
		assert (1 == m_terms .size ());
		assert (nullptr != m_unary_function);
		assert (nullptr == m_binary_function);

		if (Type :: CONSTANT == m_terms [0] -> m_type)
		{
			m_type = Type :: CONSTANT;

			m_value = m_unary_function (m_terms [0] -> value ());
			
			m_terms .clear ();
		}
		break;

	case Type :: BINARY_FUNCTION:
		
		assert (2 == m_terms .size ());
		assert (nullptr == m_unary_function);
		assert (nullptr != m_binary_function);

		if (Type :: CONSTANT == m_terms [0] -> m_type
		 && Type :: CONSTANT == m_terms [1] -> m_type)
		{
			m_type = Type :: CONSTANT;

			m_value = m_binary_function (
				m_terms [0] -> value (),
				m_terms [1] -> value ());
			
			m_terms .clear ();
		}
		break;
	};
}



void
MathExpression :: Term :: print_formula (std :: ostream & o)
const
{
	print_formula (o, true);
}



void
MathExpression :: Term :: print_formula (std :: ostream & o, bool first)
const
{
	bool started = false;

	auto print_const = [&] (Constant c)
	{
		if (auto name = const_name (c))
			o << name;
		else if (Constant (0, 0) == c)
			o << 0;
		else
		{
			bool both = c .real () != 0 && c .imag () != 0;

			if (both)
				o << '(';

			if (c .real () != 0)
				o << c .real ();

			if (both)
				o << '+';

			if (c .imag () != 0)
			{
				if (1 != c .imag ())
					o << c .imag ();

				o << 'i';
			}

			if (both)
				o << ')';
		}
	};

	switch (m_type)
	{
	case Type :: SUM:
		
		if (false == first)
			o << '(';

		for (auto & t : m_terms)
		{
			if (started)
				o << " + ";

			started = true;

			t -> print_formula (o, false);
		}

		if (false == first)
			o << ')';

		break;

	case Type :: PRODUCT:
		
		for (auto & t : m_terms)
		{
			if (started)
				o << '*';

			started = true;

			t -> print_formula (o, false);
		}

		break;

	case Type :: CONSTANT:
		print_const (m_value);
		break;

	case Type :: VARIABLE:
		o << m_var -> name ();
		break;

	case Type :: UNARY_FUNCTION:

		assert (1 == m_terms .size ());
		
		o << unary_name (m_unary_function) << '(';
		m_terms [0] -> print_formula (o, false);
		o << ')';

		break;

	case Type :: BINARY_FUNCTION:

		assert (2 == m_terms .size ());
		
		if (Math :: pow == m_binary_function)
		{
			auto & m = * (m_terms [0]);
			auto & e = * (m_terms [1]);
			
			if (Type :: CONSTANT == m .m_type)
				print_const (m .m_value);
			else if (Type :: VARIABLE == m .m_type
			 || Type :: UNARY_FUNCTION == m .m_type
			 || Type :: SUM == m .m_type
			 || (Type :: BINARY_FUNCTION == m .m_type
				 && Math :: pow != m .m_binary_function))
			{
				m .print_formula (o, false);
			}
			else
			{
				o << '(';
				m .print_formula (o, false);
				o << ')';
			}

			o << '^';

			if (Type :: CONSTANT == e .m_type)
				print_const (e .m_value);
			else if (Type :: VARIABLE == e .m_type
			 || Type :: SUM == e .m_type
			 || Type :: UNARY_FUNCTION == e .m_type)
			{
				e .print_formula (o, false);
			}
			else
			{
				o << '(';
				e .print_formula (o, false);
				o << ')';
			}


		}
		else if (Math :: multiply == m_binary_function)
		{
			auto & l = m_terms [0];
			auto & r = m_terms [1];

			l -> print_formula (o, false);

			if (Type :: CONSTANT == r -> m_type
			 && Constant (0, 1) == r -> m_value)
			switch (l ->m_type)
			{
			case Type :: PRODUCT:
			case Type :: SUM:
			case Type :: CONSTANT:
			case Type :: VARIABLE:
				o << 'i';
				break;

			default:
				o << '*';
				r -> print_formula (o, false);
			};
		}
		else
		{
			o << binary_name (m_binary_function) << '(';
			m_terms [0] -> print_formula (o, false);
			o << ',';
			m_terms [1] -> print_formula (o, false);
			o << ')';
		}

		break;
	};
}



void
MathExpression :: Term :: print_tree
(
	std :: ostream & o,
	const char * tab,
	unsigned depth
)
const
{
	if (nullptr == tab)
		tab = "\t";

	auto start = [&] () -> std :: ostream &
	{
		for (unsigned i = 0; i < depth; ++i)
			o << tab;

		return o;
	};

	switch (m_type)
	{
	case Type :: SUM:
		start () << "SUM\n";
		
		for (auto & t : m_terms)
			t -> print_tree (o, tab, depth + 1);

		break;

	case Type :: PRODUCT:
		start () << "PRODUCT\n";
		
		for (auto & t : m_terms)
			t -> print_tree (o, tab, depth + 1);

		break;

	case Type :: CONSTANT:
		start () << m_value << "\n";
		break;

	case Type :: VARIABLE:
		start ()
			<< m_var -> name () << " = "
			<< m_var -> value () << "\n";
		break;

	case Type :: UNARY_FUNCTION:

		assert (1 == m_terms .size ());
		
		start () << unary_name (m_unary_function) << "\n";
		
		m_terms [0] -> print_tree (o, tab, depth + 1);

		break;

	case Type :: BINARY_FUNCTION:

		assert (2 == m_terms .size ());
		
		start () << binary_name (m_binary_function) << "\n";
		
		m_terms [0] -> print_tree (o, tab, depth + 1);
		m_terms [1] -> print_tree (o, tab, depth + 1);

		break;
	};
}



std :: ostream &
operator << (std :: ostream & o, const MathExpression :: Term & t)
{
	t .print_formula (o);

	return o;
}
