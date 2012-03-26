%{

#include "stdio.h"
#include "qstring.h"
#include "dDB/DralDBSyntax.h"
#include "expressions/Expression.h"
#include "parser.h"

YY_BUFFER_STATE curBuf;



%}

%option yylineno
%option never-interactive

D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*


%%

[ \t]+	/* ignore whitespace */

[\n\r]	

0[xX][0-9a-fA-F]+ { /* Hex constant */
         bool ok = true;
	 yylval.value = QString(yytext).toULongLong(&ok,0);
         if ( !ok ) {
          printf("Error lexing at HEX number: %s\n", yytext);
          exit(-1);
         }
	 return VALUE; 
        }

{D}+  {  
	yylval.value = atoi(yytext);
	return VALUE;
      }

{D}*\.({D}+)?([eE][\+\-]?{D}+)?  {  /* a constant; make new tree node */
	yylval.fpvalue = atof(yytext);
	return FPVALUE; 
	}

"=="	 { yylval.op = OperatorEqual;        return EQ; }
"!="	 { yylval.op = OperatorNotEqual;     return NE; }
"<"	 { yylval.op = OperatorLowerThan;        return LT; }
"<="	 { yylval.op = OperatorLowerEqual;   return LE; }
">"	 { yylval.op = OperatorGreaterThan;      return GT; }
">="	 { yylval.op = OperatorGreaterEqual; return GE; }
"=~"	 { yylval.op = OperatorRegExpMatch;  return MATCH; }
"&&"	 { yylval.op = OperatorLogicalAnd;   return LAND; }
"||"	 { yylval.op = OperatorLogicalOr;    return LOR; } 
"!"	 { yylval.op = OperatorLogicalNot;  return LNOT; } 
"&"	 { yylval.op = OperatorBitwiseAnd;  return BAND; }
"|"	 { yylval.op = OperatorBitwiseOr;   return BOR; }
"^"	 { yylval.op = OperatorBitwiseXor;  return BXOR; }
"~"	 { yylval.op = OperatorBitwiseNot;  return BNOT; }
"<<"	 { yylval.op = OperatorShiftLeft;  return SHFL; } 
">>"	 { yylval.op = OperatorShiftRight; return SHFR; } 
"+"	 { yylval.op = OperatorAdd; return ADD; } 
"-"	 { yylval.op = OperatorSub; return SUB; } 
"*"	 { yylval.op = OperatorMul; return MUL; } 
"/"	 { yylval.op = OperatorDiv; return DIV; } 
"%"	 { yylval.op = OperatorMod; return MOD; } 
"fp64"   { yylval.op = OperatorToDouble; return CVT; } 
"uint64" { yylval.op = OperatorToUINT64; return CVT; } 

 /* 
  * Language Keywords 
  */
if	      { return IF; } 
else	      {  return ELSE; } 
map	      {  return MAP; } 
fade	      {  return FADE; } 

 /*
  * Tag Queries, Accessors & modifiers
  */
itag	      { return ITAG;        }
stag	      { return STAG;        }
ctag	      { return CTAG;        }
ntag	      { return NTAG;        }
item	      { return ITEM;        }
item_inside   { return ITEM_INSIDE; }
lookup        { return LOOKUP;      }

 /* 
  * Cell Properties 
  */
color       { return FCOLOR;    }
fcolor      { return FCOLOR;    }
lcolor      { return LCOLOR;    }
bcolor      { return BCOLOR;    }
letter      { return LETTER;    }
font        { return FONT;      }
shape       { return SHAPE;     }
italic      { return ITALIC;    }
underline   { return UNDERLINE; }
bold        { return BOLD;      }

[(){}\[\]=;,]   { return yytext[0]; }

 /*
  * SHape Literals 
  */

rectangle       { yylval.shapevalue = EVENT_SHAPE_RECTANGLE; return SHAPEVALUE; }
circle          { yylval.shapevalue = EVENT_SHAPE_CIRCLE;    return SHAPEVALUE; }
diamond         { yylval.shapevalue = EVENT_SHAPE_RHOMB;     return SHAPEVALUE; }
rhomb           { yylval.shapevalue = EVENT_SHAPE_RHOMB;     return SHAPEVALUE; }
triangle        { yylval.shapevalue = EVENT_SHAPE_TRIANGLE;  return SHAPEVALUE; }
\"rectangle\"   { yylval.shapevalue = EVENT_SHAPE_RECTANGLE; return SHAPEVALUE; }
\"circle\"      { yylval.shapevalue = EVENT_SHAPE_CIRCLE;    return SHAPEVALUE; }
\"diamond\"     { yylval.shapevalue = EVENT_SHAPE_RHOMB;     return SHAPEVALUE; }
\"rhomb\"       { yylval.shapevalue = EVENT_SHAPE_RHOMB;     return SHAPEVALUE; }
\"triangle\"    { yylval.shapevalue = EVENT_SHAPE_TRIANGLE;  return SHAPEVALUE; }

transparent { return TRANSPARENT; } 

\'.\'	{ 
        /* remove single quote at begin and end */
        yylval.letter = QString(yytext).remove('\'').ascii()[0];
	return CHAR ; 
	}

\"[^\"]*\"	{ 
        /* remove comma at begin and end */
        /* strcpy(yytext, QString(yytext).remove('"').ascii()); */
        yylval.str = new QString(yytext);
        yylval.str->remove('"');
	return STRING ; 
	}

[a-zA-Z0-9_#]+	{ 
        yylval.str = new QString(yytext);
	return STRING ; 
	}

.	{ 
          exit(-1);
        }

%%

void SetParserInputString(const char *str)
{
 curBuf = yy_scan_string(str);
}

void CleanParserInputString()
{
 yy_delete_buffer(curBuf);
}
