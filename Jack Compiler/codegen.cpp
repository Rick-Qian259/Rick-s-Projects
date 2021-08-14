#include "iobuffer.h"
#include "symbols.h"
#include "abstract-syntax-tree.h"

// to shorten our code:
using namespace std ;
using namespace CS_IO_Buffers ;
using namespace Jack_Compiler ;

// ***** WHAT TO DO *****
//
// MODIFY the skeleton code below to walk an abstract syntax tree, ast, of a Jack class
//        and generate the equivalent Hack Virtual Machine Code.
//
// NOTE: the correct segment and offset is recorded with every variable in the ast
//       so the code generator does not need to use any symbol tables
//
// The skeleton code has one function per node in the abstract tree
//  - they each extract all fields into local variables
//  - they each call the appropriate walk_* function to walk any sub-trees
//
// The structure of the skeleton code is just a suggestion
//  - you may want to change the parameters / results to suit your own logic
//  - you can change it as much as you like

// forward declarations of one function per node in the abstract syntax tree
void walk_class(ast t) ;
void walk_class_var_decs(ast t) ;
void walk_var_dec(ast t) ;
void walk_subr_decs(ast t) ;
void walk_subr(ast t) ;
void walk_constructor(ast t) ;
void walk_function(ast t) ;
void walk_method(ast t) ;
void walk_param_list(ast t) ;
void walk_subr_body(ast t) ;
void walk_var_decs(ast t) ;
void walk_statements(ast t) ;
void walk_statement(ast t) ;
void walk_let(ast t) ;
void walk_let_array(ast t) ;
void walk_if(ast t) ;
void walk_if_else(ast t) ;
void walk_while(ast t) ;
void walk_do(ast t) ;
void walk_return(ast t) ;
void walk_return_expr(ast t) ;
void walk_expr(ast t) ;
void walk_term(ast t) ;
void walk_int(ast t) ;
void walk_string(ast t) ;
void walk_bool(ast t) ;
void walk_null(ast t) ;
void walk_this(ast t) ;
void walk_unary_op(ast t) ;
void walk_var(ast t) ;
void walk_array_index(ast t) ;
void walk_call_as_function(ast t) ;
void walk_call_as_method(ast t) ;
void walk_subr_call(ast t) ;
void walk_expr_list(ast t) ;
void walk_infix_op(ast t) ;

string class_name_ = "" ;           //class name
int class_var_num = 0 ;             //how many global variables in class
string class_var_decs_name = "" ;   // funtion name
int field_count = 0 ;               // for static push 

bool tr_or_fa ;                     // for walk_while() condition 
bool is_func = true ;               // function return will call, method will not call
string uop_temp = "" ;
vector<string> uop_store ;

int sub_call_num = 0 ;              // call method the number
int while_count = -1 ;              // for while
int if_count = -1 ;                 // for if
bool end_expr = false ;             // for expr
bool whe_op = false ;               // for term has operator
string operation_tem = "" ;         //store the operator

// walk an ast class node with fields:
// class_name - a string
// var_decs   - ast vector of variable declarations
// subr_decs  - ast vector of subroutine declarations
//
void walk_class(ast t)
{
    string myclassname = get_class_class_name(t) ;
    ast var_decs = get_class_var_decs(t) ;
    ast subr_decs = get_class_subr_decs(t) ;
    // get the class name used for below
    class_name_ = myclassname ;

    walk_class_var_decs(var_decs) ;
    walk_subr_decs(subr_decs) ;
}

// walk an ast class var decs node
// it is an ast vector of variable declarations
//
void walk_class_var_decs(ast t)
{
    int ndecs = size_of_class_var_decs(t) ;
    for ( int i = 0 ; i < ndecs ; i++ )
    {
        walk_var_dec(get_class_var_decs(t,i)) ;
    }
}

// walk an ast variable declaration with fields
// name - a string
// type - a string - "int", "char", "boolean" or a class name
// segment - a string
// offset - an int
// this is used for statics, fields, parameters and local variables
//
void walk_var_dec(ast t)
{
    string name = get_var_dec_name(t) ;
    string type = get_var_dec_type(t) ;
    string segment = get_var_dec_segment(t) ;
    // record the field variables number
    if (segment == "this" )
    {
        field_count++ ;
    }
}

// walk an ast class var decs node
// it is an ast vector of subroutine declarations
//
void walk_subr_decs(ast t)
{
    int size = size_of_subr_decs(t) ;
    for ( int i = 0 ; i < size ; i++ )
    {
        walk_subr(get_subr_decs(t,i)) ;
    }
}

// walk an ast subroutine node with a single field
// subr - an ast constructor, ast function or ast method node
//
void walk_subr(ast t)
{
    ast subr = get_subr_subr(t) ;
    // initialise the label number when code run into a new function
    while_count = -1 ;
    if_count = -1 ;

    switch(ast_node_kind(subr))
    {
    case ast_constructor:
        walk_constructor(subr) ;
        break ;
    case ast_function:
        walk_function(subr) ;
        break ;
    case ast_method:
        walk_method(subr) ;
        break ;
    default:
        fatal_error(0,"Unexpected subroutine kind") ;
        break ;
    }
}

// walk an ast constructor node with fields
// vtype - a string - the name of the class
// name - a string
// param list - an ast vector of variable declarations
// subr body - an ast subr body node
//
void walk_constructor(ast t)
{
    string vtype = get_constructor_vtype(t) ;
    string name = get_constructor_name(t) ;
    ast param_list = get_constructor_param_list(t) ;
    ast subr_body = get_constructor_subr_body(t) ;
    // record the constructor name 
    class_var_decs_name = name ;
    class_var_num = size_of_var_decs( get_subr_body_decs( subr_body ) ) ;

    write_to_output( "function " + class_name_ + "." + class_var_decs_name + " " + to_string(class_var_num) +"\n") ;
    walk_param_list(param_list) ;

    write_to_output("push constant " + to_string(field_count) + "\n") ;
    write_to_output("call Memory.alloc 1\n") ;
    write_to_output("pop pointer 0\n") ;

    walk_subr_body(subr_body) ;
}

// walk an ast function node with fields
// vtype - a string - one of "void", "int", "char", "boolean" or the name of a class
// name - a string
// param list - an ast vector of variable declarations
// subr body - an ast subr body node
//
void walk_function(ast t)
{
    // string vtype = get_function_vtype(t) ;
    string name = get_function_name(t) ;
    ast param_list = get_function_param_list(t) ;
    ast subr_body = get_function_subr_body(t) ;
    // consider whether is in a function 
    is_func = true ; 
    class_var_decs_name = name ;
    class_var_num = size_of_var_decs( get_subr_body_decs( subr_body ) ) ;

    write_to_output( "function " + class_name_ + "." + class_var_decs_name + " " + to_string(class_var_num) +"\n") ;
    walk_param_list(param_list) ;
    walk_subr_body(subr_body) ;
}

// walk an ast method node with fields
// vtype - a string - one of "void", "int", "char", "boolean" or the name of a class
// name - a string
// param list - an ast vector of variable declarations
// subr body - an ast subr body node
//
void walk_method(ast t)
{
    //string vtype = get_method_vtype(t) ;
    string name = get_method_name(t) ;
    ast param_list = get_method_param_list(t) ;
    ast subr_body = get_method_subr_body(t) ;
    string par_num = "" ;                           // parameter's number

    is_func = false ; 
    class_var_decs_name = name ;
    // get the local variables number
    class_var_num = size_of_var_decs( get_subr_body_decs( subr_body ) ) ;
    par_num = to_string(size_of_param_list(param_list)) ;

    write_to_output( "function " + class_name_ + "." + class_var_decs_name + " " + to_string(class_var_num) +"\n") ;
    write_to_output("push argument 0\n") ;
    write_to_output("pop pointer 0\n") ;

    walk_param_list(param_list) ;
    walk_subr_body(subr_body) ;
}

// walk an ast param list node
// it is an ast vector of variable declarations
//
void walk_param_list(ast t)
{
    int ndecs = size_of_param_list(t) ;
    for ( int i = 0 ; i < ndecs ; i++ )
    {
        walk_var_dec(get_param_list(t,i)) ;
    }
}

// walk an ast subr body node with fields
// decs - an ast vector of variable declarations
// body - an ast vector of statement nodes
//
void walk_subr_body(ast t)
{
    ast decs = get_subr_body_decs(t) ;
    ast body = get_subr_body_body(t) ;
    walk_var_decs(decs) ;
    walk_statements(body) ;
}

// walk an ast param list node
// it is an ast vector of variable declarations
//
void walk_var_decs(ast t)
{
    int ndecs = size_of_var_decs(t) ;
    for ( int i = 0 ; i < ndecs ; i++ )
    {
        walk_var_dec(get_var_decs(t,i)) ;
    }
}

// walk an ast statements node
// it is an ast vector of statement nodes
//
void walk_statements(ast t)
{
    int nstatements = size_of_statements(t) ;
    for ( int i = 0 ; i < nstatements ; i++ )
    {
        walk_statement(get_statements(t,i)) ;
    }
}

// walk an ast statement node with a single field
// statement - one of the following ast nodes, let, let array, if, if else, while, do, return, return expr or statements
//
void walk_statement(ast t)
{
    ast statement = get_statement_statement(t) ;

    switch (ast_node_kind(statement))
    {
    case ast_let:
        walk_let(statement) ;
        break ;
    case ast_let_array:
        walk_let_array(statement) ;
        break ;
    case ast_if:
        if_count++ ;                        // get the if label number
        walk_if(statement) ;
        break ;
    case ast_if_else:
        if_count++ ;                        // get the if else label number
        walk_if_else(statement) ;
        break ;
    case ast_while:
        while_count++ ;                     // get the while lable number
        walk_while(statement) ;
        break ;
    case ast_do:
        walk_do(statement) ;
        break ;
    case ast_return:
        walk_return(statement) ;
        break ;
    case ast_return_expr:
        walk_return_expr(statement) ;
        break ;
    case ast_statements:
        walk_statements(statement) ;
        break ;
    default:
        fatal_error(0,"Unexpected statement kind") ;
        break ;
    }
}

// walk an ast let node with fields
// var  - an ast variable
// expr - an ast expr node
//
void walk_let(ast t)
{
    ast var = get_let_var(t) ;
    ast expr = get_let_expr(t) ;

    walk_expr(expr) ;        //  2nd
    end_expr = true ;
    walk_var(var) ;          //  1st
    end_expr = false ;
}

// walk an ast let array node with fields
// var    - an ast variable
// index  - an ast expr node
// expr   - an ast expr node
//
void walk_let_array(ast t)
{
    ast var = get_let_array_var(t) ;
    ast index = get_let_array_index(t) ;
    ast expr = get_let_array_expr(t) ;

    walk_expr(index) ;              // 2nd
    walk_var(var) ;                 // 1st
    write_to_output("add\n") ;
    walk_expr(expr) ;               // 3rd
    write_to_output("pop temp 0\n") ;
    write_to_output("pop pointer 1\n") ;
    write_to_output("push temp 0\n") ;
    write_to_output("pop that 0\n") ;
}

// walk an ast if node with fields
// condition - an ast expr node
// if true   - an ast statements node
//
void walk_if(ast t)
{
    ast condition = get_if_condition(t) ;
    ast if_true = get_if_if_true(t) ;
    int temp = if_count ;                   // used for the false label number

    walk_expr(condition) ;

    write_to_output("if-goto IF_TRUE" + to_string(if_count) + "\n" ) ;
    write_to_output("goto IF_FALSE" + to_string(if_count) + "\n" ) ;
    write_to_output("label IF_TRUE" + to_string(if_count) + "\n" ) ;

    walk_statements(if_true) ;

    write_to_output("label IF_FALSE" + to_string(temp) + "\n" ) ;
}

// walk an ast if else node with fields
// condition - an ast expr node
// if true   - an ast statements node
// if else   - an ast statements node
//
void walk_if_else(ast t)
{   
    ast condition = get_if_else_condition(t) ;
    ast if_true = get_if_else_if_true(t) ;
    ast if_false = get_if_else_if_false(t) ;
    int temp = if_count ;                   // used for the false label number

    int false_state_num = size_of_statements(if_false) ;

    walk_expr(condition) ;

    write_to_output("if-goto IF_TRUE" + to_string(if_count) + "\n" ) ;
    write_to_output("goto IF_FALSE" + to_string(if_count) + "\n" ) ;
    write_to_output("label IF_TRUE" + to_string(if_count) + "\n" ) ;

    walk_statements(if_true) ;

    write_to_output("goto IF_END" + to_string(temp) + "\n" ) ;
    write_to_output("label IF_FALSE" + to_string(temp) + "\n" ) ;
    if(false_state_num == 0)
    {
        write_to_output("label IF_END" + to_string(temp) + "\n" ) ;
    }

    walk_statements(if_false) ;

    if(false_state_num != 0)
    {
        write_to_output("label IF_END" + to_string(temp) + "\n" ) ;
    }
}

// walk an ast while node with fields
// condition - an ast expr node
// body      - an ast statements node
//
void walk_while(ast t)
{
    ast condition = get_while_condition(t) ;
    ast body = get_while_body(t) ;
    int temp = while_count ;                     // used for the false label number

    write_to_output("label WHILE_EXP" + to_string(while_count) + "\n") ;

    walk_expr(condition) ;

    write_to_output("not\n") ;  
    write_to_output("if-goto WHILE_END" + to_string(while_count) + "\n") ;

    walk_statements(body) ;

    write_to_output("goto WHILE_EXP" + to_string(temp) + "\n" ) ;
    write_to_output("label WHILE_END" + to_string(temp) + "\n" ) ;
}

// walk an ast do node with a single field
// call - an ast call as function node or an ast call as method node
//
void walk_do(ast t)
{
    ast call = get_do_call(t) ;

    switch(ast_node_kind(call))
    {
    case ast_call_as_function:
        walk_call_as_function(call) ;
        write_to_output("pop temp 0\n") ; 
        break ;
    case ast_call_as_method:
        walk_call_as_method(call) ;
        write_to_output("pop temp 0\n") ; 
        break ;
    default:
        fatal_error(0,"Unexpected call kind") ;
        break ;
    }
}

// walk an ast return node, it has not fields
//
void walk_return(ast t)
{
    write_to_output("push constant 0\n") ;
    write_to_output("return\n") ;
} 

// walk an ast return expr node with a single field
// expr - an ast expr node
//
void walk_return_expr(ast t)
{
    ast expr = get_return_expr(t) ;

    walk_expr(expr) ;
    write_to_output("return\n") ;
}

// walk an ast param list node
// it is an ast vector of ast term and infix op nodes
// it must be of odd length > 0, ie 1, 3, 5, 7, ...
// all elements at even indices are an ast term node
// all elements at odd indices are an ast infix op
//
void walk_expr(ast t)
{
    int term_ops = size_of_expr(t) ;

    for ( int i = 0 ; i < term_ops ; i++ )
    {
        ast term_op = get_expr(t,i) ;
        if ( i % 2 == 0 )
        {
            walk_term(term_op) ;
        }
        else
        {
            walk_infix_op(term_op) ;
            whe_op = true ;
        }
    }
}

// walk an ast term node with a single field
// term - one of the following ast nodes:
//        int, string, bool, null, this, expr, unary op,
//        variable, array index, call as function or call as method
//
void walk_term(ast t)
{
    ast term = get_term_term(t) ;
    string be_operation_tem = "" ;          // for ast_expr
    bool be_whe_op = false ;                // for ast_expr
    string be_un_operation_tem = "" ;       // for ast_unary_op
    bool be_un_whe_op = false ;             // for ast_unary_op
    string be_func_op = "" ;                // for ast_call_as_function
    bool be_func_whe_op = false ;           // for ast_call_as_function
    string be_meth_op = "" ;                // for ast_call_as_method
    bool be_meth_whe_op = false ;           // for ast_call_as_method
    string be_arr_op = "" ;                 // for array
    bool be_arr_whe_op = false ;            // for array
    string be_null_op = "" ;                // for null

    switch(ast_node_kind(term))
    {
    case ast_int:
        walk_int(term) ;
        break ;
    case ast_string:
        walk_string(term) ;
        break ;
    case ast_bool:
        walk_bool(term) ;
        break ;
    case ast_null:
        walk_null(term) ;
        if ( whe_op )
        {
            write_to_output(operation_tem + "\n") ;
            whe_op = false ;
        }
        break ;
    case ast_this:
        walk_this(term) ;
        break ;
    case ast_expr:
        be_operation_tem = operation_tem ;
        be_whe_op = whe_op ;
        whe_op = false ;
        walk_expr(term) ;
        if(be_whe_op)
        {
            write_to_output(be_operation_tem + "\n") ;
            whe_op = false ;
        }
        break ;
    case ast_unary_op:
        be_un_operation_tem = operation_tem ;
        be_un_whe_op = whe_op ;
        whe_op = false ;
        walk_unary_op(term) ;
        if (be_un_whe_op)
        {
            write_to_output(be_un_operation_tem + "\n") ;
            whe_op = false ;
        }
        break ;
    case ast_var:
        walk_var(term) ;
        break ;
    case ast_array_index:
        be_arr_op = operation_tem ;
        be_arr_whe_op = whe_op ;
        whe_op = false ;
        walk_array_index(term) ;
        if (be_arr_whe_op)
        {
            write_to_output(be_arr_op + "\n") ;
            whe_op = false ;
        }
        break ;
    case ast_call_as_function:
        be_func_op = operation_tem ;
        be_func_whe_op = whe_op ;
        whe_op = false ;
        walk_call_as_function(term) ;
        if ( be_func_whe_op)
        {
            write_to_output(be_func_op + "\n") ;
            whe_op = false ;
        }
        break ;
    case ast_call_as_method:
        be_meth_op = operation_tem ;
        be_meth_whe_op = whe_op ;
        whe_op = false ;
        walk_call_as_method(term) ;
        if ( be_meth_whe_op )
        {
            write_to_output(be_meth_op + "\n") ;
            whe_op = false ;
        }
        break ;
    default:
        fatal_error(0,"Unexpected term kind") ;
        break ;
    }
}

// walk an ast int node with a single field
// constant - an integer in the range -32,768 to 32,767
//
void walk_int(ast t)
{
    int _constant = get_int_constant(t) ;
    if ( _constant > 32767 || _constant < -32768)
    {
        fatal_error(0,"Out of range!") ;
    }
    write_to_output("push constant " + to_string(_constant) + "\n") ;
    if (whe_op)
    {
        write_to_output(operation_tem + "\n") ;
        whe_op = false ;
    }
}

// walk an ast string node with a single field
// constant - a string
//
void walk_string(ast t)
{
    string _constant = get_string_constant(t) ;
    write_to_output("push constant " + to_string(_constant.size()) + "\n" ) ;
    write_to_output("call String.new 1\n") ;
    for (int i = 0; i < _constant.size(); i++)
    {
        write_to_output("push constant " + to_string(_constant[i]) + "\n" ) ;
        write_to_output("call String.appendChar 2\n") ;
    }
}

// walk an ast bool node with a single field
// constant - either true or false
//
void walk_bool(ast t)
{
    bool _constant = get_bool_t_or_f(t) ;
    tr_or_fa = _constant ;
    write_to_output("push constant 0\n") ;
    if(_constant)
    {
        write_to_output("not\n") ;
    }
}

// walk an ast null node, it has not fields
//
void walk_null(ast t)
{
    write_to_output("push constant 0\n") ;
}

// walk an ast this node, it has not fields
//
void walk_this(ast t)
{
    write_to_output("push pointer 0\n") ;
}

// walk an ast unary op node with fields
// op   - a string, either "-" or "~"
// term - an ast term node
//        int, string, bool, null, this, expr, unary op,
//        variable, array index, call as function or call as method
//
void walk_unary_op(ast t)
{
    string uop = get_unary_op_op(t);
    ast term = get_unary_op_term(t) ;
    walk_term(term) ;
    if ( uop == "-" )
    {
        write_to_output("neg\n") ;
    }
    else if ( uop == "~" )
    {
        write_to_output("not\n") ;
    }
    else 
    {
        fatal_error(0,"Expected unary operator!") ;
    }
}

// walk an ast variable node with fields
// name - a string
// type - a string - "int", "char", "boolean" or a class name
// segment - a string
// offset - an int
//
void walk_var(ast t)
{
    string name = get_var_name(t) ;
    string type = get_var_type(t) ;
    string segment = get_var_segment(t) ;
    int offset = get_var_offset(t) ;
    if (!end_expr)
    {
        write_to_output("push " + segment + " " + to_string(offset) + "\n") ;
    }
    if (whe_op)
    {
        write_to_output(operation_tem + "\n") ;
        whe_op = false ;
    }
    if (end_expr)
    {
        write_to_output("pop " + segment + " " + to_string(offset) + "\n") ;
    }
}

// walk an ast array index node with fields
// var   - an ast variable node
// index - an ast expr node
//
void walk_array_index(ast t)
{
    ast var = get_array_index_var(t) ;
    ast index = get_array_index_index(t) ;

    walk_expr(index) ;
    walk_var(var) ;
    write_to_output("add\n") ;
    write_to_output("pop pointer 1\n") ;
    write_to_output("push that 0\n") ;
}

// walk an ast subr call as method with fields
// class name - a string
// call       - an ast subr call node
//
void walk_call_as_function(ast t)
{
    string class_name = get_call_as_function_class_name(t) ;
    ast subr_call = get_call_as_function_subr_call(t) ;
    walk_subr_call(subr_call) ;
    write_to_output("call " + class_name + "." + get_subr_call_subr_name(subr_call) + " " + to_string(sub_call_num) + "\n") ;
}

// walk an ast subr call as method with fields
// class name - a string
// var        - an ast variable or ast this node, hidden first parameter of the call
// call       - an ast subr call node
//
void walk_call_as_method(ast t)
{
    string class_name = get_call_as_method_class_name(t) ;
    ast var = get_call_as_method_var(t) ;
    ast subr_call = get_call_as_method_subr_call(t) ;

    switch(ast_node_kind(var))
    {
    case ast_this:
        walk_this(var) ;
        break ;
    case ast_var:
        walk_var(var) ;
        break ;
    default:
        fatal_error(0,"Expected var or this") ;
        break ;
    }
    walk_subr_call(subr_call) ;
    write_to_output("call " + class_name + "." + get_subr_call_subr_name(subr_call) + " " + to_string(sub_call_num+1) + "\n") ;
}

// walk an ast subr call node with fields
// name      - a string
// expr list - a vector of ast expr nodes
//
void walk_subr_call(ast t)
{
    string subr_name = get_subr_call_subr_name(t) ;
    ast expr_list = get_subr_call_expr_list(t) ;

    walk_expr_list(expr_list) ;
}

// walk an ast expr list node
// it is an ast vector of ast expr nodes
//
void walk_expr_list(ast t)
{
    int nexpressions = size_of_expr_list(t) ;
    sub_call_num = nexpressions ;
    for ( int i = 0 ; i < nexpressions ; i++ )
    {
        walk_expr(get_expr_list(t,i)) ;
    }
}

// walk an ast infix op node with a single field
// op - a string - one of "+", "-", "*", "/", "&", "|", "<", ">", "="
//
void walk_infix_op(ast t)
{
    string op = get_infix_op_op(t) ;
    if (op == "+")
    {
        operation_tem = "add" ;
    }
    else if (op == "-")
    {
        operation_tem = "sub" ;
    }
    else if (op == "*")
    {
        operation_tem = "call Math.multiply 2" ;
    }
    else if (op == "/")
    {
        operation_tem = "call Math.divide 2" ;
    }
    else if (op == "&")
    {
        operation_tem = "and" ;
    }
    else if (op == "|")
    {
        operation_tem = "or" ;
    }
    else if (op == "<")
    {
        operation_tem = "lt" ;
    }
    else if (op == ">")
    {
        operation_tem = "gt" ;
    }
    else if (op == "=")
    {
        operation_tem = "eq" ;
    }
    else
    {
        fatal_error(0,"Expected operator!") ;
    }

}

// main program
int main(int argc,char **argv)
{
    // walk an AST parsed from XML and print VM code
    walk_class(ast_parse_xml()) ;

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}

