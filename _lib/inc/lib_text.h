
// ****************************************************************************
//
//                               Text strings
//
// ****************************************************************************

#if USE_TEXT		// 1=use text strings, 0=not used (lib_text*)

#ifndef _LIB_TEXT_H
#define _LIB_TEXT_H

// hour format
#define HOURFORM_INT	0	// integer hour: "1.23"
#define HOURFORM_SPC	1	// space hour: " 1:23"
#define HOURFORM_ZERO	2	// zero hour: "01.23"
#define HOURFORM_PM	3	// English hour: "1.23p"

// second format
#define SECFORM_NO	0	// no seconds: "1:23"
#define SECFORM_SEC	1	// seconds: "1:23:45"
#define SECFORM_MS	2	// milliseconds: "1:23:45.678"

// time separator
#define TIMESEP_NO	0	// no time separator
#define TIMESEP_DOT	1	// colon ":" and dot "."
#define TIMESEP_COMMA	2	// colon ":" and comma ","
#define TIMESEP_DOTCOM	3	// dot "." and comma ","

// day of week format
#define DOWFORM_NUM0	1	// number 0..6 (0=Sunday)
#define DOWFORM_NUM1	2	// number 1..7 (1=Sunday)
#define DOWFORM_2	3	// 2 characters
#define DOWFORM_3	4	// 3 characters
#define DOWFORM_LONG	5	// long name

// pre-set date formats
//    d ... day 1 or 2 digits
//    D ... day 2 digits
//    m ... month 1 or 2 digits
//    M ... month 2 digits
//    n ... month name 3 letters
//    N ... month name full
//    y ... year 2 digits
//    Y ... year 4 digits
#define DATEFORM_USA_MDy	0	// USA format "M/D/y" (01/25/22)
#define DATEFORM_USA_mdY	1	// USA format "m/d/Y" (1/25/2022)
#define DATEFORM_USA_MDY	2	// USA format "M/D/Y" (01/25/2022)
#define DATEFORM_USA_ndY	3	// USA format "n d,Y" (Jan 25,2022)
#define DATEFORM_USA_NdY	4	// USA format "N d,Y" (January 25,2022)
#define DATEFORM_EU_DMy		5	// European format "D.M.y" (25.01.22)
#define DATEFORM_EU_dmY		6	// European format "d.m.Y" (25.1.2022)
#define DATEFORM_EU_DMY		7	// European format "D.M.Y" (25.01.2022)
#define DATEFORM_EU_dnY		8	// European format "d. n Y" (25. Jan 2022)
#define DATEFORM_EU_dNY		9	// European format "d. N Y" (25. January 2022)
#define DATEFORM_ISO_yMD	10	// ISO format "y-M-D" (22-01-25)
#define DATEFORM_ISO_YMD	11	// ISO format "Y-M-D" (2022-01-25)
#define DATEFORM_TECH		12	// Technical format "yMD" (220125)

// day of week name, 2 characters
extern const char* DayOfWeekName2[7];

// day of week name, 3 characters
extern const char* DayOfWeekName3[7];

// day of week name, long
extern const char* DayOfWeekNameLong[7];

// month name, 3 characters
extern const char* MonthName3[12];

// month name, long
extern const char* MonthNameLong[12];

// pre-set date formats
extern const char* DateForm[13];

// get length of ASCIIZ text string
int StrLen(const char* text);

// compare ASCIIZ text strings (returns 0=equal or returns character difference)
int StrComp(const char* text1, const char* text2);

// round coefficients, used in SetAddFloat function
extern const float SetFloatCoeff[11];

// round coefficients, used in SetAddDouble function
extern const double SetDoubleCoeff[19];

// check if float number is invalid number (#INF or #NaN)
Bool FloatIsOver(float num);

// check if double number is invalid number (#INF or #NaN)
Bool DoubleIsOver(double num);

// convert ASCIIZ text to unsigned INT number
//  - can be in HEX format 0xN or $N
//  - can be in BIN format 0bN or %N
//  - limited to min. and max. value
//  - optionally can return new text pointer (if "newtext" is not NULL)
u32 StrToUInt(const char* text, const char** newtext = NULL);

// convert ASCIIZ text to signed INT number
//  - can be in HEX format 0xN or $N
//  - can be in BIN format 0bN or %N
//  - limited to min. and max. value
//  - optionally can return new text pointer (if "newtext" is not NULL)
s32 StrToInt(const char* text, const char** newtext = NULL);

// convert ASCIIZ text to 64-bit unsigned INT number
//  - can be in HEX format 0xN or $N
//  - can be in BIN format 0bN or %N
//  - limited to min. and max. value
//  - optionally can return new text pointer (if "newtext" is not NULL)
u64 StrToUInt64(const char* text, const char** newtext = NULL);

// convert ASCIIZ text to 64-bit signed INT number
//  - can be in HEX format 0xN or $N
//  - can be in BIN format 0bN or %N
//  - limited to min. and max. value
//  - optionally can return new text pointer (if "newtext" is not NULL)
s64 StrToInt64(const char* text, const char** newtext = NULL);

// convert ASCIIZ text to float number
//  - does not check overflows
//  - optionally can return new text pointer (if "newtext" is not NULL)
float StrToFloat(const char* text, const char** newtext = NULL);

// convert ASCIIZ text to double number
//  - does not check overflows
//  - optionally can return new text pointer (if "newtext" is not NULL)
double StrToDouble(const char* text, const char** newtext = NULL);

// number of bytes of UTF-8 character
extern const u8 UTF8BytesTab[256];

// get number of bytes of UTF-8 character based on its first byte
INLINE int UTF8Bytes(u8 ch) { return UTF8BytesTab[ch]; }

// text string data (8 bytes + data)
typedef struct ALIGNED {

	// reference counter (number of owners, 0=nobody)
#if (CORES > 1) && USE_MULTICORE
	volatile u32	ref;
#else
	u32		ref;
#endif

	// length of text data (without trailing zero)
	s32		len;

	// ASCIIZ text string (including trailing zero)
	char		str[1];

} string_data;

// size of text string data header, without text data
#define SIZEOFSTRINGDATA	(sizeof(u32)+sizeof(s32))

// empty text string data
extern string_data EmptyStringData;

// text string
class string
{
protected:

	// pointer to text string data
	string_data*	m_Data;

	// attach text string data
	void Attach(string_data* data);

	// detach (and destroy) text string data
	void Detach();

	// detach and attach text string data (avoid deleting identical data)
	void DetachAttach(string_data* data);

	// create new text string data (old text string data must be detached)
	// (on error it attaches empty text string data and returns False)
	Bool NewBuffer(int length);

	// resize text string data (ref must be = 1, returns False on error)
	Bool SetSize(int length);

	// appropriate text string data before write (returns False on error)
	Bool CopyWrite();

public:

	// destructor
	~string();

// ---- constructors

	// constructor - empty string
	string();

	// construktor from text string data
	string(string_data* data);

	// constructor from other text string
	string(const string& text);

	// constructor from character
	string(char ch);

	// constructor from simple ASCII text
	//  length = text length or -1=auto ASCIIZ
	string(const char* text, int length = -1);

	// addition constructors (ASCIIZ text)
	string(string text1, string text2);
	string(const char* text1, string text2);
	string(string text1, const char* text2);
	string(char ch, string text);
	string(string text, char ch);

// ---- properties

	// get text length
	INLINE int Length() const { return this->m_Data->len; }

	// set text length (and appropriate text string before write)
	Bool SetLength(int length);

	// get reference counter
	INLINE u32 Ref() const { return this->m_Data->ref; }

	// check if string is empty
	INLINE Bool IsEmpty() const { return this->m_Data->len == 0; }
	INLINE Bool IsNotEmpty() const { return this->m_Data->len != 0; }

	// set empty string
	void Empty();
	
	// get ASCIIZ text string (need to use in printf() function)
	INLINE const char* str() const { return m_Data->str; }

	// retype operator
	INLINE operator const char*() const { return m_Data->str; }

	// get pointer to string data (only for special use - when modifying, data should not be shared between strings)
	INLINE string_data* Data() { return m_Data; }

// ---- character

	// check if character index is valid
	INLINE Bool IsValid(int inx) const { return (u32)inx < (u32)this->m_Data->len; }
	INLINE Bool IsNotValid(int inx) const { return (u32)inx >= (u32)this->m_Data->len; }

	// array operator (without index checking)
	INLINE const char& operator[] (int inx) const { return this->m_Data->str[inx]; }

	// get character at given position (returns 0 if index is not valid)
	INLINE char At(int inx) const { return (this->IsValid(inx)) ? this->m_Data->str[inx] : '\0'; }
	INLINE char First() const { return this->At(0); }
	INLINE char Last() const { return this->At(this->m_Data->len-1); }

	// set character (with index checking, returns False on error)
	Bool SetAt(int inx, char ch);
	INLINE Bool SetFirst(char ch) { return this->SetAt(0, ch); }
	INLINE Bool SetLast(char ch) { return this->SetAt(this->m_Data->len-1, ch); }

// ---- set text

	// set text string from another text string
	void Set(const string& text);

	// set text string from ASCII text (returns False on error)
	//  length = text length or -1=auto ASCIIZ
	Bool Set(const char* text, int length = -1);

	// set text string from a character (returns False on error)
	Bool Set(char ch);

	// exchange text strings
	void Exc(string& text);

	// NUL correction - check and truncate text if contains NUL character
	void CheckNul();

// ---- add text

	// add repeated text string (returns False on error)
	Bool AddRep(string text, int count);

	// add repeated ASCII text (returns False on error)
	//  length = text length or -1=auto ASCIIZ
	Bool AddRep(const char* text, int count, int length = -1);

	// add repeated character (returns False on error)
	Bool AddRep(char ch, int count);

	// add text string (returns False on error)
	INLINE Bool Add(string text) { return this->AddRep(text, 1); }

	// add ASCII text wiht length (returns False on error)
	INLINE Bool Add(const char* text, int length = -1) { return this->AddRep(text, 1, length); }

	// add character (returns False on error)
	INLINE Bool Add(char ch) { return this->AddRep(ch, 1); }

// ---- export text

	// write text into buffer (without terminating zero), returns text length
	int Write(char* buf, int maxlen = BIGINT) const;

	// write text into buffer with terminating zero (maxlen is max. length
	//   without terminating zero), returns text length
	int WriteZ(char* buf, int maxlen = BIGINT) const;

	// check ASCII string - all characters must be in range 0..0x7f
	Bool IsASCII() const;

	// get number of UTF8 characters in the string
	int UTF8Count() const;

	// get byte index of the UTF8 character in the string, given by its character index (returns -1 if character index is invalid)
	int UTF8Index(int inx) const;

	// load Unicode character from UTF-8 string and shift character index (character index must be in valid range)
	u32 GetUTF8(int* index) const;

	// shift index to next UTF8 character (character index must be in valid range)
	void NextUTF8(int* index) const;

	// export string in UTF-8 format to buffer in Unicode format (maxlen is max. length
	//   without terminating zero), returns text length in characters
	int UTF8ToUnicode(u32* buf, int maxlen = BIGINT) const;

// ---- find

	// find character in forward/reverse direction, starting at position (returns index or -1 = not found)
	int Find(char ch, int pos = 0) const;
	int FindRev(char ch, int pos = BIGINT) const;

	// find white space (space, tab, newline) in forward/reverse direction, starting at position (returns index or -1 = not found)
	int FindSpace(int pos = 0) const;
	int FindSpaceRev(int pos = BIGINT) const;

	// find non-white space in forward/reverse direction, starting at position (returns index or -1 = not found)
	int FindNoSpace(int pos = 0) const;
	int FindNoSpaceRev(int pos = BIGINT) const;

	// find text in forward/reverse direction, starting at position (returns index or -1 = not found)
	//  length = text length or -1=auto ASCIIZ
	int Find(const string& text, int pos = 0) const;
	int FindRev(const string& text, int pos = BIGINT) const;
	int Find(const char* text, int pos = 0, int length = -1) const;
	int FindRev(const char* text, int pos = BIGINT, int length = -1) const;

	// find characters from the list, starting at position (returns index or -1 = not found)
	//  count = number of characters or -1=auto ASCIIZ
	int FindList(const string& list, int pos = 0) const;
	int FindListRev(const string& list, int pos = BIGINT) const;
	int FindList(const char* list, int pos = 0, int count = -1) const;
	int FindListRev(const char* list, int pos = BIGINT, int count = -1) const;

	// find characters except the list, starting at position (returns index or -1 = not found)
	//  count = number of characters or -1=auto ASCIIZ
	int FindExcept(const string& list, int pos = 0) const;
	int FindExceptRev(const string& list, int pos = BIGINT) const;
	int FindExcept(const char* list, int pos = 0, int count = -1) const;
	int FindExceptRev(const char* list, int pos = BIGINT, int count = -1) const;

// ---- replace

	// substitute text (returns False on memory error)
	Bool Subst(string find, string subst);
	Bool Subst(const char* find, const char* subst);

	// substitute characters from the list (returns False on memory error)
	Bool SubstList(string list, string subst);
	Bool SubstList(const char* list, const char* subst);

	// unformat text - substitute white characters with one space character (returns False on memory error)
	Bool Unformat();

// ---- conversions

	// convert text to uppercase (returns False on memory error)
	Bool UpperCase();

	// convert text to lowercase (returns False on memory error)
	Bool LowerCase();

	// convert text to opposite case (returns False on memory error)
	Bool FlipCase();

	// convert text to wordcase - first letter upper (returns False on memory error)
	Bool WordCase();

	// convert text to reverse oder (returns False on memory error)
	Bool Reverse();

// ---- part of the text

	// get left part of the text (returns empty string on error)
	string Left(int length = 1) const;

	// get right part of the text (returns empty string on error)
	string Right(int length = 1) const;
	string RightFrom(int pos) const;

	// get middle part of the text (returns empty string on error)
	string Mid(int pos, int length = 1) const;

// ---- insert

	// insert text (returns False on error)
	//  length = text length or -1=auto ASCIIZ
	Bool Insert(int pos, string text);
	Bool Insert(int pos, const char* text, int length = -1);
	Bool Insert(int pos, char ch);

// ---- deletion

	// delete part of the text (returns False on memory error)
	Bool Delete(int pos, int length = 1);
	INLINE Bool DelFirst() { return this->Delete(0); }
	INLINE Bool DelLast() { return this->Delete(this->m_Data->len - 1); }
	INLINE Bool DelFrom(int pos) { return this->Delete(pos, BIGINT); }

	// find and delete all characters (returns False on memory error)
	Bool DelChar(char ch);

	// delete characters from the list (returns False on memory error)
	//  count = number of characters or -1=auto ASCIIZ
	Bool DelList(string list);
	Bool DelList(const char* list, int count = -1);

	// delete characters except the list (returns False on memory error)
	//  count = number of characters or -1=auto ASCIIZ
	Bool DelExcept(string list);
	Bool DelExcept(const char* list, int count = -1);

	// find and delete all words (returns False on memory error)
	//  length = text length or -1=auto ASCIIZ
	Bool DelWord(string text);
	Bool DelWord(const char* text, int length = -1);

	// trim white spaces from the begin od the string (returns False on memory error)
	Bool TrimLeft();

	// trim white spaces from the end of the string (returns False on memory error)
	Bool TrimRight();

	// trim white spaces from begin and end of the string (returns False on memory error)
	Bool Trim();

// ---- multi-line

	// get number of rows of multi-line text (search LF)
	int RowsNum() const;

	// get one row from multi-line text (search LF, trim CR from ends; returns False on memory error)
	string Row(int row) const;

// ---- set number

	// set/add signed/unsigned 32-bit integer number to text (returns False on memory error)
	//  num ... number (signed or unsigned)
	//  add ... True=add number, False=set number
	//  unsign ... True=unsigned, False=signed
	//  sep ... thousand separator or 0=none
	Bool SetAddInt(s32 num, Bool add, Bool unsign, char sep = 0);
	INLINE Bool SetInt(s32 num, char sep = 0) { return this->SetAddInt(num, False, False, sep); }
	INLINE Bool SetUInt(u32 num, char sep = 0) { return this->SetAddInt((s32)num, False, True, sep); }
	INLINE Bool AddInt(s32 num, char sep = 0) { return this->SetAddInt(num, True, False, sep); }
	INLINE Bool AddUInt(u32 num, char sep = 0) { return this->SetAddInt((s32)num, True, True, sep); }

	// set/add signed/unsigned 64-bit integer number to text (returns False on memory error)
	//  num ... number (signed or unsigned)
	//  add ... True=add number, False=set number
	//  unsign ... True=unsigned, False=signed
	//  sep ... thousand separator or 0=none
	Bool SetAddInt64(s64 num, Bool add, Bool unsign, char sep = 0);
	INLINE Bool SetInt64(s64 num, char sep = 0) { return this->SetAddInt64(num, False, False, sep); }
	INLINE Bool SetUInt64(u64 num, char sep = 0) { return this->SetAddInt64((s64)num, False, True, sep); }
	INLINE Bool AddInt64(s64 num, char sep = 0) { return this->SetAddInt64(num, True, False, sep); }
	INLINE Bool AddUInt64(u64 num, char sep = 0) { return this->SetAddInt64((s64)num, True, True, sep); }

	// set/add 2 digits to text (returns False on memory error)
	//  num ... number 0..99
	//  add ... True=add number, False=set number
	Bool SetAdd2Dig(int num, Bool add);
	INLINE Bool Set2Dig(int num) { return this->SetAdd2Dig(num, False); }
	INLINE Bool Add2Dig(int num) { return this->SetAdd2Dig(num, True); }

	// set/add 2 digits with space to text (returns False on memory error)
	//  num ... number 0..99
	//  add ... True=add number, False=set number
	Bool SetAdd2DigSpc(int num, Bool add);
	INLINE Bool Set2DigSpc(int num) { return this->SetAdd2DigSpc(num, False); }
	INLINE Bool Add2DigSpc(int num) { return this->SetAdd2DigSpc(num, True); }

	// set/add 4 digits to text (returns False on memory error)
	//  num ... number 0..9999
	//  add ... True=add number, False=set number
	Bool SetAdd4Dig(int num, Bool add);
	INLINE Bool Set4Dig(int num) { return this->SetAdd4Dig(num, False); }
	INLINE Bool Add4Dig(int num) { return this->SetAdd4Dig(num, True); }

	// set/add 32-bit integer to text as HEX (returns False on memory error)
	//  num ... unsigned number
	//  digits ... number of digits, or <= 0 to auto-digits
	//  add ... True=add number, False=set number
	//  sep ... 4-digit separator or 0=none
	Bool SetAddHex(u32 num, int digits, Bool add, char sep = 0);
	INLINE Bool SetHex(u32 num, int digits, char sep = 0) { return this->SetAddHex(num, digits, False, sep); }
	INLINE Bool AddHex(u32 num, int digits, char sep = 0) { return this->SetAddHex(num, digits, True, sep); }

	// set/add 64-bit integer to text as HEX (returns False on memory error)
	//  num ... unsigned number
	//  digits ... number of digits, or <= 0 to auto-digits
	//  add ... True=add number, False=set number
	//  sep ... 4-digit separator or 0=none
	Bool SetAddHex64(u64 num, int digits, Bool add, char sep = 0);
	INLINE Bool SetHex64(u64 num, int digits, char sep = 0) { return this->SetAddHex64(num, digits, False, sep); }
	INLINE Bool AddHex64(u64 num, int digits, char sep = 0) { return this->SetAddHex64(num, digits, True, sep); }

	// set/add 32-bit integer to text as BIN (returns False on memory error)
	//  num ... unsigned number
	//  digits ... number of digits, or <= 0 to auto-digits
	//  add ... True=add number, False=set number
	//  sep ... 4-digit separator or 0=none
	Bool SetAddBin(u32 num, int digits, Bool add, char sep = 0);
	INLINE Bool SetBin(u32 num, int digits, char sep = 0) { return this->SetAddBin(num, digits, False, sep); }
	INLINE Bool AddBin(u32 num, int digits, char sep = 0) { return this->SetAddBin(num, digits, True, sep); }

	// set/add 64-bit integer to text as BIN (returns False on memory error)
	//  num ... unsigned number
	//  digits ... number of digits, or <= 0 to auto-digits
	//  add ... True=add number, False=set number
	//  sep ... 4-digit separator or 0=none
	Bool SetAddBin64(u64 num, int digits, Bool add, char sep = 0);
	INLINE Bool SetBin64(u64 num, int digits, char sep = 0) { return this->SetAddBin64(num, digits, False, sep); }
	INLINE Bool AddBin64(u64 num, int digits, char sep = 0) { return this->SetAddBin64(num, digits, True, sep); }

	// set/add FLOAT number to text (returns False on error)
	//  num ... number
	//  digits ... number of digits 1..8 (recommended 6)
	//  add ... True=add number, False=set number
	Bool SetAddFloat(float num, Bool add, int digits = 6);
	INLINE Bool SetFloat(float num, int digits = 6) { return this->SetAddFloat(num, False, digits); }
	INLINE Bool AddFloat(float num, int digits = 6) { return this->SetAddFloat(num, True, digits); }

	// set/add DOUBLE number to text (returns False on error)
	//  num ... number
	//  digits ... number of digits 1..16 (recommended 14)
	//  add ... True=add number, False=set number
	Bool SetAddDouble(double num, Bool add, int digits = 14);
	INLINE Bool SetDouble(double num, int digits = 14) { return this->SetAddDouble(num, False, digits); }
	INLINE Bool AddDouble(double num, int digits = 14) { return this->SetAddDouble(num, True, digits); }

// ---- get number

	// get 32-bit number, limit to valid range
	//  - can be in HEX format 0xN or $N
	//  - can be in BIN format 0bN or %N
	//  - limited to min. and max. value
	INLINE u32 GetUInt() const { return StrToUInt(this->m_Data->str); }
	INLINE s32 GetInt() const { return StrToInt(this->m_Data->str); }

	// get 64-bit number, limit to valid range
	//  - can be in HEX format 0xN or $N
	//  - can be in BIN format 0bN or %N
	//  - limited to min. and max. value
	INLINE u64 GetUInt64() const { return StrToUInt64(this->m_Data->str); }
	INLINE s64 GetInt64() const { return StrToInt64(this->m_Data->str); }

	// get float number
	INLINE float GetFloat() const { return StrToFloat(this->m_Data->str); }

	// get double number
	INLINE double GetDouble() const { return StrToDouble(this->m_Data->str); }

// ---- date and time

	// set/add time in Unix format (returns False on memory error)
	//  ut ... Unix time
	//  ms ... number of milliseconds 0..999
	//  hourform ... hour format HOURFORM_*
	//  secform ... second format SECFORM_*
	//  timesep ... time separator TIMESEP_*
	//  add ... True=add time, False=set time
	Bool SetAddTime(u32 ut, int hourform, int secform, int timesep, Bool add, int ms = 0);
	INLINE Bool SetTime(u32 ut, int hourform, int secform, int timesep, int ms = 0)
		{ return this->SetAddTime(ut, hourform, secform, timesep, False, ms); }
	INLINE Bool AddTime(u32 ut, int hourform, int secform, int timesep, int ms = 0)
		{ return this->SetAddTime(ut, hourform, secform, timesep, True, ms); }

	// set/add day of week 2-character text from Unix time
	//  ut ... Unix time
	//  add ... True=add time, False=set time
	//  dowform ... day of week format DOWFORM_*
	Bool SetAddDow(u32 ut, Bool add, int dowform = DOWFORM_2);
	INLINE Bool SetDow(u32 ut, int dowform = DOWFORM_2) { return this->SetAddDow(ut, False, dowform); }
	INLINE Bool AddDow(u32 ut, int dowform = DOWFORM_2) { return this->SetAddDow(ut, True, dowform); }

	// set/add date in custom format (returns False on memory error)
	//   form: formatting string
	//    d ... day 1 or 2 digits
	//    D ... day 2 digits
	//    m ... month 1 or 2 digits
	//    M ... month 2 digits
	//    n ... month name 3 letters
	//    N ... month name full
	//    y ... year 2 digits
	//    Y ... year 4 digits
	Bool SetAddDateCustom(int year, int mon, int day, const char* form, Bool add);
	INLINE Bool SetDateCustom(int year, int mon, int day, const char* form)
		{ return this->SetAddDateCustom(year, mon, day, form, False); }
	INLINE Bool AddDateCustom(int year, int mon, int day, const char* form)
		{ return this->SetAddDateCustom(year, mon, day, form, True); }

	// set/add date in format DATEFORM_* (returns False on memory error)
	Bool SetDate(int year, int mon, int day, int form);
	Bool AddDate(int year, int mon, int day, int form);

#if USE_CALENDAR	// use 32-bit calendar (lib_calendar.c, lib_calendar.h)
	// set/add Unix date in format DATEFORM_* (returns False on memory error)
	Bool SetDateUnix(u32 ut, int form);
	Bool AddDateUnix(u32 ut, int form);

	// set/add Unix date and time in technical format yyyymmddhhmmssmmm (17 digits, returns False on memory error)
	Bool SetAddDateTech(u32 ut, int ms, Bool add);
#endif // USE_CALENDAR

// ---- formatted print

#if USE_PRINT		// 1=use Formatted print, 0=not used (lib_print.*)

	// set formatted print string into text, with argument list (returns text length or 0 on error)
	//  - fmt and destination text cannot be the same
	u32 PrintArg(const char* fmt, va_list args);

	// set formatted print string into text, with variadic arguments
	//  - fmt and destination text cannot be the same
	u32 Print(const char* fmt, ...);

	// add formatted print string into text, with argument list (returns text length or 0 on error)
	//  - fmt and destination text cannot be the same
	u32 AddPrintArg(const char* fmt, va_list args);

	// add formatted print string into text, with variadic arguments
	//  - fmt and destination text cannot be the same
	u32 AddPrint(const char* fmt, ...);

#endif // USE_PRINT

// ---- file

#if USE_FAT		// 1=use FAT file system, 0=not used (lib_fat.*)

	// load file (returns False on error)
	//  Loaded text may be shorter than required length.
	//  SD card must be mounted.
	Bool LoadFile(sFile* file, int size);

	// load file with name (returns False on error)
	//  SD card must be mounted.
	Bool LoadFileName(const char* name);

	// save file (returns False if text could not be saved entirely)
	//  SD card must be mounted.
	Bool SaveFile(sFile* file) const;

	// save file with name (returns False on error)
	//  Existing file will be deleted and new one will be created.
	//  SD card must be mounted.
	Bool SaveFileName(const char* name) const;

#endif // USE_FAT

// ---- operators

	// set operators
	const string& operator= (const string& text);
	const string& operator= (const char* text);
	const string& operator= (char ch);

	// add operators
	const string& operator+= (string text);
	const string& operator+= (const char* text);
	const string& operator+= (char ch);
	INLINE friend string operator+ (const string& text1, const string& text2)
		{ return string(text1, text2); }
	INLINE friend string operator+ (const char* text1, const string& text2)
		{ return string(text1, text2); }
	INLINE friend string operator+ (const string& text1, const char* text2)
		{ return string(text1, text2); }
	INLINE friend string operator+ (char chr, const string& text)
		{ return string(chr, text); }
	INLINE friend string operator+ (const string& text, char ch)
		{ return string(text, ch); }

	// compare operators
	friend Bool operator== (const string& text1, const string& text2);
	friend Bool operator== (const char* text1, const string& text2);
	friend Bool operator== (const string& text1, const char* text2);
	friend Bool operator== (char ch, const string& text);
	friend Bool operator== (const string& text, char ch);
	friend Bool operator!= (const string& text1, const string& text2);
	friend Bool operator!= (const char* text1, const string& text2);
	friend Bool operator!= (const string& text1, const char* text2);
	friend Bool operator!= (char ch, const string& text);
	friend Bool operator!= (const string& text, char ch);
	friend Bool operator< (const string& text1, const string& text2);
	friend Bool operator< (const char* text1, const string& text2);
	friend Bool operator< (const string& text1, const char* text2);
	friend Bool operator< (char ch, const string& text);
	friend Bool operator< (const string& text, char ch);
	INLINE friend Bool operator<= (const string& text1, const string& text2)
		{ return !(text2 < text1); }
	INLINE friend Bool operator<= (const char* text1, const string& text2)
		{ return !(text2 < text1); }
	INLINE friend Bool operator<= (const string& text1, const char* text2)
		{ return !(text2 < text1); }
	INLINE friend Bool operator<= (char ch, const string& text)
		{ return !(text < ch); }
	INLINE friend Bool operator<= (const string& text, char ch)
		{ return !(ch < text); }
	INLINE friend Bool operator> (const string& text1, const string& text2)
		{ return text2 < text1; }
	INLINE friend Bool operator> (const char* text1, const string& text2)
		{ return text2 < text1; }
	INLINE friend Bool operator> (const string& text1, const char* text2)
		{ return text2 < text1; }
	INLINE friend Bool operator> (char ch, const string& text)
		{ return text < ch; }
	INLINE friend Bool operator> (const string& text, char ch)
		{ return ch < text; }
	INLINE friend Bool operator>= (const string& text1, const string& text2)
		{ return !(text1 < text2); }
	INLINE friend Bool operator>= (const char* text1, const string& text2)
		{ return !(text1 < text2); }
	INLINE friend Bool operator>= (const string& text1, const char* text2)
		{ return !(text1 < text2); }
	INLINE friend Bool operator>= (char ch, const string& text)
		{ return !(ch < text); }
	INLINE friend Bool operator>= (const string& text, char ch)
		{ return !(text < ch); }

// ---- iterators

	// iterators
	INLINE char* begin() { return this->m_Data->str; }
	INLINE const char* begin() const { return this->m_Data->str; }
	INLINE char* end() { return this->m_Data->str + this->m_Data->len; }
	INLINE const char* end() const { return this->m_Data->str + this->m_Data->len; }
	INLINE char& front() { return this->m_Data->str[0]; }
	INLINE char& back() { return this->m_Data->str[this->m_Data->len - 1]; }
};

// Empty text
extern string EmptyText;

// New line CR/LF text
extern string NewLineText;

// list of text strings
class string_list
{
protected:

	// number of text strings in the list
	int		m_Num;

	// max. number of text strings in the list
	int		m_Max;

	// array of text string
	string*		m_List;

public:

	// constructor
	string_list();

	// destructor
	~string_list();

	// get number of text strings in the list
	INLINE int Num() const { return this->m_Num; }

	// set number of entries in the list (init = initialize/terminate entries; returns False on error)
	Bool SetNum(int num, Bool init = True);

	// array of text strings
	INLINE const string* List() const { return this->m_List; }
	INLINE string* List() { return this->m_List; }

	// delete all entries in the list
	void DelAll();

	// check if index is valid
	INLINE Bool IsValid(int inx) const { return (u32)inx < (u32)this->m_Num; }
	INLINE Bool IsNotValid(int inx) const { return (u32)inx >= (u32)this->m_Num; }

	// array operator (without index checking)
	INLINE string& operator[] (int inx) { return this->m_List[inx]; }
	INLINE const string& operator[] (int inx) const { return this->m_List[inx]; }

	// get text entry (with index checking)
	string Get(int inx);

	// set text entry (with index checking)
	void Set(int inx, const string& text);

	// add new empty text entry (returns index or -1 on error)
	int New();

	// add new text entry (returns index or -1 on error)
	int Add(string text);
	int Add(const char* text, int length = -1);

	// add another list to the list (returns index of first entry of -1 on error)
	int Add(const string_list& list);

	// duplicate entry (returns new index or -1 on error)
	int Dup(int inx);

	// copy list from another list (destroys old content, returns False on error)
	Bool Copy(const string_list& list);

	// insert text entry into list (limits index range, returns False on error)
	Bool Insert(int inx, string text);
	Bool Insert(int inx, const char* text, int length = -1);

	// insert another list to the list (limits index range, returns False on error)
	Bool Insert(int inx, const string_list& list);

	// delete entries from the list (check index range)
	void Delete(int inx, int count = 1);
	INLINE void DelFirst() { this->Delete(0, 1); }
	INLINE void DelLast() { this->Delete(this->m_Num - 1, 1); }
	INLINE void DelFrom(int inx) { this->Delete(inx, BIGINT); }

	// reverse order of entries in the list
	void Reverse();

	// split text string into text list by string delimiter (destroys old content of text list, returns False on error)
	//  text ... source text string to split
	//  delim ... ASCII text of delimiter (split to single characters, if delimiter is empty)
	//  length ... length of text of delimiter, or -1 to detect
	//  limit ... max. number of entries (use BIGINT to unlimited)
	Bool Split(const string& text, const char* delim, int length = -1, int limit = BIGINT);
	INLINE Bool Split(const string& text, const string& delim, int limit = BIGINT)
		{ return this->Split(text, delim.str(), delim.Length(), limit); }

	// split text string into text list by 1-character delimiter (destroys old content of text list, returns False on error)
	//  text ... source text string to split
	//  delim ... 1-character delimiter (0 = split to single characters)
	//  limit ... max. number of entries (use BIGINT to unlimited)
	Bool Split(const string& text, char delim, int limit = BIGINT);

	// split text string to words into text list (destroys old content of text list, returns False on error)
	//  text ... source text string to split
	//  limit ... max. number of entries (use BIGINT to unlimited)
	Bool SplitWords(const string& text, int limit = BIGINT);

	// split text string to rows into text list, delimiter is LF, trim CR (destroys old content of text list, returns False on error)
	//  text ... source text string to split
	//  limit ... max. number of entries (use BIGINT to unlimited)
	Bool SplitRows(const string& text, int limit = BIGINT);

	// join text strings with string delimiter (returns empty string on error)
	string Join(const char* delim, int length = -1) const;
	INLINE string Join(const string& delim) const { return this->Join(delim.str(), delim.Length()); }

	// join text string without delimiter (returns empty string on error)
	INLINE string Join() const { return this->Join(NULL, 0); }

	// join text strings with 1-character delimiter (returns empty string on error)
	string Join(char delim) const;

	// join text strings to rows with LF (returns empty string on error)
	INLINE string JoinLF() const { return this->Join(CH_LF); }

	// join text strings to rows with CR/LF (returns empty string on error)
	INLINE string JoinRows() const { return this->Join("\r\n", 2); }
};

#endif // _LIB_TEXT_H

#endif // USE_TEXT
