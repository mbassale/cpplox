// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.



// First part of user prologue.
#line 1 "/Users/mbassale/devel/cpplox/src/parser.yy"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "ast.h"
#include "scanner.h"
#include "lexer.h"

using namespace std;
using namespace cpplox::ast;


#line 56 "/Users/mbassale/devel/cpplox/src/python_parser.cpp"


#include "python_parser.hpp"




#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif



// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 18 "/Users/mbassale/devel/cpplox/src/parser.yy"
namespace PythonParser {
#line 135 "/Users/mbassale/devel/cpplox/src/python_parser.cpp"

  /// Build a parser object.
  PythonParser::PythonParser (ASTBuilder& builder_yyarg, Scanner& scanner_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      builder (builder_yyarg),
      scanner (scanner_yyarg)
  {}

  PythonParser::~PythonParser ()
  {}

  PythonParser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/

  // basic_symbol.
  template <typename Base>
  PythonParser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
  {
    switch (this->kind ())
    {
      case symbol_kind::S_INDENT: // "INDENT"
      case symbol_kind::S_DEDENT: // "DEDENT"
      case symbol_kind::S_INTEGER: // "INTEGER"
      case symbol_kind::S_IDENTIFIER: // "IDENTIFIER"
      case symbol_kind::S_STRING_LITERAL: // "STRING_LITERAL"
        value.copy< Token > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_suite: // suite
        value.copy< cpplox::ast::BlockPtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_expr: // expr
        value.copy< cpplox::ast::ExpressionPtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_simple_statement: // simple_statement
        value.copy< cpplox::ast::ExpressionStatementPtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_program: // program
        value.copy< cpplox::ast::ProgramPtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_statement: // statement
        value.copy< cpplox::ast::StatementPtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_statements: // statements
        value.copy< std::vector<cpplox::ast::StatementPtr> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

  }




  template <typename Base>
  PythonParser::symbol_kind_type
  PythonParser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  template <typename Base>
  bool
  PythonParser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  PythonParser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->kind ())
    {
      case symbol_kind::S_INDENT: // "INDENT"
      case symbol_kind::S_DEDENT: // "DEDENT"
      case symbol_kind::S_INTEGER: // "INTEGER"
      case symbol_kind::S_IDENTIFIER: // "IDENTIFIER"
      case symbol_kind::S_STRING_LITERAL: // "STRING_LITERAL"
        value.move< Token > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_suite: // suite
        value.move< cpplox::ast::BlockPtr > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_expr: // expr
        value.move< cpplox::ast::ExpressionPtr > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_simple_statement: // simple_statement
        value.move< cpplox::ast::ExpressionStatementPtr > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_program: // program
        value.move< cpplox::ast::ProgramPtr > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_statement: // statement
        value.move< cpplox::ast::StatementPtr > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_statements: // statements
        value.move< std::vector<cpplox::ast::StatementPtr> > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

  }

  // by_kind.
  PythonParser::by_kind::by_kind () YY_NOEXCEPT
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  PythonParser::by_kind::by_kind (by_kind&& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  PythonParser::by_kind::by_kind (const by_kind& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {}

  PythonParser::by_kind::by_kind (token_kind_type t) YY_NOEXCEPT
    : kind_ (yytranslate_ (t))
  {}



  void
  PythonParser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  void
  PythonParser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  PythonParser::symbol_kind_type
  PythonParser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }


  PythonParser::symbol_kind_type
  PythonParser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }



  // by_state.
  PythonParser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  PythonParser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  PythonParser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  PythonParser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  PythonParser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  PythonParser::symbol_kind_type
  PythonParser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  PythonParser::stack_symbol_type::stack_symbol_type ()
  {}

  PythonParser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_INDENT: // "INDENT"
      case symbol_kind::S_DEDENT: // "DEDENT"
      case symbol_kind::S_INTEGER: // "INTEGER"
      case symbol_kind::S_IDENTIFIER: // "IDENTIFIER"
      case symbol_kind::S_STRING_LITERAL: // "STRING_LITERAL"
        value.YY_MOVE_OR_COPY< Token > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_suite: // suite
        value.YY_MOVE_OR_COPY< cpplox::ast::BlockPtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_expr: // expr
        value.YY_MOVE_OR_COPY< cpplox::ast::ExpressionPtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_simple_statement: // simple_statement
        value.YY_MOVE_OR_COPY< cpplox::ast::ExpressionStatementPtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_program: // program
        value.YY_MOVE_OR_COPY< cpplox::ast::ProgramPtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_statement: // statement
        value.YY_MOVE_OR_COPY< cpplox::ast::StatementPtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_statements: // statements
        value.YY_MOVE_OR_COPY< std::vector<cpplox::ast::StatementPtr> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  PythonParser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s)
  {
    switch (that.kind ())
    {
      case symbol_kind::S_INDENT: // "INDENT"
      case symbol_kind::S_DEDENT: // "DEDENT"
      case symbol_kind::S_INTEGER: // "INTEGER"
      case symbol_kind::S_IDENTIFIER: // "IDENTIFIER"
      case symbol_kind::S_STRING_LITERAL: // "STRING_LITERAL"
        value.move< Token > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_suite: // suite
        value.move< cpplox::ast::BlockPtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_expr: // expr
        value.move< cpplox::ast::ExpressionPtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_simple_statement: // simple_statement
        value.move< cpplox::ast::ExpressionStatementPtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_program: // program
        value.move< cpplox::ast::ProgramPtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_statement: // statement
        value.move< cpplox::ast::StatementPtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_statements: // statements
        value.move< std::vector<cpplox::ast::StatementPtr> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  PythonParser::stack_symbol_type&
  PythonParser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_INDENT: // "INDENT"
      case symbol_kind::S_DEDENT: // "DEDENT"
      case symbol_kind::S_INTEGER: // "INTEGER"
      case symbol_kind::S_IDENTIFIER: // "IDENTIFIER"
      case symbol_kind::S_STRING_LITERAL: // "STRING_LITERAL"
        value.copy< Token > (that.value);
        break;

      case symbol_kind::S_suite: // suite
        value.copy< cpplox::ast::BlockPtr > (that.value);
        break;

      case symbol_kind::S_expr: // expr
        value.copy< cpplox::ast::ExpressionPtr > (that.value);
        break;

      case symbol_kind::S_simple_statement: // simple_statement
        value.copy< cpplox::ast::ExpressionStatementPtr > (that.value);
        break;

      case symbol_kind::S_program: // program
        value.copy< cpplox::ast::ProgramPtr > (that.value);
        break;

      case symbol_kind::S_statement: // statement
        value.copy< cpplox::ast::StatementPtr > (that.value);
        break;

      case symbol_kind::S_statements: // statements
        value.copy< std::vector<cpplox::ast::StatementPtr> > (that.value);
        break;

      default:
        break;
    }

    return *this;
  }

  PythonParser::stack_symbol_type&
  PythonParser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_INDENT: // "INDENT"
      case symbol_kind::S_DEDENT: // "DEDENT"
      case symbol_kind::S_INTEGER: // "INTEGER"
      case symbol_kind::S_IDENTIFIER: // "IDENTIFIER"
      case symbol_kind::S_STRING_LITERAL: // "STRING_LITERAL"
        value.move< Token > (that.value);
        break;

      case symbol_kind::S_suite: // suite
        value.move< cpplox::ast::BlockPtr > (that.value);
        break;

      case symbol_kind::S_expr: // expr
        value.move< cpplox::ast::ExpressionPtr > (that.value);
        break;

      case symbol_kind::S_simple_statement: // simple_statement
        value.move< cpplox::ast::ExpressionStatementPtr > (that.value);
        break;

      case symbol_kind::S_program: // program
        value.move< cpplox::ast::ProgramPtr > (that.value);
        break;

      case symbol_kind::S_statement: // statement
        value.move< cpplox::ast::StatementPtr > (that.value);
        break;

      case symbol_kind::S_statements: // statements
        value.move< std::vector<cpplox::ast::StatementPtr> > (that.value);
        break;

      default:
        break;
    }

    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  PythonParser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  PythonParser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " (";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  PythonParser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  PythonParser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  PythonParser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  PythonParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  PythonParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  PythonParser::debug_level_type
  PythonParser::debug_level () const
  {
    return yydebug_;
  }

  void
  PythonParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  PythonParser::state_type
  PythonParser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  PythonParser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  PythonParser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  PythonParser::operator() ()
  {
    return parse ();
  }

  int
  PythonParser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.kind_ = yytranslate_ (yylex (&yyla.value, builder, scanner));
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_INDENT: // "INDENT"
      case symbol_kind::S_DEDENT: // "DEDENT"
      case symbol_kind::S_INTEGER: // "INTEGER"
      case symbol_kind::S_IDENTIFIER: // "IDENTIFIER"
      case symbol_kind::S_STRING_LITERAL: // "STRING_LITERAL"
        yylhs.value.emplace< Token > ();
        break;

      case symbol_kind::S_suite: // suite
        yylhs.value.emplace< cpplox::ast::BlockPtr > ();
        break;

      case symbol_kind::S_expr: // expr
        yylhs.value.emplace< cpplox::ast::ExpressionPtr > ();
        break;

      case symbol_kind::S_simple_statement: // simple_statement
        yylhs.value.emplace< cpplox::ast::ExpressionStatementPtr > ();
        break;

      case symbol_kind::S_program: // program
        yylhs.value.emplace< cpplox::ast::ProgramPtr > ();
        break;

      case symbol_kind::S_statement: // statement
        yylhs.value.emplace< cpplox::ast::StatementPtr > ();
        break;

      case symbol_kind::S_statements: // statements
        yylhs.value.emplace< std::vector<cpplox::ast::StatementPtr> > ();
        break;

      default:
        break;
    }



      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // program: statements
#line 67 "/Users/mbassale/devel/cpplox/src/parser.yy"
                 { yylhs.value.as < cpplox::ast::ProgramPtr > () = builder.emitProgram(yystack_[0].value.as < std::vector<cpplox::ast::StatementPtr> > ()); }
#line 835 "/Users/mbassale/devel/cpplox/src/python_parser.cpp"
    break;

  case 3: // statements: %empty
#line 71 "/Users/mbassale/devel/cpplox/src/parser.yy"
                  { yylhs.value.as < std::vector<cpplox::ast::StatementPtr> > () = std::vector<cpplox::ast::StatementPtr>(); }
#line 841 "/Users/mbassale/devel/cpplox/src/python_parser.cpp"
    break;

  case 4: // statements: statements statement
#line 73 "/Users/mbassale/devel/cpplox/src/parser.yy"
      { yystack_[1].value.as < std::vector<cpplox::ast::StatementPtr> > ().push_back(yystack_[0].value.as < cpplox::ast::StatementPtr > ()); yylhs.value.as < std::vector<cpplox::ast::StatementPtr> > () = yystack_[1].value.as < std::vector<cpplox::ast::StatementPtr> > (); }
#line 847 "/Users/mbassale/devel/cpplox/src/python_parser.cpp"
    break;

  case 5: // statement: simple_statement "NEWLINE"
#line 77 "/Users/mbassale/devel/cpplox/src/parser.yy"
                               { yylhs.value.as < cpplox::ast::StatementPtr > () = yystack_[1].value.as < cpplox::ast::ExpressionStatementPtr > (); }
#line 853 "/Users/mbassale/devel/cpplox/src/python_parser.cpp"
    break;

  case 7: // simple_statement: expr
#line 82 "/Users/mbassale/devel/cpplox/src/parser.yy"
           { yylhs.value.as < cpplox::ast::ExpressionStatementPtr > () = builder.emitExpressionStatement(yystack_[0].value.as < cpplox::ast::ExpressionPtr > ());}
#line 859 "/Users/mbassale/devel/cpplox/src/python_parser.cpp"
    break;

  case 11: // if_statement: "if" expr ':' suite
#line 94 "/Users/mbassale/devel/cpplox/src/parser.yy"
      { builder.emitIfStatement(yystack_[2].value.as < cpplox::ast::ExpressionPtr > (), yystack_[0].value.as < cpplox::ast::BlockPtr > ()); }
#line 865 "/Users/mbassale/devel/cpplox/src/python_parser.cpp"
    break;

  case 12: // while_statement: "while" expr ':' suite
#line 100 "/Users/mbassale/devel/cpplox/src/parser.yy"
      { builder.emitWhileStatement(yystack_[2].value.as < cpplox::ast::ExpressionPtr > (), yystack_[0].value.as < cpplox::ast::BlockPtr > ()); }
#line 871 "/Users/mbassale/devel/cpplox/src/python_parser.cpp"
    break;

  case 13: // def_statement: "def" "IDENTIFIER" '(' ')' ':' suite
#line 106 "/Users/mbassale/devel/cpplox/src/parser.yy"
      { builder.emitDefStatement(yystack_[4].value.as < Token > (), yystack_[0].value.as < cpplox::ast::BlockPtr > ()); }
#line 877 "/Users/mbassale/devel/cpplox/src/python_parser.cpp"
    break;

  case 14: // expr: "INTEGER"
#line 110 "/Users/mbassale/devel/cpplox/src/parser.yy"
              { yylhs.value.as < cpplox::ast::ExpressionPtr > () = builder.emitIntegerLiteral(yystack_[0].value.as < Token > ()); }
#line 883 "/Users/mbassale/devel/cpplox/src/python_parser.cpp"
    break;

  case 15: // expr: "IDENTIFIER"
#line 111 "/Users/mbassale/devel/cpplox/src/parser.yy"
                 { yylhs.value.as < cpplox::ast::ExpressionPtr > () = builder.emitIdentifier(yystack_[0].value.as < Token > ()); }
#line 889 "/Users/mbassale/devel/cpplox/src/python_parser.cpp"
    break;

  case 16: // expr: "STRING_LITERAL"
#line 112 "/Users/mbassale/devel/cpplox/src/parser.yy"
                     { yylhs.value.as < cpplox::ast::ExpressionPtr > () = builder.emitStringLiteral(yystack_[0].value.as < Token > ()); }
#line 895 "/Users/mbassale/devel/cpplox/src/python_parser.cpp"
    break;

  case 17: // expr: '(' expr ')'
#line 113 "/Users/mbassale/devel/cpplox/src/parser.yy"
                   { yylhs.value.as < cpplox::ast::ExpressionPtr > () = yystack_[1].value.as < cpplox::ast::ExpressionPtr > (); }
#line 901 "/Users/mbassale/devel/cpplox/src/python_parser.cpp"
    break;

  case 18: // expr: expr '+' expr
#line 114 "/Users/mbassale/devel/cpplox/src/parser.yy"
                    { yylhs.value.as < cpplox::ast::ExpressionPtr > () = builder.emitBinaryOp('+', yystack_[2].value.as < cpplox::ast::ExpressionPtr > (), yystack_[0].value.as < cpplox::ast::ExpressionPtr > ()); }
#line 907 "/Users/mbassale/devel/cpplox/src/python_parser.cpp"
    break;

  case 19: // expr: expr '-' expr
#line 115 "/Users/mbassale/devel/cpplox/src/parser.yy"
                    { yylhs.value.as < cpplox::ast::ExpressionPtr > () = builder.emitBinaryOp('-', yystack_[2].value.as < cpplox::ast::ExpressionPtr > (), yystack_[0].value.as < cpplox::ast::ExpressionPtr > ()); }
#line 913 "/Users/mbassale/devel/cpplox/src/python_parser.cpp"
    break;

  case 20: // expr: expr '*' expr
#line 116 "/Users/mbassale/devel/cpplox/src/parser.yy"
                    { yylhs.value.as < cpplox::ast::ExpressionPtr > () = builder.emitBinaryOp('*', yystack_[2].value.as < cpplox::ast::ExpressionPtr > (), yystack_[0].value.as < cpplox::ast::ExpressionPtr > ()); }
#line 919 "/Users/mbassale/devel/cpplox/src/python_parser.cpp"
    break;

  case 21: // expr: expr '/' expr
#line 117 "/Users/mbassale/devel/cpplox/src/parser.yy"
                    { yylhs.value.as < cpplox::ast::ExpressionPtr > () = builder.emitBinaryOp('/', yystack_[2].value.as < cpplox::ast::ExpressionPtr > (), yystack_[0].value.as < cpplox::ast::ExpressionPtr > ()); }
#line 925 "/Users/mbassale/devel/cpplox/src/python_parser.cpp"
    break;

  case 22: // suite: "NEWLINE" "INDENT" statements "DEDENT"
#line 122 "/Users/mbassale/devel/cpplox/src/parser.yy"
      { yylhs.value.as < cpplox::ast::BlockPtr > () = builder.emitBlock(yystack_[1].value.as < std::vector<cpplox::ast::StatementPtr> > ()); }
#line 931 "/Users/mbassale/devel/cpplox/src/python_parser.cpp"
    break;


#line 935 "/Users/mbassale/devel/cpplox/src/python_parser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (YY_MOVE (msg));
      }


    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;


      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  PythonParser::error (const syntax_error& yyexc)
  {
    error (yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  PythonParser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

  std::string
  PythonParser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  // PythonParser::context.
  PythonParser::context::context (const PythonParser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  PythonParser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    const int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        const int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        const int yychecklim = yylast_ - yyn + 1;
        const int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }






  int
  PythonParser::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  PythonParser::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char PythonParser::yypact_ninf_ = -27;

  const signed char PythonParser::yytable_ninf_ = -1;

  const signed char
  PythonParser::yypact_[] =
  {
     -27,     8,    15,   -27,   -27,   -27,   -27,    24,    24,     6,
      24,   -27,     7,   -27,   -27,   -27,   -27,    39,    23,    30,
      10,    35,   -27,    24,    24,    24,    24,    19,    19,    18,
     -27,    39,    39,    39,    39,    31,   -27,   -27,    21,   -27,
      19,     0,   -27,   -27
  };

  const signed char
  PythonParser::yydefact_[] =
  {
       3,     0,     2,     1,    14,    15,    16,     0,     0,     0,
       0,     4,     0,     6,     8,     9,    10,     7,     0,     0,
       0,     0,     5,     0,     0,     0,     0,     0,     0,     0,
      17,    18,    19,    20,    21,     0,    11,    12,     0,     3,
       0,     0,    13,    22
  };

  const signed char
  PythonParser::yypgoto_[] =
  {
     -27,   -27,    -3,   -27,   -27,   -27,   -27,   -27,   -27,    -7,
     -26
  };

  const signed char
  PythonParser::yydefgoto_[] =
  {
       0,     1,     2,    11,    12,    13,    14,    15,    16,    17,
      36
  };

  const signed char
  PythonParser::yytable_[] =
  {
      18,    19,    37,    21,    43,     4,     5,     6,     3,     7,
       8,     9,    20,    10,    42,    22,    31,    32,    33,    34,
       4,     5,     6,    29,     7,     8,     9,    35,    10,     4,
       5,     6,    38,    40,    39,    27,    41,    10,    23,    24,
      25,    26,    28,     0,     0,    23,    24,    25,    26,    30,
      23,    24,    25,    26,    23,    24,    25,    26
  };

  const signed char
  PythonParser::yycheck_[] =
  {
       7,     8,    28,    10,     4,     5,     6,     7,     0,     9,
      10,    11,     6,    13,    40,     8,    23,    24,    25,    26,
       5,     6,     7,    13,     9,    10,    11,     8,    13,     5,
       6,     7,    14,    12,     3,    12,    39,    13,    15,    16,
      17,    18,    12,    -1,    -1,    15,    16,    17,    18,    14,
      15,    16,    17,    18,    15,    16,    17,    18
  };

  const signed char
  PythonParser::yystos_[] =
  {
       0,    20,    21,     0,     5,     6,     7,     9,    10,    11,
      13,    22,    23,    24,    25,    26,    27,    28,    28,    28,
       6,    28,     8,    15,    16,    17,    18,    12,    12,    13,
      14,    28,    28,    28,    28,     8,    29,    29,    14,     3,
      12,    21,    29,     4
  };

  const signed char
  PythonParser::yyr1_[] =
  {
       0,    19,    20,    21,    21,    22,    22,    23,    24,    24,
      24,    25,    26,    27,    28,    28,    28,    28,    28,    28,
      28,    28,    29
  };

  const signed char
  PythonParser::yyr2_[] =
  {
       0,     2,     1,     0,     2,     2,     1,     1,     1,     1,
       1,     4,     4,     6,     1,     1,     1,     3,     3,     3,
       3,     3,     4
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const PythonParser::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "\"INDENT\"",
  "\"DEDENT\"", "\"INTEGER\"", "\"IDENTIFIER\"", "\"STRING_LITERAL\"",
  "\"NEWLINE\"", "\"if\"", "\"while\"", "\"def\"", "':'", "'('", "')'",
  "'+'", "'-'", "'*'", "'/'", "$accept", "program", "statements",
  "statement", "simple_statement", "compound_statement", "if_statement",
  "while_statement", "def_statement", "expr", "suite", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const signed char
  PythonParser::yyrline_[] =
  {
       0,    67,    67,    71,    72,    77,    78,    82,    86,    87,
      88,    92,    98,   104,   110,   111,   112,   113,   114,   115,
     116,   117,   121
  };

  void
  PythonParser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  PythonParser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  PythonParser::symbol_kind_type
  PythonParser::yytranslate_ (int t) YY_NOEXCEPT
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const signed char
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      13,    14,    17,    15,     2,    16,     2,    18,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    12,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11
    };
    // Last valid token kind.
    const int code_max = 266;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

#line 18 "/Users/mbassale/devel/cpplox/src/parser.yy"
} // PythonParser
#line 1472 "/Users/mbassale/devel/cpplox/src/python_parser.cpp"

#line 125 "/Users/mbassale/devel/cpplox/src/parser.yy"


namespace PythonParser
{
  // Report an error to the user.
  auto PythonParser::error (const std::string& msg) -> void
  {
    std::cerr << msg << '\n';
  }
}
