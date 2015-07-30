%{

#include "qstring.h"
#include "../expressions/Expressions.h"
#include "parserCtrl.h"

void yyerror(void * yyctl, char *);
int  yylex(); 
Expression *BinOp(DralDB * draldb, ExpressionOperatorType, Expression *left, Expression *right);
Expression *UnaryOp(ExpressionOperatorType, Expression *left);
Expression *MapOp(DreamsDB *db, Expression *, const char *, UINT64, UINT64);

extern ExpressionList  *parseTree;
QString errstr;

//#define YYPARSE_PARAM yyctl
#define DRALDB     (((yyctl_t *)yyctl)->draldb)
#define DREAMSDB   (((yyctl_t *)yyctl)->dreamsdb)
#define CHECK(a) if ( (a) == NULL ) YYERROR; 


%}

%parse-param {void * yyctl}

/*  
 * Literal tokens 
 */
%token CHAR VALUE FPVALUE SHAPEVALUE STRING 

/*
 * Language Keywords 
 */
%token IF ELSE ITAG STAG CTAG NTAG ITEM ITEM_INSIDE LOOKUP MAP FADE
%token FCOLOR BCOLOR LCOLOR LETTER FONT SHAPE ITALIC UNDERLINE BOLD TRANSPARENT

/*
 * Operators
 */
%token EQ NE LT LE GT GE MATCH
%token LAND LOR LNOT BAND BOR BXOR BNOT
%token SHFL SHFR
%token ADD SUB MUL DIV MOD
%token UMINUS UPLUS CVT

%union {
  UINT64                    value;
  double                    fpvalue;
  QString *                 str;
  char                      letter;
  EventShape                shapevalue;
  ExpressionOperatorType    op;
  Expression                *exp;
  ExpressionList            *expList;
}

%type <letter>              CHAR
%type <value>               VALUE
%type <fpvalue>             FPVALUE
%type <shapevalue>          SHAPEVALUE
%type <str>                 STRING
%type <op>                  EQ NE LT LE GT GE MATCH
%type <op>                  LAND LOR LNOT BAND BOR BXOR BNOT
%type <op>                  SHFL SHFR
%type <op>                  ADD SUB MUL DIV MOD
%type <op>                  CVT
%type <exp>                 exp b_exp
%type <exp>                 stmt
%type <expList>             stmt_list
%type <value>               fade

//
// Precedence for C++ taken from http://web.ics.purdue.edu/~cs240/misc/operators.html
//
%left LOR
%left LAND
%left BOR
%left BXOR
%left BAND
%left EQ NE  MATCH
%left LT GT LE GE
%left SHFL SHFR
%left ADD SUB
%left MUL DIV MOD
%right LNOT BNOT 
%right UMINUS

%%

input : stmt_list { parseTree = $1; } ;

/*
 * A rule is composed of a sequence of statements
 */
stmt_list: /* empty */    { $$ = NULL; }
        | stmt_list stmt  { if ( $1 ) { $1->append($2); } 
                            else      { $$ = new ExpressionList(); $$->append($2); }
                          }
        ;

/*
 * Statements in the rule language: "if" and "assignment". 
 * Pseudo-variables allowed are "cell properties" understood by
 * Resource/Floorplan: color, bcolor, letter, etc...
 */
stmt:     IF '(' exp ')' '{' stmt_list '}'                        { $$ = new ExpressionIf($3, $6, NULL); }
        | IF '(' exp ')' '{' stmt_list '}' ELSE '{' stmt_list '}' { $$ = new ExpressionIf($3, $6, $10);  }
        | FCOLOR fade '='  exp ';'                                { $$ = new ExpressionSet(DRALDB, OperatorSetFColor,    $4, $2 > 255 ? 255 : (UINT8)$2); }
        | BCOLOR fade '='  exp ';'                                { $$ = new ExpressionSet(DRALDB, OperatorSetBColor,    $4, $2 > 255 ? 255 : (UINT8)$2); }
        | LCOLOR fade '='  exp ';'                                { $$ = new ExpressionSet(DRALDB, OperatorSetLColor,    $4, $2 > 255 ? 255 : (UINT8)$2); }
        | LETTER      '='  exp ';'                                { $$ = new ExpressionSet(DRALDB, OperatorSetLetter,    $3,  0); }
        | SHAPE       '='  exp ';'                                { $$ = new ExpressionSet(DRALDB, OperatorSetShape,     $3,  0); }
        | ITALIC      '='  b_exp ';'                              { $$ = new ExpressionSet(DRALDB, OperatorSetItalic,    $3,  0); }
        | UNDERLINE   '='  b_exp ';'                              { $$ = new ExpressionSet(DRALDB, OperatorSetUnderline, $3,  0); }
        | BOLD        '='  b_exp ';'                              { $$ = new ExpressionSet(DRALDB, OperatorSetBold,      $3,  0); }
        ;

fade:   /* empty */              { $$ = 0; }
        | '[' FADE '=' VALUE ']' { $$ = $4; }
        ;
/*
 * Expressions in the language: Expressions can be of two basic types:
 * integer or strings. Both can be used as "if" conditions, following Perl
 * rules. A NULL string or a value of Zero evaluate to false; everything
 * else evaluates to true.
 */
exp :     VALUE                               { $$ = new ExpressionConstant($1); }
        | FPVALUE                             { $$ = new ExpressionConstant($1); }
        | CHAR                                { $$ = new ExpressionConstant($1); }
        | STRING                              { $$ = new ExpressionConstant(DRALDB, *$1); }
        | SHAPEVALUE                          { $$ = new ExpressionConstant($1); }
        | '(' exp ')'                         { $$ = $2; }
        | ITEM   '(' ')'                      { $$ = new ExpressionTag(OperatorSTag, DRALDB, QString("ITEMID")); }
        | ITEM_INSIDE   '(' ')'               { $$ = new ExpressionTag(OperatorSTag, DRALDB, DRALDB->getTagName(DRALDB->getSlotItemTagId())); }
        | LOOKUP '(' ')'                      { $$ = new ExpressionTag(OperatorSTag, DRALDB, QString("__lookupidx")); }
        | ITAG   '(' STRING ')'               { $$ = new ExpressionTag(OperatorITag, DRALDB, QString(*$3)); }
        | STAG   '(' STRING ')'               { $$ = new ExpressionTag(OperatorSTag, DRALDB, QString(*$3)); }
        | NTAG   '(' STRING ')'               { $$ = new ExpressionTag(OperatorNTag, DRALDB, QString(*$3)); }
        | CTAG   '(' STRING ')'               { $$ = new ExpressionTag(OperatorCTag, DRALDB, QString(*$3)); }
        | exp EQ    exp                       { $$ = BinOp(DRALDB,$2,$1,$3); CHECK($$); }
        | exp NE    exp                       { $$ = BinOp(DRALDB,$2,$1,$3); CHECK($$); }
        | exp LT    exp                       { $$ = BinOp(DRALDB,$2,$1,$3); CHECK($$); }
        | exp LE    exp                       { $$ = BinOp(DRALDB,$2,$1,$3); CHECK($$); }
        | exp GT    exp                       { $$ = BinOp(DRALDB,$2,$1,$3); CHECK($$); }
        | exp GE    exp                       { $$ = BinOp(DRALDB,$2,$1,$3); CHECK($$); }
        | exp MATCH exp                       { $$ = BinOp(DRALDB,$2,$1,$3); CHECK($$); }
        | exp LAND  exp                       { $$ = BinOp(DRALDB,$2,$1,$3); CHECK($$); }
        | exp LOR   exp                       { $$ = BinOp(DRALDB,$2,$1,$3); CHECK($$); }
        | exp BAND  exp                       { $$ = BinOp(DRALDB,$2,$1,$3); CHECK($$); }
        | exp BOR   exp                       { $$ = BinOp(DRALDB,$2,$1,$3); CHECK($$); }
        | exp BXOR  exp                       { $$ = BinOp(DRALDB,$2,$1,$3); CHECK($$); }
        | exp SHFL  exp                       { $$ = BinOp(DRALDB,$2,$1,$3); CHECK($$); }
        | exp SHFR  exp                       { $$ = BinOp(DRALDB,$2,$1,$3); CHECK($$); }
        | exp ADD   exp                       { $$ = BinOp(DRALDB,$2,$1,$3); CHECK($$); }
        | exp SUB   exp                       { $$ = BinOp(DRALDB,$2,$1,$3); CHECK($$); }
        | exp MUL   exp                       { $$ = BinOp(DRALDB,$2,$1,$3); CHECK($$); }
        | exp DIV   exp                       { $$ = BinOp(DRALDB,$2,$1,$3); CHECK($$); }
        | exp MOD   exp                       { $$ = BinOp(DRALDB,$2,$1,$3); CHECK($$); }
        | SUB  exp  %prec UMINUS              { $$ = UnaryOp(OperatorUnaryMinus,$2);  CHECK($$); }
        | BNOT exp  %prec BNOT                { $$ = UnaryOp($1,$2);  CHECK($$); }
        | LNOT exp  %prec LNOT                { $$ = UnaryOp($1,$2);  CHECK($$); }
        | CVT    '(' exp ')'                  { $$ = UnaryOp($1,$3);  CHECK($$); }
        | MAP '(' exp ',' STRING ',' VALUE ',' VALUE ')'  { $$ = MapOp(DREAMSDB, $3,*$5,$7,$9);  CHECK($$); }
        ;

b_exp : STRING
				{
					if(strcmp(*$1, "true") == 0)
					{
						$$ = new ExpressionConstant((UINT64) 1);
					}
					else if(strcmp(*$1, "false") == 0)
					{
						$$ = new ExpressionConstant((UINT64) 0);
					}
					else
					{
						errstr = "Only \"true\" and \"false\" allowed in boolean expressions.";
						YYERROR;
					}
				}
        ;
%%

Expression *
BinOp(DralDB * draldb, ExpressionOperatorType op, Expression *left, Expression *right)
{
 ExpressionType lty = left->getType();
 ExpressionType rty = right->getType();

 if ( ExpressionToolkit::operatorMatch(op,left->getType()) == false ) {
  errstr = "Operator " + ExpressionToolkit::toString(op) + " does not accept type " + ExpressionToolkit::toString(left->getType());
  return NULL;
 }
 if ( ExpressionToolkit::operatorMatch(op,right->getType()) == false ) {
  errstr = "Operator " + ExpressionToolkit::toString(op) + " does not accept type " + ExpressionToolkit::toString(right->getType());
  return NULL;
 }

 //
 // Most common case: both sides match in type
 //
 if ( lty == rty ) {
  if ( lty == ExpressionIntegerValue ) return new ExpressionBinaryInteger(op,left,right);
  if ( lty == ExpressionStringValue )  return new ExpressionBinaryString(draldb, op,left,right);
  if ( lty == ExpressionFPValue )      return new ExpressionBinaryFP(op,left,right);
  errstr = "Type " + ExpressionToolkit::toString(lty) + " is not valid in expression";
  return NULL;
 }

 //
 // Type mismatch: Let's see if it's an Integer/FP mixture that can be
 // solved by inserting "ToDouble" expressions to convert the whole
 // expression into a FP expression
 //
 if ( (lty == ExpressionIntegerValue && rty == ExpressionFPValue     ) ||
      (lty == ExpressionFPValue      && rty == ExpressionIntegerValue)    ) {

  left  = ( lty == ExpressionIntegerValue ) ? new ExpressionUnary(OperatorToDouble, left)  : left;
  right = ( rty == ExpressionIntegerValue ) ? new ExpressionUnary(OperatorToDouble, right) : right;
  return new ExpressionBinaryFP(op, left, right);
 }

 //
 // Fall-through: This is a true mismatch. Report error
 //
 errstr  = "Types " + ExpressionToolkit::toString(left->getType()) + " and ";
 errstr +=            ExpressionToolkit::toString(right->getType());
 errstr += " do not match for operator " + ExpressionToolkit::toString(op) + " ";
 return NULL;
}

Expression *
UnaryOp(ExpressionOperatorType op, Expression *left)
{
 ExpressionType ty = left->getType();

 if ( ExpressionToolkit::operatorMatch(op,ty) == false ) {
  errstr = "Operator " + ExpressionToolkit::toString(op) + " does not accept type " + ExpressionToolkit::toString(ty);
  return NULL;
 }

 return new ExpressionUnary(op,left);
}

Expression *
MapOp(DreamsDB *db, Expression *idx, const char *mapname, UINT64 min, UINT64 max)
{
 //printf("MapOp: mapname %s\n",mapname);
 ADFMap *map = db->getMapTable(QString(mapname));

 //printf("MapOp: map %p for map %s\n",map,mapname);
 if ( map == NULL ) {
  errstr = "Error in map function: map \"" + QString(mapname) + "\" is not defined";
  return NULL;
 }

 //printf("MapOp: min %llu max %llu\n",min,max);
 if ( min > max ) {
  errstr = "Error in map function: min value (" + QString::number(min) + ") larger than max value (" + QString::number(max) + ")";
  return NULL;
 }

 return new ExpressionMap(idx, map, min, max);
}

void yyerror(void * yyctl, char *msg){
	errstr = msg;
}
