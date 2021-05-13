/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         _asn1_yyparse
#define yylex           _asn1_yylex
#define yyerror         _asn1_yyerror
#define yydebug         _asn1_yydebug
#define yynerrs         _asn1_yynerrs

#define yylval          _asn1_yylval
#define yychar          _asn1_yychar

/* Copy the first part of user declarations.  */
#line 1 "ASN1.y" /* yacc.c:339  */

/*
 * Copyright (C) 2001-2014 Free Software Foundation, Inc.
 *
 * This file is part of LIBTASN1.
 *
 * The LIBTASN1 library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

/*****************************************************/
/* File: x509_ASN.y                                  */
/* Description: input file for 'bison' program.      */
/*   The output file is a parser (in C language) for */
/*   ASN.1 syntax                                    */
/*****************************************************/

#include <int.h>
#include <parser_aux.h>
#include <structure.h>
#include <libtasn1.h>

static FILE *file_asn1;			/* Pointer to file to parse */
static int result_parse = 0;	/* result of the parser
					   algorithm */
static asn1_node p_tree;		/* pointer to the root of the
					   structure created by the
					   parser*/
static unsigned int line_number;	/* line number describing the
					   parser position inside the
					   file */
static char last_error[ASN1_MAX_ERROR_DESCRIPTION_SIZE] = "";
static char last_error_token[ASN1_MAX_ERROR_DESCRIPTION_SIZE+1] = ""; /* used when expected errors occur */
static char last_token[ASN1_MAX_NAME_SIZE+1] = ""; /* last token find in the file
					   to parse before the 'parse
					   error' */
extern char _asn1_identifierMissing[];
static const char *file_name;		/* file to parse */

static void _asn1_yyerror (const char *);
static int _asn1_yylex(void);


#line 130 "ASN1.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int _asn1_yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ASSIG = 258,
    NUM = 259,
    IDENTIFIER = 260,
    OPTIONAL = 261,
    INTEGER = 262,
    SIZE = 263,
    OCTET = 264,
    STRING = 265,
    SEQUENCE = 266,
    BIT = 267,
    UNIVERSAL = 268,
    PRIVATE = 269,
    APPLICATION = 270,
    DEFAULT = 271,
    CHOICE = 272,
    OF = 273,
    OBJECT = 274,
    STR_IDENTIFIER = 275,
    BOOLEAN = 276,
    ASN1_TRUE = 277,
    ASN1_FALSE = 278,
    TOKEN_NULL = 279,
    ANY = 280,
    DEFINED = 281,
    BY = 282,
    SET = 283,
    EXPLICIT = 284,
    IMPLICIT = 285,
    DEFINITIONS = 286,
    TAGS = 287,
    BEGIN = 288,
    END = 289,
    UTCTime = 290,
    GeneralizedTime = 291,
    GeneralString = 292,
    NumericString = 293,
    IA5String = 294,
    TeletexString = 295,
    PrintableString = 296,
    UniversalString = 297,
    BMPString = 298,
    UTF8String = 299,
    VisibleString = 300,
    FROM = 301,
    IMPORTS = 302,
    ENUMERATED = 303
  };
#endif
/* Tokens.  */
#define ASSIG 258
#define NUM 259
#define IDENTIFIER 260
#define OPTIONAL 261
#define INTEGER 262
#define SIZE 263
#define OCTET 264
#define STRING 265
#define SEQUENCE 266
#define BIT 267
#define UNIVERSAL 268
#define PRIVATE 269
#define APPLICATION 270
#define DEFAULT 271
#define CHOICE 272
#define OF 273
#define OBJECT 274
#define STR_IDENTIFIER 275
#define BOOLEAN 276
#define ASN1_TRUE 277
#define ASN1_FALSE 278
#define TOKEN_NULL 279
#define ANY 280
#define DEFINED 281
#define BY 282
#define SET 283
#define EXPLICIT 284
#define IMPLICIT 285
#define DEFINITIONS 286
#define TAGS 287
#define BEGIN 288
#define END 289
#define UTCTime 290
#define GeneralizedTime 291
#define GeneralString 292
#define NumericString 293
#define IA5String 294
#define TeletexString 295
#define PrintableString 296
#define UniversalString 297
#define BMPString 298
#define UTF8String 299
#define VisibleString 300
#define FROM 301
#define IMPORTS 302
#define ENUMERATED 303

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 62 "ASN1.y" /* yacc.c:355  */

  unsigned int constant;
  char str[ASN1_MAX_NAME_SIZE+1];
  asn1_node node;

#line 269 "ASN1.c" /* yacc.c:355  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE _asn1_yylval;

int _asn1_yyparse (void);



/* Copy the second part of user declarations.  */

#line 284 "ASN1.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   223

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  60
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  52
/* YYNRULES -- Number of rules.  */
#define YYNRULES  128
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  223

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   303

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      51,    52,     2,    49,    53,    50,    59,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    54,     2,    55,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    57,    56,    58,     2,     2,     2,     2,
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
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   135,   135,   148,   149,   152,   155,   156,   159,   160,
     163,   164,   165,   168,   169,   172,   174,   179,   180,   184,
     186,   191,   192,   196,   197,   198,   201,   203,   207,   208,
     209,   212,   214,   215,   219,   220,   224,   225,   227,   228,
     235,   238,   239,   242,   244,   250,   251,   254,   255,   259,
     260,   264,   265,   269,   270,   274,   275,   279,   280,   284,
     285,   289,   290,   294,   295,   299,   300,   304,   309,   310,
     314,   315,   316,   321,   327,   330,   332,   335,   336,   337,
     338,   339,   340,   341,   342,   343,   344,   345,   346,   347,
     348,   349,   350,   351,   352,   353,   354,   355,   358,   359,
     364,   365,   368,   371,   374,   375,   379,   381,   383,   388,
     390,   392,   397,   401,   402,   407,   409,   412,   416,   421,
     427,   428,   431,   432,   436,   439,   441,   465,   466
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "\"::=\"", "NUM", "IDENTIFIER",
  "OPTIONAL", "INTEGER", "SIZE", "OCTET", "STRING", "SEQUENCE", "BIT",
  "UNIVERSAL", "PRIVATE", "APPLICATION", "DEFAULT", "CHOICE", "OF",
  "OBJECT", "STR_IDENTIFIER", "BOOLEAN", "ASN1_TRUE", "ASN1_FALSE",
  "TOKEN_NULL", "ANY", "DEFINED", "BY", "SET", "EXPLICIT", "IMPLICIT",
  "DEFINITIONS", "TAGS", "BEGIN", "END", "UTCTime", "GeneralizedTime",
  "GeneralString", "NumericString", "IA5String", "TeletexString",
  "PrintableString", "UniversalString", "BMPString", "UTF8String",
  "VisibleString", "FROM", "IMPORTS", "ENUMERATED", "'+'", "'-'", "'('",
  "')'", "','", "'['", "']'", "'|'", "'{'", "'}'", "'.'", "$accept",
  "definitions", "pos_num", "neg_num", "pos_neg_num", "num_identifier",
  "int_identifier", "pos_neg_identifier", "constant", "constant_list",
  "obj_constant", "obj_constant_list", "class", "tag_type", "tag",
  "default", "pos_neg_list", "integer_def", "boolean_def", "Time",
  "size_def2", "size_def", "generalstring_def", "numericstring_def",
  "ia5string_def", "teletexstring_def", "printablestring_def",
  "universalstring_def", "bmpstring_def", "utf8string_def",
  "visiblestring_def", "octet_string_def", "bit_element",
  "bit_element_list", "bit_string_def", "enumerated_def", "object_def",
  "type_assig_right", "type_assig_right_tag",
  "type_assig_right_tag_default", "type_assig", "type_assig_list",
  "sequence_def", "set_def", "choise_def", "any_def", "type_def",
  "constant_def", "type_constant", "type_constant_list", "definitions_id",
  "explicit_implicit", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,    43,
      45,    40,    41,    44,    91,    93,   124,   123,   125,    46
};
# endif

#define YYPACT_NINF -129

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-129)))

#define YYTABLE_NINF -12

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      36,    -7,    74,    22,     3,  -129,    26,  -129,    34,  -129,
    -129,  -129,     5,  -129,  -129,    55,    89,  -129,  -129,    97,
      57,   101,  -129,   122,   141,   132,  -129,  -129,  -129,    32,
      77,    77,   152,   165,   150,  -129,  -129,     6,   119,   167,
      18,   168,   124,   159,  -129,  -129,   156,    21,  -129,  -129,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   126,
      30,    38,   121,   133,  -129,  -129,  -129,  -129,  -129,  -129,
    -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,
    -129,  -129,  -129,  -129,  -129,  -129,   128,    58,   183,   136,
     180,  -129,  -129,    20,     6,   121,   184,   172,    -3,   184,
    -129,   164,   121,   184,   174,  -129,  -129,  -129,  -129,  -129,
    -129,  -129,  -129,  -129,   188,   139,  -129,  -129,  -129,   191,
    -129,  -129,  -129,    42,   149,  -129,   192,   193,  -129,  -129,
    -129,   142,   169,   146,   151,    58,  -129,    53,  -129,  -129,
      77,  -129,    71,   121,   188,  -129,    83,   195,  -129,    90,
     121,   153,  -129,    94,  -129,   148,   147,  -129,   197,  -129,
     154,    43,     7,  -129,  -129,   149,  -129,   -21,  -129,    58,
     155,    20,  -129,    24,  -129,   184,  -129,  -129,   114,  -129,
    -129,  -129,  -129,   201,   188,  -129,  -129,   157,   158,  -129,
      58,  -129,    23,  -129,   160,   162,  -129,  -129,  -129,    54,
    -129,  -129,  -129,   163,  -129,    47,  -129,  -129,   169,  -129,
    -129,  -129,  -129,  -129,  -129,  -129,  -129,   204,   166,   170,
    -129,  -129,  -129
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,   126,     0,     0,     0,     1,     0,     8,     9,   125,
      19,    21,     0,   127,   128,     0,     0,   124,    22,     0,
       0,     0,    20,     0,     0,     0,   120,   121,   122,     0,
       0,     0,     0,     0,     0,     2,   123,    75,    36,     0,
       0,     0,     0,     0,    40,    97,   113,     0,    41,    42,
      47,    49,    51,    53,    55,    57,    59,    61,    63,     0,
       0,    28,     0,    77,    79,    80,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    81,    82,    78,    93,    98,
     116,    92,    96,    94,    95,   115,     0,     0,     0,     0,
       0,    45,    76,     0,    65,     0,     0,     0,    70,     0,
      74,     0,     0,     0,     0,    48,    50,    52,    54,    56,
      58,    60,    62,    64,     0,     0,    23,    24,    25,     0,
      29,    30,    99,     0,     0,     3,     0,     0,     6,     7,
     119,     0,     0,     0,     0,     0,    17,     0,    66,   107,
       0,   104,     0,     0,     0,    71,     0,     0,   110,     0,
       0,     0,    68,     0,    26,     0,     3,    12,     0,    34,
       0,     0,     0,     4,     5,     0,     9,     0,    46,     0,
       0,     0,    37,   100,   103,     0,   106,   108,     0,   112,
     114,   109,   111,     0,     0,    73,    27,     5,     0,    38,
       0,   118,     0,    43,     0,     0,    15,    18,   102,     0,
     101,   105,    72,     0,    69,     0,    35,   117,     0,    16,
      14,    32,    33,    13,    31,    67,    10,     0,     0,     0,
      11,    39,    44
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -129,  -129,  -129,  -129,  -120,  -128,     4,  -129,    39,  -129,
     -12,   -92,  -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,
     130,   -34,  -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,
    -129,  -129,    27,    68,  -129,  -129,  -129,   -60,   -30,  -129,
      46,    72,  -129,  -129,  -129,  -129,  -129,  -129,   194,  -129,
    -129,  -129
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,   128,   129,   130,    10,   160,   214,   136,   137,
      11,    12,   119,    61,    62,   200,   161,    63,    64,    65,
      91,    92,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,   152,   153,    76,    77,    78,    79,    80,   174,
     141,   142,    81,    82,    83,    84,    26,    27,    28,    29,
       3,    15
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      18,    85,   122,   159,   167,    89,    97,     7,     8,     7,
       8,     7,     8,   104,    89,   170,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   134,    89,     7,     8,    89,
     198,   193,   162,    24,   115,   139,    95,    25,   194,   102,
     199,     1,   148,   116,   117,   118,   156,   157,    90,   195,
       4,   216,   157,     6,   144,    13,    14,    90,   125,   210,
     138,     9,   125,    17,   145,   191,    35,   120,   121,    90,
     206,   135,    90,   192,     5,    96,   211,   212,   103,   213,
     219,   207,    37,   177,    38,    16,    39,    19,    40,    41,
     182,   126,   158,    20,    42,   189,    43,   217,    44,   190,
      21,    45,    46,   126,   127,    47,   171,   126,   127,    22,
     173,   172,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    24,   175,    59,    37,    25,    38,   176,
      39,    60,    40,    41,    23,    31,   175,    32,    42,    33,
      43,   179,    44,   175,    30,    45,    46,   184,   181,    47,
      18,    34,   185,     7,     8,    86,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,   184,    87,    59,
      88,   146,   202,     7,   166,   149,    93,    94,    98,   100,
      18,    99,   101,   114,   123,   124,   131,   132,    89,   140,
     143,   147,   150,   151,   154,   155,   163,   164,   168,   165,
     180,   187,   169,   186,   183,   203,   -10,   196,   220,   218,
     197,   204,   178,   188,   209,   215,   -11,   205,   221,   208,
     133,   201,   222,    36
};

static const yytype_uint8 yycheck[] =
{
      12,    31,    62,   123,   132,     8,    40,     4,     5,     4,
       5,     4,     5,    47,     8,   135,    50,    51,    52,    53,
      54,    55,    56,    57,    58,     5,     8,     4,     5,     8,
       6,    52,   124,     1,     4,    95,    18,     5,    59,    18,
      16,     5,   102,    13,    14,    15,     4,     5,    51,   169,
      57,     4,     5,    31,    57,    29,    30,    51,     4,     5,
      94,    58,     4,    58,    98,    58,    34,    29,    30,    51,
     190,    51,    51,   165,     0,    57,    22,    23,    57,   199,
     208,    58,     5,   143,     7,    51,     9,    32,    11,    12,
     150,    49,    50,     4,    17,    52,    19,    50,    21,    56,
       3,    24,    25,    49,    50,    28,    53,    49,    50,    52,
     140,    58,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,     1,    53,    48,     5,     5,     7,    58,
       9,    54,    11,    12,    33,     3,    53,     5,    17,     7,
      19,    58,    21,    53,     3,    24,    25,    53,    58,    28,
     162,    19,    58,     4,     5,     3,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    53,     3,    48,
      20,    99,    58,     4,     5,   103,    57,    10,    10,    20,
     192,    57,    26,    57,    51,    57,     3,    51,     8,     5,
      18,    27,    18,     5,    55,     4,     4,     4,    52,    57,
       5,     4,    51,    55,    51,     4,    59,    52,     4,   205,
     171,   184,   144,    59,    52,    52,    59,    59,    52,    59,
      90,   175,    52,    29
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     5,    61,   110,    57,     0,    31,     4,     5,    58,
      65,    70,    71,    29,    30,   111,    51,    58,    70,    32,
       4,     3,    52,    33,     1,     5,   106,   107,   108,   109,
       3,     3,     5,     7,    19,    34,   108,     5,     7,     9,
      11,    12,    17,    19,    21,    24,    25,    28,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    48,
      54,    73,    74,    77,    78,    79,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    94,    95,    96,    97,
      98,   102,   103,   104,   105,    98,     3,     3,    20,     8,
      51,    80,    81,    57,    10,    18,    57,    81,    10,    57,
      20,    26,    18,    57,    81,    81,    81,    81,    81,    81,
      81,    81,    81,    81,    57,     4,    13,    14,    15,    72,
      29,    30,    97,    51,    57,     4,    49,    50,    62,    63,
      64,     3,    51,    80,     5,    51,    68,    69,    81,    97,
       5,   100,   101,    18,    57,    81,   101,    27,    97,   101,
      18,     5,    92,    93,    55,     4,     4,     5,    50,    64,
      66,    76,    71,     4,     4,    57,     5,    65,    52,    51,
      64,    53,    58,    98,    99,    53,    58,    97,    93,    58,
       5,    58,    97,    51,    53,    58,    55,     4,    59,    52,
      56,    58,    71,    52,    59,    64,    52,    68,     6,    16,
      75,   100,    58,     4,    92,    59,    64,    58,    59,    52,
       5,    22,    23,    64,    67,    52,     4,    50,    66,    65,
       4,    52,    52
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    60,    61,    62,    62,    63,    64,    64,    65,    65,
      66,    66,    66,    67,    67,    68,    68,    69,    69,    70,
      70,    71,    71,    72,    72,    72,    73,    73,    74,    74,
      74,    75,    75,    75,    76,    76,    77,    77,    77,    77,
      78,    79,    79,    80,    80,    81,    81,    82,    82,    83,
      83,    84,    84,    85,    85,    86,    86,    87,    87,    88,
      88,    89,    89,    90,    90,    91,    91,    92,    93,    93,
      94,    94,    94,    95,    96,    97,    97,    97,    97,    97,
      97,    97,    97,    97,    97,    97,    97,    97,    97,    97,
      97,    97,    97,    97,    97,    97,    97,    97,    98,    98,
      99,    99,    99,   100,   101,   101,   102,   102,   102,   103,
     103,   103,   104,   105,   105,   106,   106,   107,   107,   107,
     108,   108,   109,   109,   110,   110,   110,   111,   111
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     8,     1,     2,     2,     1,     1,     1,     1,
       1,     2,     1,     1,     1,     3,     4,     1,     3,     1,
       4,     1,     2,     1,     1,     1,     3,     4,     1,     2,
       2,     2,     2,     2,     1,     3,     1,     4,     4,     7,
       1,     1,     1,     4,     7,     1,     3,     1,     2,     1,
       2,     1,     2,     1,     2,     1,     2,     1,     2,     1,
       2,     1,     2,     1,     2,     2,     3,     4,     1,     3,
       2,     3,     5,     4,     2,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     2,     2,     2,     1,     3,     4,     3,     4,     4,
       3,     4,     4,     1,     4,     3,     3,     7,     6,     4,
       1,     1,     1,     2,     4,     3,     1,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
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
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 138 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_DEFINITIONS|(yyvsp[-5].constant));
                    _asn1_set_name((yyval.node),_asn1_get_name((yyvsp[-7].node)));
                    _asn1_set_name((yyvsp[-7].node),"");
                    _asn1_set_right((yyvsp[-7].node),(yyvsp[-1].node));
                    _asn1_set_down((yyval.node),(yyvsp[-7].node));

		    p_tree=(yyval.node);
		    }
#line 1544 "ASN1.c" /* yacc.c:1646  */
    break;

  case 3:
#line 148 "ASN1.y" /* yacc.c:1646  */
    {snprintf((yyval.str),sizeof((yyval.str)),"%s",(yyvsp[0].str));}
#line 1550 "ASN1.c" /* yacc.c:1646  */
    break;

  case 4:
#line 149 "ASN1.y" /* yacc.c:1646  */
    {snprintf((yyval.str),sizeof((yyval.str)),"%s",(yyvsp[0].str));}
#line 1556 "ASN1.c" /* yacc.c:1646  */
    break;

  case 5:
#line 152 "ASN1.y" /* yacc.c:1646  */
    {snprintf((yyval.str),sizeof((yyval.str)),"-%s",(yyvsp[0].str));}
#line 1562 "ASN1.c" /* yacc.c:1646  */
    break;

  case 6:
#line 155 "ASN1.y" /* yacc.c:1646  */
    {snprintf((yyval.str),sizeof((yyval.str)),"%s",(yyvsp[0].str));}
#line 1568 "ASN1.c" /* yacc.c:1646  */
    break;

  case 7:
#line 156 "ASN1.y" /* yacc.c:1646  */
    {snprintf((yyval.str),sizeof((yyval.str)),"%s",(yyvsp[0].str));}
#line 1574 "ASN1.c" /* yacc.c:1646  */
    break;

  case 8:
#line 159 "ASN1.y" /* yacc.c:1646  */
    {snprintf((yyval.str),sizeof((yyval.str)),"%s",(yyvsp[0].str));}
#line 1580 "ASN1.c" /* yacc.c:1646  */
    break;

  case 9:
#line 160 "ASN1.y" /* yacc.c:1646  */
    {snprintf((yyval.str),sizeof((yyval.str)),"%s",(yyvsp[0].str));}
#line 1586 "ASN1.c" /* yacc.c:1646  */
    break;

  case 10:
#line 163 "ASN1.y" /* yacc.c:1646  */
    {snprintf((yyval.str),sizeof((yyval.str)),"%s",(yyvsp[0].str));}
#line 1592 "ASN1.c" /* yacc.c:1646  */
    break;

  case 11:
#line 164 "ASN1.y" /* yacc.c:1646  */
    {snprintf((yyval.str),sizeof((yyval.str)),"-%s",(yyvsp[0].str));}
#line 1598 "ASN1.c" /* yacc.c:1646  */
    break;

  case 12:
#line 165 "ASN1.y" /* yacc.c:1646  */
    {snprintf((yyval.str),sizeof((yyval.str)),"%s",(yyvsp[0].str));}
#line 1604 "ASN1.c" /* yacc.c:1646  */
    break;

  case 13:
#line 168 "ASN1.y" /* yacc.c:1646  */
    {snprintf((yyval.str),sizeof((yyval.str)),"%s",(yyvsp[0].str));}
#line 1610 "ASN1.c" /* yacc.c:1646  */
    break;

  case 14:
#line 169 "ASN1.y" /* yacc.c:1646  */
    {snprintf((yyval.str),sizeof((yyval.str)),"%s",(yyvsp[0].str));}
#line 1616 "ASN1.c" /* yacc.c:1646  */
    break;

  case 15:
#line 172 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_CONSTANT);
                                       _asn1_set_value((yyval.node),(yyvsp[-1].str),strlen((yyvsp[-1].str))+1);}
#line 1623 "ASN1.c" /* yacc.c:1646  */
    break;

  case 16:
#line 174 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_CONSTANT);
	                               _asn1_set_name((yyval.node),(yyvsp[-3].str));
                                       _asn1_set_value((yyval.node),(yyvsp[-1].str),strlen((yyvsp[-1].str))+1);}
#line 1631 "ASN1.c" /* yacc.c:1646  */
    break;

  case 17:
#line 179 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 1637 "ASN1.c" /* yacc.c:1646  */
    break;

  case 18:
#line 180 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[-2].node);
                                            _asn1_set_right(_asn1_get_last_right((yyvsp[-2].node)),(yyvsp[0].node));}
#line 1644 "ASN1.c" /* yacc.c:1646  */
    break;

  case 19:
#line 184 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_CONSTANT);
                                   _asn1_set_value((yyval.node),(yyvsp[0].str),strlen((yyvsp[0].str))+1);}
#line 1651 "ASN1.c" /* yacc.c:1646  */
    break;

  case 20:
#line 186 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_CONSTANT);
	                            _asn1_set_name((yyval.node),(yyvsp[-3].str));
                                    _asn1_set_value((yyval.node),(yyvsp[-1].str),strlen((yyvsp[-1].str))+1);}
#line 1659 "ASN1.c" /* yacc.c:1646  */
    break;

  case 21:
#line 191 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 1665 "ASN1.c" /* yacc.c:1646  */
    break;

  case 22:
#line 192 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[-1].node);
                                                    _asn1_set_right(_asn1_get_last_right((yyvsp[-1].node)),(yyvsp[0].node));}
#line 1672 "ASN1.c" /* yacc.c:1646  */
    break;

  case 23:
#line 196 "ASN1.y" /* yacc.c:1646  */
    {(yyval.constant)=CONST_UNIVERSAL;}
#line 1678 "ASN1.c" /* yacc.c:1646  */
    break;

  case 24:
#line 197 "ASN1.y" /* yacc.c:1646  */
    {(yyval.constant)=CONST_PRIVATE;}
#line 1684 "ASN1.c" /* yacc.c:1646  */
    break;

  case 25:
#line 198 "ASN1.y" /* yacc.c:1646  */
    {(yyval.constant)=CONST_APPLICATION;}
#line 1690 "ASN1.c" /* yacc.c:1646  */
    break;

  case 26:
#line 201 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_TAG);
                            _asn1_set_value((yyval.node),(yyvsp[-1].str),strlen((yyvsp[-1].str))+1);}
#line 1697 "ASN1.c" /* yacc.c:1646  */
    break;

  case 27:
#line 203 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_TAG | (yyvsp[-2].constant));
                                _asn1_set_value((yyval.node),(yyvsp[-1].str),strlen((yyvsp[-1].str))+1);}
#line 1704 "ASN1.c" /* yacc.c:1646  */
    break;

  case 28:
#line 207 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 1710 "ASN1.c" /* yacc.c:1646  */
    break;

  case 29:
#line 208 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_mod_type((yyvsp[-1].node),CONST_EXPLICIT);}
#line 1716 "ASN1.c" /* yacc.c:1646  */
    break;

  case 30:
#line 209 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_mod_type((yyvsp[-1].node),CONST_IMPLICIT);}
#line 1722 "ASN1.c" /* yacc.c:1646  */
    break;

  case 31:
#line 212 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_DEFAULT);
                                       _asn1_set_value((yyval.node),(yyvsp[0].str),strlen((yyvsp[0].str))+1);}
#line 1729 "ASN1.c" /* yacc.c:1646  */
    break;

  case 32:
#line 214 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_DEFAULT|CONST_TRUE);}
#line 1735 "ASN1.c" /* yacc.c:1646  */
    break;

  case 33:
#line 215 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_DEFAULT|CONST_FALSE);}
#line 1741 "ASN1.c" /* yacc.c:1646  */
    break;

  case 36:
#line 224 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_INTEGER);}
#line 1747 "ASN1.c" /* yacc.c:1646  */
    break;

  case 37:
#line 225 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_INTEGER|CONST_LIST);
	                                 _asn1_set_down((yyval.node),(yyvsp[-1].node));}
#line 1754 "ASN1.c" /* yacc.c:1646  */
    break;

  case 38:
#line 227 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_INTEGER);}
#line 1760 "ASN1.c" /* yacc.c:1646  */
    break;

  case 39:
#line 229 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_INTEGER|CONST_MIN_MAX);
                                         _asn1_set_down((yyval.node),_asn1_add_static_node(ASN1_ETYPE_SIZE));
                                         _asn1_set_value(_asn1_get_down((yyval.node)),(yyvsp[-1].str),strlen((yyvsp[-1].str))+1);
                                         _asn1_set_name(_asn1_get_down((yyval.node)),(yyvsp[-4].str));}
#line 1769 "ASN1.c" /* yacc.c:1646  */
    break;

  case 40:
#line 235 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_BOOLEAN);}
#line 1775 "ASN1.c" /* yacc.c:1646  */
    break;

  case 41:
#line 238 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_UTC_TIME);}
#line 1781 "ASN1.c" /* yacc.c:1646  */
    break;

  case 42:
#line 239 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_GENERALIZED_TIME);}
#line 1787 "ASN1.c" /* yacc.c:1646  */
    break;

  case 43:
#line 242 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_SIZE|CONST_1_PARAM);
	                              _asn1_set_value((yyval.node),(yyvsp[-1].str),strlen((yyvsp[-1].str))+1);}
#line 1794 "ASN1.c" /* yacc.c:1646  */
    break;

  case 44:
#line 245 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_SIZE|CONST_MIN_MAX);
	                              _asn1_set_value((yyval.node),(yyvsp[-4].str),strlen((yyvsp[-4].str))+1);
                                      _asn1_set_name((yyval.node),(yyvsp[-1].str));}
#line 1802 "ASN1.c" /* yacc.c:1646  */
    break;

  case 45:
#line 250 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 1808 "ASN1.c" /* yacc.c:1646  */
    break;

  case 46:
#line 251 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[-1].node);}
#line 1814 "ASN1.c" /* yacc.c:1646  */
    break;

  case 47:
#line 254 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_GENERALSTRING);}
#line 1820 "ASN1.c" /* yacc.c:1646  */
    break;

  case 48:
#line 255 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_GENERALSTRING|CONST_SIZE);
					  _asn1_set_down((yyval.node),(yyvsp[0].node));}
#line 1827 "ASN1.c" /* yacc.c:1646  */
    break;

  case 49:
#line 259 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_NUMERIC_STRING|CONST_UNIVERSAL);}
#line 1833 "ASN1.c" /* yacc.c:1646  */
    break;

  case 50:
#line 260 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_NUMERIC_STRING|CONST_SIZE);
					  _asn1_set_down((yyval.node),(yyvsp[0].node));}
#line 1840 "ASN1.c" /* yacc.c:1646  */
    break;

  case 51:
#line 264 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_IA5_STRING);}
#line 1846 "ASN1.c" /* yacc.c:1646  */
    break;

  case 52:
#line 265 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_IA5_STRING|CONST_SIZE);
					  _asn1_set_down((yyval.node),(yyvsp[0].node));}
#line 1853 "ASN1.c" /* yacc.c:1646  */
    break;

  case 53:
#line 269 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_TELETEX_STRING);}
#line 1859 "ASN1.c" /* yacc.c:1646  */
    break;

  case 54:
#line 270 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_TELETEX_STRING|CONST_SIZE);
					  _asn1_set_down((yyval.node),(yyvsp[0].node));}
#line 1866 "ASN1.c" /* yacc.c:1646  */
    break;

  case 55:
#line 274 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_PRINTABLE_STRING);}
#line 1872 "ASN1.c" /* yacc.c:1646  */
    break;

  case 56:
#line 275 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_PRINTABLE_STRING|CONST_SIZE);
					  _asn1_set_down((yyval.node),(yyvsp[0].node));}
#line 1879 "ASN1.c" /* yacc.c:1646  */
    break;

  case 57:
#line 279 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_UNIVERSAL_STRING);}
#line 1885 "ASN1.c" /* yacc.c:1646  */
    break;

  case 58:
#line 280 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_UNIVERSAL_STRING|CONST_SIZE);
					  _asn1_set_down((yyval.node),(yyvsp[0].node));}
#line 1892 "ASN1.c" /* yacc.c:1646  */
    break;

  case 59:
#line 284 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_BMP_STRING);}
#line 1898 "ASN1.c" /* yacc.c:1646  */
    break;

  case 60:
#line 285 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_BMP_STRING|CONST_SIZE);
					  _asn1_set_down((yyval.node),(yyvsp[0].node));}
#line 1905 "ASN1.c" /* yacc.c:1646  */
    break;

  case 61:
#line 289 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_UTF8_STRING);}
#line 1911 "ASN1.c" /* yacc.c:1646  */
    break;

  case 62:
#line 290 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_UTF8_STRING|CONST_SIZE);
					  _asn1_set_down((yyval.node),(yyvsp[0].node));}
#line 1918 "ASN1.c" /* yacc.c:1646  */
    break;

  case 63:
#line 294 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_VISIBLE_STRING);}
#line 1924 "ASN1.c" /* yacc.c:1646  */
    break;

  case 64:
#line 295 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_VISIBLE_STRING|CONST_SIZE);
					  _asn1_set_down((yyval.node),(yyvsp[0].node));}
#line 1931 "ASN1.c" /* yacc.c:1646  */
    break;

  case 65:
#line 299 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_OCTET_STRING);}
#line 1937 "ASN1.c" /* yacc.c:1646  */
    break;

  case 66:
#line 300 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_OCTET_STRING|CONST_SIZE);
                                           _asn1_set_down((yyval.node),(yyvsp[0].node));}
#line 1944 "ASN1.c" /* yacc.c:1646  */
    break;

  case 67:
#line 304 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_CONSTANT);
	                           _asn1_set_name((yyval.node),(yyvsp[-3].str));
                                    _asn1_set_value((yyval.node),(yyvsp[-1].str),strlen((yyvsp[-1].str))+1);}
#line 1952 "ASN1.c" /* yacc.c:1646  */
    break;

  case 68:
#line 309 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 1958 "ASN1.c" /* yacc.c:1646  */
    break;

  case 69:
#line 310 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[-2].node);
                                                       _asn1_set_right(_asn1_get_last_right((yyvsp[-2].node)),(yyvsp[0].node));}
#line 1965 "ASN1.c" /* yacc.c:1646  */
    break;

  case 70:
#line 314 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_BIT_STRING);}
#line 1971 "ASN1.c" /* yacc.c:1646  */
    break;

  case 71:
#line 315 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_BIT_STRING|CONST_SIZE);}
#line 1977 "ASN1.c" /* yacc.c:1646  */
    break;

  case 72:
#line 317 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_BIT_STRING|CONST_LIST);
                                _asn1_set_down((yyval.node),(yyvsp[-1].node));}
#line 1984 "ASN1.c" /* yacc.c:1646  */
    break;

  case 73:
#line 322 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_ENUMERATED|CONST_LIST);
                                _asn1_set_down((yyval.node),(yyvsp[-1].node));}
#line 1991 "ASN1.c" /* yacc.c:1646  */
    break;

  case 74:
#line 327 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_OBJECT_ID);}
#line 1997 "ASN1.c" /* yacc.c:1646  */
    break;

  case 75:
#line 330 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_IDENTIFIER);
                                       _asn1_set_value((yyval.node),(yyvsp[0].str),strlen((yyvsp[0].str))+1);}
#line 2004 "ASN1.c" /* yacc.c:1646  */
    break;

  case 76:
#line 332 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_IDENTIFIER|CONST_SIZE);
                                       _asn1_set_value((yyval.node),(yyvsp[-1].str),strlen((yyvsp[-1].str))+1);
                                       _asn1_set_down((yyval.node),(yyvsp[0].node));}
#line 2012 "ASN1.c" /* yacc.c:1646  */
    break;

  case 77:
#line 335 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2018 "ASN1.c" /* yacc.c:1646  */
    break;

  case 78:
#line 336 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2024 "ASN1.c" /* yacc.c:1646  */
    break;

  case 79:
#line 337 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2030 "ASN1.c" /* yacc.c:1646  */
    break;

  case 81:
#line 339 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2036 "ASN1.c" /* yacc.c:1646  */
    break;

  case 82:
#line 340 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2042 "ASN1.c" /* yacc.c:1646  */
    break;

  case 83:
#line 341 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2048 "ASN1.c" /* yacc.c:1646  */
    break;

  case 84:
#line 342 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2054 "ASN1.c" /* yacc.c:1646  */
    break;

  case 85:
#line 343 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2060 "ASN1.c" /* yacc.c:1646  */
    break;

  case 86:
#line 344 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2066 "ASN1.c" /* yacc.c:1646  */
    break;

  case 87:
#line 345 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2072 "ASN1.c" /* yacc.c:1646  */
    break;

  case 88:
#line 346 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2078 "ASN1.c" /* yacc.c:1646  */
    break;

  case 89:
#line 347 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2084 "ASN1.c" /* yacc.c:1646  */
    break;

  case 90:
#line 348 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2090 "ASN1.c" /* yacc.c:1646  */
    break;

  case 91:
#line 349 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2096 "ASN1.c" /* yacc.c:1646  */
    break;

  case 92:
#line 350 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2102 "ASN1.c" /* yacc.c:1646  */
    break;

  case 93:
#line 351 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2108 "ASN1.c" /* yacc.c:1646  */
    break;

  case 94:
#line 352 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2114 "ASN1.c" /* yacc.c:1646  */
    break;

  case 95:
#line 353 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2120 "ASN1.c" /* yacc.c:1646  */
    break;

  case 96:
#line 354 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2126 "ASN1.c" /* yacc.c:1646  */
    break;

  case 97:
#line 355 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_NULL);}
#line 2132 "ASN1.c" /* yacc.c:1646  */
    break;

  case 98:
#line 358 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2138 "ASN1.c" /* yacc.c:1646  */
    break;

  case 99:
#line 359 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_mod_type((yyvsp[0].node),CONST_TAG);
                                               _asn1_set_right((yyvsp[-1].node),_asn1_get_down((yyval.node)));
                                               _asn1_set_down((yyval.node),(yyvsp[-1].node));}
#line 2146 "ASN1.c" /* yacc.c:1646  */
    break;

  case 100:
#line 364 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2152 "ASN1.c" /* yacc.c:1646  */
    break;

  case 101:
#line 365 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_mod_type((yyvsp[-1].node),CONST_DEFAULT);
                                                       _asn1_set_right((yyvsp[0].node),_asn1_get_down((yyval.node)));
						       _asn1_set_down((yyval.node),(yyvsp[0].node));}
#line 2160 "ASN1.c" /* yacc.c:1646  */
    break;

  case 102:
#line 368 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_mod_type((yyvsp[-1].node),CONST_OPTION);}
#line 2166 "ASN1.c" /* yacc.c:1646  */
    break;

  case 103:
#line 371 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_set_name((yyvsp[0].node),(yyvsp[-1].str));}
#line 2172 "ASN1.c" /* yacc.c:1646  */
    break;

  case 104:
#line 374 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2178 "ASN1.c" /* yacc.c:1646  */
    break;

  case 105:
#line 375 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[-2].node);
                                                _asn1_set_right(_asn1_get_last_right((yyvsp[-2].node)),(yyvsp[0].node));}
#line 2185 "ASN1.c" /* yacc.c:1646  */
    break;

  case 106:
#line 379 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_SEQUENCE);
                                              _asn1_set_down((yyval.node),(yyvsp[-1].node));}
#line 2192 "ASN1.c" /* yacc.c:1646  */
    break;

  case 107:
#line 381 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_SEQUENCE_OF);
                                              _asn1_set_down((yyval.node),(yyvsp[0].node));}
#line 2199 "ASN1.c" /* yacc.c:1646  */
    break;

  case 108:
#line 383 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_SEQUENCE_OF|CONST_SIZE);
                                            _asn1_set_right((yyvsp[-2].node),(yyvsp[0].node));
                                            _asn1_set_down((yyval.node),(yyvsp[-2].node));}
#line 2207 "ASN1.c" /* yacc.c:1646  */
    break;

  case 109:
#line 388 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_SET);
                                     _asn1_set_down((yyval.node),(yyvsp[-1].node));}
#line 2214 "ASN1.c" /* yacc.c:1646  */
    break;

  case 110:
#line 390 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_SET_OF);
                                     _asn1_set_down((yyval.node),(yyvsp[0].node));}
#line 2221 "ASN1.c" /* yacc.c:1646  */
    break;

  case 111:
#line 392 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_SET_OF|CONST_SIZE);
                                       _asn1_set_right((yyvsp[-2].node),(yyvsp[0].node));
                                       _asn1_set_down((yyval.node),(yyvsp[-2].node));}
#line 2229 "ASN1.c" /* yacc.c:1646  */
    break;

  case 112:
#line 397 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_CHOICE);
                                             _asn1_set_down((yyval.node),(yyvsp[-1].node));}
#line 2236 "ASN1.c" /* yacc.c:1646  */
    break;

  case 113:
#line 401 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_ANY);}
#line 2242 "ASN1.c" /* yacc.c:1646  */
    break;

  case 114:
#line 402 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_ANY|CONST_DEFINED_BY);
                                        _asn1_set_down((yyval.node),_asn1_add_static_node(ASN1_ETYPE_CONSTANT));
	                                _asn1_set_name(_asn1_get_down((yyval.node)),(yyvsp[0].str));}
#line 2250 "ASN1.c" /* yacc.c:1646  */
    break;

  case 115:
#line 407 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_set_name((yyvsp[0].node),(yyvsp[-2].str));}
#line 2256 "ASN1.c" /* yacc.c:1646  */
    break;

  case 116:
#line 409 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_set_name((yyvsp[0].node), last_error_token);}
#line 2262 "ASN1.c" /* yacc.c:1646  */
    break;

  case 117:
#line 413 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_OBJECT_ID|CONST_ASSIGN);
                         _asn1_set_name((yyval.node),(yyvsp[-6].str));
                         _asn1_set_down((yyval.node),(yyvsp[-1].node));}
#line 2270 "ASN1.c" /* yacc.c:1646  */
    break;

  case 118:
#line 417 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_OBJECT_ID|CONST_ASSIGN|CONST_1_PARAM);
                         _asn1_set_name((yyval.node),(yyvsp[-5].str));
                         _asn1_set_value((yyval.node),(yyvsp[-4].str),strlen((yyvsp[-4].str))+1);
                         _asn1_set_down((yyval.node),(yyvsp[-1].node));}
#line 2279 "ASN1.c" /* yacc.c:1646  */
    break;

  case 119:
#line 422 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_INTEGER|CONST_ASSIGN);
                         _asn1_set_name((yyval.node),(yyvsp[-3].str));
                         _asn1_set_value((yyval.node),(yyvsp[0].str),strlen((yyvsp[0].str))+1);}
#line 2287 "ASN1.c" /* yacc.c:1646  */
    break;

  case 120:
#line 427 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2293 "ASN1.c" /* yacc.c:1646  */
    break;

  case 121:
#line 428 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2299 "ASN1.c" /* yacc.c:1646  */
    break;

  case 122:
#line 431 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2305 "ASN1.c" /* yacc.c:1646  */
    break;

  case 123:
#line 432 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[-1].node);
                                                          _asn1_set_right(_asn1_get_last_right((yyvsp[-1].node)),(yyvsp[0].node));}
#line 2312 "ASN1.c" /* yacc.c:1646  */
    break;

  case 124:
#line 436 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_OBJECT_ID);
                                                          _asn1_set_down((yyval.node),(yyvsp[-1].node));
                                                          _asn1_set_name((yyval.node),(yyvsp[-3].str));}
#line 2320 "ASN1.c" /* yacc.c:1646  */
    break;

  case 125:
#line 439 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_OBJECT_ID);
                                                          _asn1_set_name((yyval.node),(yyvsp[-2].str));}
#line 2327 "ASN1.c" /* yacc.c:1646  */
    break;

  case 126:
#line 441 "ASN1.y" /* yacc.c:1646  */
    {(yyval.node)=_asn1_add_static_node(ASN1_ETYPE_OBJECT_ID);
                                                          _asn1_set_name((yyval.node),(yyvsp[0].str));}
#line 2334 "ASN1.c" /* yacc.c:1646  */
    break;

  case 127:
#line 465 "ASN1.y" /* yacc.c:1646  */
    {(yyval.constant)=CONST_EXPLICIT;}
#line 2340 "ASN1.c" /* yacc.c:1646  */
    break;

  case 128:
#line 466 "ASN1.y" /* yacc.c:1646  */
    {(yyval.constant)=CONST_IMPLICIT;}
#line 2346 "ASN1.c" /* yacc.c:1646  */
    break;


#line 2350 "ASN1.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
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

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 470 "ASN1.y" /* yacc.c:1906  */




static const char *key_word[] = {
  "::=","OPTIONAL","INTEGER","SIZE","OCTET","STRING",
  "SEQUENCE","BIT","UNIVERSAL","PRIVATE","OPTIONAL",
  "DEFAULT","CHOICE","OF","OBJECT","IDENTIFIER",
  "BOOLEAN","TRUE","FALSE","APPLICATION","ANY","DEFINED",
  "SET","BY","EXPLICIT","IMPLICIT","DEFINITIONS","TAGS",
  "BEGIN","END","UTCTime","GeneralizedTime",
  "GeneralString","FROM","IMPORTS","NULL","ENUMERATED",
  "NumericString", "IA5String", "TeletexString", "PrintableString",
  "UniversalString", "BMPString", "UTF8String", "VisibleString"};

static const int key_word_token[] = {
  ASSIG, OPTIONAL, INTEGER, SIZE, OCTET, STRING, SEQUENCE, BIT, UNIVERSAL,
      PRIVATE, OPTIONAL, DEFAULT, CHOICE, OF, OBJECT, STR_IDENTIFIER,
      BOOLEAN, ASN1_TRUE, ASN1_FALSE, APPLICATION, ANY, DEFINED, SET, BY,
      EXPLICIT, IMPLICIT, DEFINITIONS, TAGS, BEGIN, END, UTCTime,
      GeneralizedTime, GeneralString, FROM, IMPORTS, TOKEN_NULL,
      ENUMERATED, NumericString, IA5String, TeletexString, PrintableString,
      UniversalString, BMPString, UTF8String, VisibleString
};

/*************************************************************/
/*  Function: _asn1_yylex                                    */
/*  Description: looks for tokens in file_asn1 pointer file. */
/*  Return: int                                              */
/*    Token identifier or ASCII code or 0(zero: End Of File) */
/*************************************************************/
static int
_asn1_yylex ()
{
  int c, counter = 0, k, lastc;
  char string[ASN1_MAX_NAME_SIZE + 1];  /* will contain the next token */
  size_t i;

  while (1)
    {
      while ((c = fgetc (file_asn1)) == ' ' || c == '\t' || c == '\n')
        if (c == '\n')
          line_number++;

      if (c == EOF)
        {
          snprintf (last_token, sizeof(last_token), "End Of File");
          return 0;
        }

      if (c == '(' || c == ')' || c == '[' || c == ']' ||
          c == '{' || c == '}' || c == ',' || c == '.' ||
          c == '+' || c == '|')
        {
          last_token[0] = c;
          last_token[1] = 0;
          return c;
        }
      if (c == '-')
        {                       /* Maybe the first '-' of a comment */
          if ((c = fgetc (file_asn1)) != '-')
            {
              ungetc (c, file_asn1);
              last_token[0] = '-';
              last_token[1] = 0;
              return '-';
            }
          else
            {                   /* Comments */
              lastc = 0;
              counter = 0;
              /* A comment finishes at the next double hypen or the end of line */
              while ((c = fgetc (file_asn1)) != EOF && c != '\n' &&
                     (lastc != '-' || (lastc == '-' && c != '-')))
                lastc = c;
              if (c == EOF)
                {
                  snprintf (last_token, sizeof(last_token), "End Of File");
                  return 0;
                }
              else
                {
                  if (c == '\n')
                    line_number++;
                  continue;     /* next char, please! (repeat the search) */
                }
            }
        }
      string[counter++] = c;
      /* Till the end of the token */
      while (!
             ((c = fgetc (file_asn1)) == EOF || c == ' ' || c == '\t'
              || c == '\n' || c == '(' || c == ')' || c == '[' || c == ']'
              || c == '{' || c == '}' || c == ',' || c == '.'))
        {
          if (counter >= ASN1_MAX_NAME_SIZE)
            {
              result_parse = ASN1_NAME_TOO_LONG;
              return 0;
            }
          string[counter++] = c;
        }
      ungetc (c, file_asn1);
      string[counter] = 0;
      snprintf (last_token, sizeof(last_token), "%s", string);

      /* Is STRING a number? */
      for (k = 0; k < counter; k++)
        if (!isdigit (string[k]))
          break;
      if (k >= counter)
        {
          snprintf (yylval.str, sizeof(yylval.str), "%s", string);
          return NUM;           /* return the number */
        }

      /* Is STRING a keyword? */
      for (i = 0; i < (sizeof (key_word) / sizeof (char *)); i++)
        if (!strcmp (string, key_word[i]))
          return key_word_token[i];

      /* STRING is an IDENTIFIER */
      snprintf (yylval.str, sizeof(yylval.str), "%s", string);
      return IDENTIFIER;
    }
}

/*************************************************************/
/*  Function: _asn1_create_errorDescription                  */
/*  Description: creates a string with the description of the*/
/*    error.                                                 */
/*  Parameters:                                              */
/*    error : error to describe.                             */
/*    error_desc: string that will contain the         */
/*                      description.                         */
/*************************************************************/
static void
_asn1_create_errorDescription (int error, char *error_desc)
{
  if (error_desc == NULL)
    return;


  switch (error)
    {
    case ASN1_FILE_NOT_FOUND:
      snprintf(error_desc, ASN1_MAX_ERROR_DESCRIPTION_SIZE, "%s file was not found", file_name);
      break;
    case ASN1_SYNTAX_ERROR:
      snprintf(error_desc, ASN1_MAX_ERROR_DESCRIPTION_SIZE, "%s", last_error);
      break;
    case ASN1_NAME_TOO_LONG:
      snprintf (error_desc, ASN1_MAX_ERROR_DESCRIPTION_SIZE,
                "%s:%u: name too long (more than %u characters)", file_name,
                line_number, ASN1_MAX_NAME_SIZE);
      break;
    case ASN1_IDENTIFIER_NOT_FOUND:
      snprintf (error_desc, ASN1_MAX_ERROR_DESCRIPTION_SIZE,
                "%s:: identifier '%s' not found", file_name,
                _asn1_identifierMissing);
      break;
    default:
      error_desc[0] = 0;
      break;
    }

}

/**
 * asn1_parser2tree:
 * @file: specify the path and the name of file that contains
 *   ASN.1 declarations.
 * @definitions: return the pointer to the structure created from
 *   "file" ASN.1 declarations.
 * @error_desc: return the error description or an empty
 * string if success.
 *
 * Function used to start the parse algorithm.  Creates the structures
 * needed to manage the definitions included in @file file.
 *
 * Returns: %ASN1_SUCCESS if the file has a correct syntax and every
 *   identifier is known, %ASN1_ELEMENT_NOT_EMPTY if @definitions not
 *   %NULL, %ASN1_FILE_NOT_FOUND if an error occured while
 *   opening @file, %ASN1_SYNTAX_ERROR if the syntax is not
 *   correct, %ASN1_IDENTIFIER_NOT_FOUND if in the file there is an
 *   identifier that is not defined, %ASN1_NAME_TOO_LONG if in the
 *   file there is an identifier whith more than %ASN1_MAX_NAME_SIZE
 *   characters.
 **/
int
asn1_parser2tree (const char *file, asn1_node * definitions,
                  char *error_desc)
{

  p_tree = NULL;

  if (*definitions != NULL)
    return ASN1_ELEMENT_NOT_EMPTY;

  *definitions = NULL;

  file_name = file;

  /* open the file to parse */
  file_asn1 = fopen (file, "r");

  if (file_asn1 == NULL)
    {
      result_parse = ASN1_FILE_NOT_FOUND;
    }
  else
    {
      result_parse = ASN1_SUCCESS;

      line_number = 1;
      yyparse ();

      fclose (file_asn1);

      if (result_parse == ASN1_SUCCESS)
        {                       /* syntax OK */
          /* set IMPLICIT or EXPLICIT property */
          _asn1_set_default_tag (p_tree);
          /* set CONST_SET and CONST_NOT_USED */
          _asn1_type_set_config (p_tree);
          /* check the identifier definitions */
          result_parse = _asn1_check_identifier (p_tree);
          if (result_parse == ASN1_SUCCESS)
            {                   /* all identifier defined */
              /* Delete the list and keep the ASN1 structure */
              _asn1_delete_list ();
              /* Convert into DER coding the value assign to INTEGER constants */
              _asn1_change_integer_value (p_tree);
              /* Expand the IDs of OBJECT IDENTIFIER constants */
              _asn1_expand_object_id (p_tree);

              *definitions = p_tree;
            }
          else                  /* some identifiers not defined */
            /* Delete the list and the ASN1 structure */
            _asn1_delete_list_and_nodes ();
        }
      else                      /* syntax error */
        /* Delete the list and the ASN1 structure */
        _asn1_delete_list_and_nodes ();
    }

  _asn1_create_errorDescription (result_parse, error_desc);

  return result_parse;
}

/**
 * asn1_parser2array:
 * @inputFileName: specify the path and the name of file that
 *   contains ASN.1 declarations.
 * @outputFileName: specify the path and the name of file that will
 *   contain the C vector definition.
 * @vectorName: specify the name of the C vector.
 * @error_desc: return the error description or an empty
 *   string if success.
 *
 * Function that generates a C structure from an ASN1 file.  Creates a
 * file containing a C vector to use to manage the definitions
 * included in @inputFileName file. If @inputFileName is
 * "/aa/bb/xx.yy" and @outputFileName is %NULL, the file created is
 * "/aa/bb/xx_asn1_tab.c".  If @vectorName is %NULL the vector name
 * will be "xx_asn1_tab".
 *
 * Returns: %ASN1_SUCCESS if the file has a correct syntax and every
 *   identifier is known, %ASN1_FILE_NOT_FOUND if an error occured
 *   while opening @inputFileName, %ASN1_SYNTAX_ERROR if the syntax is
 *   not correct, %ASN1_IDENTIFIER_NOT_FOUND if in the file there is
 *   an identifier that is not defined, %ASN1_NAME_TOO_LONG if in the
 *   file there is an identifier whith more than %ASN1_MAX_NAME_SIZE
 *   characters.
 **/
int
asn1_parser2array (const char *inputFileName, const char *outputFileName,
                   const char *vectorName, char *error_desc)
{
  char *file_out_name = NULL;
  char *vector_name = NULL;
  const char *char_p, *slash_p, *dot_p;

  p_tree = NULL;

  file_name = inputFileName;

  /* open the file to parse */
  file_asn1 = fopen (inputFileName, "r");

  if (file_asn1 == NULL)
    result_parse = ASN1_FILE_NOT_FOUND;
  else
    {
      result_parse = ASN1_SUCCESS;

      line_number = 1;
      yyparse ();

      fclose (file_asn1);

      if (result_parse == ASN1_SUCCESS)
        {                       /* syntax OK */
          /* set IMPLICIT or EXPLICIT property */
          _asn1_set_default_tag (p_tree);
          /* set CONST_SET and CONST_NOT_USED */
          _asn1_type_set_config (p_tree);
          /* check the identifier definitions */
          result_parse = _asn1_check_identifier (p_tree);

          if (result_parse == ASN1_SUCCESS)
            {                   /* all identifier defined */

              /* searching the last '/' and '.' in inputFileName */
              char_p = inputFileName;
              slash_p = inputFileName;
              while ((char_p = strchr (char_p, '/')))
                {
                  char_p++;
                  slash_p = char_p;
                }

              char_p = slash_p;
              dot_p = inputFileName + strlen (inputFileName);

              while ((char_p = strchr (char_p, '.')))
                {
                  dot_p = char_p;
                  char_p++;
                }

              if (outputFileName == NULL)
                {
                  /* file_out_name = inputFileName + _asn1_tab.c */
                  file_out_name = malloc (dot_p - inputFileName + 1 +
                                          strlen ("_asn1_tab.c"));
                  memcpy (file_out_name, inputFileName,
                          dot_p - inputFileName);
                  file_out_name[dot_p - inputFileName] = 0;
                  strcat (file_out_name, "_asn1_tab.c");
                }
              else
                {
                  /* file_out_name = inputFileName */
                  file_out_name =
                      (char *) malloc (strlen (outputFileName) + 1);
                  strcpy (file_out_name, outputFileName);
                }

              if (vectorName == NULL)
                {
                  /* vector_name = file name + _asn1_tab */
                  vector_name = malloc (dot_p - slash_p + 1 +
                                        strlen ("_asn1_tab"));
                  memcpy (vector_name, slash_p, dot_p - slash_p);
                  vector_name[dot_p - slash_p] = 0;
                  strcat (vector_name, "_asn1_tab");
                }
              else
                {
                  /* vector_name = vectorName */
                  vector_name = (char *) malloc (strlen (vectorName) + 1);
                  strcpy (vector_name, vectorName);
                }

              /* Save structure in a file */
              _asn1_create_static_structure (p_tree,
                                             file_out_name, vector_name);

              free (file_out_name);
              free (vector_name);
            }                   /* result == OK */
        }                       /* result == OK */

      /* Delete the list and the ASN1 structure */
      _asn1_delete_list_and_nodes ();
    }                           /* inputFile exist */

  _asn1_create_errorDescription (result_parse, error_desc);

  return result_parse;
}

/*************************************************************/
/*  Function: _asn1_yyerror                                  */
/*  Description: function called when there are syntax errors*/
/*  Parameters:                                              */
/*    char *s : error description                            */
/*  Return: int                                              */
/*                                                           */
/*************************************************************/
static void
_asn1_yyerror (const char *s)
{
  /* Sends the error description to the std_out */

  if (strcmp (last_token, "VisibleString") == 0 ||
      strcmp (last_token, "PrintableString") == 0 ||
      strcmp (last_token, "UniversalString") == 0 ||
      strcmp (last_token, "IA5String") == 0 ||
      strcmp (last_token, "UTF8String") == 0 ||
      strcmp (last_token, "NumericString") == 0 ||
      strcmp (last_token, "TeletexString") == 0 ||
      strcmp (last_token, "BMPString") == 0)
    {
      snprintf (last_error_token, sizeof(last_error_token),
                "%s", last_token);
      fprintf(stderr, "%s:%u: Warning: %s is a built-in ASN.1 type.\n",
               file_name, line_number, last_token);
      return;
    }
  last_error_token[0] = 0;

  if (result_parse != ASN1_NAME_TOO_LONG)
    {
      snprintf (last_error, sizeof(last_error),
                "%s:%u: Error: %s near '%s'", file_name,
                line_number, s, last_token);
      result_parse = ASN1_SYNTAX_ERROR;
    }

  return;
}
