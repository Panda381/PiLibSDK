
// ****************************************************************************
//
//                           List of text strings
//
// ****************************************************************************

#include "../../includes.h"	// includes

#if USE_TEXT		// 1=use text strings, 0=not used (lib_text*)

// constructor
string_list::string_list()
{
	this->m_Num = 0;
	this->m_Max = 0;
	this->m_List = NULL;
}

// destructor
string_list::~string_list()
{
	this->DelAll();
}

// delete all entries in the list
void string_list::DelAll()
{
	int i;
	for (i = this->m_Num - 1; i >= 0; i--) this->m_List[i].~string();
	MemFree(this->m_List);
	this->m_Num = 0;
	this->m_Max = 0;
	this->m_List = NULL;
}

// set number of entries in the list (init = initialize/terminate entries; returns False on error)
Bool string_list::SetNum(int num, Bool init /*= True*/)
{
	int i;

	// no change
	if (num == this->m_Num) return True;

	// prepare new number or max. entries (multiply of 16 entries)
	int max2 = (num + 0x0f) & ~0x0f;

	// increase size of memory block
	if (max2 > this->m_Max)
	{
		string* data2 = (string*)MemResize(this->m_List, max2*sizeof(string));
		if (data2 == NULL) return False; // memory error
		this->m_List = data2;
		this->m_Max = max2;
	}

	if (init)
	{
		i = this->m_Num;

		// increase number of entries
		if (num > i)
		{
			for (; i < num; i++)
			{
				new (&this->m_List[i]) string;
			}
		}

		// decrease number of entries
		else
		{
			for (i--; i >= num; i--) this->m_List[i].~string();
		}
	}

	// decrease size of memory block (with 1-step hysteresis)
	if (max2 + 16 < this->m_Max)
	{
		string* data2 = (string*)MemResize(this->m_List, max2*sizeof(string));
		if (data2 != NULL)
		{
			this->m_List = data2;
			this->m_Max = max2;
		}
	}

	// set new number of entries
	this->m_Num = num;
	return True;
}

// get text entry (with index checking)
string string_list::Get(int inx)
{
	if (this->IsValid(inx)) return this->m_List[inx];
	return EmptyText;
}

// set text entry (with index checking)
void string_list::Set(int inx, const string& text)
{
	if (this->IsValid(inx)) this->m_List[inx] = text;
}

// add new empty text entry (returns index or -1 on error)
int string_list::New()
{
	int inx = this->m_Num; // index of new last entry
	if (!this->SetNum(inx + 1, True)) return -1; // increase number of entries, return on error
	return inx;
}

// add new text entry (returns index or -1 on error)
int string_list::Add(string text)
{
	int inx = this->m_Num; // index of new last entry
	if (!this->SetNum(inx + 1, False)) return -1; // increase number of entries, return on error
	new (&this->m_List[inx]) string(text);
	return inx;
}

// add new text entry (returns index or -1 on error)
int string_list::Add(const char* text, int length /*= -1*/)
{
	int inx = this->m_Num; // index of new last entry
	if (!this->SetNum(inx + 1, False)) return -1; // increase number of entries, return on error
	new (&this->m_List[inx]) string(text, length);
	return inx;
}

// add another list to the list (returns index of first entry of -1 on error)
int string_list::Add(const string_list& list)
{
	int inx = this->m_Num;		// entries in this list
	int num = list.m_Num;		// number of entries in another list
	if (!this->SetNum(inx+num, False)) return -1; // increase number of entries, return on error
	int i;
	for (i = 0; i < num; i++)
	{
		new (&this->m_List[inx+i]) string(list.m_List[i]);
	}
	return inx;
}

// duplicate entry (returns new index or -1 on error)
int string_list::Dup(int inx)
{
	if (this->IsNotValid(inx)) return -1;
	return this->Add(this->m_List[inx]);
}

// copy list from another list (destroys old content, returns FALSE on error)
Bool string_list::Copy(const string_list& list)
{
	// delete all entries in the list
	this->DelAll();

	// add another list to this list (and check result - should be 0)
	return (this->Add(list) >= 0);
}

// insert text entry into list (limits index range, returns False on error)
Bool string_list::Insert(int inx, string text)
{
	// limit index
	if (inx < 0) inx = 0;
	int num = this->m_Num;
	if (inx > num) inx = num;

	// increase number of entries
	if (!this->SetNum(num + 1, False)) return False;

	// move remaining strings
	memmove(&this->m_List[inx+1], &this->m_List[inx], (num - inx)*sizeof(string));

	// setup new entry
	new (&this->m_List[inx]) string(text);
	return True;
}

Bool string_list::Insert(int inx, const char* text, int length /*= -1*/)
{
	// limit index
	if (inx < 0) inx = 0;
	int num = this->m_Num;
	if (inx > num) inx = num;

	// increase number of entries
	if (!this->SetNum(num + 1, False)) return False;

	// move remaining strings
	memmove(&this->m_List[inx+1], &this->m_List[inx], (num - inx)*sizeof(string));

	// setup new entry
	new (&this->m_List[inx]) string(text, length);
	return True;
}

// insert another list to the list (limits index range, returns False on error)
//  - source list and destination list cannot be the same
Bool string_list::Insert(int inx, const string_list& list)
{
	// cannot insert list into itself
	if (this == &list) return False;

	// limit index
	if (inx < 0) inx = 0;
	int num = this->m_Num;
	if (inx > num) inx = num;

	// number of entries of source list
	int num2 = list.m_Num;

	// increase number of entries
	if (!this->SetNum(num + num2, False)) return False;

	// move remaining strings
	memmove(&this->m_List[inx+num2], &this->m_List[inx], (num - inx)*sizeof(string));

	// copy new entries
	int i;
	for (i = 0; i < num2; i++)
	{
		new (&this->m_List[inx+i]) string(list.m_List[i]);
	}
	return True;
}

// delete entries from the list (check index range)
void string_list::Delete(int inx, int count /*= 1*/)
{
	// check index and count
	if ((count <= 0) || this->IsNotValid(inx)) return;

	// limit number of entries
	if (count > this->m_Num - inx) count = this->m_Num - inx;

	// destroy deleted entries
	int i;
	for (i = 0; i < count; i++) this->m_List[inx+i].~string();

	// move remaining entries
	memmove(&this->m_List[inx], &this->m_List[inx + count], (this->m_Num - inx - count)*sizeof(string));

	// set new number of entries
	this->SetNum(this->m_Num - count, False);
}

// reverse order of entries in the list
void string_list::Reverse()
{
	int i = 0;
	int j = this->m_Num - 1;
	string text;
	for (; i < j; i++, j--) this->m_List[i].Exc(this->m_List[j]);
}

// split text string into text list by string delimiter (destroys old content of text list, returns False on error)
//  text ... source text string to split
//  delim ... ASCII text of delimiter (split to single characters, if delimiter is empty)
//  length ... length of text of delimiter, or -1 to detect
//  limit ... max. number of entries (use BIGINT to unlimited)
Bool string_list::Split(const string& text, const char* delim, int length /*= -1*/, int limit /*= BIGINT*/)
{
	// delete old content
	this->DelAll(); // delete all entries from the list
	if (text.IsEmpty()) return True; // source text is empty
	if (limit <= 0) return True; // max. number of entries is 0

	// prepare length of delimiter
	if (delim == NULL) length = 0;
	if (length < 0) length = StrLen(delim);

	// prepare to split
	int pos; // current pointer in source text
	int oldpos = 0; // old pointer of start of substring
	limit--; // limit number without last string

	// if delimiter text is empty - split to single characters
	if (length == 0)
	{
		// length of source text
		int len = text.Length();

		// limit number of entries
		if (len > limit) len = limit;

		// set length of the list, do not initialize entries
		if (!this->SetNum(len, False)) return False; 

		// add single-character entries
		for (; oldpos < len; oldpos++) new (&this->m_List[oldpos]) string(text[oldpos]);
	}

	// split string
	else
	{
		while (this->m_Num < limit)
		{
			// find next delimiter (-1 = not found)
			pos = text.Find(delim, oldpos, length);
			if (pos < 0) break;

			// add this entry to the list (-1 = error)
			if (this->Add(text.Mid(oldpos, pos - oldpos)) < 0) return False;

			// shift position to start of new text
			oldpos = pos + length;
		}
	}

	// add rest of the text
	return (this->Add(text.RightFrom(oldpos)) >= 0);
}

// split text string into text list by 1-character delimiter (destroys old content of text list, returns False on error)
//  text ... source text string to split
//  delim ... 1-character delimiter (0 = split to single characters)
//  limit ... max. number of entries (use BIGINT to unlimited)
Bool string_list::Split(const string& text, char delim, int limit /*= BIGINT*/)
{
	// delimiter is 0 - use empty text delimiter
	if (delim == 0) return this->Split(text, NULL, 0, limit);

	// delete old content
	this->DelAll(); // delete all entries from the list
	if (text.IsEmpty()) return True; // source text is empty
	if (limit <= 0) return True; // max. number of entries is 0

	// prepare to split
	int pos; // current pointer in source text
	int oldpos = 0; // old pointer of start of substring
	limit--; // limit number without last string

	// split string
	while (this->m_Num < limit)
	{
		// find next delimiter (-1 = not found)
		pos = text.Find(delim, oldpos);
		if (pos < 0) break;

		// add this entry to the list (-1 = error)
		if (this->Add(text.Mid(oldpos, pos - oldpos)) < 0) return False;

		// shift position to start of new text
		oldpos = pos + 1;
	}

	// add rest of the text
	return (this->Add(text.RightFrom(oldpos)) >= 0);
}

// split text string to words into text list (destroys old content of text list, returns False on error)
//  text ... source text string to split
//  limit ... max. number of entries (use BIGINT to unlimited)
Bool string_list::SplitWords(const string& text, int limit /*= BIGINT*/)
{
	// delete old content
	this->DelAll(); // delete all entries from the list
	if (text.IsEmpty()) return True; // source text is empty
	if (limit <= 0) return True; // max. number of entries is 0

	// prepare to split
	int pos; // current pointer in source text
	int oldpos = 0; // old pointer of start of substring
	limit--; // limit number without last string

	// split string
	for (;;)
	{
		// find non white space character - find start of word
		pos = text.FindNoSpace(oldpos);
		if (pos < 0) return True; // no next word, store nothing else

		// use it as start of next word
		oldpos = pos;

		// limit for number of entries has been exceeded
		if (this->m_Num >= limit) break;

		// find white space character - find end of word
		pos = text.FindSpace(oldpos);
		if (pos < 0) break; // no next delimiter, this word will be rest of the string

		// add this word to the list (-1 = error)
		if (this->Add(text.Mid(oldpos, pos - oldpos)) < 0) return False;

		// shift old position (skip delimiter)
		oldpos = pos;
	}

	// add rest of the text
	return (this->Add(text.RightFrom(oldpos)) >= 0);
}

// split text string to rows into text list, delimiter is LF (destroys old content of text list, returns False on error)
//  text ... source text string to split
//  limit ... max. number of entries (use BIGINT to unlimited)
Bool string_list::SplitRows(const string& text, int limit /*= BIGINT*/)
{
	// delete old content
	this->DelAll(); // delete all entries from the list
	if (text.IsEmpty()) return True; // source text is empty
	if (limit <= 0) return True; // max. number of entries is 0

	// prepare to split
	int pos; // current pointer in source text
	int oldpos = 0; // old pointer of start of substring
	int p1, p2; // save pointers
	limit--; // limit number without last string

	// split string
	while (this->m_Num < limit)
	{
		// find next delimiter LF
		pos = text.Find(CH_LF, oldpos);
		if (pos < 0) break; // no next delimiter

		// save pointers
		p1 = oldpos; // start of current row
		p2 = pos; // end of current row

		// reduce CR characters from start of the row
		while ((p1 < p2) && (text[p1] == CH_CR)) p1++;

		// reduce CR characters from end of the row
		while ((p1 < p2) && (text[p2-1] == CH_CR)) p2--;

		// add this row to the list (-1 = error)
		if (this->Add(text.Mid(p1, p2 - p1)) < 0) return False;

		// shift old position (skip delimiter)
		oldpos = pos + 1;
	}

	// reduce CR from start of last row
	while ((oldpos < text.Length()) && (text[oldpos] == CH_CR)) oldpos++;

	// do not store last empty string
	if (oldpos == text.Length()) return True;

	// add rest of the text
	return (this->Add(text.RightFrom(oldpos)) >= 0);
}

// join text strings with string delimiter (returns empty string on error)
string string_list::Join(const char* delim, int length /*= -1*/) const
{
	// output empty string
	string text;
	int num = this->m_Num;
	if (num == 0) return text;

	// prepare length of delimiter
	if (delim == NULL) length = 0;
	if (length < 0) length = StrLen(delim);

	// prepare total length of result string
	int len = (num-1)*length;
	int inx;
	for (inx = 0; inx < num; inx++) len += this->m_List[inx].Length();

	// allocate size of result string
	if (!text.SetLength(len)) return text;

	// copy first string
	char* dst = text.Data()->str;
	dst += this->m_List[0].Write(dst);

	// copy other strings
	for (inx = 1; inx < num; inx++)
	{
		// copy delimiter
		memcpy(dst, delim, length);
		dst += length;

		// copy string
		dst += this->m_List[inx].Write(dst);
	}
	return text;
}

// join text strings with 1-character delimiter (returns empty string on error)
string string_list::Join(char delim) const
{
	// delimiter is 0 - use empty text delimiter
	if (delim == 0) return this->Join(NULL, 0);

	// output empty string
	string text;
	int num = this->m_Num;
	if (num == 0) return text;

	// prepare total length of result string
	int len = num-1;
	int inx;
	for (inx = 0; inx < num; inx++) len += this->m_List[inx].Length();

	// allocate size of result string
	if (!text.SetLength(len)) return text;

	// copy first string
	char* dst = text.Data()->str;
	dst += this->m_List[0].Write(dst);

	// copy other strings
	for (inx = 1; inx < num; inx++)
	{
		// store delimiter
		*dst++ = delim;

		// copy string
		dst += this->m_List[inx].Write(dst);
	}
	return text;
}

#endif // USE_TEXT
