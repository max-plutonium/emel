/*
 * Copyright (C) 2015 Max E. Kosykh
 *
 * This file is part of the EMEL library.
 *
 * The EMEL library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * The EMEL library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the EMEL library. If not, see
 * <http://www.gnu.org/licenses/>.
 */
%{
    #include "ast.h"
    #define YYSTYPE emel::ast::node
    #include "gnu_parser.hpp"
    extern YYSTYPE *yylval;
    extern yy::location *yylloc;

    std::size_t yycolumn = 1;

    #define YY_USER_ACTION yylloc->begin.line = yylloc->end.line = yylineno; \
        yylloc->begin.column = yycolumn; yylloc->end.column = yycolumn + yyleng; \
        yycolumn += yyleng;
%}

%option c++
%option yylineno
%option noyywrap
%option caseless

COMMENT (\/\/[^\n]*)|(\/\*[^*]*\*+([^/*][^*]*\*+)*\/)|(\'[^\']*\')
ID      [a-zA-Z_][a-zA-Z0-9_]*
DIGIT   [0-9]
NUMBER  -?({DIGIT}+|({DIGIT}*\.{DIGIT}+)|({DIGIT}+\.{DIGIT}*))
TEXT    \"([^\"]|\")*\"

%%
{COMMENT}                       /* игнорируем комментарии */;
true                            *yylval = true; return yy::parser::token::true_;
false                           *yylval = false; return yy::parser::token::false_;
class                           return yy::parser::token::class_;
end[ \t]*class                  return yy::parser::token::end_class;
while                           return yy::parser::token::while_;
end[ \t]*while                  return yy::parser::token::end_while;
for                             return yy::parser::token::for_;
end[ \t]*for                    return yy::parser::token::end_for;
if                              return yy::parser::token::if_;
else                            return yy::parser::token::else_;
end[ \t]*if                     return yy::parser::token::end_if;
switch                          return yy::parser::token::switch_;
case                            return yy::parser::token::case_;
default                         return yy::parser::token::default_;
end[ \t]*switch                 return yy::parser::token::end_switch;
continue                        return yy::parser::token::continue_;
break                           return yy::parser::token::break_;
return                          return yy::parser::token::return_;
try                             return yy::parser::token::try_;
catch                           return yy::parser::token::catch_;
byref                           return yy::parser::token::byref;
byval                           return yy::parser::token::byval;
asexternal                      return yy::parser::token::as_external;
refof                           return yy::parser::token::ref_of;
valof                           return yy::parser::token::val_of;
\:                              return yy::parser::token::colon;
\,                              return yy::parser::token::comma;
\.                              return yy::parser::token::dot;
\;                              return yy::parser::token::semicolon;
\=                              return yy::parser::token::assign;
\?                              return yy::parser::token::q_sign;
\|                              return yy::parser::token::or_;
\^                              return yy::parser::token::xor_;
\&                              return yy::parser::token::and_;
\~                              return yy::parser::token::not_;
\==                             return yy::parser::token::eq;
\!=                             return yy::parser::token::ne;
\<                              return yy::parser::token::lt;
\>                              return yy::parser::token::gt;
\<=                             return yy::parser::token::lte;
\>=                             return yy::parser::token::gte;
\+                              return yy::parser::token::plus;
\-                              return yy::parser::token::minus;
\*                              return yy::parser::token::mul;
\/                              return yy::parser::token::div;
\(                              return yy::parser::token::left_paren;
\)                              return yy::parser::token::right_paren;
\{                              return yy::parser::token::left_brace;
\}                              return yy::parser::token::right_brace;
\[                              return yy::parser::token::left_bracket;
\]                              return yy::parser::token::right_bracket;
{ID}                            *yylval = std::string(yytext); return yy::parser::token::id;
{NUMBER}                        *yylval = std::atof(yytext); return yy::parser::token::number;
{TEXT}                          *yylval = std::string(yytext); return yy::parser::token::text;
\n                              yycolumn = 1;
%%
