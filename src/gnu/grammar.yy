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
    #include <functional>

    using namespace emel;
    #define YYSTYPE emel::ast::node

    extern std::string *current_file;
    extern ast::node root_nodes;
%}

%locations
%parse-param {std::function<int (YYSTYPE *, yy::location *)> &yylex}

%initial-action
{
    // initialize the initial location object
    @$.begin.filename = @$.end.filename = current_file;
};

%token id
%token number
%token text
%token true_ false_
%token class_ end_class
%token while_ end_while
%token for_ end_for
%token if_ else_ end_if
%token switch_ case_ default_ end_switch
%token continue_ break_ return_
%token try_ catch_
%token byref byval as_external ref_of val_of
%token colon comma dot semicolon
%token assign q_sign or_ xor_ and_ not_
%token eq ne lt gt lte gte
%token plus minus mul div
%token left_paren right_paren
%token left_brace right_brace
%token left_bracket right_bracket

%start root

%%
root: /* пусто */ | classes  { root_nodes = std::move($1); }

classes: class_def  {
                $$ = std::vector<ast::node> { boost::get<ast::class_>($1) };
            }
        | classes class_def {
                boost::get<std::vector<ast::node>>($$)
                        .emplace_back(boost::get<ast::class_>($2));
            }

class_def: class_ id colon id methods end_class
            {
                $$ = ast::class_ {
                    boost::get<std::string>($2),
                    boost::get<std::string>($4),
                    { },
                    std::move(boost::get<std::vector<ast::method>>($5))
                };
            }

class_def: class_ id methods end_class
            {
                $$ = ast::class_ {
                    boost::get<std::string>($2),
                    "",
                    { },
                    std::move(boost::get<std::vector<ast::method>>($3))
                };
            }

methods: method_def  {
                $$ = std::vector<ast::method> { boost::get<ast::method>($1) };
            }
        | methods method_def {
                boost::get<std::vector<ast::method>>($1)
                        .emplace_back(boost::get<ast::method>($2));
            }

method_def: id left_paren params right_paren left_brace expressions.opt right_brace
            {
                $$ = ast::method {
                    boost::get<std::string>($1),
                    boost::get<std::vector<ast::param>>($3),
                    boost::get<std::vector<ast::node>>($6)
                };
            }

params: /* пусто */ { $$ = std::vector<ast::param> { }; }
        | param {
                $$ = std::vector<ast::param> { boost::get<ast::param>($1) };
            }
        | params comma param {
                boost::get<std::vector<ast::param>>($$)
                    .emplace_back(boost::get<ast::param>($3));
            }

param: id       { $$ = ast::param { boost::get<std::string>($1), false }; }
param: id byref { $$ = ast::param { boost::get<std::string>($1), true }; }

expressions: block_expression {
                $$ = std::vector<ast::node> { $1 };
            }
        | expressions block_expression {
                boost::get<std::vector<ast::node>>($$).emplace_back($2);
            }

expressions.opt: /* пусто */ { $$ = std::vector<ast::node> { }; } | expressions

block_expression: try_block | switch_branch | if_branch
        | for_loop | while_loop | line_expression

line_expression: expression semicolon
expression: continue_branch | break_branch | return_branch | assignment | ternary
expression.opt: /* пусто */ { $$ = ast::node { }; } | expression

try_block: try_ left_paren id right_paren expressions.opt catch_ {
                 $$ = ast::try_ {
                     boost::get<std::string>($3),
                     boost::get<std::vector<ast::node>>($5)
                 };
            }

switch_branch: switch_ left_paren expression right_paren case_branches end_switch {
                $$ = ast::switch_ {
                    $3, boost::get<std::vector<ast::node>>($5)
                };
            }

case_branches: /* пусто */ { $$ = std::vector<ast::node> { }; }
        | case_branches case_branch {
                boost::get<std::vector<ast::node>>($$).emplace_back($2);
            }

case_branch: case_value expressions {
                $$ = ast::case_ {
                    boost::get<std::vector<ast::node>>($1),
                    boost::get<std::vector<ast::node>>($2)
                };
            }

case_branch: default_ colon expressions {
                $$ = ast::case_ {
                    { },
                    boost::get<std::vector<ast::node>>($3)
                };
            }

case_value: case_ value colon { $$ = std::vector<ast::node> { $2 }; }
        | case_value case_ value colon {
                boost::get<std::vector<ast::node>>($$).emplace_back($3);
            }

if_branch: if_ left_paren expression right_paren expressions.opt end_if {
                $$ = ast::if_ {
                    $3, boost::get<std::vector<ast::node>>($5), { }
                };
            }

if_branch: if_ left_paren expression right_paren
            expressions.opt else_ expressions.opt end_if {
                $$ = ast::if_ {
                    $3, boost::get<std::vector<ast::node>>($5),
                    boost::get<std::vector<ast::node>>($7)
                };
            }

for_loop: for_ left_paren expression.opt semicolon
            expression.opt semicolon expression.opt right_paren expressions.opt end_for {
                $$ = ast::for_ {
                    $3, $5, $7, boost::get<std::vector<ast::node>>($9)
                };
            }

while_loop: while_ left_paren expression right_paren expressions.opt end_while {
                $$ = ast::while_ {
                    $3, boost::get<std::vector<ast::node>>($5)
                };
            }

continue_branch: continue_ { $$ = ast::continue_ { }; }
break_branch: break_ { $$ = ast::break_ { }; }
return_branch: return_ expression { $$ = ast::return_ { $2 }; }

assignment: id byval.opt assign expression {
                $$ = ast::assign {
                    boost::get<std::string>($1), $4, false
                };
            }

assignment: id byval.opt as_external assign expression {
                $$ = ast::assign {
                    boost::get<std::string>($1), $5, true
                };
            }

byval.opt: /* пусто */ | byval

ternary: log_or
ternary: log_or q_sign expression colon log_or {
                $$ = ast::ternary {
                    $1, $3, $5
                };
            }

log_or: log_xor
log_or: log_xor or_ log_xor {
                $$ = ast::bin_op {
                    ast::op_kind::or_, $1, $3
                };
            }

log_xor: log_and
log_xor: log_and xor_ log_and {
                $$ = ast::bin_op {
                    ast::op_kind::xor_, $1, $3
                };
            }

log_and: rel_eq
log_and: rel_eq and_ rel_eq {
                $$ = ast::bin_op {
                    ast::op_kind::and_, $1, $3
                };
            }

rel_eq: rel_order
rel_eq: rel_order eq rel_order {
                $$ = ast::bin_op {
                    ast::op_kind::eq, $1, $3
                };
            }
        | rel_order ne rel_order {
                $$ = ast::bin_op {
                    ast::op_kind::ne, $1, $3
                };
            }

rel_order: addition
rel_order: addition lt addition {
                $$ = ast::bin_op {
                    ast::op_kind::lt, $1, $3
                };
            }
        | addition gt addition {
                $$ = ast::bin_op {
                    ast::op_kind::gt, $1, $3
                };
            }
        | addition lte addition {
                $$ = ast::bin_op {
                    ast::op_kind::lte, $1, $3
                };
            }
        | addition gte addition {
                $$ = ast::bin_op {
                    ast::op_kind::gte, $1, $3
                };
            }

addition: multiplication
addition: multiplication plus multiplication {
                $$ = ast::bin_op {
                    ast::op_kind::add, $1, $3
                };
            }
        | multiplication minus multiplication {
                $$ = ast::bin_op {
                    ast::op_kind::sub, $1, $3
                };
            }

multiplication: factor
multiplication: factor mul factor {
                $$ = ast::bin_op {
                    ast::op_kind::mul, $1, $3
                };
            }
        | factor div factor {
                $$ = ast::bin_op {
                    ast::op_kind::div, $1, $3
                };
            }

factor: call | value | var_ref
        | left_paren expression right_paren { $$ = $2; }
        | unary_not | unary_neg | plus factor { $$ = $2; }

value: text | number | true_ | false_

var_ref: id         { $$ = ast::variable { boost::get<std::string>($1), false }; }
var_ref: ref_of id  { $$ = ast::variable { boost::get<std::string>($1), true }; }

unary_not: not_ factor {
                 $$ = ast::un_op {
                     ast::op_kind::not_, $2
                 };
             }

unary_neg: minus factor {
                 $$ = ast::un_op {
                     ast::op_kind::neg, $2
                 };
             }

call: id_list left_paren expr_list right_paren {
                $$ = ast::call {
                    boost::get<std::vector<ast::node>>($1),
                    boost::get<std::vector<ast::node>>($3),
                    { }
                };
            }

call: id_list left_paren expr_list right_paren dot call {
                $$ = ast::call {
                    boost::get<std::vector<ast::node>>($1),
                    boost::get<std::vector<ast::node>>($3),
                    $6
                };
            }

id_list: id {
                $$ = std::vector<ast::node> { $1 };
            }
        | id_list dot id {
                boost::get<std::vector<ast::node>>($$).emplace_back($3);
            }

expr_list: expression.opt {
                $$ = std::vector<ast::node> { $1 };
            }
        | expr_list comma expression.opt {
                boost::get<std::vector<ast::node>>($$).emplace_back($3);
            }

%%

void yy::parser::error(const location_type &, const std::string &)
{
}
