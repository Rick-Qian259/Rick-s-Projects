// a skeleton implementation of a tokeniser

#include "tokeniser-extras.h"

// to shorten the code
using namespace std ;

/////////////////////////////////////////////////////////////////////////

namespace Assignment_Tokeniser
{

    // the current input character
    // the eof marker can be confused with a legal character but not one accepted by our tokeniser

    char ch ;

    int counter ;
    int comment ;			// for '/' '/' together
    int eol_ ;				// =1, in eol spelling
    int adhoc_t ;			// for '/' '*' together
    int adhoc_loop ;		// =1, in adhoc spelling

    int count_bp ;			// count before point for scientific
    int count_point ;		// count after point
    int count_e ;			// count after e
    int line ;				// number line
    int column ;			// number column
    int count_column ;		// temporary
    int count_line ;		// temporary

    // to create a new token we need the characters read since the last token was created
    // the tokeniser always reads one character past a token so ch is never part of a new token
    // when nextch() is called the old ch value must be part of the token currently being parsed
    // this minimal implementation just remembers the spelling of the current token

    static string spelling ;
    static string temp ;

    // number 123.456e+3, head=1, beforepoint=23, afterpoint=456, after_e = 3, signs = +

    static string head ;
    static string before_point ;
    static string after_point ;
    static string after_e ;
    static string sign ;

	// for token_context to store the characters
//     char store [255][255] ;

    // create a new token using characters remembered since the last token was created
    // in the final submission tests new_token() will require the correct line and column numbers
    // this will require keeping an entire history of the input, not just the latest spelling
    // NOTE: the first character in the input is in column 1 of line 1

    Token new_token(TokenKind kind)
    {
        // create a new token object and reset spelling
        // Token token = new_token(kind,spelling,0,0) ;
        Token token = new_token(kind,spelling,line,column) ;


        // reset the spelling
        spelling = "" ;

        column = count_column ;
		line = count_line ;

        return token ;
    }

    // generate a context string for the given token
    // all lines are shown after the preprocssing of special characters
    // it shows the line before the token, with a '$' marking the end of line,
    // the line containing the token up to the end of the token, and
    // a line of spaces ending with a ^ under the token's first character to mark its position
    // the lines are prefixed by line numbers, right justified in four spaces, followed by ": ", eg "  45: "
    // NOTE: if a token includes newline character(s), only display the token before the first newline followed by a '$'
    // NOTE: this function is not required for the milestone submission
    // NOTE: it is a good idea to build a data structure to remember where each line starts

    string token_context(Token token)
    {
        return "" ;
    }

    // read next character if not at the end of input and update the line and column numbers
    // additional code will be required here to handle preprocessing of '\t' and '\r'
    // in some cases you may wish to remember a character to use next time instead of calling read_char()

    void nextch()
    {
        if ( ch == EOF ) return ;           // stop reading once we have read EOF

	// this is part dealing of scientific number

	/*	if (c_have(cg_digit) )
		{
	    	if (count_bp == 0)
		    {
				head = ch ;
				count_bp ++ ;
		    }
		    else if ( count_point == 0 && count_bp > 0 )
		    {
				before_point += ch ;
				count_bp ++ ;
		    }
		    else if ( count_point > 0 )
		    {
				after_point += ch ;
				count_point ++ ;
		    }
		    else if ( count_e == 1 )
		    {
				after_e += ch ;
		    }

			   spelling += ch ;
			    ch = read_char() ;

		    if ( c_have(cg_start_of_fraction) )
		    {
				count_point = 1 ;
				count_bp = -1 ;
		    }
			    else if ( c_have(cg_start_of_exponent) )
		    {
			count_point = -1 ;
			count_e = 1 ;
		    }
			else if ( c_have(cg_sign) )
	    	{
				sign = ch ;
	    	}
	    	else if ( count_e == 1 && (ch == ' '|| ch == '\n') )
	    	{
				spelling = head + '.' + before_point + after_point + 'e' + sign + after_e ;
	    	}
		}
		else if ( c_have(cg_start_of_fraction) || c_have(cg_start_of_exponent) )
		{
	    	spelling += ch ;
	    	ch = read_char() ;
		}
		else if (c_have(cg_start_of_exponent))
		{
		}*/


		if( ch == '\t' )
		{
	    	ch = ' ' ;
	    	spelling += ch ;
    	    counter = 2;
	    	comment = 0 ;
	    	adhoc_t = 0 ;
		}
		else if ( counter > 0 )
		{
	    	ch = ' ' ;
	    	spelling += ch ;
	    	counter-- ;
		}
		else if ( ch == '/' )
		{
	    	comment++ ;
	    	adhoc_t = 1 ;
	    	spelling += ch ;
            ch = read_char() ;                  // read the next character
	    	if ( comment == 2 )
	    	{
				spelling = "" ;
				adhoc_t = 0 ;
				comment = 0 ;
				eol_ = 1 ;
	    	}
	    	if ( adhoc_loop == 1)
	    	{
				spelling = temp ;
				adhoc_loop = 0 ;
	    	}
		}

		else if ( ch == '*' )
		{
	    	temp = spelling ;
	    	spelling += ch ;
	    	ch = read_char() ;
	    	if( adhoc_t == 1 )
	    	{
				spelling = "" ;
				adhoc_t = 0;
				adhoc_loop = 1;
				eol_ = 0 ;
	    	}
	    	if ( ch == '\r' )
	    	{
				ch = read_char() ;
	    	}
		}
		else if ( ch == '\n' )
		{
	    	if ( eol_ == 1)
	    	{
	        	eol_ = 0 ;
	        	ch = read_char() ;
	    	}
	    	else
	    	{
				spelling += ch ;
				ch = read_char() ;
	        	comment = 0 ;
	        	adhoc_t = 0 ;
	    	}
	    	count_line ++ ;
	    	count_column = 0 ;
		}
		else if ( ch == '\r' )
		{
	    	ch = read_char() ;
	    	if ( eol_ == 1 && ch == '\n' )
	    	{
				eol_ = 0 ;
				ch = read_char() ;
	    	}
	    	else
	    	{
				spelling += ch ;
				ch = read_char() ;
	        	comment = 0 ;
	        	adhoc_t = 0 ;
	    	}
		}
		else
		{
            spelling += ch ;                    // remember the old ch, it is part of the current token being parsed
            ch = read_char() ;                  // read the next character
	    	comment = 0 ;
	    	adhoc_t = 0 ;
	    	if ( adhoc_loop == 1 && ch == '\r' )
	    	{
				ch = read_char() ;
	    	}
		}

		count_column ++ ;
    }

    // initialise the tokeniser
    void initialise_tokeniser()
    {
                                            // add any other initialisation code you need here
		counter = 0 ;
		comment = 0 ;
		eol_ = 0 ;
		adhoc_t = 0 ;
		adhoc_loop = 0 ;
        ch = '\n' ;                         // initialise ch to avoid accidents
        nextch() ;                          // make first call to nextch to initialise ch using the input
        spelling = "" ;                     // discard the initial '\n', it is not part of the input
        count_column = 1;
		count_line = 1;
		line = 1 ;
		column = 1;
    }
}
