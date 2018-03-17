
// wedit
// fork from: edi version 5.0   20-Feb-2002

/* ======= */
/* include */
/* ======= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <values.h>
#include <unistd.h>
#include <ctype.h>
#include <curses.h>

int Debug = 0;
FILE *logfile;

int CurrentCols;
int CurrentLines;

struct SequenceCode
{
   char *Sequence;
   int Code;
};

struct SequenceCode EscapeCode[] =
{
   {"[A",KEY_UP   },  /* ANSI cursor sequences */
   {"[B",KEY_DOWN },  /* vt100, xterm, sun-cmd */
   {"[C",KEY_RIGHT},
   {"[D",KEY_LEFT },

   {"OA",KEY_UP   },  /* vt100 cursor sequences */
   {"OB",KEY_DOWN },  /* in application mode    */
   {"OC",KEY_RIGHT},
   {"OD",KEY_LEFT },

   {"[11~",KEY_F( 1)},  /* function key sequences  */
   {"[12~",KEY_F( 2)},  /* for dtterm, xterm & kde */
   {"[13~",KEY_F( 3)},
   {"[14~",KEY_F( 4)},
   {"[15~",KEY_F( 5)},
   {"[17~",KEY_F( 6)},
   {"[18~",KEY_F( 7)},
   {"[19~",KEY_F( 8)},
   {"[20~",KEY_F( 9)},
   {"[21~",KEY_F(10)},
   {"[23~",KEY_F(11)},
   {"[24~",KEY_F(12)},

   {"[25~",KEY_F(13)},  /* shifted function keys */
   {"[26~",KEY_F(14)},  /* xterm (XFree 4.xx)    */
   {"[28~",KEY_F(15)},
   {"[29~",KEY_F(16)},
   {"[31~",KEY_F(17)},
   {"[32~",KEY_F(18)},
   {"[33~",KEY_F(19)},
   {"[34~",KEY_F(20)},
   {"[35~",KEY_F(21)},
   {"[36~",KEY_F(22)},
   {"[37~",KEY_F(23)},
   {"[38~",KEY_F(24)},

   {"[11;2~",KEY_F(13)},  /* shifted function keys */
   {"[12;2~",KEY_F(14)},  /* VT420PC               */
   {"[13;2~",KEY_F(15)},
   {"[14;2~",KEY_F(16)},
   {"[15;2~",KEY_F(17)},
   {"[17;2~",KEY_F(18)},
   {"[18;2~",KEY_F(19)},
   {"[19;2~",KEY_F(20)},
   {"[20;2~",KEY_F(21)},
   {"[21;2~",KEY_F(22)},
   {"[23;2~",KEY_F(23)},
   {"[24;2~",KEY_F(24)},

   {"[41~",KEY_F(13)},  /* function key sequences */
   {"[42~",KEY_F(14)},  /* defined in .Xdefaults  */
   {"[43~",KEY_F(15)},
   {"[44~",KEY_F(16)},
   {"[45~",KEY_F(17)},
   {"[46~",KEY_F(18)},
   {"[47~",KEY_F(19)},
   {"[48~",KEY_F(20)},
   {"[49~",KEY_F(21)},
   {"[50~",KEY_F(22)},
   {"[51~",KEY_F(23)},
   {"[52~",KEY_F(24)},

   {"Oq",KEY_F(1) },  /* vt100 keypad sequences */
   {"Or",KEY_F(2) },  /* in application mode    */
   {"Os",KEY_F(3) },
   {"Ot",KEY_F(4) },
   {"Ou",KEY_F(5) },
   {"Ov",KEY_F(6) },
   {"Ow",KEY_F(7) },
   {"Ox",KEY_F(8) },
   {"Oy",KEY_F(9) },

   {"OP",KEY_F(1) },  /* vt100 function sequences */
   {"OQ",KEY_F(2) },  /* in application mode      */
   {"OR",KEY_F(3) },
   {"OS",KEY_F(4) },

   {"Oo",KEY_F(14) },
   {"Oj",KEY_F(15) },
   {"Om",KEY_F(16) },
   {"Ok",KEY_F(17) },
   {"OM",KEY_F(18) },
   {"Ol",KEY_F(19) },
   {"Op",KEY_F(20) },

   {"[224z",KEY_F( 1)},  /* function key sequences */
   {"[225z",KEY_F( 2)},  /* for sun-cmd            */
   {"[226z",KEY_F( 3)},
   {"[227z",KEY_F( 4)},
   {"[228z",KEY_F( 5)},
   {"[229z",KEY_F( 6)},
   {"[230z",KEY_F( 7)},
   {"[231z",KEY_F( 8)},
   {"[232z",KEY_F( 9)},
   {"[233z",KEY_F(10)},
   {"[234z",KEY_F(11)},
   {"[235z",KEY_F(12)},

   {"[236z",KEY_F(13)},  /* function key sequences */
   {"[237z",KEY_F(14)},  /* for sun-cmd            */
   {"[238z",KEY_F(15)},
   {"[239z",KEY_F(16)},
   {"[240z",KEY_F(17)},
   {"[241z",KEY_F(18)},
   {"[242z",KEY_F(19)},
   {"[243z",KEY_F(20)},
   {"[244z",KEY_F(21)},
   {"[245z",KEY_F(22)},
   {"[246z",KEY_F(23)},
   {"[247z",KEY_F(24)},

   {"[216z",KEY_PPAGE},
   {"[222z",KEY_NPAGE},
   {"[214z",KEY_HOME },
   {"[220z",KEY_END  },

   {"[1~",KEY_HOME },  /* vt220 special keys */
   {"[2~",KEY_IC   },
   {"[3~",KEY_DC   },
   {"[4~",KEY_END  },
   {"[5~",KEY_PPAGE},
   {"[6~",KEY_NPAGE}
};

#define MAXCODE (sizeof(EscapeCode) / sizeof(struct SequenceCode))

/* ====== */
/* define */
/* ====== */

#define LONG int
int FuncLines = 2;

#define CONTROL(A) (A & 0x1F)
#define Escape   27
#define LINEMAX 256

typedef char String[LINEMAX];

#define MARKED 1

int MenuKey = KEY_F(1);

struct ColorStruct
{
   int Color;
   char *Name;
} ColorTab[] =
{
   { COLOR_BLACK       ,"Black"       },
   { COLOR_WHITE       ,"White"       },
   { COLOR_RED         ,"Red"         },
   { COLOR_GREEN       ,"Green"       },
   { COLOR_BLUE        ,"Blue"        },
   { COLOR_YELLOW      ,"Yellow"      },
   { COLOR_MAGENTA     ,"Magenta"     },
   { COLOR_CYAN        ,"Cyan"        }
};

int AllColor  =  A_COLOR;
int TextColor =  1; /* Normal Text                */
int MarkColor =  2; /* Marked Text                */
int MenuColor =  3; /* Menu & Function Key Labels */
int MeSeColor =  4; /* Selected Menu Color        */
int StatColor =  5; /* Status Line                */
int FramColor =  6; /* Frames & Box Color         */
int LabeColor =  7; /* Labels in FORTRAN          */
int HighColor =  8; /* Reserved Words             */
int CommColor =  9; /* Comments                   */
int EquaColor = 10; /* Assignment operator =      */

int ContColor;
int PromColor;


struct AttributeStruct
{
   int   Foreground;
   int   Background;
   int  *Attribute;
   char *Name;
} AttributeTab[] =
{
   {COLOR_BLACK  ,COLOR_WHITE ,&TextColor,"TextColor"},
   {COLOR_RED    ,COLOR_WHITE ,&MarkColor,"MarkColor"},
   {COLOR_YELLOW ,COLOR_BLUE  ,&MenuColor,"MenuColor"},
   {COLOR_BLUE   ,COLOR_YELLOW,&MeSeColor,"MenuSelectColor"},
   {COLOR_BLUE   ,COLOR_WHITE ,&StatColor,"StatusColor"},
   {COLOR_YELLOW ,COLOR_BLACK ,&FramColor,"BoxColor"},
   {COLOR_RED    ,COLOR_WHITE ,&LabeColor,"LabelColor"},
   {COLOR_BLUE   ,COLOR_WHITE ,&HighColor,"HighColor"},
   {COLOR_GREEN  ,COLOR_WHITE ,&CommColor,"CommentColor"},
   {COLOR_MAGENTA,COLOR_WHITE ,&EquaColor,"EquateColor"}
};

int Pad = 0;

String ESCBuffer = "<ESC>";
int KeyCode;

int TabStop[LINEMAX];

struct FunctionArray
{
        char  *Label;
        char  *Name;
        void (*Func)();
};

struct FunctionArray Ukey[24];

struct LinDesc
{                        /*A complete text line, with links*/
        LONG     No;
        struct LinDesc *Next;   /*Pointer to previous line in stream*/
        struct LinDesc *Prev;   /*Pointer to next line in stream*/
        char    *Text;   /*Pointer to line buffer*/
        int      Flag;   /*Define these in the const section*/
        int      Leng;   /*This pads the record to X*4 bytes*/
};

struct LinDesc DummyLine = {0,NULL,NULL,"~",0,1};

struct WinDesc
{
   LONG  LastNo;            /*Last line in file               */
   struct WinDesc *Next;     /* Next window or NULL            */
        struct LinDesc *FirLine;  /* Pointer to first line in file  */
        struct LinDesc *TopLine;  /* Pointer to first line in view  */
        struct LinDesc *CurLine;  /* Pointer to line cursor is On   */
        int    StaScreenRow;      /* Screen row for status line     */
        int    TopScreenRow;      /* Screen row for first edit line */
        int    BotScreenRow;      /* Screen row for last  edit line */
        int    BotWindowRow;      /* Last line in window            */
        int    Insert;            /*Insert mode flag                */
        int    Row;               /*Last cursor address*/
        int    Col;               /*Last cursor address*/
        int    Ofs;               /*Offset of displayed text column*/
        int    Dirty;             /*EditChangeFlag*/
        char   FileName[256];     /*File being edited*/
};

struct WinDesc  PriWin;
struct WinDesc  SecWin;

struct LinDesc *Block1;       /* Points to beginning of block */
struct LinDesc *Block2;       /* Points to end of block       */
int      BlockW;       /* Points to window of block    */

int    Found      ;    /* Set by search && replace cmds */
int    Replast    ;    /* Determines which operation EditFindNext does */
int    IgnCase    ;    /* Used by ScanText */
int    SearchBack ;
int    Global     ;
int    Ntimes     ;
int    WholeWord  ;    /* Used by ScanText */
int    Ask        ;
int    ReturnSplit;
String SearchStr  ;    /* Used by EditSearch, EditSearchReplace */
String ReplaceStr ;    /* Used by EditSearchReplace */
String OptStr     ;    /* Used by EditFindNext */
String Inifile = "EDI.INI" ;

int  TargetLine   = 1;

char FilePrompt[] = "FileName:";

#define SUBMENUWIDTH 12

char *BlockMenu[] =
{
  "Read        ",
  "Write       ",
  "Delete      ",
  "Unmark      "
};

char *SearchMenu[] =
{
  "Find        ",
  "Replace     ",
  "Next        "
};

char *GotoMenu[] =
{
  "Begin block ",
  "End   blocK ",
  "Line        "
};

char *FileMenu[] =
{
  "Open        ",
  "Close       ",
  "Save        ",
  "save As     ",
  "Quit        "
};

struct MenuDesc
{
        char *MenuText;
        char **SubText;
        int Items;
};

struct MenuDesc MenuData[] =
{
        {"Block    ",BlockMenu ,sizeof(BlockMenu ) / sizeof(char *)},
        {"Search   ",SearchMenu,sizeof(SearchMenu) / sizeof(char *)},
        {"Goto     ",GotoMenu  ,sizeof(GotoMenu  ) / sizeof(char *)},
        {"File     ",FileMenu  ,sizeof(FileMenu  ) / sizeof(char *)}
};

#define MainTopics (sizeof(MenuData) / sizeof(struct MenuDesc))

int  NoSelection;                 /* Number of entrys in submenu         */

int  MenuStatus   ;               /* Status of selection                 */
int  CurrSubMenu  ;               /* Current submenu                     */
int  CurrSelection;               /* Current selection within submenu    */
                                                                                         /* on screen                           */
int  Linked       ;

String  UndoBuffer   ;
String  DirMask      = "*.*";
String  DeleteString ;
String  RenameString ;
String  BlockFileName;
String  WriteFileName;
String  FilePath     ;

char *CReserved[] =
{
   "break",
   "case",
   "char",
   "default",
   "do",
   "double",
   "else",
   "endif",
   "enum",
   "extern",
   "float",
   "for",
   "if",
   "int",
   "long",
   "register",
   "return",
   "short",
   "sizeof",
   "static",
   "struct",
   "switch",
   "typedef",
   "union",
   "void",
   "while"
};

char *FortranReserved[] =
{
        "GOTO",
        "TO",
        "DO",         /* special - don't move */
        "BACKSPACE",
        "BLOCK",
        "CALL",
        "CHARACTER",
        "CLOSE",
        "COMMON",
        "COMPLEX",
        "CONTINUE",
        "DATA",
        "DIMENSION",
        "DOUBLE",
        "ELSE",
        "ELSEIF",
        "END",
        "ENDDO",
        "ENDFILE",
        "ENTRY",
        "EQUIVALENCE",
        "EXTERNAL",
        "FORMAT",
        "FUNCTION",
        "GO",
        "IF",
        "IMPLICIT",
        "INCLUDE",
        "INTEGER",
        "INQUIRE",
        "LOGICAL",
        "NAMELIST",
        "OPEN",
        "PARAMETER",
        "PAUSE",
        "POINTER",
        "PRECISION",
        "PRINT",
        "PROGRAM",
        "READ",
        "REAL",
        "RETURN",
        "REWIND",
        "SAVE",
        "STOP",
        "SUBROUTINE",
        "THEN",
        "WRITE"
};

#define FORTRANWORDS (sizeof(FortranReserved) / sizeof(char *))
#define CWORDS (sizeof(CReserved) / sizeof(char *))
#define FORTRAN 1
#define CSYNTAX 2

char **SyntaxHighlite = FortranReserved;
int  SyntaxWords = FORTRANWORDS;
int  Syntax;
int  SyntaxKey;


void DrawFunctionKeys(void);

void UpdateCursor(void)
{
}

void DefineColors(void)
{
   int i,n;

   if (has_colors())
   {
      if (logfile)
      {
         fprintf(logfile,"Terminal has colors\n");
         fprintf(logfile,"COLORS      = %5d\n",COLORS);
         fprintf(logfile,"COLOR_PAIRS = %5d\n",COLOR_PAIRS);
      }
      n = sizeof(AttributeTab) / sizeof(struct AttributeStruct);
      if (n > COLOR_PAIRS) n = COLOR_PAIRS;

      for (i=0 ; i < n ; ++i)
      {
         init_pair(*(AttributeTab[i].Attribute),
                     AttributeTab[i].Foreground,
                     AttributeTab[i].Background);
      }
      TextColor = COLOR_PAIR(TextColor);
      MarkColor = COLOR_PAIR(MarkColor);
      MenuColor = COLOR_PAIR(MenuColor);
      MeSeColor = COLOR_PAIR(MeSeColor);
      StatColor = COLOR_PAIR(StatColor);
      FramColor = COLOR_PAIR(FramColor);
      LabeColor = COLOR_PAIR(LabeColor);
      HighColor = COLOR_PAIR(HighColor);
      CommColor = COLOR_PAIR(CommColor);
      EquaColor = COLOR_PAIR(EquaColor);
      PromColor = FramColor;
      ContColor = TextColor;
   }
   else
   {
      TextColor = A_NORMAL;
      MarkColor = A_REVERSE;
      MenuColor = A_STANDOUT;
      MeSeColor = A_BOLD;
      StatColor = TextColor;
      FramColor = A_BOLD;
      LabeColor = TextColor;
      PromColor = FramColor;
      HighColor = TextColor | A_BOLD;
      ContColor = TextColor;
      CommColor = TextColor;
      EquaColor = TextColor;
   }
}

int MapESC(void)
{
   int Code = 0;
   int Index;
   int Key;

   ESCBuffer[5] = getch();
   Index = 6;
   switch(ESCBuffer[5])
   {
      case 'O': ESCBuffer[Index++] = getch();
                break;
      case '[': while (Index < sizeof(ESCBuffer) - 2)
                {
                   ESCBuffer[Index++] = Key = getch();
                   if (Key >= 'A' && Key <= 'Z') break;
                   if (Key >= 'a' && Key <= '~') break;
                }
   }
   ESCBuffer[Index] = 0;
   for (Index = 0 ; Index < MAXCODE ; ++Index)
   if (strcmp(ESCBuffer+5,EscapeCode[Index].Sequence) == 0)
   {
         Code = EscapeCode[Index].Code;
         break;
   }
   return Code;
}

int TrueLength(struct LinDesc *p)

{
        int l;

        for (l = p->Leng ; l && p->Text[l-1] == ' ' ; --l);
        return l;
}

void ClearText(struct LinDesc *p)
{
        memset(p->Text,' ',p->Leng);
        p->Text[p->Leng] = 0;
}

struct LinDesc *NewLine(int Leng)
{
        struct LinDesc *Line;

        Line = (struct LinDesc *)calloc(1,sizeof(struct LinDesc));
        Line->Leng = Leng |= 15;
        Line->Text = (char *)malloc(Leng+1);
        ClearText(Line);
        return Line;
}

void FreeLine(struct LinDesc *Line)
{
        free(Line->Text);
        free(Line);
}

void ResizeLine(struct LinDesc *Line, int Leng)
{
        if (Line->Leng < Leng)
        {
                Leng |= 15;
                Line->Text = (char *)realloc(Line->Text,Leng+1);
                memset(Line->Text+Line->Leng,' ',Leng-Line->Leng);
                Line->Text[Leng] = 0;
                Line->Leng = Leng;
  }
}

void SpliceAfter(struct LinDesc *NL, struct LinDesc *Line)
{
        NL->Next = Line->Next;
        NL->Prev = Line;
        Line->Next = NL;
        if (NL->Next) NL->Next->Prev = NL;
}

void SpliceBefore(struct LinDesc *NL, struct LinDesc *Line)
{
  NL->Prev = Line->Prev;
  NL->Next = Line;
  Line->Prev = NL;
  if (NL->Prev) NL->Prev->Next = NL;
}

int IsEmpty(struct LinDesc *Line)
{
        int i;

        for (i=0 ; i<Line->Leng ; ++i) if (Line->Text[i] != ' ') return 0;
        return 1;
}

void WinDescInit(struct WinDesc *Win, int First, int Last)
{
   Win->Next         = NULL;
   Win->FirLine      = NewLine(0);
   Win->FirLine->No  = 1;
   Win->TopLine      = Win->FirLine;
   Win->CurLine      = Win->FirLine;
   Win->StaScreenRow = First;
   Win->TopScreenRow = First+1;
   Win->BotScreenRow = Last;
   Win->BotWindowRow = Last-First-1;
   Win->Row          = 0;
   Win->Col          = 0;
   Win->Insert       = 0;
   Win->Ofs          = 0;
   Win->LastNo       = 1;
   Win->Dirty        = 0;
   Win->FileName[0]  = 0;
}

void ResizeWin(struct WinDesc *Win, int First, int Last)
{
  Win->StaScreenRow = First;
  Win->TopScreenRow = First+1;
  Win->BotScreenRow = Last   ;
  Win->BotWindowRow = Last-First-1;
  Win->Row = 0;
  Win->Col = 0;
  Win->Ofs = 0;
}

LONG AdjustNo(struct LinDesc *p)
{
        LONG i = p->No;
        while (p)
        {
                p->No = i++;
                p = p->Next;
        }
        return i-1;
} /* AdjustNo */

void Realign(void)
{
        struct WinDesc *w;
        struct LinDesc *q;

        for (w = &PriWin ; w ; w = w->Next)
        {
                w->Row = 0;
                for (q = w->TopLine ; q != w->CurLine ; q = q->Next)
                        if (w->Row < w->BotWindowRow) w->Row++;
                        else w->TopLine = w->TopLine->Next;
        }
}

void DeleteLine(struct LinDesc *p)
/*      (1) The line deleted may be some window"s TopLine.
        (2) The line deleted may be some window"s CurLine.
        (3) (Less horrible); the line will shorten a window"s visible span.
        (4) The line deleted may be pointed to by Block1.
        (5) The line deleted may be pointed to by Block2. */
{
        struct WinDesc *w;
        struct LinDesc *t;
        struct LinDesc *Next = p->Next;
        struct LinDesc *Prev = p->Prev;

        PriWin.Dirty = 1;
        PriWin.Col   = 0;
        PriWin.Ofs   = 0;
        strcpy(UndoBuffer,p->Text);
        if (p == Block1 && Block2 == NULL) Block1 = NULL;
        else if (p == Block1) Block1 = Next;
        else if (p == Block2) Block2 = Prev;

        if (!Prev && !Next)
        {
                ClearText(p);
                return;
        }

        if (Prev) Prev->Next = Next;
        if (Next) Next->Prev = Prev;

        for (w = &PriWin ; w ; w = w->Next)
        {
                if (!Prev)
                {
                        w->FirLine = Next;
                        w->FirLine->No = 1;
                }
                if (w->TopLine == p)
                {
                        if (Next)
                        {
                                w->TopLine = Next;
                                if (w->CurLine == p) w->CurLine = Next;
                                else w->Row--;
                        }
                        else
                        {
                                w->TopLine = Prev;
                                if (w->CurLine == p) w->CurLine = Prev;
                        }
                }
                else if (w->CurLine == p)
                {
                        if (Next) w->CurLine = Next;
                        else
                        {
                                w->CurLine = Prev;
                                w->Row--;
                        }
                }
                else
                {          /* Check for line in the middle */
                        for (t = w->TopLine; t != w->CurLine ; t = t->Next)
                        {
                                if (t == p)
                                {         /* Try to advance CurLine */
                                        if (w->CurLine->Next) w->CurLine = w->CurLine->Next;
                                        else w->Row--;
                                        break;
                                }
                        }
                }
                w->LastNo = AdjustNo(w->FirLine);
        }
        FreeLine(p);
        Realign();
} /* DeleteLine */

void UnmarkBlock(void)
{
   while (Block1 && Block1->Flag & MARKED)
   {
      Block1->Flag &= ~MARKED;
      Block1 = Block1->Next;
   }
   Block1 = Block2 = NULL;
}

void FortranLine(struct LinDesc *p, int xo)
{
   int cl,i,j,k,l,n,Blank,NoQuote,NoApost;
   int c[256];
   char *s;

   s = p->Text;
   n = TrueLength(p);
   cl = TextColor;
   if (s[0] == 'C' || s[0] == 'c' || s[0] == '!') cl = CommColor;
   Blank = cl + ' ';
   for (i=0 ; i<n ; ++i) c[i] = cl;
   if (cl == TextColor)
   {
      for (i= 0 ; i<5 ; ++i) c[i] = LabeColor;
      for (i=72 ; i<n ; ++i) c[i] = LabeColor;
      c[5] = ContColor;
      NoQuote = NoApost = 1;
      for (i=6 ; i<n && i<72 ; ++i)
      {
         if (NoQuote && NoApost && s[i] == '!')
         {
            while (i < n) c[i++] = CommColor;
            break;
         }
         if (s[i] == '\"') NoQuote = !NoQuote;
         if (s[i] == '\'') NoApost = !NoApost;
         if (NoQuote && NoApost && s[i] == '=') c[i] = EquaColor;
         else if (NoQuote && NoApost && (i == 0 || !isalpha(s[i-1])))
         for (j=0 ; j<SyntaxWords ; ++j)
         {
            l = strlen(SyntaxHighlite[j]);
            if (i <= n-l && strncasecmp(s+i,SyntaxHighlite[j],l) == 0 &&
               (i == n-l || !isalpha(s[i+l])))
            {
               for (k=0 ; k<l ; ++k) c[i++] = HighColor;
               if (j < 3) /* DO & GOTO */
               while (i < n && (s[i] == ' ' || isdigit(s[i]))) c[i++] = LabeColor;
               break;
            }
         }
      }
   }
   n -= xo;
   if (n < 0) n = 0;
   if (n >= COLS) n = COLS-1;
   for (i = 0 ; i < n    ; ++i)
   {
      attron(c[i+xo]);
      if (s[i+xo] == 9) addch(ACS_BULLET);
      else              addch(s[i+xo]);
      attroff(c[i+xo]);
   }
   clrtoeol();
}

void Fortran90Line(struct LinDesc *p, int xo)
{
   int cl,i,j,k,l,n,Blank,NoQuote,NoApost;
   int c[256];
   char *s;

   s = p->Text;
   n = TrueLength(p);
   cl = TextColor;
   if (s[0] == 'C' || s[0] == 'c' || s[0] == '!') cl = CommColor;
   Blank = cl + ' ';
   for (i=0 ; i<n ; ++i) c[i] = cl;
   if (cl == TextColor)
   {
      for (i= 0 ; i<5 ; ++i) c[i] = LabeColor;
      for (i=72 ; i<n ; ++i) c[i] = LabeColor;
      c[5] = ContColor;
      NoQuote = NoApost = 1;
      for (i=6 ; i<n && i<72 ; ++i)
      {
         if (s[i] == '\"') NoQuote = !NoQuote;
         if (s[i] == '\'') NoApost = !NoApost;
         if (NoQuote && NoApost && s[i] == '=') c[i] = EquaColor;
         else if (NoQuote && NoApost && (i == 0 || !isalpha(s[i-1])))
         for (j=0 ; j<SyntaxWords ; ++j)
         {
            l = strlen(SyntaxHighlite[j]);
            if (i <= n-l && strncasecmp(s+i,SyntaxHighlite[j],l) == 0 &&
               (i == n-l || !isalpha(s[i+l])))
            {
               for (k=0 ; k<l ; ++k) c[i++] = HighColor;
               if (j < 3) /* DO & GOTO */
               while (i < n && (s[i] == ' ' || isdigit(s[i]))) c[i++] = LabeColor;
               break;
            }
         }
      }
   }
   n -= xo;
   if (n < 0) n = 0;
   if (n >= COLS) n = COLS-1;
   for (i = 0 ; i < n    ; ++i)
   {
      attron(c[i+xo]);
      if (s[i+xo] == 9) addch(ACS_BULLET);
      else              addch(s[i+xo]);
      attroff(c[i+xo]);
   }
   clrtoeol();
}

void CLine(struct LinDesc *p, int xo)
{
   int cl,i,j,k,l,n,Blank,NoQuote,NoApost;
   int c[256];
   char *s;

   s = p->Text;
   n = TrueLength(p);
   cl = TextColor;
   Blank = cl + ' ';
   for (i=0 ; i<n ; ++i) c[i] = cl;
   NoQuote = NoApost = 1;
   for (i=0 ; i<n  ; ++i)
   {
      if (s[i] == '\"') NoQuote = !NoQuote;
      if (s[i] == '\'') NoApost = !NoApost;
      if (NoQuote && NoApost && s[i] == '=') c[i] = EquaColor;
      else if (NoQuote && NoApost && (i == 0 || !isalpha(s[i-1])))
      for (j=0 ; j<SyntaxWords ; ++j)
      {
         l = strlen(SyntaxHighlite[j]);
         if (i <= n-l && strncmp(s+i,SyntaxHighlite[j],l) == 0 &&
            (i == n-l || !isalpha(s[i+l])))
         {
            for (k=0 ; k<l ; ++k) c[i++] = HighColor;
            break;
         }
      }
   }
   n -= xo;
   if (n < 0) n = 0;
   if (n >= COLS) n = COLS-1;
   for (i = 0 ; i < n    ; ++i)
   {
      attron(c[i+xo]);
      if (s[i+xo] == 9) addch(ACS_BULLET);
      else              addch(s[i+xo]);
      attroff(c[i+xo]);
   }
   clrtoeol();
}

void EditWrline(struct LinDesc *p, int xo)
{
   int i,c,n;
   char *s;

   attroff(AllColor|HighColor);
   if (Syntax == FORTRAN && !(p->Flag & MARKED)) FortranLine(p,xo);
   else if (Syntax == CSYNTAX && !(p->Flag & MARKED)) CLine(p,xo);
   else
   {
      if (p->Flag & MARKED) c = MarkColor;
      else                  c = TextColor;
      attron(c);
      s = p->Text;
      n = strlen(s) - xo;
      if (n >= COLS) n = COLS-1;
      s += xo;
      for (i=0 ; i < n ; ++i)
      {
         if (s[i] == 9) addch(ACS_BULLET);
         else           addch(s[i]);
      }
      clrtoeol();
   }
}

void ResizeTerminal(void)
{
        int MidRow;

        PriWin.CurLine = PriWin.TopLine;

        if (PriWin.Next)
        {
                MidRow = LINES / 2 - 1;
                if (PriWin.StaScreenRow == 1)
                {
                        ResizeWin(&PriWin,0,MidRow-1);
                        ResizeWin(&SecWin,MidRow,LINES-FuncLines-1);
                }
                else
                {
                        ResizeWin(&SecWin,0,MidRow-1);
                        ResizeWin(&PriWin,MidRow,LINES-FuncLines-1);
                }
        }
        else ResizeWin(&PriWin,0,LINES-FuncLines-1);
        DrawFunctionKeys();
        Realign();
        UpdateCursor();
}

void EditUpdPhyscr(void)
{
   struct WinDesc *w;
   struct LinDesc *p;
   int Row;

   for (w = &PriWin ; w ; w = w->Next)
   {
        p = w->TopLine;
        for (Row = w->TopScreenRow ; Row <= w->BotScreenRow ; ++Row)
        if (p)
        {
                move(Row,0);
                EditWrline(p,w->Ofs);
                p = p->Next;
        }
        else
        {
           move(Row,0);
           EditWrline(&DummyLine,0);
        }
   }
   refresh();
} /*EditUpdPhyscr*/

void StatusLine(void)
{
   struct WinDesc *w;
   String st;
   int i,l;

   if (CurrentCols != COLS || CurrentLines != LINES)
   {
      ResizeTerminal();
      CurrentCols = COLS;
      CurrentLines = LINES;
      EditUpdPhyscr();
   }

   attroff(AllColor);
   for (w = &PriWin ; w ; w = w->Next)
   {
      l = 0;
      move(w->StaScreenRow,0);
      sprintf(st,"%7d",w->CurLine->No);
      attron(StatColor|A_UNDERLINE);
      addstr(st);
      l += strlen(st);
      sprintf(st,"%4d",w->Col+1);
      addstr(st);
      l += strlen(st);
      addch(' ');
      attron(A_BOLD);
      addstr(w->FileName);
      attroff(A_BOLD);
      addch(' ');
      l += strlen(w->FileName) + 2;
      sprintf(st," Size %d",w->LastNo);
      addstr(st);
      l += strlen(st);
      if (w->Insert)
      {
         addstr("  Insert");
         l += strlen("  Insert");
      }
      if (Debug)
      {
         sprintf(st,"   KeyCode: %d %dx%d %s",
                 KeyCode,COLS,LINES,ESCBuffer);
         addstr(st);
         l += strlen(st);
      }
      for (i=l ; i < COLS ; ++i) addch(' ');
      attroff(StatColor|A_UNDERLINE);
   }
}  /* void StatusLine */

void AdjustOfs(void)
{
  if (PriWin.Col <  PriWin.Ofs) PriWin.Ofs = PriWin.Col;
  if (PriWin.Col >= PriWin.Ofs + COLS)
                PriWin.Ofs  = PriWin.Col - COLS + 1;
}

void EmitChar(int ch)
{
   int Col;
   struct LinDesc *p;

   Col = PriWin.Col;
   if (Col > LINEMAX-2) return;
   p = PriWin.CurLine;
   PriWin.Dirty = 1;
   if (Col >= p->Leng) ResizeLine(p,Col+1);
   if (PriWin.Insert)
   {
      if (p->Text[p->Leng-1] != ' ') ResizeLine(p,p->Leng+1);
      memmove(p->Text+Col+1,p->Text+Col,p->Leng-Col-1);
   }
   p->Text[PriWin.Col++] = ch;
   AdjustOfs();
   move(PriWin.Row+PriWin.TopScreenRow,0);
   EditWrline(p,PriWin.Ofs);
   UpdateCursor();
} /* EmitChar */

void EditInsertBlank(void)
{
        struct LinDesc *p;
        int Col,Leng;

        Col = PriWin.Col;
        p = PriWin.CurLine;
        Leng = p->Leng;
        if (Leng > LINEMAX-1) return;
        if (p->Text[Leng-1] != ' ') ResizeLine(p,Leng+1);
        memmove(p->Text+Col+1,p->Text+Col,p->Leng-Col-1);
        p->Text[Col] = ' ';
}  /*EditInsertBlank*/

void Message(char *mess)
{
   WINDOW *sub;

   sub = subwin(stdscr,3,strlen(mess)+3,(LINES-3)/2,(COLS-strlen(mess)-3)/2);
   wattroff(sub,AllColor);
   wattron(sub,FramColor);
   box(sub,0,0);
   wmove(sub,1,1);
   wattroff(sub,FramColor);
   wattron(sub,MenuColor);
   waddstr(sub,mess);
   wattroff(sub,MenuColor);
   wrefresh(sub);
   wgetch(sub);
   delwin(sub);
   flushinp();
} /* Message */

int BoolPrompt(char *prompt)
{
   int ch,l;
   WINDOW *sub;

   l = strlen(prompt);
   sub = subwin(stdscr,3,l+3,LINES/2-2,(COLS-l-3)/2);
   wattron(sub,FramColor);
   box(sub,0,0);
   attron(PromColor);
   wmove(sub,1,1);
   waddstr(sub,prompt);
   attroff(PromColor);
   do ch = toupper(wgetch(sub));
   while (ch != 'Y' && ch != 'J' && ch != 'N' && ch != 27);
   if (ch == 'J') ch = 'Y';
   delwin(sub);
   flushinp();
   return ch;
}

void EditLeftWord(void)
{
   if (PriWin.Col) PriWin.Col--;
   while (PriWin.Col &&
      (isalpha(PriWin.CurLine->Text[PriWin.Col-1]) ||
      !isalpha(PriWin.CurLine->Text[PriWin.Col  ]))) PriWin.Col--;
   AdjustOfs();
} /*EditLeftWord*/

void EditRightWord(void)
{
   if (PriWin.Col < PriWin.CurLine->Leng-1) PriWin.Col++;
   while (PriWin.Col < PriWin.CurLine->Leng-1 &&
      (isalpha(PriWin.CurLine->Text[PriWin.Col-1]) ||
      !isalpha(PriWin.CurLine->Text[PriWin.Col  ]))) PriWin.Col++;
   AdjustOfs();
} /*EditRightWod*/

void EditLeftChar(void)
/* This routine moves the cursor left one character. */
{
  if (PriWin.Col) PriWin.Col--;
  AdjustOfs();
} /*EditLeftChar*/

void EditRightChar(void)
{ /*EditRightChar*/
  if (PriWin.Col < LINEMAX-2) PriWin.Col++;
  AdjustOfs();
} /*EditRightChar*/

void EditScrollUp(void)
{
        if (PriWin.TopLine->Prev)
        {
                PriWin.TopLine = PriWin.TopLine->Prev;
                if (PriWin.Row == PriWin.BotWindowRow)
                         PriWin.CurLine = PriWin.CurLine->Prev;
                else PriWin.Row++;
        }
} /*EditScrollUp*/

void EditUpLine(void)
{
        if (PriWin.CurLine->Prev)
        {
                PriWin.CurLine = PriWin.CurLine->Prev;
                if (PriWin.Row == 0) PriWin.TopLine = PriWin.TopLine->Prev;
                else PriWin.Row--;
        }
} /*EditUpLine*/

void EditScrollDown(void)
{
        if (PriWin.TopLine->Next)
        {
                PriWin.TopLine = PriWin.TopLine->Next;
                if (PriWin.Row == 0) PriWin.CurLine = PriWin.CurLine->Next;
                else PriWin.Row--;
        }
} /*EditScrollDown*/

void EditDownLine(void)
{
   if (PriWin.CurLine->Next)
   {
      PriWin.CurLine = PriWin.CurLine->Next;
                if (PriWin.Row == PriWin.BotWindowRow) PriWin.TopLine = PriWin.TopLine->Next;
      else PriWin.Row++;
   }
} /*EditDownLine*/

void EditUpPage(void)
/*  This routine scrolls up the current window n-1 lines, where n is
        the number of displayable lines in the window.
*/
{ /*EditUpPage*/
        int i;

        if (PriWin.TopLine->Prev == NULL)
                {
                  PriWin.Row  = 0;
                  PriWin.CurLine = PriWin.TopLine;
                  return;
                }
                for (i=0 ; i < PriWin.BotWindowRow && PriWin.TopLine->Prev ; ++i)
                  {
                         PriWin.TopLine = PriWin.TopLine->Prev;
                         PriWin.CurLine = PriWin.CurLine->Prev;
                  }
} /*EditUpPage*/

void EditDownPage(void)
/*  This routine scrolls down the current window n-1 lines, where
        n-1 is the number of displayable lines in the window.
*/
{ /*EditDownPage*/
  int i;

                for (i=0 ; i < PriWin.BotWindowRow && PriWin.TopLine->Next; ++i)
                  {
                         PriWin.TopLine = PriWin.TopLine->Next;
                         if ( PriWin.CurLine->Next == NULL) PriWin.Row--;
                         else PriWin.CurLine = PriWin.CurLine->Next;
                  }
} /*EditDownPage*/

void GotoXY(int X, int Y)
{
   PriWin.Col = X;
   if (Y < 0) Y = 0;
   if (Y > PriWin.BotWindowRow) Y = PriWin.BotWindowRow;

   while (PriWin.Row > Y) EditUpLine();
   while (PriWin.Row < Y) EditDownLine();
}

void DeleteRightChar(void)
{
  char *cp;

  if (PriWin.Col < PriWin.CurLine->Leng )
  {
          PriWin.Dirty = 1;
          cp = PriWin.CurLine->Text + PriWin.Col;
          memcpy(cp,cp+1, PriWin.CurLine->Leng - PriWin.Col - 1);
          PriWin.CurLine->Text[PriWin.CurLine->Leng-1] = ' ';
  }
} /* DeleteRightChar */

void DeleteTextRight(void)
{
        int i;

        if (PriWin.Col <= PriWin.CurLine->Leng )
                for (i = PriWin.Col ; i < PriWin.CurLine->Leng ; ++i)
                        PriWin.CurLine->Text[i] = ' ';
} /*DeleteTextRight*/

void EditTab(void)
{
   int i;

   i = PriWin.Col;
   if (PriWin.Insert)
   {
      EmitChar(9);
      while (PriWin.Col < LINEMAX-1 && !TabStop[PriWin.Col]) EmitChar(9);
   }
   else
   {
      if    (PriWin.Col < LINEMAX-1)                         PriWin.Col++;
      while (PriWin.Col < LINEMAX-1 && !TabStop[PriWin.Col]) PriWin.Col++;
   }
   AdjustOfs();
} /*EditTab*/

void SplitLine(void)
{
        struct LinDesc *p;
        int l,c;

        PriWin.Dirty = 1;
        c = TrueLength(PriWin.CurLine);
        l = c - PriWin.Col;
        if (l < 0)  l = 0;
        p = NewLine(l);
        SpliceAfter(p,PriWin.CurLine);
        if (l)
        {
                memcpy(p->Text,PriWin.CurLine->Text+PriWin.Col,l);
                memset(PriWin.CurLine->Text+PriWin.Col,' ',l);
        }
        Realign();
        PriWin.LastNo = AdjustNo(PriWin.CurLine);
} /*SplitLine*/

void EdifInsertLine(void)
{
        struct LinDesc *p;

        PriWin.Dirty = 1;
        PriWin.Col   = 0;
        PriWin.Ofs   = 0;
        p = NewLine(0);
        p->No   = PriWin.CurLine->No  ;
        SpliceBefore(p,PriWin.CurLine);
        p->Flag = PriWin.CurLine->Flag;
        if (PriWin.FirLine == PriWin.CurLine) PriWin.FirLine = p;
        if (PriWin.TopLine == PriWin.CurLine) PriWin.TopLine = p;
   if (Block1         == PriWin.CurLine) Block1         = p;
        PriWin.CurLine = p;
        UpdateCursor();
        Realign();
        PriWin.LastNo = AdjustNo(p);
        if (Linked) SecWin.LastNo = PriWin.LastNo;
} /*EdifInsertLine*/

void EditEndLine(void)
{
        PriWin.Col = PriWin.CurLine->Leng;
        while (PriWin.Col && PriWin.CurLine->Text[PriWin.Col-1] == ' ')
                PriWin.Col--;
        AdjustOfs();
}

int Joinline(void)
{
        struct LinDesc *p;
        struct LinDesc *q;
        int l1,l2,nl;

        p  = PriWin.CurLine;
        q  = p->Next;
        l1 = TrueLength(p);
        l2 = TrueLength(q);
        nl = l1 + l2;
        if (nl < 240)
        {
                ResizeLine(p,nl);
                memcpy(p->Text+l1,q->Text,l2);
                DeleteLine(q);
        }
        return l1;
}

void DeleteLeftChar(void)
{
        int Col = PriWin.Col;
        struct LinDesc *Line = PriWin.CurLine;
        char *Text = Line->Text;
        int   Leng = Line->Leng;

        if (Col == 0)
        {
                if (Line->Prev)
                {
                        PriWin.CurLine = Line->Prev;
                        if (PriWin.Row == 0) PriWin.TopLine = PriWin.TopLine->Prev;
                        else PriWin.Row--;
                        Col = Joinline();
                }
        }
        else if (Col < Leng)
        {
                 memmove(Text+Col-1,Text+Col,Leng-Col);
                 Text[Leng-1] = ' ';
                 Col--;
        }
        else Col--;
   PriWin.Col = Col;
        PriWin.Dirty = 1;
        AdjustOfs();
} /* DeleteLeftChar */

void EdifNewLine(void)
{
        struct LinDesc *p;
        int i,HalfPagesize;

        if (PriWin.Insert && ReturnSplit) SplitLine();
        if (PriWin.CurLine->Next == NULL)
        {
                PriWin.Dirty = 1;
                p = NewLine(0);
                PriWin.CurLine->Next = p;
                p->Prev = PriWin.CurLine;
                p->No   = ++PriWin.LastNo;
        }
        PriWin.CurLine = PriWin.CurLine->Next;
        if (PriWin.Row == PriWin.BotWindowRow)
        {
                HalfPagesize = PriWin.BotWindowRow / 2;
                for (i=1 ; i < HalfPagesize && PriWin.TopLine->Next ; ++i)
                        PriWin.TopLine = PriWin.TopLine->Next;
        }
        else PriWin.Row++;
        PriWin.Ofs = 0;
        PriWin.Col = 0;
        Realign();
} /*EdifNewLine*/

void EditWindowTopFile(void)
{
        PriWin.CurLine = PriWin.TopLine = PriWin.FirLine;
        PriWin.Row   = 0;
        PriWin.Col   = 0;
        PriWin.Ofs   = 0;
}

void EditAlignLast(void)
{
        PriWin.Row = 0;
        PriWin.TopLine = PriWin.CurLine;
        while (PriWin.Row < PriWin.BotWindowRow && PriWin.TopLine->Prev)
        {
                PriWin.TopLine = PriWin.TopLine->Prev;
                PriWin.Row++;
        }
} /*EditAlignLast*/

void EditWindowBottomFile(void)
{
        struct LinDesc *p;

        p = PriWin.TopLine;
        while (p->Next) p = p->Next;
        PriWin.CurLine = p;
        PriWin.Col = 0;
        PriWin.Ofs = 0;
        EditAlignLast();
        EditEndLine();
} /*EditWindowBottomFile*/

void MarkBlock(void)
{
        struct LinDesc *p;

        if (Block1 && Block2) UnmarkBlock();
        if (Block1 == NULL)
        {
                Block1 = PriWin.CurLine;
                Block1->Flag |= MARKED;
        }
        else
        {
      if (Block1 == PriWin.CurLine) return;
                Block2 = PriWin.CurLine;

                for (p = PriWin.FirLine ; p != Block1 && p != Block2 ; p = p->Next);
                if (p == Block2)
                {
                        Block2 = Block1;
                        Block1 = p;
                }

                for (p = Block1 ; p != Block2 ; p = p->Next) p->Flag |= MARKED;
      Block2->Flag |= MARKED;
        }
        BlockW = 0;
} /*MarkBlock*/

void ToggleWindow(void)
{
        struct WinDesc TmpWin;
        if (PriWin.Next)
        {
                TmpWin = PriWin;
                PriWin = SecWin;
                SecWin = TmpWin;
                PriWin.Next = &SecWin;
                SecWin.Next = NULL;
                BlockW = !BlockW;
                UpdateCursor();
        }
}  /* void ToggleWindow */

void EditBottomBlock(void)
{
        if (Block1 == NULL) return;
        if (BlockW) ToggleWindow();
        if (Block2) PriWin.CurLine = Block2;
        else        PriWin.CurLine = Block1;
        EditAlignLast();
} /*EditBottomBlock*/

void EditTopBlock(void)
{
        if (Block1 == NULL) return;
        if (BlockW) ToggleWindow();
        PriWin.TopLine = PriWin.CurLine = Block1;
        PriWin.Row = PriWin.Col = PriWin.Ofs = 0;
        Realign();
} /*EditTopBlock*/

void EditBlockMove(void)
{
        struct LinDesc *BlockE;

        if (Block1 == NULL || PriWin.CurLine->Flag & MARKED) return;
        if (Block2) BlockE = Block2;
        else        BlockE = Block1;
        if (Block1->Prev) Block1->Prev->Next = BlockE->Next;
        if (BlockE->Next) BlockE->Next->Prev = Block1->Prev;
        Block1->Prev = PriWin.CurLine->Prev;
        if (Block1->Prev) Block1->Prev->Next = Block1;
        BlockE->Next = PriWin.CurLine;
        PriWin.CurLine->Prev = BlockE;
        Realign();
        PriWin.Dirty = 1;
} /*EditBlockMove*/

void CopyBlock(void)
{
        struct LinDesc *p;
        struct LinDesc *q;
        struct LinDesc *b;

        if (Block1 == NULL || (PriWin.CurLine->Flag & MARKED && Block2)) return;
        p = PriWin.CurLine->Prev;
        b = Block1;
        while (b)
        {
                q = NewLine(b->Leng);
                memcpy(q->Text,b->Text,b->Leng);
                if (p) p->Next = q;
                else
                {
                        PriWin.FirLine     = q;
                        PriWin.FirLine->No = 1;
                }
                q->Prev = p;
                q->Next = PriWin.CurLine;
                PriWin.CurLine->Prev = q;
                p = q;
                if (b == Block2 || Block2 == NULL) break;
                b = b->Next;
        }
        Realign();
        PriWin.LastNo = AdjustNo(PriWin.FirLine);
        PriWin.Dirty = 1;
} /*CopyBlock*/

void EditBlockDelete(void)
{
        struct LinDesc *p;
        struct LinDesc *q;

        if (BlockW) ToggleWindow();
        p = Block1;
        while (p && p->Flag)
        {
                q = p;
                p = p->Next;
                DeleteLine(q);
        }
        Block1 = NULL;
        Block2 = NULL;
        PriWin.Col = 0;
        PriWin.Ofs = 0;
        Realign();
        PriWin.LastNo = AdjustNo(PriWin.FirLine);
        PriWin.Dirty = 1;
} /*EditBlockDelete*/

void EdifFileWrite(char *Fn, struct LinDesc *Start, struct LinDesc *Finish)
{
   int i,n;
   FILE *Outfile;
   struct LinDesc *p;

   Outfile = fopen(Fn,"wb");
   for (p = Start; p != Finish ; p = p->Next)
   {
      n = TrueLength(p);
      for (i = 0 ; i < n ; ++i)
      {
          putc(p->Text[i],Outfile);
          if (p->Text[i] == 9) while (p->Text[i+1] == 9 && !TabStop[i+1]) ++i;
      }
      putc(10,Outfile);
   }
   fclose(Outfile);
} /* EdifFileWrite */

int ReadUnixLn(FILE *Infile, char *Buffer)
{
  int i,j,l;
  char Buf[256];

  if (!fgets(Buf,255,Infile)) return 0;
  l = strlen(Buf);
  if (l && Buf[l-1] == 10) Buf[--l] = 0; // remove linefeed
  if (l && Buf[l-1] == 13) Buf[--l] = 0; // remove carriage return
  for (i=0 , j=0 ; i <= l , j < 256 ; ++i) // expand tabs
  {
     if (Buf[i] == 9)
     {
        Buffer[j++] = 9;
        while (!TabStop[j]) Buffer[j++] = 9;
     }
     else Buffer[j++] = Buf[i];
  }
  while (j && Buffer[j-1] == ' ') Buffer[--j] = 0; // remove trailing blanks
  return 1;
}

void EdifFileRead(struct WinDesc *Win, char *Fn)
{
   String Buffer;
   FILE *Infile;
   struct LinDesc *p;
   struct LinDesc *q;

   Infile = fopen(Fn,"r");
   if (Infile == NULL) return;
   ReadUnixLn(Infile,Buffer);
   if (Win->CurLine->Next == NULL && IsEmpty(Win->CurLine))
   {
      ResizeLine(Win->CurLine,strlen(Buffer));
      p = Win->CurLine;
   }
   else
   {
      p = NewLine(strlen(Buffer));
      SpliceAfter(p,Win->CurLine);
   }
   memcpy(p->Text,Buffer,strlen(Buffer));
   while (ReadUnixLn(Infile,Buffer))
   {
      q = NewLine(strlen(Buffer));
      memcpy(q->Text,Buffer,strlen(Buffer));
      SpliceAfter(q,p);
      p = q;
   }
   fclose(Infile);
   Realign();
   Win->LastNo = AdjustNo(Win->FirLine);
} /*EdifFileRead*/

void EditWindowUnlink(void)
{
         PriWin.FileName[0] = 0;
         PriWin.Insert  = 0;
         PriWin.Row     = 0;
         PriWin.Col     = 0;
         PriWin.Ofs     = 0;
         PriWin.FirLine = NewLine(0);
         PriWin.FirLine->No = 1;
         PriWin.TopLine = PriWin.FirLine;
         PriWin.CurLine = PriWin.FirLine;
         PriWin.Ofs    = 0;
         PriWin.LastNo  = 1;
         PriWin.Dirty   = 0;
         Linked = 0;
}  /*EditWindowUnlink*/

void EditCenterLine(void)
/*  This routine centers all text on the current line as a single entity. */
{
        int r,l;
        String tmp;

        PriWin.Dirty = 1;
        for (l=0;l < PriWin.CurLine->Leng && PriWin.CurLine->Text[l]==' ';++l);
        for (r=PriWin.CurLine->Leng-1;r>0 && PriWin.CurLine->Text[r]==' ';--r);
        if (r < l) return;
        memcpy(tmp,PriWin.CurLine->Text+l,r-l+1);
        ResizeLine(PriWin.CurLine,COLS);
        memset(PriWin.CurLine->Text,' ',COLS);
        memcpy(PriWin.CurLine->Text+(COLS-r+l+1)/2,tmp,r-l+1);
} /*EditCenterLine*/

void strupr(char *s)
{
   while (*s)
   {
      if (*s >= 'a' && *s <= 'z') *s -= 32;
      ++s;
   }
}

int ScanText(int c)
{
        struct LinDesc *q;
        String s;
        int i,n;
        char *cp;

        n = strlen(SearchStr);
        q = PriWin.CurLine;
        while (q)
        {
                if (c < q->Leng)
                {
                        strcpy(s,q->Text+c);
                        if (IgnCase) strupr(s);
                        cp = strstr(s,SearchStr);
                        if (cp)
                        {
                                i = (int)(cp - s);
                                if (!WholeWord ||
                                ((i==0         || !isalnum(s[i-1])) &&
                                 (i==q->Leng-n || !isalnum(s[i+n]))))
                                {
                                        PriWin.CurLine = q;
                                        return c+i+n;
                                }
                        }
                }
                c = 0;
                if (SearchBack) q = q->Prev;
                else            q = q->Next;
        }
        return -1;
} /* ScanText */

int DoReplace(void)
{
   int sl,rl,dl,c,Ch;
   struct LinDesc *p;

   sl = strlen(SearchStr);
   rl = strlen(ReplaceStr);
   dl = rl - sl;
   p  = PriWin.CurLine;
        c  = PriWin.Col;

   EditUpdPhyscr();
   UpdateCursor();
   if (Ask)
   {
      Ch = BoolPrompt("Replace (Y/N):");
      if (Ch != 'Y') return Ch;
        }
   if (dl == 0) memcpy(p->Text+c-sl,ReplaceStr,rl);
   else if (dl < 0)
   {
      memmove(p->Text+c+dl,p->Text+c,p->Leng-c);
      memcpy(p->Text+c-sl,ReplaceStr,rl);
      memset(p->Text+p->Leng+dl,' ',-dl);
   }
   else
   {
      if (dl > p->Leng - TrueLength(p)) ResizeLine(p,p->Leng+dl);
      memmove(p->Text+c+dl,p->Text+c,p->Leng-c-dl);
      memcpy(p->Text+c-sl,ReplaceStr,rl);
   }
   PriWin.Dirty = 1;
   PriWin.Col += dl;
   return 0;
}

void Find(int prompt, int Replace)
{
        int i,c;
        char *cp;

        WINDOW *sub;

   attroff(AllColor);
        if (prompt)
        {
                sub = subwin(stdscr,5,COLS-2,LINES/2-2,1);
                werase(sub);
           wattron(sub,FramColor);
                box(sub,0,0);
                attron(PromColor);
                mvwaddstr(sub,1,1,"Search :");
                if (Replace) mvwaddstr(sub,2,1,"Replace:");
                mvwaddstr(sub,3,1,"Options:");
                attroff(PromColor);
                echo();
                wmove(sub,1,9);
                wgetstr(sub,SearchStr);
                if (Replace)
                {
                        wmove(sub,2,9);
                        wgetstr(sub,ReplaceStr);
                }
                wmove(sub,3,9);
                wgetstr(sub,OptStr);
                noecho();
                delwin(sub);
                Found      = 1;
                Replast    = 0;                  /*For Ctrl-L command*/
                Global     = strchr(OptStr,'G') != NULL;
                SearchBack = strchr(OptStr,'B') != NULL;
                IgnCase    = strchr(OptStr,'U') != NULL;
                WholeWord  = strchr(OptStr,'W') != NULL;
                Ask        = strchr(OptStr,'N') == NULL;
                cp = strpbrk(OptStr,"0123456789");
                if (cp) Ntimes = atoi(cp);
                else
                {
                if (Replace && Global) Ntimes = MAXINT;
                else                   Ntimes = 1;
          }
          if (IgnCase) strupr(SearchStr);

          if (Global)
          {                        /* Go to top/bottom of file */
                  if (SearchBack) EditWindowBottomFile();
                  else            EditWindowTopFile();
          }
  }
  if (SearchStr[0] == 0) return;
  i = 0;
  while (i++ < Ntimes && Found)
  {
          c = ScanText(PriWin.Col);
          if (c >= 0)
          {
                  PriWin.TopLine = PriWin.CurLine;
                  PriWin.Col = c;
                  PriWin.Row = 0;
                  Realign();
        if (Replace)
        {
                          if (DoReplace() == 27) break;
                  }
                }
                else Found = 0;
        }
        AdjustOfs();
        Replast = Replace;
}  /* void Find */

void EditFind(void)
{
   Find(1,0);
}

void EditInsertCtrlChar(void)
{
  int ch;
  ch = wgetch(stdscr);
  if (isalpha(ch)) ch = CONTROL(ch);
  EmitChar(ch);
} /*EditInsertCtrlChar*/

void EditUndo(void)
{ /*EditUndo*/
  struct LinDesc *p;

         PriWin.Dirty = 1;
         PriWin.Col   = 0;
         PriWin.Ofs   = 0;
         p = NewLine(strlen(UndoBuffer));
         memcpy(p->Text,UndoBuffer,strlen(UndoBuffer));
         p->No = PriWin.CurLine->No;
         SpliceBefore(p,PriWin.CurLine);
         UpdateCursor();
         Realign();
         PriWin.LastNo = AdjustNo(p);
} /*EditUndo*/

void ClearSubMenu(WINDOW *sub)
{
        NoSelection = 0;
        if (sub) delwin(sub);
        EditUpdPhyscr();
}  /* void ClearSubMenu */

void ClearFile(void)
{
        struct LinDesc *p;
        struct LinDesc *q;

        Realign();
        if (!BlockW) Block1 = Block2 = NULL;
        p = PriWin.FirLine->Next;
        while (p)
        {
                q = p;
                p = p->Next;
                FreeLine(q);
        }
        PriWin.FileName[0] = 0;
        ClearText(PriWin.FirLine);
        PriWin.FirLine->Next = NULL;
        PriWin.FirLine->No   =    1;
        PriWin.TopLine = PriWin.FirLine;
        PriWin.CurLine = PriWin.FirLine;
        PriWin.Row = 0;
        PriWin.Col = 0;
        PriWin.Ofs = 0;
        PriWin.Dirty    = 0;
        PriWin.LastNo = AdjustNo(PriWin.FirLine);
   PriWin.FileName[0] = 0;
        UpdateCursor();
        EditUpdPhyscr();
} /* void ClearFile */

void ShowSyntax(void)
{
   switch (Syntax)
   {
      case FORTRAN: Ukey[SyntaxKey].Label = "FORTR";
                    SyntaxWords = FORTRANWORDS;
                    SyntaxHighlite = FortranReserved;
                    break;
      case CSYNTAX: Ukey[SyntaxKey].Label = "  C  ";
                    SyntaxWords = CWORDS;
                    SyntaxHighlite = CReserved;
                    break;
           default: Ukey[SyntaxKey].Label = "Syntx";
                    Syntax = 0;
                    break;
   }
}

void ProcessFileName(char *FileName)
{
   int l;

   l = strlen(FileName);

   if (l > 2 && !strcmp(FileName+l-2,".f"  )) Syntax = FORTRAN;
   if (l > 4 && !strcmp(FileName+l-4,".f90")) Syntax = FORTRAN;
   if (l > 2 && !strcmp(FileName+l-2,".c"  )) Syntax = CSYNTAX;
   if (l > 4 && !strcmp(FileName+l-4,".cpp")) Syntax = CSYNTAX;
   if (logfile) fprintf(logfile,"File <%s> Syntax = %d\n",FileName,Syntax);
   ShowSyntax();
} /* void ProcessFileName */

void GetFileName(char *prompt, char *Name)
{
        WINDOW *sub;

        sub = subwin(stdscr,3,COLS-2,LINES/2-2,1);
        werase(sub);
        wattron(sub,FramColor);
        box(sub,0,0);
        attron(PromColor);
        mvwaddstr(sub,1,1,prompt);
        attroff(PromColor);
        echo();
        wgetstr(sub,Name);
        noecho();
        delwin(sub);
}

void WriteFile(int BlockOnly)
{
  char *StPtr;
  FILE *fl;
  int ch;
  struct LinDesc *p1;
  struct LinDesc *p2;

  if (BlockOnly && Block1 == NULL) return;
  if (BlockOnly) StPtr = BlockFileName;
  else           StPtr = WriteFileName;
  GetFileName(FilePrompt,StPtr);
  ProcessFileName(StPtr);
  if (StPtr[0])
  {
         ch = 'Y';
         fl = fopen(StPtr,"r");
         if (fl)
         {
                fclose(fl);
                ch = BoolPrompt("File exists. Continue (Y/N)?");
         }
         if (ch == 'Y')
         {
                if (BlockOnly)
                {
                  p1 = Block1;
                  if (Block2) p2 = Block2->Next;
                  else        p2 = Block1->Next;
                }
                else
                {
                  p1 = PriWin.TopLine;
                  while (p1->Prev) p1 = p1->Prev;
                  p2 = NULL;        /* Last line has nothing after */
                }
                EdifFileWrite(StPtr, p1, p2);
                if (!BlockOnly)
                {
                         PriWin.Dirty = 0;
                         strcpy(PriWin.FileName,WriteFileName);
                }
         }
  }
} /* void WriteFile */

void SaveFile(void)
{
   int l;
   String st;

   if (!PriWin.FileName[0])
   {
      WriteFile(0);
      return;
   }
   strcpy(st,PriWin.FileName);
   l = strlen(st);
        if (l > 1 && st[l-1] == '.') strcat(st,"bak");
   else if (l > 2 && st[l-2] == '.') strcpy(st+l-1,"bak");
   else if (l > 3 && st[l-3] == '.') strcpy(st+l-2,"bak");
   else if (l > 4 && st[l-4] == '.') strcpy(st+l-3,"bak");
   else strcat(st,".bak");
   remove(st);
   rename(PriWin.FileName,st);
   EdifFileWrite(PriWin.FileName,PriWin.FirLine,NULL);
   PriWin.Dirty = 0;
}  /* void SaveFile */

int CloseFile(void)
{
        int ch;

        if (Linked)
        {
                EditWindowUnlink();
                return 1;
        }
        else
        {
                if (PriWin.Dirty)
                {
                         ch = BoolPrompt("Save changes (Y/N)? ");
                         if (ch ==  27) return 0;
                         if (ch == 'Y') SaveFile();
                }
                ClearFile();
                return 1;
        }
} /* CloseFile */

void CloseWindow(void)
{
        if (PriWin.Next)
        {
                if (!Linked)
                {
                        if (!CloseFile()) return;
                }
                if (!BlockW) Block1 = Block2 = NULL;
                Linked     = 0;
                PriWin = SecWin;
                ResizeWin(&PriWin,0,LINES-FuncLines-1);
                PriWin.Next = NULL;
                Realign();
                UpdateCursor();
        }
}  /* void CloseWindow */

void OpenWindow(void)
{
   int MidRow;
   if (PriWin.Next == NULL)
   {
        SecWin = PriWin;
        MidRow = LINES / 2 - 1;
        ResizeWin(&SecWin,0,MidRow-1);
        ResizeWin(&PriWin,MidRow,LINES-FuncLines-1);
        PriWin.Next = &SecWin;
        SecWin.Next = NULL;
        Linked = 1;
        Realign();
        UpdateCursor();
   }
}  /* void OpenWindow */

void ToggleSplit(void)
{
        if (PriWin.Next) CloseWindow();
        else             OpenWindow();
}

void ExitFile(void)
{
   int ch = 'N';
        if (PriWin.Dirty) ch = BoolPrompt("Save Changes (Y/N)?");
        if (ch == 27) return;
        if (ch == 'Y') SaveFile();
        move(LINES-1,0);
        refresh();
        endwin();
        keypad(stdscr,0);
        fputc('\n',stderr);
        exit(0);
}  /* void ExitFile */

void ExitSave(void)
{
        if (PriWin.Dirty) SaveFile();
        if (PriWin.Next && !Linked)
        {
                ToggleWindow();
                if (PriWin.Dirty) SaveFile();
        }
        move(LINES-1,0);
        refresh();
        endwin();
        keypad(stdscr,0);
        fputc('\n',stderr);
        exit(0);
}  /* void ExitSave */

void ReadBlock(void)
{
  GetFileName(FilePrompt,BlockFileName);
  if (BlockFileName[0])
  {
         EdifFileRead(&PriWin,BlockFileName);
         PriWin.Dirty = 1;
  }
}  /* void ReadBlock */

void OpenFile(struct WinDesc *Win, int prompting)
{
        int i;
        FILE *fl;
        int ch,lc;

        if (prompting) GetFileName("Open File:",Win->FileName);
        if (Win->FileName[0])
        {
                fl = fopen(Win->FileName,"rb");
                if (fl)
                {
                 ch = 0;
                 for (i=0 ; i < 255 && ch != 10 ; ++i)
                 {
                                lc = ch;
                                ch = fgetc(fl);
                 }
                 fclose(fl);
                 EdifFileRead(Win,Win->FileName);
                }
                else Message("File not found - Press any key");
        }
        Win->Dirty = 0;
}  /* void OpenFile */

void ToggleInsert(void)
{
  PriWin.Insert = !PriWin.Insert;
}

void GotoLine(void)
{
        int i;
        String Line;
        WINDOW *sub;
        struct LinDesc *p;

        sub = subwin(stdscr,3,20,LINES/2-2,(COLS-20)/2);
   wattroff(sub,AllColor);
        werase(sub);
        wattron(sub,FramColor);
        box(sub,0,0);
        attron(PromColor);
        mvwaddstr(sub,1,1,"Goto Line:");
        attroff(PromColor);
        echo();
        wgetstr(sub,Line);
        noecho();
        delwin(sub);
        TargetLine = atoi(Line);
        for (i=1, p = PriWin.FirLine; p && i < TargetLine ; ++i, p=p->Next);
        if (p)
        {
                PriWin.CurLine = PriWin.TopLine = p;
                PriWin.Row = 0;
        }
        else EditWindowBottomFile();
}  /* GotoLine */

void WriteMain(int SubMenu)
{
   int i,Att;
   move(0,0);
   attroff(AllColor);
   for (i=0 ; i < MainTopics ; ++i)
   {
      if (i != SubMenu) Att = MenuColor;
      else              Att = MeSeColor;
      attron(Att);
      addch(' ');
      attron(A_BOLD);
      addch(MenuData[i].MenuText[0]);
      attroff(A_BOLD);
      addstr(MenuData[i].MenuText+1);
      attroff(Att);
   }
   refresh();
}  /* void WriteMain */

void WriteSelection(WINDOW *sub, int Selection)
{
   int i,Att;

   wattroff(sub,AllColor);
   for (i=0 ; i < MenuData[CurrSubMenu].Items ; ++i)
   {
      if (i != Selection) Att = MenuColor;
      else                Att = MeSeColor;
      wmove(sub,i+1,1);
      wattron(sub,Att|A_BOLD);
      waddch(sub,MenuData[CurrSubMenu].SubText[i][0]);
      wattroff(sub,A_BOLD);
      waddstr(sub,MenuData[CurrSubMenu].SubText[i]+1);
      wattroff(sub,Att);
   }
   wrefresh(sub);
}  /* void WriteSelection */

void InitMainMenu(void)
{
        move(0,0);
        clrtoeol();
        WriteMain(0);
        CurrSubMenu   = 0;
        CurrSelection = 0;
        NoSelection   = 0;
}  /* void InitMainMenu */

WINDOW *InitSubMenu(int SubMenu)
{
   WINDOW *sub;

   NoSelection = MenuData[SubMenu].Items;
   sub = subwin(stdscr,NoSelection+2,SUBMENUWIDTH+2,1,10*SubMenu);
   wattroff(sub,AllColor);
   wattron(sub,MenuColor);
   box(sub,0,0);
   wattroff(sub,MenuColor);
   CurrSelection = 0;
   WriteSelection(sub,0);
   return sub;
}   /* function InitSubMenu */

void PulldownMenu(void)
{
   int ch, i;
   WINDOW *sub;

   sub = NULL;
   InitMainMenu();
   move(0,COLS-25);
   attroff(AllColor);
   attron(MenuColor);
   addstr(" Press menu key to leave ");
   attroff(MenuColor);
   refresh();
   MenuStatus = 0;
   do
   {
      if (MenuStatus == 0)
      {
         ch = wgetch(stdscr);
         if (ch == 27) ch = MapESC();
      }
      else --MenuStatus;
      switch (ch)
      {
         case KEY_LEFT : CurrSubMenu--; break;
         case KEY_RIGHT: CurrSubMenu++; break;
         case KEY_F(1) : break;
         case   13     : break;
         case   27     : break;
         default       : for (i = 0 ; i < MainTopics ; ++i)
                         if (MenuData[i].MenuText[0] == ch ||
                             MenuData[i].MenuText[0] == ch-32)
                         {
                            CurrSubMenu = i;
                            MenuStatus = 2;
                            ch = 255;
                            break;
                         }
      }
      if (CurrSubMenu < 0 ) CurrSubMenu = MainTopics-1;
      if (CurrSubMenu >= MainTopics ) CurrSubMenu = 0;
      WriteMain(CurrSubMenu);
      if (MenuStatus == 2) ch = 255;
                if (MenuData[CurrSubMenu].SubText && (ch==13 || MenuStatus==1))
                {
                        sub = InitSubMenu(CurrSubMenu);
                        do
                        {
                                ch = wgetch(stdscr);
                                if (ch == 27) ch = MapESC();
                                switch (ch)
                                {
                                        case KEY_DOWN : ++CurrSelection; break;
                                        case KEY_UP   : --CurrSelection; break;
                                        case KEY_LEFT :
                                        case KEY_RIGHT: MenuStatus = 3;
                                                                                 ClearSubMenu(sub);
                                                                                 sub = NULL;
                                                                                 break;
                                        case KEY_F(1) :
                                        case   27     : ClearSubMenu(sub);
                                                                                 sub = NULL;
                                                                                 CurrSelection = 0;
                                                                                 MenuStatus    = 0;
                                                                                 ch = 255;
                                                                                 break;
                                        case   13     : break;
                                        default       : for (i = 0 ; i < NoSelection ; ++i)
                                                          if (MenuData[CurrSubMenu].SubText[i][0] == ch
                                                          ||  MenuData[CurrSubMenu].SubText[i][0] == ch-32)
                                                          {
                                                                  ch = 13;
                                                                  CurrSelection = i;
                                                          }
                                                                  break;
                                }  /* end switch (ch) */
                                if (ch != 255 && ch != KEY_LEFT && ch != KEY_RIGHT)
                                {
               if (CurrSelection >= NoSelection) CurrSelection = 0;
               if (CurrSelection < 0 ) CurrSelection = NoSelection-1;
                                        WriteSelection(sub,CurrSelection);
            }
            if (ch == 13)
                                switch (CurrSubMenu)
            {
               case 0:switch (CurrSelection)
               {
                  case 0:ReadBlock();       break;
                                                case 1:WriteFile(1);      break;
                  case 2:EditBlockDelete(); break;
                  case 3:UnmarkBlock();     break;
               } break;
               case 1:switch (CurrSelection)
               {
                  case 0:Find(1,0);         break;
                  case 1:Find(1,1);         break;
                  case 2:Find(0,Replast);   break;
                                        } break;
               case 2:switch (CurrSelection)
               {
                                                case 0:EditTopBlock();     break;
                                                case 1:EditBottomBlock();  break;
                                                case 2:GotoLine();         break;
                                        } break;
                                        case 3:switch (CurrSelection)
                                        {
                                                case 0:if (CloseFile()) OpenFile(&PriWin,1); break;
                                                case 1:CloseFile();      break;
                                                case 2:SaveFile();       break;
                                                case 3:WriteFile(0);     break;
                                                case 4:ExitFile();       break;
                                        } break;
                                }
                        } while (ch!=255 && ch!=13 && ch!=KEY_LEFT && ch!=KEY_RIGHT);
                }
        }
        while (ch != 13 && ch != 27 && ch != KEY_F(1));
        ClearSubMenu(sub);
   flushinp();
}  /* void PullDownMenu */

int SecondChar(char *st)
{
        int ch,i,l;
        l = strlen(st);
        move(PriWin.StaScreenRow,COLS-l-1);
        standout();
        addstr(st);
        standend();
        ch = wgetch(stdscr);
        if (isalpha(ch)) ch = CONTROL(ch);
        move(PriWin.StaScreenRow,COLS-l-1);
        for (i = 0 ; i < l ; ++i) addch(' ');
        return ch;
}  /* function SecondChar */

void EditK(void)
{
        int ch;

        ch = SecondChar("<Ctrl-K>");
        switch (ch)
   {
      case CONTROL('B') : MarkBlock();        break;
      case CONTROL('C') : CopyBlock();        break;
      case CONTROL('H') : UnmarkBlock();      break;
      case CONTROL('K') : MarkBlock();        break;
      case CONTROL('R') : ReadBlock();        break;
      case CONTROL('S') : SaveFile();         break;
      case CONTROL('V') : EditBlockMove();    break;
      case CONTROL('W') : WriteFile(1);       break;
      case CONTROL('X') : ExitFile();         break;
      case CONTROL('Y') : EditBlockDelete();  break;
        }
} /*EditK*/

void EditO(void)
{
   switch (SecondChar("<Ctrl-O>"))
   {
      case CONTROL('C') : EditCenterLine();   break;
      case CONTROL('G') : ToggleWindow();     break;
      case CONTROL('N') : GotoLine();         break;
      case CONTROL('O') : OpenWindow();       break;
      case CONTROL('Y') : CloseWindow();      break;
   }
} /*EditO*/

void EditQ(void)
{
   switch(SecondChar("<Ctrl-Q>"))
   {
      case CONTROL('A') : Find(1,1);              break;
      case CONTROL('B') : EditTopBlock();         break;
                case CONTROL('C') : EditWindowBottomFile(); break;
      case CONTROL('D') : EditEndLine();          break;
      case CONTROL('F') : Find(1,0);              break;
      case CONTROL('K') : EditBottomBlock();      break;
      case CONTROL('R') : EditWindowTopFile();    break;
      case CONTROL('S') : PriWin.Col = PriWin.Ofs = 0; break;
      case CONTROL('Y') : DeleteTextRight();      break;
   }
} /*EditQ*/

char *HelpText[] =
{
        "^D Cursor >       ^KB Mark Block       ^QS Goto Start of Line    ",
        "^S Cursor <       ^KK Mark Block       ^QD Goto End of Line      ",
        "^E Cursor ^       ^KH Hide Block       ^QY Delete to End of Line ",
        "^X Cursor v       ^KC Copy Block                                 ",
        "^R Page   ^       ^KV Move Block        Search & Replace Options ",
        "^C Page   v       ^KR Read Block        ------------------------ ",
        "^W Scroll ^       ^KW Write Block       N : No Ask on Replace    ",
        "^Z Scroll v       ^KY Delete Block      G : Global               ",
        "^A Word   <       ^KS Save File         B : Backwards from cursor",
        "^F Word   >       ^KX Exit File         U : Ignore Case          ",
        "^G Del Char >     ^OC Center Line       W : Whole words Only     ",
        "^H Del Char <     ^OG Toggle Window     number : Repeat Counter  ",
        "^N Insert Line    ^ON Goto Line         e.g. : UNG20             ",
        "^Y Delete Line    ^OO Open Window                                ",
        "^L Repeat Find    ^OY Close Window                               ",
        "^P Special Char   ^QB Goto Block Top                             ",
        "^K Command ^Kx    ^QK Goto Block End                             ",
        "^O Command ^Ox    ^QR Goto File Top                              ",
        "^Q Command ^Qx    ^QC Goto File End                              ",
        "Press any key to continue               edi 5.0 19-Feb-2002      "
};

void Help(void)
{
   int i,c,r;
   WINDOW *sub;

   c = (COLS-67)/2;
   r = 0;
   sub = subwin(stdscr,22,67,r,c);
   wattroff(sub,AllColor);
   wattron(sub,FramColor);
   box(sub,0,0);
   wattron(sub,PromColor);
   for (i=0 ; i < 20 ; ++i)
           mvwaddstr(sub,i+1,1,HelpText[i]);
   wattroff(sub,PromColor);
   wgetch(sub);
   flushinp();
   delwin(sub);
} /* Help */

void FuncSyntax(void)
{
   ++Syntax;
   ShowSyntax();
   DrawFunctionKeys();
}

void FuncDeleteLine(void)
{
        DeleteLine(PriWin.CurLine);
}

void FuncPos1(void)
{
        PriWin.Col = PriWin.Ofs = 0;
}

void TogglePad(void)
{
   Pad = !Pad;
   keypad(stdscr,Pad);
}

struct FunctionArray FunctionTab[] =
{
        {"Menu ","Menu"        ,PulldownMenu        },
        {"Find ","Find"        ,EditFind            },
        {"Win  ","ToggleWindow",ToggleWindow        },
        {"UnMrk","Unmark"      ,UnmarkBlock         },
        {"Mark ","Mark"        ,MarkBlock           },
        {"Quit ","ExitSave"    ,ExitSave            },
        {"InsCh","InsertBlank" ,EditInsertBlank     },
        {"InsLn","InsertLine"  ,EdifInsertLine      },
        {"Split","SplitScreen" ,ToggleSplit         },
        {"Help ","Help"        ,Help                },
        {"Scr v","ScrollDown"  ,EditScrollDown      },
        {"Wrd->","WordRight"   ,EditRightWord       },
        {"First","TopFile"     ,EditWindowTopFile   },
        {"Last ","BottomFile"  ,EditWindowBottomFile},
        {"Home ","Home"        ,EditWindowTopFile   },
        {"Copy ","Copy"        ,CopyBlock           },
        {"Move ","Move"        ,EditBlockMove       },
        {"Exit ","Exit"        ,ExitFile            },
        {"UnDel","Undelete"    ,EditUndo            },
        {"DelLn","DeleteLine"  ,FuncDeleteLine      },
        {"Syntx","Syntax"      ,FuncSyntax          },
        {"Help ","Help"        ,Help                },
        {"Scr ^","ScrollUp"    ,EditScrollUp        },
        {"<-Wrd","WordLeft"    ,EditLeftWord        },
        {" <-- ","KEY_LEFT"    ,EditLeftChar        },
        {" --> ","KEY_RIGHT"   ,EditRightChar       },
        {"  ^  ","KEY_UP"      ,EditUpLine          },
        {"  v  ","KEY_DOWN"    ,EditDownLine        },
        {" |<- ","Pos1"        ,FuncPos1            },
        {" ->| ","EndLine"     ,EditEndLine         },
        {"PgUp ","PageUp"      ,EditUpPage          },
        {"PgDn ","PageDown"    ,EditDownPage        },
        {"Insrt","Insert"      ,ToggleInsert        },
        {" Del ","Delete"      ,DeleteRightChar     }
};

void ProcChar(int Key)
{
   if (Key == 27) Key = MapESC();
   if (Key > KEY_F0  && Key < KEY_F(25)) Ukey[Key-KEY_F(1)].Func();
   else switch (Key)
   {
        case CONTROL('A'): EditLeftWord();         break;
        case CONTROL('C'): EditDownPage();         break;
        case CONTROL('D'): EditRightChar();        break;
        case CONTROL('E'): EditUpLine();           break;
        case CONTROL('F'): EditRightWord();        break;
        case CONTROL('R'): EditUpPage();           break;
        case CONTROL('S'): EditLeftChar();         break;
        case CONTROL('V'): ToggleInsert();         break;
        case CONTROL('W'): EditScrollUp();         break;
        case CONTROL('X'): EditDownLine();         break;
        case CONTROL('Z'): EditScrollDown();       break;
        case KEY_LEFT    : EditLeftChar();         break;
        case KEY_RIGHT   : EditRightChar();        break;
        case KEY_UP      : EditUpLine();           break;
        case KEY_DOWN    : EditDownLine();         break;
        case KEY_HOME    : FuncPos1();             break;
        case KEY_END     : EditEndLine();          break;
        case KEY_PPAGE   : EditUpPage();           break;
        case KEY_NPAGE   : EditDownPage();         break;
        case KEY_IC      : ToggleInsert();         break;
        case KEY_DC      : DeleteRightChar();      break;
        case KEY_SR      : EditScrollUp();         break;
        case KEY_SF      : EditScrollDown();       break;
        case KEY_SLEFT   : EditLeftWord();         break;
        case KEY_SRIGHT  : EditRightWord();        break;
        case KEY_FIND    : FuncPos1();             break;
//      case KEY_FIND    : Find(1,0);              break;
        case KEY_F0      : PulldownMenu();         break;
        case CONTROL('B'): TogglePad();            break;
        case CONTROL('G'): DeleteRightChar();      break;
        case CONTROL('H'): DeleteLeftChar();       break;
        case CONTROL('I'): EditTab();              break;
        case CONTROL('K'): EditK();                break;
        case CONTROL('L'): Find(0,Replast);        break;
        case CONTROL('M'): EdifNewLine();          break;
        case CONTROL('N'): EdifInsertLine();       break;
        case CONTROL('O'): EditO();                break;
        case CONTROL('P'): EditInsertCtrlChar();   break;
        case CONTROL('Q'): EditQ();                break;
        case CONTROL('Y'): DeleteLine(PriWin.CurLine);break;
        case          127: DeleteLeftChar();      break;
   }
}

void DrawFunctionKeys(void)
{
   int i,j,LabelLength,FunctionKeys,Space;
   String st;

   attroff(AllColor);
   attron(TextColor);
   LabelLength  = strlen(Ukey[0].Label) + 3;
   FunctionKeys = ((COLS-1) / LabelLength) & ~1;
   if (FunctionKeys > 12) FunctionKeys = 12;
   Space        = COLS - FunctionKeys * LabelLength;
   move(LINES-FuncLines,0);
   attron(MenuColor);
   addstr("XXX");
   refresh();
   attroff(MenuColor);
   move(LINES-FuncLines,0);
   clrtoeol();
   if (FuncLines > 1)
   {
   move(LINES-2,0);
   for (i = 0; i < FunctionKeys ; ++i)
   {
      if (Space > 0 && i == FunctionKeys/2)
         for (j=0 ; j < Space ; ++j) addch(' ');
      addstr(" F");
      attroff(TextColor);
      attron(MenuColor);
      addstr(Ukey[i+12].Label);
      attroff(MenuColor);
      attron(TextColor);
      if (i != FunctionKeys-1) addch(' ');
   }
   }
   move(LINES-1,0);
   clrtoeol();
   for (i = 0; i < FunctionKeys ; ++i)
   {
      if (Space > 0 && i == FunctionKeys/2)
         for (j=0 ; j < Space ; ++j) addch(' ');
      sprintf(st,"%2d",i+1);
      addstr(st);
      attroff(TextColor);
      attron(MenuColor);
      addstr(Ukey[i].Label);
      attroff(MenuColor);
      attron(TextColor);
      if (i != FunctionKeys-1) addch(' ');
   }
   refresh();
}  /* DrawFunctionKeys */

char *SkipBlanks(char *cp)
{
        while (*cp == ' ') ++cp;
        return cp;
}

char *SearchEq(char *cp)
{
        while (*cp && *cp++ != '=');
        while (*cp == ' ') ++cp;
        return cp;
}

void ReadIni(void)
{
   int i,n,F;
   FILE *fp;
   char Line[256];
   char *cp,*ep,*fn;
   char *EDI;

   EDI = getenv("EDI");
   if (EDI) strcpy(Inifile,EDI);

   n = sizeof(FunctionTab) / sizeof(struct FunctionArray);
   for (i=0 ; i < 24 ; ++i) Ukey[i] = FunctionTab[i];

   fp = fopen(Inifile,"r");
   if (fp)
   {
      while (fgets(Line,256,fp))
      {
         strupr(Line);
         cp = SkipBlanks(Line);
         if (strncasecmp(cp,"Lines",5) == 0)
         {
            cp = SearchEq(cp+5);
            LINES = atoi(cp);
            if (LINES != 50) LINES = 28;
         }
         else if (strncasecmp(cp,"Tabs",4) == 0)
         {
            memset(TabStop,0,256 * sizeof(int));
            cp = SearchEq(cp+4);
            while (cp < Line+256 && (*cp == ' ' || isdigit(*cp)))
            {
               i = (int)strtol(cp,&ep,0);
               if (i < 256 && i > 0) TabStop[i-1] = 1;
               cp = ep;
            }
         }
         else if (strncasecmp(cp,"ReturnSplit",11) == 0) ReturnSplit = 1;
         else if (*cp == 'F')
         {
            F = atoi(++cp);
            if (F > 0 && F < 13)
            {
               cp = SearchEq(cp+1);
               fn = cp;
               while (isalpha(*cp)) ++cp;
               *cp = 0;
               for (i=0 ; i < n ; ++i)
               {
                  if (strcasecmp(fn,FunctionTab[i].Name) == 0)
                  Ukey[F-1] = FunctionTab[i];
               }
            }
         }
         else if (*cp == 'S' && *(cp+1) == 'F')
         {
            F = atoi(cp+2);
            if (F > 0 && F < 13)
            {
               cp = SearchEq(cp+3);
               fn = cp;
               while (isalpha(*cp)) ++cp;
               *cp = 0;
               for (i=0 ; i < n ; ++i)
               {
                  if (strcasecmp(fn,FunctionTab[i].Name) == 0)
                  Ukey[F-1+12] = FunctionTab[i];
               }
            }
         }
         else
         for (F=0; F<sizeof(AttributeTab)/sizeof(struct AttributeStruct) ; ++F)
         if (strncasecmp(cp,AttributeTab[F].Name,strlen(AttributeTab[F].Name)) == 0)
         {
            cp = SearchEq(cp+strlen(AttributeTab[F].Name));
            fn = cp;
            while (isalpha(*cp)) ++cp;
            for (i=0;i<sizeof(ColorTab)/sizeof(struct ColorStruct);++i)
            {
               if (strncasecmp(fn,ColorTab[i].Name,strlen(ColorTab[i].Name)) == 0)
               AttributeTab[F].Foreground = ColorTab[i].Color;
            }
            cp = SkipBlanks(cp);
            fn = cp;
            while (isalpha(*cp)) ++cp;
            *cp = 0;
            for (i=0;i<sizeof(ColorTab)/sizeof(struct ColorStruct);++i)
            {
               if (strncasecmp(fn,ColorTab[i].Name,strlen(ColorTab[i].Name)) == 0)
               AttributeTab[F].Background = ColorTab[i].Color;
            }

         }
      }
      fclose(fp);
   }
   for (i=0 ; i < 24 ; ++i)
      if (strcmp(Ukey[i].Label,"Syntx") == 0) SyntaxKey = i;
}

mmask_t mm;

MEVENT mevent;

int main(int argc, char *argv[])
{
   int i;

   Debug = argc > 2;
   if (Debug) logfile = fopen("edi.log","wa");
   for (i=0 ; i < 256 ; i+=8) TabStop[i] = 1;
   ReadIni();
   getcwd(FilePath,255);
   initscr();
   if (logfile) fprintf(logfile,"Terminal = <%s>\n",termname());
   start_color();
   DefineColors();
   cbreak();
   nonl();
   noecho();
   keypad(stdscr,1);
   raw();
   move(0,0);
   erase();
   bkgdset(TextColor|' ');
   refresh();
   mm = mousemask(BUTTON1_CLICKED,NULL);
   WinDescInit(&PriWin,0,LINES-FuncLines-1);
   if (argc > 1)
   {
      strcpy(PriWin.FileName,argv[1]);
      ProcessFileName(PriWin.FileName);
   }
   OpenFile(&PriWin,(argc < 2));
   DrawFunctionKeys();
   EditUpdPhyscr();
   UpdateCursor();
   while(1)
   {
      StatusLine();
      move(PriWin.Row+PriWin.TopScreenRow,PriWin.Col-PriWin.Ofs);
      KeyCode = wgetch(stdscr);
      if (KeyCode == KEY_MOUSE)
      {
         getmouse(&mevent);
         GotoXY(mevent.x,mevent.y-1);
      }
      else if (KeyCode >= ' ' && KeyCode < 127) EmitChar(KeyCode);
      else                                      ProcChar(KeyCode);
      UpdateCursor();
      EditUpdPhyscr();
   }
   return 0;
}
