/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_MINI_CB)
#define BOOST_SPIRIT_MINI_CB

#include "definitions.hpp"

///////////////////////////////////////////////////////////////////////////////
//  Our expression grammar and compiler
///////////////////////////////////////////////////////////////////////////////
template <typename Iterator>
expression<Iterator>::expression(
    code_t& code
  , vars_t& vars
  , functions_t& functions)
  : expression::base_type(expr)
  , code(code)
  , vars(vars)
  , functions(functions)
  , op(code)
{
    expr =
        equality_expr.alias()
        ;

    equality_expr =
        relational_expr
        >> *(   ("==" > relational_expr     [op(op_eq)])
            |   ("!=" > relational_expr     [op(op_neq)])
            )
        ;

    relational_expr =
        logical_expr
        >> *(   ("<=" > logical_expr        [op(op_lte)])
            |   ('<' > logical_expr         [op(op_lt)])
            |   (">=" > logical_expr        [op(op_gte)])
            |   ('>' > logical_expr         [op(op_gt)])
            )
        ;

    logical_expr =
        additive_expr
        >> *(   ("&&" > additive_expr       [op(op_and)])
            |   ("||" > additive_expr       [op(op_or)])
            )
        ;

    additive_expr =
        multiplicative_expr
        >> *(   ('+' > multiplicative_expr  [op(op_add)])
            |   ('-' > multiplicative_expr  [op(op_sub)])
            )
        ;

    multiplicative_expr =
        unary_expr
        >> *(   ('*' > unary_expr           [op(op_mul)])
            |   ('/' > unary_expr           [op(op_div)])
            )
        ;

    unary_expr =
            primary_expr
        |   ('!' > primary_expr             [op(op_not)])
        |   ('-' > primary_expr             [op(op_neg)])
        |   ('+' > primary_expr)
        ;

    primary_expr =
        float_                              [op(op_float, _1)]
        |   variable
        |   function_call
        |   sine_function										[op(op_sin)]
        |   cosine_function									[op(op_cos)]
        |   sqrt_function									[op(op_sqrt)]
        |   pow_function										[op(op_pow)]
        |   log_function										[op(op_log)]
        |   floor_function									[op(op_floor)]
        |   lit("true")                     [op(op_true)]
        |   lit("false")                    [op(op_false)]
        |   '(' > expr > ')'
        ;

    variable =
        (
            lexeme[
                vars
                >> !(alnum | '_')           // make sure we have whole words
            ]
        )                                   [op(op_load, _1)]
        ;

    function_call =
        functions                           [_a = _1]
        >>  '('
        >> -(
                expr                        [++_b]
                >> *(',' > expr             [++_b])
            )
        >   lit(')')                        [op(_a, _b, _pass)]
        ;

		sine_function = lit("sin") >> '(' > expr > lit(')')   												
        ;

		cosine_function =	lit("cos") >> '(' > expr > lit(')')   												
        ;

		sqrt_function =	lit("sqrt") >> '(' > expr > lit(')')   												
        ;

		pow_function =	lit("pow") >> '(' > expr > ',' > expr > lit(')')   												
        ;
				
		log_function =	lit("log") >> '(' > expr > lit(')')   												
        ;

		floor_function =	lit("floor") >> '(' > expr > lit(')')   												
        ;

    expr.name("expression");
    equality_expr.name("equality-expression");
    relational_expr.name("relational-expression");
    logical_expr.name("logical-expression");
    additive_expr.name("additive-expression");
    multiplicative_expr.name("multiplicative-expression");
    unary_expr.name("unary-expression");
    primary_expr.name("primary-expression");
    variable.name("variable");
    function_call.name("function-call");
		sine_function.name("sine_function");
		cosine_function.name("cosine_function");
		sqrt_function.name("sqrt_function");
		pow_function.name("pow_function");
		log_function.name("log_function");
		floor_function.name("floor_function");

    on_error<fail>(expr, ::error_handler(_4, _3, _2)); // $$$ fix!

#if 0
		debug(primary_expr);
		debug(variable);
		debug(sine_function);
		debug(cosine_function);
#endif
}

#endif
