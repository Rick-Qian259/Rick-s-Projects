// convert Hack VM Code into Hack Assembler

#include "iobuffer.h"
#include "tokeniser.h"
#include "assembler-internal.h"

// to make out programs a bit neater
using namespace std ;

using namespace CS_IO_Buffers ;
using namespace Hack_Virtual_Machine ;

////////////////////////////////////////////////////////////////
/************     MODIFY CODE BETWEEN HERE     **************/
static int compare_count = 0 ;
static string func_name = "" ;
static int call_count = 0; 

static void vm_add()
{
    output_assembler("@SP") ;
    output_assembler("AM=M-1") ;
    output_assembler("D=M") ;
    output_assembler("A=A-1") ;
    output_assembler("M=D+M") ;
}

static void vm_sub()
{
    output_assembler("@SP") ;
    output_assembler("AM=M-1") ;
    output_assembler("D=M") ;
    output_assembler("A=A-1") ;
    output_assembler("M=M-D") ;
    output_assembler("D=A+1") ;
    output_assembler("@SP") ;
    output_assembler("M=D") ;
}

static void vm_and()
{
    output_assembler("@SP") ;
    output_assembler("AM=M-1") ;
    output_assembler("D=M") ;
    output_assembler("A=A-1") ;
    output_assembler("M=D&M") ;
}

static void vm_or()
{
    output_assembler("@SP") ;
    output_assembler("AM=M-1") ;
    output_assembler("D=M") ;
    output_assembler("A=A-1") ;
    output_assembler("M=D|M") ;
}

static void vm_neg()
{
    output_assembler("@SP") ;
    output_assembler("A=M-1") ;
    output_assembler("M=-M") ;
}

static void vm_not()
{
    output_assembler("@SP") ;
    output_assembler("A=M-1") ;
    output_assembler("M=!M") ;
}

static void vm_eq(string func_name, int c)
{
    output_assembler("@SP") ;
    output_assembler("AM=M-1") ;
    output_assembler("D=M") ;
    output_assembler("A=A-1") ;
    output_assembler("D=M-D") ;
    output_assembler("@" + func_name + "$EQT" + to_string(c)) ;		// equal to zero
    output_assembler("D;JEQ") ;
    output_assembler("@" + func_name + "$NEQ" + to_string(c)) ;		// not equal to zero
    output_assembler("0;JMP") ;
    output_assembler("(" + func_name + "$EQT" + to_string(c)+")") ;	//(EQT)
    output_assembler("@SP") ;
    output_assembler("A=M-1") ;
    output_assembler("M=-1") ;
    output_assembler("@" + func_name + "$END" + to_string(c)) ;		// @END
    output_assembler("0;JMP") ;
    output_assembler("") ;
    output_assembler("(" + func_name + "$NEQ" + to_string(c)+")") ;	//(NEQ)
    output_assembler("@SP") ;
    output_assembler("A=M-1") ;
    output_assembler("M=0") ;
    output_assembler("@" + func_name + "$END" + to_string(c)) ;
    output_assembler("0;JMP") ;
    output_assembler("(" + func_name + "$END" + to_string(c)+")") ;	// (END)
}

static void vm_gt(string func_name, int c)
{
    output_assembler("@SP") ;		//RAM[SP-2] = R13 
    output_assembler("AM=M-1") ;	//RAM[SP-1] = R14
    output_assembler("D=M") ;
    output_assembler("@14") ;
    output_assembler("M=D") ;
    output_assembler("@SP") ;
    output_assembler("A=M-1") ;
    output_assembler("D=M") ;
    output_assembler("@13") ;
    output_assembler("M=D") ;

    output_assembler("@14") ;
    output_assembler("D=M") ;
    output_assembler("@" + func_name + "$YLTZ" + to_string(c)) ;	//y less than zero
    output_assembler("D;JLT") ;
    output_assembler("@" + func_name + "$YGTZ" + to_string(c)) ;	//y greater than zero
    output_assembler("D;JGE") ;

    output_assembler("(" + func_name + "$YLTZ" + to_string(c)+")") ;	// y < 0, then judge x < 0 ?
    output_assembler("@13") ;
    output_assembler("D=M") ;
    output_assembler("@" + func_name + "$RET" + to_string(c)) ;		// result true
    output_assembler("D;JGE") ;
    output_assembler("@14") ;
    output_assembler("D=D-M") ;
    output_assembler("@" + func_name + "$RET" + to_string(c)) ;		// result true
    output_assembler("D;JGT") ;
    output_assembler("@" + func_name + "$REF" + to_string(c)) ;		// result false
    output_assembler("D;JLE") ;

    output_assembler("(" + func_name + "$YGTZ" + to_string(c)+")") ;
    output_assembler("@13") ;
    output_assembler("D=M") ;
    output_assembler("@" + func_name + "$REF" + to_string(c)) ;		// result false
    output_assembler("D;JLE") ;
    output_assembler("@14") ;
    output_assembler("D=D-M") ;
    output_assembler("@" + func_name + "$RET" + to_string(c)) ;		// result true
    output_assembler("D;JGT") ;
    output_assembler("@" + func_name + "$REF" + to_string(c)) ;		// result false
    output_assembler("D;JLE") ;

    output_assembler("(" + func_name + "$RET" + to_string(c)+")") ;	// (RET) 
    output_assembler("@SP") ;
    output_assembler("A=M-1") ;
    output_assembler("M=-1") ;
    output_assembler("@" + func_name + "$END" + to_string(c)) ;		// end
    output_assembler("0;JMP") ;
    output_assembler("(" + func_name + "$REF" + to_string(c)+")") ;	// (REF) 
    output_assembler("@SP") ;
    output_assembler("A=M-1") ;
    output_assembler("M=0") ;
    output_assembler("@" + func_name + "$END" + to_string(c)) ;		// end
    output_assembler("0;JMP") ;
    output_assembler("(" + func_name + "$END" + to_string(c)+")") ;	// (END)
} 

static void vm_lt(string func_name, int c)
{
    output_assembler("@SP") ;		//RAM[SP-2] = R13 
    output_assembler("AM=M-1") ;	//RAM[SP-1] = R14
    output_assembler("D=M") ;
    output_assembler("@14") ;
    output_assembler("M=D") ;
    output_assembler("@SP") ;
    output_assembler("A=M-1") ;
    output_assembler("D=M") ;
    output_assembler("@13") ;
    output_assembler("M=D") ;

    output_assembler("@14") ;
    output_assembler("D=M") ;
    output_assembler("@" + func_name + "$YLTZ" + to_string(c)) ;	//y less than zero
    output_assembler("D;JLT") ;
    output_assembler("@" + func_name + "$YGTZ" + to_string(c)) ;	//y greater than zero
    output_assembler("D;JGE") ;

    output_assembler("(" + func_name + "$YLTZ" + to_string(c)+")") ;	// y < 0, then judge x < 0 ?
    output_assembler("@13") ;
    output_assembler("D=M") ;
    output_assembler("@" + func_name + "$REF" + to_string(c)) ;		// result false
    output_assembler("D;JGE") ;
    output_assembler("@14") ;
    output_assembler("D=D-M") ;
    output_assembler("@" + func_name + "$RET" + to_string(c)) ;		// result true
    output_assembler("D;JLT") ;
    output_assembler("@" + func_name + "$REF" + to_string(c)) ;		// result false
    output_assembler("D;JGE") ;

    output_assembler("(" + func_name + "$YGTZ" + to_string(c)+")") ;	// y > 0, then if x <= 0 ?
    output_assembler("@13") ;
    output_assembler("D=M") ;
    output_assembler("@" + func_name + "$RET" + to_string(c)) ;		// result true
    output_assembler("D;JLT") ;
    output_assembler("@14") ;
    output_assembler("D=D-M") ;
    output_assembler("@" + func_name + "$RET" + to_string(c)) ;		// result true
    output_assembler("D;JLT") ;
    output_assembler("@" + func_name + "$REF" + to_string(c)) ;		// result false
    output_assembler("D;JGE") ;

    output_assembler("(" + func_name + "$RET" + to_string(c)+")") ;	// (RET) 
    output_assembler("@SP") ;
    output_assembler("A=M-1") ;
    output_assembler("M=-1") ;
    output_assembler("@" + func_name + "$END" + to_string(c)) ;		// end
    output_assembler("0;JMP") ;
    output_assembler("(" + func_name + "$REF" + to_string(c)+")") ;	// (REF) 
    output_assembler("@SP") ;
    output_assembler("A=M-1") ;
    output_assembler("M=0") ;
    output_assembler("@" + func_name + "$END" + to_string(c)) ;		// end
    output_assembler("0;JMP") ;
    output_assembler("(" + func_name + "$END" + to_string(c)+")") ;	// (END)
} 


static void func_return()
{
    output_assembler("@LCL") ;
    output_assembler("D=M") ;
    output_assembler("@13") ;
    output_assembler("M=D") ;		//R13 = LCL
    output_assembler("@5") ;
    output_assembler("D=A") ;
    output_assembler("@13") ;
    output_assembler("A=M-D") ;
    output_assembler("D=M") ;
    output_assembler("@14") ;
    output_assembler("M=D") ;		//return address = R13-5
    
    output_assembler("@ARG") ;
    output_assembler("D=M") ;
    output_assembler("@15") ;
    output_assembler("M=D") ;		// ARG=POP()

    output_assembler("@SP") ;
    output_assembler("AM=M-1") ;	
    output_assembler("D=M") ;
    output_assembler("@15") ;
    output_assembler("A=M") ;
    output_assembler("M=D") ;		
    output_assembler("@ARG") ;
    output_assembler("D=M+1") ;
    output_assembler("@SP") ;
    output_assembler("M=D") ;		// SP = ARG + 1

    output_assembler("@13") ;
    output_assembler("AM=M-1") ;
    output_assembler("D=M") ;
    output_assembler("@THAT") ;
    output_assembler("M=D") ;		// THAT = R13 -1

    output_assembler("@13") ;
    output_assembler("AM=M-1") ;
    output_assembler("D=M") ;
    output_assembler("@THIS") ;
    output_assembler("M=D") ;		// THIS = R13 -2
 
    output_assembler("@13") ;
    output_assembler("AM=M-1") ;
    output_assembler("D=M") ;
    output_assembler("@ARG") ;
    output_assembler("M=D") ;		// ARG = R13 -3

    output_assembler("@13") ;
    output_assembler("A=M-1") ;
    output_assembler("D=M") ;
    output_assembler("@LCL") ;
    output_assembler("M=D") ;		// LCL = R13 -4

    output_assembler("@14") ;
    output_assembler("A=M") ;
    output_assembler("0;JMP") ;		// GOTO return address

}

// translate vm operator command into assembly language
static void translate_vm_operator(TokenKind the_op)
{
    start_of_vm_operator_command(the_op) ;
// + - & | ! = > < return
    switch(the_op)
    {
    case tk_add:
        vm_add() ;
        break ;
    case tk_sub:
        vm_sub() ;
        break ;
    case tk_and:
        vm_and() ;
        break ;
    case tk_or:
        vm_or() ;
        break ;
    case tk_neg:
        vm_neg() ;
        break ;
    case tk_not:
        vm_not() ;
        break ;
    case tk_eq:
        vm_eq(func_name, compare_count) ;
	compare_count++ ;
        break ;
    case tk_gt:
        vm_gt(func_name, compare_count) ;
	compare_count++ ;
        break ;
    case tk_lt:
        vm_lt(func_name, compare_count) ;
	compare_count++ ;
        break ;
    case tk_return:
	func_return() ;
        break ;

    default: ;
    }
    end_of_vm_command() ;
}

// translate vm jump command into assembly language
static void translate_vm_jump(TokenKind jump, string label)
{
    start_of_vm_jump_command(jump,label) ;
// goto if-goto label
    switch(jump)
    {
	case tk_goto:
	    output_assembler("@" + func_name + "$" + label) ;
	    output_assembler("0;JMP") ;
	    break ;
	case tk_if_goto:
	    output_assembler("@SP") ;
	    output_assembler("AM=M-1") ;
	    output_assembler("D=M") ;
	    output_assembler("@" + func_name + "$" + label) ;
	    output_assembler("D;JNE") ;
       	    break ;
	case tk_label:
	    output_assembler("(" + func_name + "$" + label + ")") ;
	    break ;

	default : ;
    }

    end_of_vm_command() ;
}

static void func_function()
{
    output_assembler("@SP") ;
    output_assembler("A=M") ;
    output_assembler("M=0") ;
    output_assembler("@SP") ;
    output_assembler("M=M+1") ;
}


static void call_lcl()
{
    output_assembler("@LCL") ;
    output_assembler("A=M") ;
    output_assembler("D=A") ;
    output_assembler("@SP") ;
    output_assembler("A=M") ;
    output_assembler("M=D") ;
    output_assembler("@SP") ;
    output_assembler("M=M+1") ;
}

static void call_arg()
{
    output_assembler("@ARG") ;
    output_assembler("A=M") ;
    output_assembler("D=A") ;
    output_assembler("@SP") ;
    output_assembler("A=M") ;
    output_assembler("M=D") ;
    output_assembler("@SP") ;
    output_assembler("M=M+1") ;
}

static void call_this()
{
    output_assembler("@THIS") ;
    output_assembler("A=M") ;
    output_assembler("D=A") ;
    output_assembler("@SP") ;
    output_assembler("A=M") ;
    output_assembler("M=D") ;
    output_assembler("@SP") ;
    output_assembler("M=M+1") ;
}

static void call_that()
{
    output_assembler("@THAT") ;
    output_assembler("A=M") ;
    output_assembler("D=A") ;
    output_assembler("@SP") ;
    output_assembler("A=M") ;
    output_assembler("M=D") ;
    output_assembler("@SP") ;
    output_assembler("M=M+1") ;
}

static void func_call(int count, int num, string func_name)
{
    output_assembler("@" +func_name + "$ret." + to_string(count) ) ;
    output_assembler("D=A") ;
    output_assembler("@SP") ;
    output_assembler("A=M") ;
    output_assembler("M=D") ;
    output_assembler("@SP") ;
    output_assembler("M=M+1") ;
    call_lcl() ;
    call_arg() ;
    call_this() ;
    call_that() ;
    output_assembler("@" + to_string(5+num)) ;
    output_assembler("D=A") ;
    output_assembler("@R14") ;
    output_assembler("M=D") ;
    output_assembler("@SP") ;
    output_assembler("A=M-D") ;
    output_assembler("D=A") ;
    output_assembler("@ARG") ;
    output_assembler("M=D") ;
    output_assembler("@SP") ;
    output_assembler("D=M") ;
    output_assembler("@LCL") ;
    output_assembler("M=D") ;
}


// translate vm call or function command into assembly language
static void translate_vm_function(TokenKind func, string label, int n)
{
    start_of_vm_func_command(func,label,n) ;

    if ( func == tk_function )
    {
        func_name = label ;
	output_assembler("(" + label + ")" ) ;
	// push on n 0s to the stack

	for( int i = 0; i < n; i++ )
	{
	    func_function() ;
	}
    }
    else if ( func == tk_call )
    {
	func_call(call_count, n, func_name) ;
        output_assembler("@" + label ) ;
        output_assembler("0;JMP") ;
        output_assembler("(" + func_name + "$ret." + to_string(call_count)+")") ;	// (ret.)     
	call_count++ ;
    }

    end_of_vm_command() ;
}


static void pop_pointer_0()
{
    output_assembler("@SP") ;
    output_assembler("AM=M-1") ;
    output_assembler("D=M") ;
    output_assembler("@THIS") ;
    output_assembler("M=D") ;
}

static void pop_pointer_1()
{
    output_assembler("@SP") ;
    output_assembler("AM=M-1") ;
    output_assembler("D=M") ;
    output_assembler("@THAT") ;
    output_assembler("M=D") ;
}

static void pop_temp(int offset)
{
    output_assembler("@5") ;
    output_assembler("D=A") ;
    output_assembler("@" + to_string(offset)) ;
    output_assembler("D=D+A") ;
    output_assembler("@13") ;
    output_assembler("M=D") ;
    output_assembler("@SP") ;
    output_assembler("AM=M-1") ;
    output_assembler("D=M") ;
    output_assembler("@13") ;
    output_assembler("A=M") ;
    output_assembler("M=D") ;
}

static void pop_static(int offset)
{
    output_assembler("@16") ;
    output_assembler("D=A") ;
    output_assembler("@" + to_string(offset)) ;
    output_assembler("D=D+A") ;
    output_assembler("@13") ;
    output_assembler("M=D") ;
    output_assembler("@SP") ;
    output_assembler("AM=M-1") ;
    output_assembler("D=M") ;
    output_assembler("@13") ;
    output_assembler("A=M") ;
    output_assembler("M=D") ;
}

static void pop_lcl(int offset)
{
    output_assembler("@LCL") ;
    output_assembler("D=M") ;
    output_assembler("@" + to_string(offset)) ;
    output_assembler("D=D+A") ;
    output_assembler("@13") ;
    output_assembler("M=D") ;
    output_assembler("@SP") ;
    output_assembler("AM=M-1") ;
    output_assembler("D=M") ;
    output_assembler("@13") ;
    output_assembler("A=M") ;
    output_assembler("M=D") ;
}

static void pop_arg(int offset)
{
    output_assembler("@ARG") ;
    output_assembler("D=M") ;
    output_assembler("@" + to_string(offset)) ;
    output_assembler("D=D+A") ;
    output_assembler("@13") ;
    output_assembler("M=D") ;
    output_assembler("@SP") ;
    output_assembler("AM=M-1") ;
    output_assembler("D=M") ;
    output_assembler("@13") ;
    output_assembler("A=M") ;
    output_assembler("M=D") ;
}

static void pop_this(int offset)
{
    output_assembler("@THIS") ;
    output_assembler("D=M") ;
    output_assembler("@" + to_string(offset)) ;
    output_assembler("D=D+A") ;
    output_assembler("@13") ;
    output_assembler("M=D") ;
    output_assembler("@SP") ;
    output_assembler("AM=M-1") ;
    output_assembler("D=M") ;
    output_assembler("@13") ;
    output_assembler("A=M") ;
    output_assembler("M=D") ;
}

static void pop_that(int offset)
{
    output_assembler("@THAT") ;
    output_assembler("D=M") ;
    output_assembler("@" + to_string(offset)) ;
    output_assembler("D=D+A") ;
    output_assembler("@13") ;
    output_assembler("M=D") ;
    output_assembler("@SP") ;
    output_assembler("AM=M-1") ;
    output_assembler("D=M") ;
    output_assembler("@13") ;
    output_assembler("A=M") ;
    output_assembler("M=D") ;
}

static void push_constant(int offset)
{
    output_assembler("@" + to_string(offset)) ;
    output_assembler("D=A") ;
    output_assembler("@SP") ;
    output_assembler("AM=M+1") ;
    output_assembler("A=A-1") ;
    output_assembler("M=D") ;
}

static void push_static(int offset)
{
    output_assembler("@" + to_string(offset)) ;
    output_assembler("D=A") ;
    output_assembler("@16") ;
    output_assembler("A=D+A") ;
    output_assembler("D=M") ;
    output_assembler("@SP") ;
    output_assembler("AM=M+1") ;
    output_assembler("A=A-1") ;
    output_assembler("M=D") ;
}

static void push_temp(int offset)
{
    output_assembler("@" + to_string(offset)) ;
    output_assembler("D=A") ;
    output_assembler("@5") ;
    output_assembler("A=D+A") ;
    output_assembler("D=M") ;
    output_assembler("@SP") ;
    output_assembler("AM=M+1") ;
    output_assembler("A=A-1") ;
    output_assembler("M=D") ;
}

static void push_lcl(int offset)
{
    output_assembler("@LCL") ;
    output_assembler("D=M") ;
    output_assembler("@" + to_string(offset)) ;
    output_assembler("A=D+A") ;
    output_assembler("D=M") ;
    output_assembler("@SP") ;
    output_assembler("AM=M+1") ;
    output_assembler("A=A-1") ;
    output_assembler("M=D") ;
}

static void push_arg(int offset)
{
    output_assembler("@ARG") ;
    output_assembler("D=M") ;
    output_assembler("@" + to_string(offset)) ;
    output_assembler("A=D+A") ;
    output_assembler("D=M") ;
    output_assembler("@SP") ;
    output_assembler("AM=M+1") ;
    output_assembler("A=A-1") ;
    output_assembler("M=D") ;
}

static void push_this(int offset)
{
    output_assembler("@THIS") ;
    output_assembler("D=M") ;
    output_assembler("@" + to_string(offset)) ;
    output_assembler("A=D+A") ;
    output_assembler("D=M") ;
    output_assembler("@SP") ;
    output_assembler("AM=M+1") ;
    output_assembler("A=A-1") ;
    output_assembler("M=D") ;
}

static void push_that(int offset)
{
    output_assembler("@THAT") ;
    output_assembler("D=M") ;
    output_assembler("@" + to_string(offset)) ;
    output_assembler("A=D+A") ;
    output_assembler("D=M") ;
    output_assembler("@SP") ;
    output_assembler("AM=M+1") ;
    output_assembler("A=A-1") ;
    output_assembler("M=D") ;
}

static void push_pointer_0()
{
    output_assembler("@3") ;
    output_assembler("D=M") ;
    output_assembler("@SP") ;
    output_assembler("AM=M+1") ;
    output_assembler("A=A-1") ;
    output_assembler("M=D") ;
}

static void push_pointer_1()
{
    output_assembler("@4") ;
    output_assembler("D=M") ;
    output_assembler("@SP") ;
    output_assembler("AM=M+1") ;
    output_assembler("A=A-1") ;
    output_assembler("M=D") ;
}

// translate vm push or pop command into assembly language
static void translate_vm_stack(TokenKind stack,TokenKind segment,int offset)
{
    start_of_vm_stack_command(stack,segment,offset) ;

    // pop into stack
    if( stack == tk_pop )
    {
	switch(segment)
     	{
	case tk_pointer:
	    if(offset == 0)
	    {
 	        pop_pointer_0() ;
	    }
	    else
	    {
	        pop_pointer_1() ;
	    }
	    break ;
	case tk_temp:
	    pop_temp(offset) ;
	    break;
	case tk_static:
	    pop_static(offset) ;
 	    break;
     	case tk_local:
	    pop_lcl(offset) ;
	    break;
	case tk_argument:
	    pop_arg(offset) ;
	    break;
	case tk_this:
	    pop_this(offset) ;
	    break;
	case tk_that:
	    pop_that(offset) ;
	    break;

	default: ;
	}
    }
    // push in to stack
    else
    {
	switch(segment)
	{
    	case tk_constant:
	    push_constant(offset) ;
	    break ;
	case tk_static:
	    push_static(offset) ;
            break ;
	case tk_temp:
	    push_temp(offset) ;
	    break ;
	case tk_local:
	    push_lcl(offset) ;
	    break;
	case tk_argument:
	    push_arg(offset) ;
	    break;
	case tk_this:
	    push_this(offset) ;
    	    break;
    	case tk_that:
	    push_that(offset) ;
	    break;
	case tk_pointer:
	    if(offset == 0)
	    {
	        push_pointer_0() ;
	    }
	    else
	    {
		push_pointer_1() ;
	    }
	    break;
	    default: ;
	}
    }
    end_of_vm_command() ;
}


/************            AND HERE              **************/
///////////////////////////////////////////////////////////////

// grammar to be parsed:
// vm_class ::= command* eoi
// command ::= vm_operator | vm_jump | vm_func | vm_stack
// vm_operator ::= 'add' | 'and' | 'eq' | 'gt' | 'lt' | 'neg' | 'not' | 'or' | 'sub' | 'return'
// vm_jump ::= jump label
// vm_func ::= func label number
// vm_stack ::= stack segment number

// forward declare parsing functions - one per rule
static void parse_class() ;
static void parse_command() ;
static void parse_op() ;
static void parse_jump() ;
static void parse_func() ;
static void parse_stack() ;

// Note: always read one token after the one recognised

// vm_class ::= command* eoi
static void parse_class()
{
    // tell the output system we are starting to translate VM commands for a Jack class
    start_of_vm_class() ;

    while ( have(tk_vm_command) ) parse_command() ;
    mustbe(tk_eoi) ;

    // tell the output system we have just finished translating VM commands for a Jack class
    end_of_vm_class() ;
}

// command ::= vm_operator | vm_jump | vm_func | vm_stack
static void parse_command()
{
    if ( have(tk_vm_operator) ) parse_op() ; else
    if ( have(tk_vm_jump) )     parse_jump() ; else
    if ( have(tk_vm_func) )     parse_func() ; else
    if ( have(tk_vm_stack) )    parse_stack() ; else
    mustbe(tk_vm_command) ;
}

// vm_operator ::= 'add' | 'and' | 'eq' | 'gt' | 'lt' | 'neg' | 'not' | 'or' | 'sub' | 'return'
static void parse_op()
{
    TokenKind the_op = token_kind(mustbe(tk_vm_operator)) ;

    translate_vm_operator(the_op) ;
}

// vm_jump ::= jump label
// jump command - label, goto label or if-goto label
static void parse_jump()
{
    TokenKind command = token_kind(mustbe(tk_vm_jump)) ;
    string label = token_spelling(mustbe(tk_a_label)) ;

    translate_vm_jump(command,label) ;
}

// vm_func ::= func label number
// call function-name #args or function function-name #locals
static void parse_func()
{
    TokenKind command = token_kind(mustbe(tk_vm_func)) ;
    string label = token_spelling(mustbe(tk_a_label)) ;
    int number = token_ivalue(mustbe(tk_a_number)) ;

    translate_vm_function(command,label,number) ;
}

// vm_stack ::= stack segment number
// push segment offset or pop segment offset
static void parse_stack()
{
    TokenKind stack = token_kind(mustbe(tk_vm_stack)) ;
    TokenKind segment = token_kind(mustbe(tk_vm_segment)) ;
    int number = token_ivalue(mustbe(tk_a_number)) ;

    translate_vm_stack(stack,segment,number) ;
}

// main program
int main(int argc,char **argv)
{
    // initialise the tokeniser by reading the first token
    next_token() ;

    // parse a class and output VM code
    parse_class() ;

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}

