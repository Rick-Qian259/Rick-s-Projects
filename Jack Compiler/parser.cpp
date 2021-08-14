/*
    CS assignment3  
    created by: Qianxun Huai
    ID:a1813515
    time:23.10.2020
*/

#include <string>
#include <vector>
#include "iobuffer.h"
#include "symbols.h"
#include "abstract-syntax-tree.h"

// to shorten our code:
using namespace std ;
using namespace CS_IO_Buffers ;
using namespace CS_Symbol_Tables ;
using namespace Jack_Compiler ;

// ***** WHAT TO DO *****
//
// MODIFY the skeleton code below to parse a Jack class and construct the equivalent abstract syntax tree, ast.
//        The main function will print the ast formatted as XML
//
// NOTE: the correct segment and offset is recorded with every variable in the ast
//       so the code generator does not need to use any symbol tables
//
// NOTE: use the fatal_token_context() function to print fatal error messages with the current
//       input file position marked.
//
// The skeleton code has one function per non-terminal in the Jack grammar
//  - comments describing how to create relevant ast nodes prefix most functions
//  - ast nodes are immutable - you must create all the sub-trees before creating a new node
//
// The structure of the skeleton code is just a suggestion
//  - it matches the grammar but does not quite match the ast structure
//  - you will need to change the parameters / results / functions to suit your own logic
//  - you can change it as much as you like
//
// NOTE: the best place to start is to write code to parse the input without attempting to create
//       any ast nodes. If correct this parser will report errors when it finds syntax errors but
//       correct programs will fail because the tree printer will not be provided with a valid ast.
//       Once the parsing works modify your code to create the ast nodes.
//
// NOTE: the return create_empty() ; statements are only present so that this skeleton code will compile.
//       these statements need to be replaced by code that creates the correct ast nodes.
//

// The Jack grammar to be recognised:
// program          ::= One or more classes, each class in a separate file named <class_name>'.Jack'
// class            ::= 'class' identifier '{' class_var_decs subr_decs '}'
// class_var_decs   ::= (static_var_dec | field_var_dec)*
// static_var_dec   ::= 'static' type identifier (',' identifier)* ';'
// field_var_dec    ::= 'field' type identifier (',' identifier)* ';'
// type             ::= 'int' | 'char' | 'boolean' | identifier
// vtype            ::= 'void' | type
// subr_decs        ::= (constructor | function | method)*
// constructor      ::= 'constructor' identifier identifier '(' param_list ')' subr_body
// function         ::= 'function' vtype identifier '(' param_list ')' subr_body
// method           ::= 'method' vtype identifier '(' param_list ')' subr_body
// param_list       ::= ((type identifier) (',' type identifier)*)?
// subr_body        ::= '{' var_decs statements '}'
// var_decs         ::= var_dec*
// var_dec          ::= 'var' type identifier (',' identifier)* ';'
    
// statements       ::= statement*
// statement        ::= let | if | while | do | return
// let              ::= 'let' identifier index? '=' expr ';'
// if               ::= 'if' '(' expr ')' '{' statements '}' ('else' '{' statements '}')?
// while            ::= 'while' '(' expr ')' '{' statements '}'
// do               ::= 'do' identifier (id_call | call) ';'
// return           ::= 'return' expr? ';'

// expr             ::= term (infix_op term)*
// term             ::= integer_constant | string_constant | 'true' | 'false' | 'null' | 'this' | '(' expr ')' | unary_op term | var_term
// var_term         ::= identifier (index | id_call | call)?
// id_call          ::= '.' identifier call
// call             ::= '(' expr_list ')'
// expr_list        ::= (expr (',' expr)*)?
// infix_op         ::= '+' | '-' | '*' | '/' | '&' | '|' | '<' | '>' | '='
// unary_op         ::= '-' | '~'


// forward declarations of one function per non-terminal rule in the Jack grammar - except for program
// NOTE: parse_type(), parse_vtype(), parse_unary_op() all return a Token not an ast
//
ast parse_class() ;
ast parse_class_var_decs() ;
ast parse_static_var_dec() ;
ast parse_field_var_dec() ;
Token parse_type() ;
Token parse_vtype() ;
ast parse_subr_decs() ;
ast parse_constructor() ;
ast parse_function() ;
ast parse_method() ;
ast parse_param_list(int offset) ;
ast parse_subr_body() ;
ast parse_var_decs() ;
ast parse_var_dec() ;

ast parse_statements() ;
ast parse_statement() ;
ast parse_let() ;
ast parse_if() ;
ast parse_while() ;
ast parse_do() ;
ast parse_return() ;

ast parse_expr() ;
ast parse_term() ;
ast parse_var_term() ;
ast parse_index() ;
ast parse_id_call() ;
ast parse_call() ;
ast parse_expr_list() ;
ast parse_infix_op() ;
Token parse_unary_op() ;

vector<ast> var_declaration ;
vector<ast> subr_declaration ;

symbols st_fi_table = create_variables() ;
symbols sub_var_table = create_variables() ;
string className ;
 
// symbols let_table = create_variables() ;
// symbols field_table = create_variables() ;
// symbols variables_table = create_variables() ;
//
int var_dec_offset ;
int static_offset = 0 ;
int field_offset = 0 ;

void error_message(string message)
{
    fatal_error("fatal error occurs " + message) ;
}

// class ::= 'class' identifier '{' class_var_decs subr_decs '}'
// create_class(myclassname,class_var_decs,class_subrs)
//
ast parse_class()
{
    ast class_var_decs ;
    ast class_subrs ;
    string myclassname ;

    mustbe(tk_class) ;
    myclassname = token_spelling( mustbe(tk_identifier) ) ;
    className = myclassname ;
    mustbe(tk_lcb) ;

    // if (have(tg_starts_class_var))
    // {
    class_var_decs = parse_class_var_decs() ;
    // }
    // if ( have(tg_starts_subroutine) )
    // {
    class_subrs = parse_subr_decs() ;
    // } 
  //   while ( !have(tk_rcb) )
  //   {
		// // class_var_decs ::= (static_var_dec | field_var_dec)*
  //       // subr_decs ::= (constructor | function | method)*
  //       if (have(tg_starts_class_var))
  //       {
  //           class_var_decs = parse_class_var_decs() ;
  //       }
  //       else if ( have(tg_starts_subroutine) )
  //       {
  //           class_subrs = parse_subr_decs() ;
  //       }
  //   }
    mustbe(tk_rcb) ;
    if ( !have(tk_eoi) )
    {
        error_message("end of input") ;
    }
    return create_class(myclassname, class_var_decs, class_subrs) ;
}

// class_var_decs ::= (static_var_dec | field_var_dec)*
// returns ast_class_var_decs: create_class_var_decs(vector<ast> decs)
// create_class_var_decs must be passed a vector of ast_var_dec
//
ast parse_class_var_decs()
{
    ast var_decs_temp ;
    while (have(tg_starts_class_var))
    {
        if ( have(tk_static) )
        {
            var_decs_temp = parse_static_var_dec() ;
            var_declaration.push_back(var_decs_temp) ;
        }
        else if ( have(tk_field) )
        {
            var_decs_temp = parse_field_var_dec() ;
            var_declaration.push_back(var_decs_temp) ;
        }
    }
    if ( have(tk_eoi) )
    {
        error_message("end of input") ;
    }
    return create_class_var_decs( var_declaration ) ;
}

// static_var_dec ::= 'static' type identifier (',' identifier)* ';'
// returns vector of ast_var_dec
//
// ast_var_dec: create_var_dec(string name,string segment,int offset,string type)
// create_var_dec must be passed:
// . name: the variable's name
// . segment: the variable's segment
// . offset: the variable's position in it's segment
// . type: the variable's type
//
ast parse_static_var_dec()
{
    vector<ast> decs2 ;
    string name ;
    string segment = "static" ;
    string type ;

    mustbe( tk_static ) ;
    type = token_spelling( parse_type() )  ;
    while ( have(tk_identifier) )
    {
        name = token_spelling( mustbe( tk_identifier ) ) ;
        st_variable myvar(name, type, segment, static_offset) ;
        if ( !insert_variables(st_fi_table, name, myvar) )
        {
            error_message(" parse_static_var_dec() ") ;
        }
        decs2.push_back( create_var_dec( name, segment, static_offset, type ) ) ;
        static_offset++ ;
        if ( have(tk_comma) )
        {
            next_token() ;
        }
        else 
        {
            mustbe(tk_semi) ;
        }
    }
    if ( have(tk_eoi) )
    {
        error_message("end of input") ;
    }
    // mustbe( tk_semi ) ;
    pop_error_context() ;
    return create_var_decs(decs2) ;
}

// field_var_dec ::= 'field' type identifier (',' identifier)* ';'
// returns vector of ast_var_dec
//
// ast_var_dec: create_var_dec(string name,string segment,int offset,string type)
// create_var_dec must be passed:
// . name: the variable's name
// . segment: the variable's segment
// . offset: the variable's position in it's segment
// . type: the variable's type
//
ast parse_field_var_dec()
{
    vector<ast> decs2 ;
    string name ;
    string segment = "this" ;
    string type ;

    mustbe( tk_field ) ;
    type = token_spelling( mustbe(tg_starts_type) ) ;
    while ( have(tk_identifier) )
    {
        name = token_spelling( mustbe( tk_identifier ) ) ;
        st_variable myvar(name, type, segment, field_offset) ;
        if ( !insert_variables(st_fi_table, name, myvar) )
        {
            error_message(" parse_field_var_dec() ") ;
        }
        decs2.push_back( create_var_dec( name, segment, field_offset, type ) ) ;
        field_offset++ ;
        if ( have(tk_comma) )
        {
            next_token() ;
        }
        else
        {
            mustbe(tk_semi) ;
        }
    }
    if ( have(tk_eoi) )
    {
        error_message("end of input") ;
    }
    // mustbe( tk_semi ) ;
    return create_var_decs(decs2) ;
}

// type ::= 'int' | 'char' | 'boolean' | identifier
// returns the Token for the type
//
Token parse_type()
{
    if ( have(tk_int) )
    {
        return mustbe(tk_int) ;
    }
    else if ( have(tk_char) )
    {
        return mustbe(tk_char) ;
    }
    else if ( have(tk_boolean) )
    {
        return mustbe(tk_boolean) ;
    }
    else if ( have(tk_identifier) )
    {
        return mustbe(tk_identifier) ;
    }
    else 
    {
        error_message(" parse_type() ") ;
    }
    if ( have(tk_eoi) )
    {
        error_message("end of input") ;
    }
    return 0 ;
}

// vtype ::= 'void' | type
// returns the Token for the type
//
Token parse_vtype()
{
    return mustbe(tg_starts_vtype) ;
}

// subr_decs ::= (constructor | function | method)*
// returns ast_subr_decs: create_subr_decs(vector<ast> subrs)
// create_subr_decs must be passed an vector of ast_subr
// ast_subr: create_subr(ast subr)
// create_subr must be passed one of: ast_constructor, ast_function or ast_method
//
ast parse_subr_decs()
{
    ast subr_decs_temp = nullptr ;

    while ( have(tg_starts_subroutine) )
    {
        if ( have(tk_constructor) )
        {
            subr_decs_temp = parse_constructor() ;
        }
        else if ( have(tk_function) )
        {
            subr_decs_temp = parse_function() ;
        }
        else if ( have(tk_method) )
        {
            subr_decs_temp = parse_method() ;
        }
        else
        {
            error_message(" parse_subr_decs() ") ;
        }
        subr_declaration.push_back( create_subr(subr_decs_temp ) ) ;
        if ( have(tk_eoi) )
        {
            error_message("end of input") ;
        }
    }

    return create_subr_decs(subr_declaration) ;
}

// constructor ::= 'constructor' identifier identifier '(' param_list ')' subr_body
// returns ast_constructor: create_constructor(string vtype,string name,ast params,ast body)
// . vtype: the constructor's return type, this must be it's class name
// . name: the constructor's name within its class
// . params: ast_param_list - the constructor's parameters
// . body: ast_subr_body - the constructor's body
//
ast parse_constructor()
{
    ast params_tmp = nullptr ;
    ast body_tmp ;
    string vtype ;
    string name ;
    sub_var_table = create_variables() ;
    int offset = -1 ;

    mustbe( tk_constructor );
    vtype = token_spelling( mustbe( tk_identifier ) ) ;
    name = token_spelling( mustbe( tk_identifier ) ) ;
    mustbe( tk_lrb ) ;
    while ( !have(tk_rrb) )
    {
        if (have(tg_starts_type))
        {
            params_tmp = parse_param_list(offset) ;
        }
    }
    mustbe( tk_rrb ) ;
    body_tmp = parse_subr_body() ;
    delete_variables(sub_var_table) ;
    if ( have(tk_eoi) )
    {
        error_message("end of input") ;
    }
    return create_constructor(vtype, name, params_tmp, body_tmp) ;
}
//
// function ::= 'function' vtype identifier '(' param_list ')' subr_body
// returns ast_function: create_function(string vtype,string name,ast params,ast body)
// . vtype: the function's return type
// . name: the function's name within its class
// . params: ast_param_list - the function's parameters
// . body: ast_subr_body - the functions's body
//
ast parse_function()
{
    ast params_tmp = nullptr ;
    ast body_tmp ;
    string vtype ;
    string name ;
    sub_var_table = create_variables() ;
    int offset = -1 ;

    mustbe( tk_function );
    vtype = token_spelling( parse_vtype() ) ;
    name = token_spelling( mustbe( tk_identifier ) ) ;
    mustbe( tk_lrb ) ;
    while ( !have(tk_rrb) )
    {
        if (have(tg_starts_type))
        {
            params_tmp = parse_param_list(offset) ;
        }
    }
    mustbe( tk_rrb ) ;
    body_tmp = parse_subr_body() ;
    delete_variables(sub_var_table) ;
    if ( have(tk_eoi) )
    {
        error_message("end of input") ;
    }
    return create_function(vtype, name, params_tmp, body_tmp) ;
}

// method ::= 'method' vtype identifier '(' param_list ')' subr_body
// returns ast_method: create_method(string vtype,string name,ast params,ast body)
// . vtype: the method's return type
// . name: the method;s name within its class
// . params: ast_param_list - the method's explicit parameters
// . body: ast_subr_body - the method's body
//
ast parse_method()
{
    ast params_tmp = nullptr ;
    ast body_tmp ;
    string vtype ;
    string name ;
    sub_var_table = create_variables() ;
    int offset = 0 ;

    mustbe( tk_method );
    vtype = token_spelling( parse_vtype() ) ;
    name = token_spelling( mustbe( tk_identifier ) ) ;
    mustbe( tk_lrb ) ;
    while ( !have(tk_rrb) )
    {
        if (have(tg_starts_type))
        {
            params_tmp = parse_param_list(offset) ;
        } 
    }
    // params_tmp = parse_param_list(offset) ;
    mustbe( tk_rrb ) ;
    body_tmp = parse_subr_body() ;
    delete_variables(sub_var_table) ;
    if ( have(tk_eoi) )
    {
        error_message("end of input") ;
    }
    return create_method(vtype, name, params_tmp, body_tmp) ;
}

// param_list ::= ((type identifier) (',' type identifier)*)?
// returns ast_param_list: create_param_list(vector<ast> params)
// create_param_list must be passed a vector of ast_var_dec
//
// ast_var_dec: create_var_dec(string name,string segment,int offset,string type)
// create_var_dec must be passed:
// . name: the variable's name
// . segment: the variable's segment
// . offset: the variable's position in it's segment
// . type: the variable's type
//
ast parse_param_list(int offset)
{
    // type::= 'int' | 'char' | 'boolean' | identifier
    vector<ast> params ;
    string name ;
    string segment = "argument" ;
    string type ;

    type = token_spelling( mustbe( tg_starts_type ) ) ;
    name = token_spelling( mustbe( tk_identifier ) ) ;
    offset ++ ;
    params.push_back( create_var_dec(name, segment, offset, type) ) ;
    st_variable myvar(name, type, segment, offset);
    if ( !insert_variables(sub_var_table, name, myvar) )
    {
        error_message(" parse_param_list() ") ;
    }
    if ( have( tk_comma ) )
    {
        while ( have( tk_comma ) )
        {
            mustbe(tk_comma) ;
            type = token_spelling( mustbe( tg_starts_type ) ) ;
            name = token_spelling( mustbe( tk_identifier ) ) ;
            offset ++ ;
            params.push_back( create_var_dec(name, segment, offset, type) ) ;
            st_variable myvar(name, type, segment, offset);
            if ( !insert_variables(sub_var_table, name, myvar) )
            {
                error_message(" parse_param_list() ") ;
            }
            if ( have(tk_rrb) )
            {
                break ;
            }
        }
    }
    else if ( have(tk_rrb) )
    {
        return create_param_list( params ) ;
    }
    else 
    {
        error_message(" parse_param_list() ") ;
    }
    if ( have(tk_eoi) )
    {
        error_message("end of input") ;
    }
    return create_param_list( params ) ;
}

// subr_body ::= '{' var_decs statements '}'
// returns ast_subr_body: create_subr_body(ast decs,ast body)
// create_subr_body must be passed:
// . decs: ast_var_decs - the subroutine's local variable declarations
// . body: ast_statements - the statements within the body of the subroutinue
//
ast parse_subr_body()
{
    mustbe( tk_lcb ) ;
    ast decs = nullptr ;
    ast body = nullptr ;
    // if ( have(tk_var) ) 
    // {
    decs = parse_var_decs() ;
    // }
    // if ( have(tg_starts_statement) )
    // {
    body = parse_statements() ;
    // }
    mustbe( tk_rcb ) ;
    if ( have(tk_eoi) )
    {
        error_message("end of input") ;
    }
    return create_subr_body(decs, body) ;
}

// var_decs ::= var_dec*
// returns ast_var_decs: create_var_decs(vector<ast> decs)
// create_var_decs must be passed a vector of ast_var_dec
//
ast parse_var_decs()
{
    vector<ast> decs ;
    var_dec_offset = -1 ;
    while ( have(tk_var) )
    {
        decs.push_back( parse_var_dec() ) ;
    }
    if ( have(tk_eoi) )
    {
        error_message("end of input") ;
    }
    return create_var_decs( decs ) ;
}

// var_dec ::= 'var' type identifier (',' identifier)* ';'
// returns vector of ast_var_dec
//
// ast_var_dec: create_var_dec(string name,string segment,int offset,string type)
// create_var_dec must be passed:
// . name: the variable's name
// . segment: the variable's segment
// . offset: the variable's position in it's segment
// . type: the variable's type
//
ast parse_var_dec()
{
    vector<ast> decs ;
    string name ;
    string segment = "local" ;
    string type ;

    mustbe( tk_var ) ;
    type = token_spelling( mustbe(tg_starts_type) ) ;
    while ( have(tg_starts_type) )
    {
        name = token_spelling( mustbe(tk_identifier) ) ;
        var_dec_offset ++ ;
        decs.push_back(create_var_dec(name, segment, var_dec_offset, type) ) ;
        st_variable myvar(name, type, segment, var_dec_offset) ;
        if ( !insert_variables(sub_var_table, name, myvar) )
        {
            error_message(" parse_var_dec() ") ;
        }
        if ( have(tk_comma) )
        {
            next_token() ;
        }
    }
    mustbe(tk_semi) ;
    if ( have(tk_eoi) )
    {
        error_message("end of input") ;
    }
    return create_var_decs(decs) ;
}

// statements ::= statement*
// create_statements(vector<ast> statements)
// create_statements must be passed a vector of ast_statement
//
ast parse_statements()
{
    vector<ast> statement_st ;
    while ( have(tg_starts_statement) )
    {
        statement_st.push_back(parse_statement()) ;
    }
    if ( have(tk_eoi) )
    {
        error_message("end of input") ;
    }
    return create_statements( statement_st ) ;
}

// statement ::= let | if | while | do | return
// create_statement(ast statement)
// create_statement initialiser must be one of: ast_let, ast_let_array, ast_if, ast_if_else, ast_while, ast_do, ast_return or ast_return_expr
//
ast parse_statement()
{
    ast statement_temp = nullptr ;
    if ( have(tk_let) )
    {
        statement_temp = parse_let() ;
    }
    else if ( have(tk_if) )
    {
        statement_temp = parse_if() ;
    }
    else if ( have(tk_while) )
    {
        statement_temp = parse_while() ;
    }
    else if ( have(tk_do) )
    {
        statement_temp = parse_do() ;
    }
    else if ( have(tk_return) )
    {
        statement_temp = parse_return() ;
    }
    else
    {
        error_message(" parse_statement() ") ;
    }
    if ( have(tk_eoi) )
    {
        error_message("end of input") ;
    }
    return create_statement(statement_temp) ;
}

// let ::= 'let' identifier index? '=' expr ';'
// return one of: ast_let or ast_let_array
//
// create_let(ast var,ast expr)
// . var: ast_var - the variable being assigned to
// . expr: ast_expr - the variable's new value
//
// create_let_array(ast var,ast index,ast expr)
// . var: ast_var - the variable for the array being assigned to
// . index: ast_expr - the array index
// . expr: ast_expr - the array element's new value
//
ast parse_let()
{
    ast var = nullptr ;
    ast expr = nullptr ;
    string name ;
    bool if_index = false ;
    ast index = nullptr ;

    mustbe(tk_let) ;
    name = token_spelling( mustbe(tk_identifier) ) ;
    st_variable sub_var_myvar = lookup_variables(sub_var_table, name) ;
    st_variable static_myvar = lookup_variables(st_fi_table, name) ;
    if (sub_var_myvar.offset != -1)
    {
        var = create_var(sub_var_myvar.name, sub_var_myvar.segment, sub_var_myvar.offset, sub_var_myvar.type) ;
    }
    else if (static_myvar.offset != -1)
    {
        var = create_var(static_myvar.name, static_myvar.segment, static_myvar.offset, static_myvar.type) ;
    }

    if ( have(tk_lsb) )
    {
        index = parse_index() ;
        if_index = true ;
    }
    mustbe(tk_eq) ;
    expr = parse_expr() ;
    mustbe(tk_semi) ;
    if (if_index)
    {
        write_to_traces(" let_in3 ") ;
        return create_let_array(var, index, expr) ;
    }
    else 
    {
        return create_let(var, expr) ;
    }
}

// if ::= 'if' '(' expr ')' '{' statements '}' ('else' '{' statements '}')?
// return one of: ast_if or ast_if_else
//
// create_if(ast condition,ast if_true)
// . condition: ast_expr - the if condition
// . if_true: ast_statements - the if true branch
//
// create_if_else(ast condition,ast if_true,ast if_false)
// . condition: ast_expr - the if condition
// . if_true: ast_statements - the if true branch
// . if_false: ast_statements - the if false branch
//
ast parse_if()
{
    ast expr = nullptr ;
    ast if_state = nullptr ;
    ast if_else_state = nullptr ;
    bool ret_t_f = true ;

    mustbe(tk_if) ;
    mustbe(tk_lrb) ;
    // if ( have(tg_starts_term) )
    // {
    expr = parse_expr() ;
    // }
    mustbe(tk_rrb) ;
    mustbe(tk_lcb) ;
    // if ( have(tg_starts_statement) )
    // {
    if_state = parse_statements() ;
    // }
    mustbe(tk_rcb) ;

    // while ( have(tk_else) )
    if ( have(tk_else) ) 
    {
        ret_t_f = false ;
        mustbe(tk_else) ;
        mustbe(tk_lcb) ;
        if ( have(tg_starts_statement) )
        {
            if_else_state = parse_statements() ;
        }
        mustbe(tk_rcb) ;
    }
    
    if ( ret_t_f )
    {
        return create_if(expr, if_state ) ;
    }
    else
    {
        return create_if_else(expr, if_state, if_else_state ) ; 
    }
}

// while ::= 'while' '(' expr ')' '{' statements '}'
// returns ast_while: create_while(ast condition,ast body)
// . condition: ast_expr - the loop condition
// . body: ast_statements - the loop body
//
ast parse_while()
{
    ast condition = nullptr ;
    ast body = nullptr ;
    mustbe(tk_while) ;
    mustbe(tk_lrb) ;
    // if ( have(tg_starts_term) )
    // {
    condition = parse_expr() ;
    // }
    mustbe(tk_rrb) ;
    mustbe(tk_lcb) ;
    // if ( have(tg_starts_statement) )
    // {
    body = parse_statements() ;
    // }
    mustbe(tk_rcb) ;
    if ( have(tk_eoi) )
    {
        error_message("end of input") ;
    }
    return create_while(condition, body) ;
}

// do ::= 'do' identifier (call | id_call) ';'
// returns ast_do: create_do(ast call)
// create_do must be passed one of: ast_call_as_function or ast_call_as_method
//
// ast_call_as_function: create_call_as_function(string class_name,ast subr_call)
// create_ast_call_as_function must be passed:
// . class_name: name of the function's class
// . subr_call: ast_subr_call - the function's name within it's class and it's explicit arguments
//
// ast_call_as_method: create_call_as_method(string class_name,ast object,ast subr_call)
// create_ast_call_as_method must be passed:
// . class_name: name of the method's class
// . object: ast_expr - the object the method is applied to
// . subr_call: ast_subr_call - the method's name within it's class and it's explicit arguments
//
ast parse_do()
{
    string name ;
    string name_temp ;
    ast subr_call = nullptr ;
    ast call = nullptr ;
    ast result = nullptr ;
    ast object = nullptr ;
    ast expr_list = nullptr ;

    if (have(tk_do))
    {
        mustbe( tk_do ) ;
        name = token_spelling( mustbe(tk_identifier) ) ;
        st_variable sub_var_myvar = lookup_variables(sub_var_table, name) ;
        st_variable static_myvar = lookup_variables(st_fi_table, name) ;
        // call 
        if ( have(tk_lrb) )
        {
            subr_call = parse_call() ;
            mustbe(tk_semi) ;
            object = create_this();
            call = create_call_as_method(className, object, create_subr_call(name, subr_call) ) ;
            result = create_do( call ) ;
        }
         // id call
        if ( have(tk_stop) )
        {
            if ( sub_var_myvar.offset != -1 )
            {
                subr_call = parse_id_call() ;
                // mustbe(tk_semi) ;
                object = create_var(sub_var_myvar.name, sub_var_myvar.segment, sub_var_myvar.offset, sub_var_myvar.type ) ;
                call = create_call_as_method(sub_var_myvar.type, object, subr_call ) ;
                result = create_do(call) ;
                if ( have(tk_semi) )
                {
                    mustbe(tk_semi) ; 
                }
            }
            else if ( static_myvar.offset != -1 )
            {
                subr_call = parse_id_call() ;
                object = create_var(static_myvar.name, static_myvar.segment, static_myvar.offset, static_myvar.type ) ;
                call = create_call_as_method(static_myvar.type, object, subr_call ) ;
                result = create_do(call) ;
                if ( have(tk_semi) )
                {
                    mustbe(tk_semi) ; 
                }
            }
            else
            {
                mustbe(tk_stop) ;
                name_temp = token_spelling( mustbe(tk_identifier) ) ;
                mustbe(tk_lrb) ;
                expr_list = parse_expr_list() ;
                mustbe(tk_rrb) ;
                subr_call = create_subr_call( name_temp, expr_list) ;
                call = create_call_as_function(name, subr_call) ;
                if ( have(tk_semi) )
                {
                    mustbe(tk_semi) ; 
                }
                result = create_do(call) ;
            }
        }
    }
    if ( have(tk_eoi) )
    {
        error_message("end of input") ;
    }
    return result ;
}

// return ::= 'return' expr? ';'
// returns one of: ast_return or ast_return_expr
//
// ast_return: create_return()
//
// ast_return_expr: create_return_expr(ast expr)
// create_return_expr must be passed an ast_expr
//
ast parse_return()
{
    ast expr ;
    mustbe(tk_return) ;
    if ( have(tk_semi) )
    {
        mustbe(tk_semi) ;
        return create_return() ;
    }
    else
    {
        expr = parse_expr() ;
        mustbe(tk_semi) ;
        return create_return_expr(expr) ;
    }
}

// expr ::= term (infix_op term)*
// returns ast_expr: create_expr(vector<ast> expr)
// the vector passed to create_expr:
// . must be an odd length of at least 1, ie 1, 3, 5, ...
// . must be  a vector alternating between ast_term and ast_infix_op nodes, ending with an ast_term
//
ast parse_expr()
{
    vector<ast> expr ;
    expr.push_back( parse_term() ) ;
    while ( have(tg_infix_op) )
    {
        expr.push_back( parse_infix_op() ) ;
        expr.push_back( parse_term() ) ;
    }
    if (expr.size() % 2 == 0)
    {
        error_message(" parse_expr() ") ;
        return 0 ;
    }
    return create_expr( expr ) ;
}

// term ::= integer_constant | string_constant | 'true' | 'false' | 'null' | 'this' | '(' expr ')' 
//                                                      | unary_op term | var_term
// returns ast_term: create_term(ast term)
// create_term must be passed one of: ast_int, ast_string, ast_bool, ast_null, ast_this, ast_expr,
//                                    ast_unary_op, ast_var, ast_array_index, ast_call_as_function, ast_call_as_method
//
// ast_int: create_int(int _constant)
// create_int must be passed an integer value in the range 0 to 32767
//
// ast_string: create_string(string _constant)
// create_string must be passed any legal Jack string literal
//
// ast_bool: create_bool(bool t_or_f)
// create_bool must be passed true or false
//
// ast_null: create_null()
//
// ast_this: create_this()
//
// ast_unary_op: create_unary_op(string op,ast term)
// create_unary_op must be passed:
// . op: the unary op
// . term: ast_term
//
ast parse_term()
{
    ast term = nullptr ;
    int int_constant ;
    string str_constant ;
    string op ;

    if ( have(tk_integerConstant) )
    {
        int_constant = token_ivalue( mustbe(tk_integerConstant) ) ;
        if (int_constant < 0 || int_constant > 32767)
        {
            error_message(" parse_term() ") ;
        }
        term = create_int(int_constant) ;

    }
    else if ( have(tk_stringConstant) )
    {
        str_constant = token_spelling( mustbe(tk_stringConstant) ) ;
        term = create_string(str_constant) ;
    }
    else if ( have(tk_true) )
    {
        mustbe(tk_true) ;
        term = create_bool(true) ;
    }
    else if ( have(tk_false) )
    {
        mustbe(tk_false) ;
        term = create_bool(false) ;
    }
    else if ( have(tk_null) )
    {
        mustbe(tk_null) ;
        term = create_null() ;
    }
    else if ( have(tk_this) )
    {
        mustbe(tk_this) ;
        term = create_this() ;
    }
    else if ( have(tk_lrb) )
    {
        mustbe(tk_lrb) ;
        term = parse_expr() ;
        mustbe(tk_rrb) ;
    }
    else if ( have(tg_unary_op) )
    {
        op = token_spelling( parse_unary_op() ) ;
        term = create_unary_op(op, parse_term()) ;
    }
    else if ( have(tk_identifier) )
    {
        term = parse_var_term() ;
    }
    else
    {
        error_message(" parse_term() ") ;
    }
    if ( have(tk_eoi) )
    {
        error_message("end of input") ;
    }
    return create_term(term) ;
}

// var_term ::= identifier (index | id_call | call)?
// returns one of: ast_var, ast_array_index, ast_call_as_function or ast_call_as_method
//
// ast_var: create_var(string name,string segment,int offset,string type)
// create_ast_var must be passed:
// . name: the variable's name
// . segment: the variable's segment
// . offset: the variable's position in it's segment
// . type: the variable's type
//
// ast_array_index: create_array_index(ast var,ast index)
// create_ast_array_index must be passed:
// . var: ast_var - the array variable
// . index: ast_expr - the index into the array
//
// ast_call_as_function: create_call_as_function(string class_name,ast subr_call)
// create_ast_call_as_function must be passed:
// . class_name: name of the function's class
// . subr_call: ast_subr_call - the function's name within it's class and it's explicit arguments
//
// ast_call_as_method: create_call_as_method(string class_name,ast object,ast subr_call)
// create_ast_call_as_method must be passed:
// . class_name: name of the method's class
// . object: ast_expr - the object the method is applied to
// . subr_call: ast_subr_call - the method's name within it's class and it's explicit arguments
//
ast parse_var_term()
{
    string name ;
    ast result = nullptr ;
    ast subr_call = nullptr ;
    ast object = nullptr ;
    ast call_temp = nullptr ;
    ast var = nullptr ;
    ast index = nullptr ;

    name = token_spelling( mustbe( tk_identifier ) ) ;
    // index
    if ( have(tk_lsb) )
    {
        st_variable term_var = lookup_variables(st_fi_table, name) ;
        st_variable sub_var = lookup_variables(sub_var_table, name) ; 
        if (term_var.offset != -1)
        {
            var = create_var(term_var.name, term_var.segment, term_var.offset, term_var.type) ;
            index = parse_index() ;
            result = create_array_index(var, index) ;
        }
        else if (sub_var.offset != -1)
        {
            var = create_var(sub_var.name, sub_var.segment, sub_var.offset, sub_var.type) ;
            index = parse_index() ;
            result = create_array_index(var, index) ;
            write_to_traces( " P33 " ) ;
        }
    }
    // id_call
    else if ( have(tk_stop) )
    {
        subr_call = parse_id_call() ;
        st_variable term_var = lookup_variables(st_fi_table, name) ;
        st_variable sub_var = lookup_variables(sub_var_table, name) ;
        if (term_var.offset != -1)
        {
            object = create_var(term_var.name, term_var.segment, term_var.offset, term_var.type) ;
            name = get_var_type(object) ;
            result = create_call_as_method(name, object, subr_call) ;
        }
        else if (sub_var.offset != -1)
        {
            object = create_var(sub_var.name, sub_var.segment, sub_var.offset, sub_var.type) ;
            name = get_var_type(object) ;
            result = create_call_as_method(name, object, subr_call) ;
        }
        else
        {
            result = create_call_as_function(name, subr_call) ;
        }
    }
    else if ( have(tk_lrb) )
    {
        object = create_this() ;
        call_temp = parse_call() ;
        subr_call = create_subr_call(name, call_temp) ;
        result = create_call_as_method(className, object, subr_call) ;
    }
    else
    {
        st_variable term_var = lookup_variables(st_fi_table, name) ;
        st_variable sub_var = lookup_variables(sub_var_table, name) ;
        if ( term_var.offset != -1 )
        {
            result = create_var(term_var.name, term_var.segment, term_var.offset, term_var.type) ;
        }
        else if ( sub_var.offset != -1)
        {
            result = create_var(sub_var.name, sub_var.segment, sub_var.offset, sub_var.type) ;
        }
    }
    if ( have(tk_eoi) )
    {
        error_message("end of input") ;
    }
    return result ;
}

// index ::= '[' expr ']'
// returns ast_expr
//
ast parse_index()
{
    ast expr ;
    mustbe( tk_lsb ) ;
    write_to_traces( " in1 " ) ;
    expr = parse_expr() ;
    write_to_traces( " in2 " ) ;
    mustbe( tk_rsb ) ;
    if ( have(tk_eoi) )
    {
        error_message("end of input") ;
    }
    return expr ;
}

// id_call ::= '.' identifier call
// returns ast_subr_call: create_subr_call(string subr_name,ast expr_list)
// create_subr_call must be passed:
// . subr_name: the constructor, function or method's name within its class
// . expr_list: ast_expr_list - the explicit arguments to the call
//
ast parse_id_call()
{
    string name ;
    ast expr_list = nullptr ;
    mustbe(tk_stop) ;
    name = token_spelling( mustbe(tk_identifier) ) ;
    expr_list = parse_call() ;
    if ( have(tk_eoi) )
    {
        error_message("end of input") ;
    }
    return create_subr_call(name, expr_list) ;
}

// call ::= '(' expr_list ')'
// returns ast_expr_list
//
ast parse_call()
{
    ast call_list = nullptr ;

    mustbe(tk_lrb) ;
    call_list = parse_expr_list() ;
    mustbe(tk_rrb) ;
    if ( have(tk_eoi) )
    {
        error_message("end of input") ;
    }
    return call_list;
}

// expr_list ::= (expr (',' expr)*)?
// returns ast_expr_list: create_expr_list(vector<ast> exprs)
// create_expr_list must be passed: a vector of ast_expr
//
ast parse_expr_list()
{
    vector<ast> exprs ;
    if ( have(tg_starts_term) )
    {
        exprs.push_back( parse_expr() ) ;
        while ( have(tk_comma) )
        {
            next_token() ;
            exprs.push_back( parse_expr() ) ;
        }
    }
    if ( have(tk_eoi) )
    {
        error_message("end of input") ;
    }
    return create_expr_list( exprs ) ;
}

// infix_op ::= '+' | '-' | '*' | '/' | '&' | '|' | '<' | '>' | '='
// returns ast_op: create_infix_op(string infix_op)
// create_infix_op must be passed:
// infix_op: the infix op
//
ast parse_infix_op()
{
    ast result = nullptr ;
    result = create_infix_op( token_spelling(mustbe(tg_infix_op)) ) ;
    if ( have(tk_eoi) )
    {
        error_message("end of input") ;
    }
    return result ;
}

// unary_op ::= '-' | '~'
// returns Token for the unary_op
//
Token parse_unary_op()
{
    return mustbe(tg_unary_op) ;
}

// use in main function 
ast jack_parser()
{
    // read the first token to get the tokeniser initialised
    next_token() ;
    if ( have(tk_eoi) )
    {
        error_message("end of input") ;
    }

    // construct tree and return as result
    return parse_class() ;
}

// main program
int main(int argc,char **argv)
{
    // parse a Jack class and print the abstract syntax tree as XML
    ast_print_as_xml(jack_parser(),4) ;

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}

