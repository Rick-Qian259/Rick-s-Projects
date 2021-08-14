// a skeleton implementation of a tokeniser

#include "tokeniser-extras.h"

// to shorten the code
using namespace std ;

/////////////////////////////////////////////////////////////////////////

namespace Assignment_Tokeniser
{

    static TokenKind new_token_kind ;

    // return the next Token object by reading more of the input
    // you must read input using the nextch() function
    // the last character read is in the static variable ch
    // always read one character past the end of the token being returned

	// when read '\t'

    static void parse_tt()
    {
		new_token_kind = tk_space ;
		nextch() ;
    }

    // when read '\r'

    static void parse_rr()
    {
		new_token_kind = tk_newline ;
		nextch() ;
    }

	// when read space or '\n'

    static void parse_wspace(TokenKind kind)
    {
		new_token_kind = kind ;
		nextch() ;
    }

	// for keywords

    static void parse_keyword()
    {
	if( ch == 'd' )			//done key word
	{
	    new_token_kind = tk_done ;
	    while ( c_have(tg_keyword)) nextch() ;
	}
	if( ch == 'w' )			//while key word
	{
	    new_token_kind = tk_while ;
	    while ( c_have(tg_keyword)) nextch() ;
	}
	if( ch == 'p' )			//procedure key word
	{
	    new_token_kind = tk_procedure ;
	    while ( c_have(tg_keyword)) nextch() ;
	}
	if( ch == 'i' )			// ifgo key word
	{
	    new_token_kind = tk_if_goto ;
	    while ( c_have(tg_keyword)) nextch() ;
	}
	if( ch == 't' )			// this key word
	{
	    new_token_kind = tk_this ;
	    while ( c_have(tg_keyword)) nextch() ;
	}

    }

	// when read the first character, judge whether is a key word.

    static bool is_keyword()
    {
		bool result = false ;
		if ( c_have_next('d') && c_have_next('o') && c_have_next('n') && c_have_next('e'))
		{
	    	result = true ;
		}

		if ( c_have_next('w') && c_have_next('h') && c_have_next('i') && c_have_next('l') && c_have_next('e') )
		{
	    	result = true ;
		}

		if ( c_have_next('p') && c_have_next('r') && c_have_next('o') && c_have_next('c') && c_have_next('e')
			&& c_have_next('d') && c_have_next('u') && c_have_next('r') && c_have_next('e') )
		{
	    	result = true ;
		}

		if ( c_have_next('i') && c_have_next('f') && c_have_next('-') && c_have_next('g') && c_have_next('o') && c_have_next('t') && c_have_next('o'))
		{
	    	result = true ;
		}
		if ( c_have_next('t') && c_have_next('h') && c_have_next('i') && c_have_next('s') )
		{
	    	result = true ;
		}
		return result ;
    }

	// parse identifier
	// when read the first character, match whether is keywords, if not create identifier.

    static void parse_identifier()
    {
		if(c_have_next(ch =='D'))
		{
	    	c_mustbe('=');
	    	new_token_kind = tk_identifier ;
		}
		else if ( ch == 'd' )
		{
	    	if ( is_keyword() )
	    	{
	    		new_token_kind = tk_done ;
	        	while ( c_have(tg_keyword)) nextch() ;
	    	}
	    	else
	    	{
				new_token_kind = tk_identifier ;
				while ( c_have(cg_extends_identifier)) nextch() ;
	    	}
		}
		else if ( ch == 'w' )
		{
	    	if ( is_keyword() )
	    	{
	        	new_token_kind = tk_while ;
	        	while ( c_have(tg_keyword)) nextch() ;
			}
	    	else
	    	{
				new_token_kind = tk_identifier ;
				while ( c_have(cg_extends_identifier)) nextch() ;
			}
		}
		else if ( ch == 'p' )
		{
	    	if ( is_keyword() )
	    	{
	        	new_token_kind = tk_procedure ;
	        	while ( c_have(tg_keyword)) nextch() ;
	    	}
	    	else
	    	{
				new_token_kind = tk_identifier ;
				while ( c_have(cg_extends_identifier)) nextch() ;
	    	}
		}
		else if ( ch == 'i' )
		{
	    	if ( is_keyword() )
	    	{
	        	new_token_kind = tk_if_goto ;
	        	while ( c_have(tg_keyword)) nextch() ;
		    }
	    	else
	    	{
				new_token_kind = tk_identifier ;
				while ( c_have(cg_extends_identifier)) nextch() ;
	    	}
		}
		else if ( ch == 't' )
		{
	    	if ( is_keyword() )
	    	{
	        	new_token_kind = tk_this ;
	        	while ( c_have(tg_keyword)) nextch() ;
	    	}
	    	else
	    	{
				new_token_kind = tk_identifier ;
				while ( c_have(cg_extends_identifier)) nextch() ;
	    	}
		}
		else
		{
	    	new_token_kind = tk_identifier ;
            do nextch() ; while ( c_have(cg_extends_identifier)) ;
		}
    }

	//	parse scientific

    static void parse_scientific()
    {
	    while (c_have(cg_digit) || c_have(cg_start_of_fraction)) nextch() ;
	    c_mustbe(cg_start_of_exponent) ;
	    while ( c_have(cg_digit) || c_have(cg_sign)) nextch() ;
        new_token_kind = tk_scientific ;
    }

	// when read a number, judgge if it has '+' ,'-', E or e, if not create integer, or create scientific

    static void parse_number()
    {
		if (ch == '0')
		{
	    	nextch() ;
	    	if( c_have(cg_start_of_fraction) || c_have(cg_start_of_exponent))
	    	{
				parse_scientific() ;
	    	}
	    	else
	    	{
				new_token_kind = tk_integer ;
	    	}
		}
		else
		{
	    	do nextch() ; while ( c_have(cg_digit)) ;
	    	if( c_have(cg_start_of_fraction) || c_have(cg_start_of_exponent))
	    	{
				parse_scientific() ;
	    	}
	    	else
	    	{
				new_token_kind = tk_integer ;
	    	}
		}
    }

	// parse symbol

    static void parse_symbol(TokenKind kind)
    {
		new_token_kind = kind ;
	    do nextch() ; while(c_have(tg_symbol)) ;
    }

	// parse '!='

    static void parse_not()
    {
		nextch() ;
		c_mustbe('=') ;
		new_token_kind = tk_not_eq ;
    }

    // parse '=='

    static void parse_eq()
    {
        nextch() ;
		c_mustbe('=') ;
		new_token_kind = tk_eq ;
    }

    // parse '-='

    static void parse_sub()
    {
		nextch() ;
		c_mustbe('=') ;
		new_token_kind = tk_sub_assign ;
    }

    // parse '+='

    static void parse_add()
    {
        nextch() ;
		c_mustbe('=') ;
		new_token_kind = tk_add_assign ;
    }

    // parse '*='
    static void parse_mult()
    {
		c_mustbe('=') ;
		new_token_kind = tk_mult_assign ;
    }

    // parse '/='
    static void parse_div()
    {
		c_mustbe('=') ;
		new_token_kind = tk_div_assign ;
    }

    // '<<' | '<<<'
    // always read the third character, if it is '<' create '<<<', else create '<<'

    static void parse_lshift_or()
    {
		nextch() ;
		if(c_have_next('<'))
		{
	    	if(ch != '<')
	    	{
	        	new_token_kind = tk_lshift ;
	    	}
	    	else
	    	{
				nextch() ;
				new_token_kind = tk_lshift_l ;
	    	}
		}
    }

    // '>>' | '>>>'

    static void parse_rshift_or()
    {
		nextch() ;
		if(c_have_next('>'))
		{
	    	if(ch != '>')
	    	{
	        	new_token_kind = tk_rshift ;
	    	}
	    	else
	    	{
				nextch() ;
	        	new_token_kind = tk_rshift_l ;
	    	}
		}
    }

	// if first ch = '/', judge it is '//' or '/*' or '/='

    static void parse_div_comment()
    {
		nextch() ;
		if( ch == '/' )
		{
	    	new_token_kind = tk_eol_comment ;
     	    do nextch() ; while ( c_have( cg_eol_comment_char )) ;
	    	nextch() ;
		}
		else if ( ch == '*' )
		{
	    	new_token_kind = tk_adhoc_comment ;
	    	while ( c_have( cg_adhoc_comment_char ))
	    	{
				nextch() ;
				if ( c_have_next('*') && c_have_next('/') )
				{
		    		break ;
				}
	    	}
		}
		else
		{
	    	c_mustbe( '=' ) ;
	    	new_token_kind = tk_div_assign ;
		}
    }

    // judge it is mult

    static bool is_mult()
    {
		bool result = false ;
		if( c_have_next('*') && (ch == '=') )//c_have_next('=') )
		{
	    	result = true ;
		}
		return result ;
    }

    // judge it is comment

    static void parse_comment()
    {
		if( ch == '/')
		{
	    	new_token_kind = tk_eol_comment ;
     	    do nextch() ; while ( c_have( cg_eol_comment_char )) ;
		}
    }

	// for '-=' '+=' '*=' '/=' '!=' '==' '<=' '>='
    static void parse_two_symbol()
    {
		switch(ch)
		{
		case '-':
        	parse_sub() ;
	    	break ;
		case '+':
	    	parse_add() ;
	    	break ;
		case '*':
	    	if( is_mult() )
	    	{
	        	parse_mult() ;
	    	}
	    	else
	    	{
				parse_comment() ;
	    	}
		    break ;
		case '/':
		    parse_div_comment() ;
		    break ;
		case '!':
	        parse_not() ;
		    break ;
		case '=':
		    parse_eq() ;
		    break ;
		case '<':
		    parse_lshift_or() ;
		    break ;
		case '>':
		    parse_rshift_or() ;
		    break ;
		}
    }

    Token next_token()
    {
        switch(ch)      // ch is always the next char to read
        {
                        // this should follow the style used in the workshops ...
                        // but remember that the token grammar is different in this assignment
                        //
                        // add additional case labels here for characters that can start tokens
                        // call a parse_*() function to parse the token
                        //
		case 'a'...'z':
		    parse_identifier() ;
		    break;
		case 'A'...'Z':
		    parse_identifier() ;
		    break;
		case '$':
		    parse_identifier() ;
		    break;

		case ' ':
		    parse_wspace(tk_space) ;
		    break;
		case '\n':
		    parse_wspace(tk_newline) ;
		    break;

		case '0'...'9':
		    parse_number() ;
		    break ;

		case '@':
		    parse_symbol(tk_at) ;
		    break;
		case '.':
		    parse_symbol(tk_stop) ;
		    break;
		case '{':
		    parse_symbol(tk_lcb) ;
		    break;
		case '}':
		    parse_symbol(tk_rcb) ;
		    break;
		case '(':
		    parse_symbol(tk_lrb) ;
		    break;
		case ')':
		    parse_symbol(tk_rrb) ;
		    break;
		case '[':
		    parse_symbol(tk_lsb) ;
		    break;
		case ']':
		    parse_symbol(tk_rsb) ;
		    break;

	    case '=':
		    parse_two_symbol();
		    break;
		case '!':
		    parse_two_symbol();
		    break;
		case '-':
		    parse_two_symbol();
		    break;
		case '+':
		    parse_two_symbol();
		    break;
		case '*':
		    parse_two_symbol();
		    break;
		case '/':
		    parse_two_symbol();
		    break;
		case '<':
		    parse_two_symbol();
		    break ;
		case '>':
		    parse_two_symbol() ;
		    break ;

		case '\t':
		    parse_tt() ;
		    break;
		case '\r':
		    parse_rr() ;
		    break;
        case EOF:
            new_token_kind = tk_eoi ;
            break ;
        default:
            c_did_not_find(cg_start_of_token) ;
        }

        Token token = new_token(new_token_kind) ;

                        // before returning a token check if the kind or spelling needs updating
                        // ...

        return token ;
    }
}
