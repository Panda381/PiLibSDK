
// ****************************************************************************
//
//                               Text strings
//
// ****************************************************************************

#include "../../includes.h"	// includes

#if USE_TEXT		// 1=use text strings, 0=not used (lib_text*)

// get length of ASCIIZ text string
int StrLen(const char* text)
{
	if (text == NULL) return 0;
	const char* t = text;
	while (*t++ != 0) {};
	return (t - text) - 1;
}

// compare ASCIIZ text strings (returns 0=equal or returns character difference)
int StrComp(const char* text1, const char* text2)
{
	char ch1, ch2;
	for (;;)
	{
		ch1 = *text1++;
		ch2 = *text2++;
		if (ch1 != ch2) return ch1 - ch2;
		if (ch1 == 0) break;
	}
	return 0;
}

// number of bytes of UTF8 character
const u8 UTF8BytesTab[256] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3, 4,4,4,4,4,4,4,4,5,5,5,5,6,6,6,6
};

const u32 OffsetsFromUTF8[6] = { 0x00000000UL, 0x00003080UL, 0x000E2080UL,
  0x03C82080UL, 0xFA082080UL, 0x82082080UL };

// empty text string data
string_data EmptyStringData = { .ref = BIGINT, .len = 0, .str = { '\0' } };
string EmptyText;

// New line CR/LF text
string NewLineText("\r\n");

// attach text string data
void string::Attach(string_data* data)
{
	// set pointer to text data
	this->m_Data = data;

	// increment reference counter
#if (CORES > 1) && USE_MULTICORE
	IncLock32(&data->ref);
#else
	data->ref++;
#endif
}

// detach (and destroy) text string data
void string::Detach()
{
	// get pointer to text string data
	string_data* data = this->m_Data;

	// decrement reference counter
#if (CORES > 1) && USE_MULTICORE
	u32 ref = DecLock32(&data->ref);
#else
	u32 ref = data->ref - 1;
	data->ref = ref;
#endif
	// free text string data, if this was last owner
	if (ref == 0) MemFree(data);
}

// detach and attach text string data (avoid deleting identical data)
void string::DetachAttach(string_data* data)
{
	// get pointer to text string data
	string_data* olddata = this->m_Data;

	if (data != olddata)
	{
		this->Detach();
		this->Attach(data);
	}
}

// create new text string data (old text string data must be detached)
// (on error it attaches empty text string data and returns False)
Bool string::NewBuffer(int length)
{
	// allocate new text buffer
	string_data* data = (string_data*)MemAlloc(length + 1 + SIZEOFSTRINGDATA);

	// memory error
	if (data == NULL)
	{
		// attach empty string on error
		this->Attach(&EmptyStringData);
		return False;
	}

	// setup new text string
	this->m_Data = data;
	data->ref = 1;
	data->len = length;
	data->str[length] = 0;
	return True;
}	

// resize text string data (ref must be = 1, returns False on error)
Bool string::SetSize(int length)
{
	// get pointer to old text string data
	string_data* olddata = this->m_Data;

	// reallocate text buffer
	string_data* data = (string_data*)MemResize(olddata, length + 1 + SIZEOFSTRINGDATA);

	// memory error, do nothing
	if (data == NULL) return False;

	// setup new text string
	this->m_Data = data;
	data->len = length;
	data->str[length] = 0;
	return True;
}

// appropriate text string data before write (returns False on error)
Bool string::CopyWrite()
{
	string_data* data = this->m_Data;
	// If this string does not own the data exclusively,
	//   it must make a copy before modifying it.
	if (data->ref > 1)
	{
		// create new buffer
		int len = data->len;
		if (!this->NewBuffer(len))
		{
			this->Detach(); // detach empty string
			this->m_Data = data; // return original data
			return False;
		}

		// copy data
		memcpy(this->m_Data->str, data->str, len);

		// detach old string data
#if (CORES > 1) && USE_MULTICORE
		u32 ref = DecLock32(&data->ref);
#else
		u32 ref = data->ref - 1;
		data->ref = ref;
#endif
		// free text string data, if this was last owner
		if (ref == 0) MemFree(data);
	}
	return True;
}

// destructor
string::~string()
{
	this->Detach();
}

// constructor - empty string
string::string()
{
	this->Attach(&EmptyStringData);
}

// construktor from text string data
string::string(string_data* data)
{
	this->Attach(data);
}

// constructor from other text string
string::string(const string& text)
{
	this->Attach(text.m_Data);
}

// constructor from character
string::string(char ch)
{
	// empty text
	if (ch == 0)
		this->Attach(&EmptyStringData);
	else
	{
		if (this->NewBuffer(1)) this->m_Data->str[0] = ch;
	}
}

// constructor from simple ASCII text
//  length = text length or -1=auto ASCIIZ
string::string(const char* text, int length /*= -1*/)
{
	// get string length
	if (text == NULL) length = 0;
	if (length < 0) length = StrLen(text);
	
	// empty text
	if ((length == 0) || (text == NULL))
		this->Attach(&EmptyStringData);
	else
	{	
		// copy text
		if (this->NewBuffer(length)) memcpy(this->m_Data->str, text, length);
	}
}

// addition constructors (ASCIIZ text)
string::string(string text1, string text2)
{
	int len1 = text1.m_Data->len;
	int len2 = text2.m_Data->len;
	if (!this->NewBuffer(len1 + len2)) return;
	memcpy(this->m_Data->str, text1.m_Data->str, len1);
	memcpy(&this->m_Data->str[len1], text2.m_Data->str, len2);
}

string::string(const char* text1, string text2)
{
	int len1 = StrLen(text1);
	int len2 = text2.m_Data->len;
	if (!this->NewBuffer(len1 + len2)) return;
	memcpy(this->m_Data->str, text1, len1);
	memcpy(&this->m_Data->str[len1], text2.m_Data->str, len2);
}

string::string(string text1, const char* text2)
{
	int len1 = text1.m_Data->len;
	int len2 = StrLen(text2);
	if (!this->NewBuffer(len1 + len2)) return;
	memcpy(this->m_Data->str, text1.m_Data->str, len1);
	memcpy(&this->m_Data->str[len1], text2, len2);
}

string::string(char ch, string text)
{
	int len = text.m_Data->len;
	if (!this->NewBuffer(1 + len)) return;
	this->m_Data->str[0] = ch;
	memcpy(&this->m_Data->str[1], text.m_Data->str, len);
}

string::string(string text, char ch)
{
	int len = text.m_Data->len;
	if (!this->NewBuffer(len + 1)) return;
	memcpy(this->m_Data->str, text.m_Data->str, len);
	this->m_Data->str[len] = ch;
}

// set text length (and appropriate text string before write)
Bool string::SetLength(int length)
{
	if (!this->CopyWrite()) return False;
	return this->SetSize(length);
}

// set empty string
void string::Empty()
{
	this->Detach();
	this->Attach(&EmptyStringData);
}

// set character (with index checking, returns False on error)
Bool string::SetAt(int inx, char ch)
{
	if (this->IsNotValid(inx) || (ch == 0)) return False;
	if (!this->CopyWrite()) return False;
	this->m_Data->str[inx] = ch;
	return True;
}

// set text string from another text string
void string::Set(const string& text)
{
	this->DetachAttach(text.m_Data);
}

// set text string from ASCII text (returns False on error)
//  length = text length or -1=auto ASCIIZ
Bool string::Set(const char* text, int length /*= -1*/)
{
	// get string length
	if (text == NULL) length = 0;
	if (length < 0) length = StrLen(text);

	// empty text
	if (length == 0)
		this->Empty();
	else
	{	
		// detach old text
		this->Detach();

		// allocate new buffer
		if (!this->NewBuffer(length)) return False;

		// copy text
		memcpy(this->m_Data->str, text, length);
	}
	return True;
}

// set text string from a character (returns False on error)
Bool string::Set(char ch)
{
	// empty text
	if (ch == 0)
		this->Empty();
	else
	{	
		// detach old text
		this->Detach();

		// allocate new buffer
		if (!this->NewBuffer(1)) return False;

		// set text
		this->m_Data->str[0] = ch;
	}
	return True;
}

// exchange text strings
void string::Exc(string& text)
{
	string_data* data = this->m_Data;
	this->m_Data = text.m_Data;
	text.m_Data = data;
}

// NUL correction - check and truncate text if contains NUL character
void string::CheckNul()
{
	int len = StrLen(this->m_Data->str);
	if (len < this->m_Data->len) this->SetLength(len);
}

// add repeated text string (returns False on error)
Bool string::AddRep(string text, int count)
{
	// add nothing
	if (count <= 0) return True;
	int len2 = text.m_Data->len;
	if (len2 == 0) return True;

	// set text length (and appropriate text string before write)
	int len1 = this->m_Data->len;
	if (!this->SetLength(len1 + len2*count)) return False;

	// copy text
	char* dst = &this->m_Data->str[len1];
	const char* src = text.m_Data->str;
	for (; count > 0; count--)
	{
		memcpy(dst, src, len2);
		dst += len2;
	}
	return True;
}

// add repeated ASCII text (returns False on error)
//  length = text length or -1=auto ASCIIZ
Bool string::AddRep(const char* text, int count, int length /*= -1*/)
{
	// add nothing
	if (count <= 0) return True;
	if (text == NULL) length = 0;
	if (length < 0) length = StrLen(text);
	if (length == 0) return True;

	// set text length (and appropriate text string before write)
	int len = this->m_Data->len;
	if (!this->SetLength(len + length*count)) return False;

	// copy text
	char* dst = &this->m_Data->str[len];
	for (; count > 0; count--)
	{
		memcpy(dst, text, length);
		dst += length;
	}
	return True;
}

// add repeated character (returns False on error)
Bool string::AddRep(char ch, int count)
{
	// add nothing
	if ((count <= 0) || (ch == 0)) return True;

	// set text length (and appropriate text string before write)
	int len = this->m_Data->len;
	if (!this->SetLength(len + count)) return False;

	// write character
	char* dst = &this->m_Data->str[len];
	for (; count > 0; count--) *dst++ = ch;
	return True;
}

// write text into buffer (without terminating zero), returns text length
int string::Write(char* buf, int maxlen /*= BIGINT*/) const
{
	int len = this->m_Data->len;
	if (len > maxlen) len = maxlen;
	if (len < 0) len = 0;
	memcpy(buf, this->m_Data->str, len);
	return len;
}

// write text into buffer with terminating zero (maxlen is max. length
//   without terminating zero), returns text length
int string::WriteZ(char* buf, int maxlen /*= BIGINT*/) const
{
	int len = this->Write(buf, maxlen);
	buf[len] = 0;
	return len;
}

// check ASCII string - all characters must be in range 0..0x7f
Bool string::IsASCII() const
{
	int len = this->m_Data->len;
	const char* src = this->m_Data->str;
	char ch;
	for (; len > 0; len--)
	{
		ch = *src++;
		if ((u8)ch >= (u8)0x80) return False;
	}
	return True;
}

// get number of UTF8 characters in the string
int string::UTF8Count() const
{
	int len = this->m_Data->len;
	const char* src = this->m_Data->str;
	int count = 0;
	for (; len > 0;)
	{
		int bytes = UTF8BytesTab[*src];
		src += bytes;
		len -= bytes;
		count++;
	}
	return count;
}

// get byte index of the UTF8 character in the string, given by its character index (returns -1 if character index is invalid)
int string::UTF8Index(int inx) const
{
	if (inx < 0) return -1; // invalid character index
	if (inx == 0) return 0; // first character
	int len = this->m_Data->len;
	int index = 0;
	const char* src = this->m_Data->str;
	for (; index < len;)
	{
		index += UTF8BytesTab[src[index]];
		inx--;
		if (inx == 0) break;
	}
	if (index > len) index = -1;
	return index;
}

// load Unicode character from UTF8 string and shift character index (character index must be in valid range)
u32 string::GetUTF8(int* index) const
{
	u32 ch = 0;
	int bytes = UTF8BytesTab[this->At(*index)];
	switch (bytes)
	{
	case 6:	// ilegal UTF-8
		ch += (u8)this->At(*index);
		*index += 1;
		ch <<= 6;

	case 5:	// ilegal UTF-8
		ch += (u8)this->At(*index);
		*index += 1;
		ch <<= 6;

	case 4:	ch += (u8)this->At(*index);
		*index += 1;
		ch <<= 6;

	case 3:	ch += (u8)this->At(*index);
		*index += 1;
		ch <<= 6;

	case 2:	ch += (u8)this->At(*index);
		*index += 1;
		ch <<= 6;

	case 1:	ch += (u8)this->At(*index);
		*index += 1;
	}

	ch -= OffsetsFromUTF8[bytes-1];
	return ch;
}

// shift index to next UTF8 character (character index must be in valid range)
void string::NextUTF8(int* index) const
{
	int bytes = UTF8BytesTab[this->At(*index)];
	*index += bytes;
}

// export string in UTF-8 format to buffer in Unicode format (maxlen is max. length
//   without terminating zero), returns text length in characters
int string::UTF8ToUnicode(u32* buf, int maxlen /*= BIGINT*/) const
{
	int count = 0;
	int index = 0;
	int len = this->m_Data->len;
	while ((count < maxlen) && (index < len))
	{
		*buf++ = this->GetUTF8(&index);
		count++;
	}
	return count;
} 

// find character in forward/reverse direction, starting at position (returns index or -1 = not found)
int string::Find(char ch, int pos /*= 0*/) const
{
	// invalid character
	if (ch == 0) return -1;

	// limit minimal starting position
	if (pos < 0) pos = 0;

	// prepare number of remaining characters to search for
	int i = this->m_Data->len - pos;

	// search data
	const char* src = &this->m_Data->str[pos];
	for (; i > 0; i--)
	{
		if (*src == ch) return (int)(src - this->m_Data->str);
		src++;
	}
	return -1;
}

int string::FindRev(char ch, int pos /*= BIGINT*/) const
{
	// invalid character
	if (ch == 0) return -1;

	// limit maximal starting position
	if (pos >= this->m_Data->len) pos = this->m_Data->len - 1;
	if (pos < 0) return -1; // because C++ compiler may not accept pointer out of bounds

	// search data
	const char* src = &this->m_Data->str[pos];
	for (; pos >= 0; pos--)
	{
		if (*src == ch) return pos;
		src--;
	}
	return -1;
}

// find white space (space, tab, newline) in forward/reverse direction, starting at position (returns index or -1 = not found)
int string::FindSpace(int pos /*= 0*/) const
{
	// limit minimal starting position
	if (pos < 0) pos = 0;

	// prepare number of remaining characters to search for
	int i = this->m_Data->len - pos;

	// search data
	const char* src = &this->m_Data->str[pos];
	for (; i > 0; i--)
	{
		if ((u8)*src <= (u8)32) return (int)(src - this->m_Data->str);
		src++;
	}
	return -1;
}

int string::FindSpaceRev(int pos /*= BIGINT*/) const
{
	// limit maximal starting position
	if (pos >= this->m_Data->len) pos = this->m_Data->len - 1;
	if (pos < 0) return -1; // because C++ compiler may not accept pointer out of bounds

	// search data
	const char* src = &this->m_Data->str[pos];
	for (; pos >= 0; pos--)
	{
		if ((u8)*src <= (u8)32) return pos;
		src--;
	}
	return -1;
}

// find non-white space in forward/reverse direction, starting at position (returns index or -1 = not found)
int string::FindNoSpace(int pos /*= 0*/) const
{
	// limit minimal starting position
	if (pos < 0) pos = 0;

	// prepare number of remaining characters to search for
	int i = this->m_Data->len - pos;

	// search data
	const char* src = &this->m_Data->str[pos];
	for (; i > 0; i--)
	{
		if ((u8)*src > (u8)32) return (int)(src - this->m_Data->str);
		src++;
	}
	return -1;
}

int string::FindNoSpaceRev(int pos /*= BIGINT*/) const
{
	// limit maximal starting position
	if (pos >= this->m_Data->len) pos = this->m_Data->len - 1;
	if (pos < 0) return -1; // because C++ compiler may not accept pointer out of bounds

	// search data
	const char* src = &this->m_Data->str[pos];
	for (; pos >= 0; pos--)
	{
		if ((u8)*src > (u8)32) return pos;
		src--;
	}
	return -1;
}

// find text in forward/reverse direction, starting at position (returns index or -1 = not found)
//  length = text length or -1=auto ASCIIZ
int string::Find(const string& text, int pos /*= 0*/) const
{
	// limit start position
	if (pos < 0) pos = 0;

	// check maximal position
	int i = this->m_Data->len;
	if (pos > i) return -1;

	// empty text
	int len = text.m_Data->len;
	if (len == 0) return -1;

	// one-character text
	const char* f = text.m_Data->str;
	char ch = *f;
	if (len == 1) return this->Find(ch, pos);

	// find string
	f++;
	len--;
	const char* src = &this->m_Data->str[pos];
	i = i - pos - len;	// number of search positions
	for (; i >= 0; i--)
	{
		if (*src++ == ch)
		{
			if (memcmp(src, f, len) == 0)
				return (int)(src - this->m_Data->str - 1);
		}
	}
	return -1;
}

int string::FindRev(const string& text, int pos /*= BIGINT*/) const
{
	// limit start position
	int len = text.m_Data->len;
	if (pos > this->m_Data->len - len) pos = this->m_Data->len - len;
	if (pos < 0) return -1; // because C++ compiler may not accept pointer out of bounds

	// empty text
	if (len == 0) return -1;

	// one-character text
	const char* f = text.m_Data->str;
	char ch = *f;
	if (len == 1) return this->FindRev(ch, pos);

	// find string
	f++;
	len--;
	const char* src = &this->m_Data->str[pos];
	for (; pos >= 0; pos--)
	{
		if (*src == ch)
		{
			if (memcmp(src+1, f, len) == 0) return pos;
		}
		src--;
	}
	return -1;
}

int string::Find(const char* text, int pos /*= 0*/, int length /*= -1*/) const
{
	// limit start position
	if (pos < 0) pos = 0;

	// check maximal position
	int i = this->m_Data->len;
	if (pos > i) return -1;

	// get text length
	if (text == NULL) length = 0;
	if (length < 0) length = StrLen(text);

	// empty text
	if (length == 0) return -1;

	// one-character text
	const char* f = text;
	char ch = *f;
	if (length == 1) return this->Find(ch, pos);

	// find string
	f++;
	length--;
	const char* src = &this->m_Data->str[pos];
	i = i - pos - length;	// number of search positions
	for (; i >= 0; i--)
	{
		if (*src++ == ch)
		{
			if (memcmp(src, f, length) == 0)
				return (int)(src - this->m_Data->str - 1);
		}
	}
	return -1;
}

int string::FindRev(const char* text, int pos /*= BIGINT*/, int length /*= -1*/) const
{
	// get text length
	if (text == NULL) length = 0;
	if (length < 0) length = StrLen(text);

	// limit start position
	if (pos > this->m_Data->len - length) pos = this->m_Data->len - length;
	if (pos < 0) return -1; // because C++ compiler may not accept pointer out of bounds

	// empty text
	if (length == 0) return -1;

	// one-character text
	const char* f = text;
	char ch = *f;
	if (length == 1) return this->FindRev(ch, pos);

	// find string
	f++;
	length--;
	const char* src = &this->m_Data->str[pos];
	for (; pos >= 0; pos--)
	{
		if (*src == ch)
		{
			if (memcmp(src+1, f, length) == 0) return pos;
		}
		src--;
	}
	return -1;
}

// find characters from the list in forward/reverse direction, starting at position (returns index or -1 = not found)
//  count = number of characters or -1=auto ASCIIZ
int string::FindList(const string& list, int pos /*= 0*/) const
{
	int i, j, count;
	char ch;
	const char *src, *lst, *ls;

	// limit minimal starting position
	if (pos < 0) pos = 0;

	// prepare number of characters of the list
	count = list.m_Data->len;
	if (count == 0) return -1;

	// prepare number of remaining characters to search for
	i = this->m_Data->len - pos;

	// search data
	src = &this->m_Data->str[pos];
	lst = list.m_Data->str;
	for (; i > 0; i--)
	{
		ch = *src;
		ls = lst;
		for (j = count; j > 0; j--)
		{
			if (ch == *ls) return (int)(src - this->m_Data->str);
			ls++;
		}
		src++;
	}
	return -1;
}

int string::FindListRev(const string& list, int pos /*= BIGINT*/) const
{
	int i, j, count;
	char ch;
	const char *src, *lst, *ls;

	// limit maximal starting position
	if (pos >= this->m_Data->len) pos = this->m_Data->len - 1;
	if (pos < 0) return -1; // because C++ compiler may not accept pointer out of bounds

	// prepare number of characters of the list
	count = list.m_Data->len;
	if (count == 0) return -1;

	// search data
	src = &this->m_Data->str[pos];
	lst = list.m_Data->str;
	for (; pos >= 0; pos--)
	{
		ch = *src;
		ls = lst;
		for (j = count; j > 0; j--)
		{
			if (ch == *ls) return pos;
			ls++;
		}
		src--;
	}
	return -1;
}

int string::FindList(const char* list, int pos /*= 0*/, int count /*= -1*/) const
{
	int i, j;
	char ch;
	const char *src, *lst, *ls;

	// limit minimal starting position
	if (pos < 0) pos = 0;

	// prepare number of characters of the list
	if (list == NULL) count = 0;
	if (count < 0) count = StrLen(list);
	if (count == 0) return -1;

	// prepare number of remaining characters to search for
	i = this->m_Data->len - pos;

	// search data
	src = &this->m_Data->str[pos];
	lst = list;
	for (; i > 0; i--)
	{
		ch = *src;
		ls = lst;
		for (j = count; j > 0; j--)
		{
			if (ch == *ls) return (int)(src - this->m_Data->str);
			ls++;
		}
		src++;
	}
	return -1;
}

int string::FindListRev(const char* list, int pos /*= BIGINT*/, int count /*= -1*/) const
{
	int i, j;
	char ch;
	const char *src, *lst, *ls;

	// limit maximal starting position
	if (pos >= this->m_Data->len) pos = this->m_Data->len - 1;
	if (pos < 0) return -1; // because C++ compiler may not accept pointer out of bounds

	// prepare number of characters of the list
	if (list == NULL) count = 0;
	if (count < 0) count = StrLen(list);
	if (count == 0) return -1;

	// search data
	src = &this->m_Data->str[pos];
	lst = list;
	for (; pos >= 0; pos--)
	{
		ch = *src;
		ls = lst;
		for (j = count; j > 0; j--)
		{
			if (ch == *ls) return pos;
			ls++;
		}
		src--;
	}
	return -1;
}

// find characters except the list in forward/reverse direction, starting at position (returns index or -1 = not found)
//  count = number of characters or -1=auto ASCIIZ
int string::FindExcept(const string& list, int pos /*= 0*/) const
{
	int i, j, count;
	char ch;
	const char *src, *lst, *ls;

	// limit minimal starting position
	if (pos < 0) pos = 0;
	int len = this->m_Data->len;
	if (pos >= len) return -1;

	// prepare number of characters of the list
	count = list.m_Data->len;
	if (count == 0) return pos;

	// prepare number of remaining characters to search for
	i = this->m_Data->len - pos;

	// search data
	src = &this->m_Data->str[pos];
	lst = list.m_Data->str;
	for (; i > 0; i--)
	{
		ch = *src;
		ls = lst;
		for (j = count; j > 0; j--)
		{
			if (ch == *ls) break;
			ls++;
		}
		if (j == 0) return (int)(src - this->m_Data->str);
		src++;
	}
	return -1;
}

int string::FindExceptRev(const string& list, int pos /*= BIGINT*/) const
{
	int i, j, count;
	char ch;
	const char *src, *lst, *ls;

	// limit maximal starting position
	int len = this->m_Data->len;
	if (pos >= len) pos = len - 1;
	if (pos < 0) return -1; // because C++ compiler may not accept pointer out of bounds

	// prepare number of characters of the list
	count = list.m_Data->len;
	if (count == 0) return pos;

	// search data
	src = &this->m_Data->str[pos];
	lst = list.m_Data->str;
	for (; pos >= 0; pos--)
	{
		ch = *src;
		ls = lst;
		for (j = count; j > 0; j--)
		{
			if (ch == *ls) break;
			ls++;
		}
		if (j == 0) return (int)(src - this->m_Data->str);
		src--;
	}
	return -1;
}

int string::FindExcept(const char* list, int pos /*= 0*/, int count /*= -1*/) const
{
	int i, j;
	char ch;
	const char *src, *lst, *ls;

	// limit minimal starting position
	if (pos < 0) pos = 0;
	int len = this->m_Data->len;
	if (pos >= len) return -1;

	// prepare number of characters of the list
	if (list == NULL) count = 0;
	if (count < 0) count = StrLen(list);
	if (count == 0) return pos;

	// prepare number of remaining characters to search for
	i = this->m_Data->len - pos;

	// search data
	src = &this->m_Data->str[pos];
	lst = list;
	for (; i > 0; i--)
	{
		ch = *src;
		ls = lst;
		for (j = count; j > 0; j--)
		{
			if (ch == *ls) break;
			ls++;
		}
		if (j == 0) return (int)(src - this->m_Data->str);
		src++;
	}
	return -1;
}

int string::FindExceptRev(const char* list, int pos /*= BIGINT*/, int count /*= -1*/) const
{
	int i, j;
	char ch;
	const char *src, *lst, *ls;

	// limit maximal starting position
	int len = this->m_Data->len;
	if (pos >= len) pos = len - 1;
	if (pos < 0) return -1; // because C++ compiler may not accept pointer out of bounds

	// prepare number of characters of the list
	if (list == NULL) count = 0;
	if (count < 0) count = StrLen(list);
	if (count == 0) return pos;

	// search data
	src = &this->m_Data->str[pos];
	lst = list;
	for (; pos >= 0; pos--)
	{
		ch = *src;
		ls = lst;
		for (j = count; j > 0; j--)
		{
			if (ch == *ls) break;
			ls++;
		}
		if (j == 0) return (int)(src - this->m_Data->str);
		src--;
	}
	return -1;
}

// substitute text (returns False on memory error)
Bool string::Subst(string find, string subst)
{
	int len;

	// text length
	int findlen = find.m_Data->len;
	if (findlen == 0) return True;
	int substlen = subst.m_Data->len;
	int pos = 0;
	for (;;)
	{
		// find text
		pos = this->Find(find, pos);
		if (pos < 0) break; // text not found

		// copy buffer before write
		if (!this->CopyWrite()) return False;

		// increase buffer size
		len = this->m_Data->len;
		if (substlen > findlen)
		{
			// set new size of the text
			if (!this->SetSize(len + (substlen - findlen))) return False;
		}

		// move second part of the text
		if (substlen != findlen)
		{
			memmove(this->m_Data->str + pos + substlen,
				this->m_Data->str + pos + findlen,
				len - pos - findlen);
		}

		// copy substituted text
		memcpy(this->m_Data->str + pos, subst.m_Data->str, substlen);

		// decrease buffer size
		if (substlen < findlen)
		{
			// set new size of the text
			if (!this->SetSize(len + (substlen - findlen))) return False;
		}

		// shift current position
		pos += substlen;
	}
	return True;
}

Bool string::Subst(const char* find, const char* subst)
{
	int len;

	// text length
	int findlen = StrLen(find);
	if (findlen == 0) return True;
	int substlen = StrLen(subst);
	int pos = 0;
	for (;;)
	{
		// find text
		pos = this->Find(find, pos, findlen);
		if (pos < 0) break; // text not found

		// copy buffer before write
		if (!this->CopyWrite()) return False;

		// increase buffer size
		len = this->m_Data->len;
		if (substlen > findlen)
		{
			// set new size of the text
			if (!this->SetSize(len + (substlen - findlen))) return False;
		}

		// move second part of the text
		if (substlen != findlen)
		{
			memmove(this->m_Data->str + pos + substlen,
				this->m_Data->str + pos + findlen,
				len - pos - findlen);
		}

		// copy substituted text
		memcpy(this->m_Data->str + pos, subst, substlen);

		// decrease buffer size
		if (substlen < findlen)
		{
			// set new size of the text
			if (!this->SetSize(len + (substlen - findlen))) return False;
		}

		// shift current position
		pos += substlen;
	}
	return True;
}

// substitute characters from the list (returns False on memory error)
Bool string::SubstList(string list, string subst)
{
	// number of characters
	int len = list.m_Data->len;
	int len2 = subst.m_Data->len;
	if (len2 < len) len = len2;
	if (len == 0) return True;

	// copy buffer before write
	if (!this->CopyWrite()) return False;

	// replace characters
	int i, j;
	const char* lst = list.m_Data->str;
	const char* sub = subst.m_Data->str;
	char* src = this->m_Data->str;
	char ch;
	for (i = this->m_Data->len; i > 0; i--)
	{
		ch = *src;
		for (j = 0; j < len; j++)
		{
			if (ch == lst[j])
			{
				*src = sub[j];
				break;
			}
		}
		src++;
	}
	return True;
}

Bool string::SubstList(const char* list, const char* subst)
{
	// number of characters
	int len = StrLen(list);
	int len2 = StrLen(subst);
	if (len2 < len) len = len2;
	if (len == 0) return True;

	// copy buffer before write
	if (!this->CopyWrite()) return False;

	// replace characters
	int i, j;
	const char* lst = list;
	const char* sub = subst;
	char* src = this->m_Data->str;
	char ch;
	for (i = this->m_Data->len; i > 0; i--)
	{
		ch = *src;
		for (j = 0; j < len; j++)
		{
			if (ch == lst[j])
			{
				*src = sub[j];
				break;
			}
		}
		src++;
	}
	return True;
}

// unformat text - substitute white characters with one space character (returns False on memory error)
Bool string::Unformat()
{
	// copy buffer before write
	if (!this->CopyWrite()) return False;

	// prepare registers
	int len = this->m_Data->len;
	char* dst = this->m_Data->str;
	char* src = dst;
	Bool lastspace = False; // flag - last character was space
	char ch;

	// substitute
	for (; len > 0; len--)
	{
		ch = *src++;
		if ((u8)ch <= (u8)32) // white character
		{
			// save space character, if last character was not space
			if (!lastspace)
			{
				*dst++ = ' ';
				lastspace = True;
			}
		}
		else
		{
			// save valid character
			*dst++ = ch;
			lastspace = False; // last character was not space
		}
	}

	// resize buffer
	len = (int)(dst - this->m_Data->str);
	if (len != this->m_Data->len) return this->SetLength(len);
	return True;
}

// convert text to uppercase (returns False on memory error)
Bool string::UpperCase()
{
	// copy buffer before write
	if (!this->CopyWrite()) return False;

	// convert text
	int len = this->m_Data->len;
	char* src = this->m_Data->str;
	char ch;
	for (; len > 0; len--)
	{
		ch = *src;
		if ((ch >= 'a') && (ch <= 'z')) *src = ch - 32;
		src++;
	}
	return True;
}

// convert text to lowercase (returns False on memory error)
Bool string::LowerCase()
{
	// copy buffer before write
	if (!this->CopyWrite()) return False;

	// convert text
	int len = this->m_Data->len;
	char* src = this->m_Data->str;
	char ch;
	for (; len > 0; len--)
	{
		ch = *src;
		if ((ch >= 'A') && (ch <= 'Z')) *src = ch + 32;
		src++;
	}
	return True;
}

// convert text to opposite case (returns False on memory error)
Bool string::FlipCase()
{
	// copy buffer before write
	if (!this->CopyWrite()) return False;

	// convert text
	int len = this->m_Data->len;
	char* src = this->m_Data->str;
	char ch;
	for (; len > 0; len--)
	{
		ch = *src;
		if ((ch >= 'A') && (ch <= 'Z'))
			*src = ch + 32;
		else
			if ((ch >= 'a') && (ch <= 'z'))
				*src = ch - 32;
		src++;
	}
	return True;
}

// convert text to wordcase - first letter upper (returns False on memory error)
Bool string::WordCase()
{
	// copy buffer before write
	if (!this->CopyWrite()) return False;

	// convert text
	int len = this->m_Data->len;
	char* src = this->m_Data->str;
	char ch;
	Bool first = True;
	Bool up, low;
	for (; len > 0; len--)
	{
		ch = *src;

		// check letter type
		up = ((ch >= 'A') && (ch <= 'Z'));
		low = ((ch >= 'a') && (ch <= 'z'));

		// alphabetic character
		if (up || low)
		{
			// first letter of the word - convert to upper case
			if (first)
			{
				if (low) *src = ch - 32;
			}

			// next letter of the word - convert to lower case
			else
			{
				if (up) *src = ch + 32;
			}

			// not first letter
			first = False;
		}
		else
			// first letter
			first = True;
		src++;
	}
	return True;
}

// convert text to reverse oder (returns False on memory error)
Bool string::Reverse()
{
	// copy buffer before write
	if (!this->CopyWrite()) return False;

	// reverse the text
	char* src = this->m_Data->str;
	char ch;
	int i = 0; // index of first character
	int j = this->m_Data->len - 1; // index of last character
	for (; i < j;)
	{
		ch = src[i];
		src[i] = src[j];
		src[j] = ch;
		i++;
		j--;
	}
	return True;
}

// get left part of the text (returns empty string on error)
string string::Left(int length /*= 1*/) const
{
	if (length < 0) length = 0;
	int n = this->m_Data->len;
	if (length > n) length = n;
	return string(this->m_Data->str, length);
}

// get right part of the text (returns empty string on error)
string string::Right(int length /*= 1*/) const
{
	if (length < 0) length = 0;
	int n = this->m_Data->len;
	if (length > n) length = n;
	return string(this->m_Data->str + n - length, length);
}

string string::RightFrom(int pos) const
{
	if (pos < 0) pos = 0;
	int n = this->m_Data->len;
	if (pos > n) pos = n;
	return string(this->m_Data->str + pos, n - pos);
}

// get middle part of the text (returns empty string on error)
string string::Mid(int pos, int length /*= 1*/) const
{
	if (pos < 0) pos = 0;
	int n = this->m_Data->len;
	if (pos > n) pos = n;
	if (length < 0) length = 0;
	if ((u32)(pos+length) > (u32)n) length = n - pos;
	return string(this->m_Data->str + pos, length);
}

// insert text (returns False on error)
Bool string::Insert(int pos, string text)
{
	// prepare text length
	int length = text.m_Data->len;
	if (length == 0) return True;

	// limit position
	if (pos < 0) pos = 0;
	int len = this->m_Data->len;
	if (pos > len) pos = len;

	// copy before write and set length of the text
	if (!this->SetLength(len + length)) return False;

	// move rest of the text
	memmove(this->m_Data->str + pos + length, this->m_Data->str + pos, len - pos);

	// copy inserted text
	memcpy(this->m_Data->str + pos, text.m_Data->str, length);

	return True;
}

Bool string::Insert(int pos, const char* text, int length /*= -1*/)
{
	// prepare text length
	if (text == NULL) length = 0;
	if (length < 0) length = StrLen(text);
	if (length == 0) return True;

	// limit position
	if (pos < 0) pos = 0;
	int len = this->m_Data->len;
	if (pos > len) pos = len;

	// copy before write and set length of the text
	if (!this->SetLength(len + length)) return False;

	// move rest of the text
	memmove(this->m_Data->str + pos + length, this->m_Data->str + pos, len - pos);

	// copy inserted text
	memcpy(this->m_Data->str + pos, text, length);

	return True;
}

Bool string::Insert(int pos, char ch)
{
	// limit position
	if (pos < 0) pos = 0;
	int len = this->m_Data->len;
	if (pos > len) pos = len;

	// copy before write and set length of the text
	if (!this->SetLength(len + 1)) return False;

	// move rest of the text
	memmove(this->m_Data->str + pos + 1, this->m_Data->str + pos, len - pos);

	// set new character
	this->m_Data->str[pos] = ch;

	return True;
}

// delete part of the text (returns False on memory error)
Bool string::Delete(int pos, int length /*= 1*/)
{
	// limit position
	if (pos < 0) pos = 0;
	int len = this->m_Data->len;
	if (pos > len) pos = len;

	// limit length
	if (length < 0) length = 0;
	if (length > len) length = len;
	if ((u32)(pos + length) > (u32)len) length = len - pos;
	if (length == 0) return True;

	// copy buffer before write
	if (!this->CopyWrite()) return False;

	// move rest of data
	memmove(this->m_Data->str + pos, this->m_Data->str + pos + length, len - pos - length);

	// set new length of the text
	return this->SetLength(len - length);
}

// find and delete all characters (returns False on memory error)
Bool string::DelChar(char ch)
{
	// empty string
	int len = this->m_Data->len;
	if (len == 0) return True;

	// copy buffer before write
	if (!this->CopyWrite()) return False;

	// delete characters
	char* dst = this->m_Data->str;
	char* src = dst;
	char ch2;
	for (; len > 0; len--)
	{
		ch2 = *src++;
		if (ch != ch2) *dst++ = ch2;
	}

	// resize buffer
	if (src != dst) return this->SetLength(dst - this->m_Data->str);
	return True;
}

// delete characters from the list (returns False on memory error)
Bool string::DelList(string list)
{
	// prepare number of characters in the list
	int count = list.m_Data->len;
	if (count == 0) return True;
	count--; // index of last character in the list

	// empty string
	int len = this->m_Data->len;
	if (len == 0) return True;

	// copy buffer before write
	if (!this->CopyWrite()) return False;

	// delete characters
	char* dst = this->m_Data->str;
	char* src = dst;
	const char* lst = list.m_Data->str;
	char ch;
	int i;
	for (; len > 0; len--)
	{
		ch = *src++;
		for (i = count; i >= 0; i--) if (ch == lst[i]) break;
		if (i < 0) *dst++ = ch;
	}

	// resize buffer
	if (src != dst) return this->SetLength(dst - this->m_Data->str);
	return True;
}

Bool string::DelList(const char* list, int count /*= -1*/)
{
	// prepare number of characters in the list
	if (list == NULL) count = 0;
	if (count < 0) count = StrLen(list);
	if (count == 0) return True;
	count--; // index of last character in the list

	// empty string
	int len = this->m_Data->len;
	if (len == 0) return True;

	// copy buffer before write
	if (!this->CopyWrite()) return False;

	// delete characters
	char* dst = this->m_Data->str;
	char* src = dst;
	const char* lst = list;
	char ch;
	int i;
	for (; len > 0; len--)
	{
		ch = *src++;
		for (i = count; i >= 0; i--) if (ch == lst[i]) break;
		if (i < 0) *dst++ = ch;
	}

	// resize buffer
	if (src != dst) return this->SetLength(dst - this->m_Data->str);
	return True;
}

// delete characters except the list (returns False on memory error)
Bool string::DelExcept(string list)
{
	// prepare number of characters in the list
	int count = list.m_Data->len;
	if (count == 0)
	{
		this->Empty();
		return True;
	}
	count--; // index of last character in the list

	// empty string
	int len = this->m_Data->len;
	if (len == 0) return True;

	// copy buffer before write
	if (!this->CopyWrite()) return False;

	// delete characters
	char* dst = this->m_Data->str;
	char* src = dst;
	const char* lst = list.m_Data->str;
	char ch;
	int i;
	for (; len > 0; len--)
	{
		ch = *src++;
		for (i = count; i >= 0; i--)
		{
			if (ch == lst[i])
			{
				*dst++ = ch;
				break;
			}
		}
	}

	// resize buffer
	if (src != dst) return this->SetLength(dst - this->m_Data->str);
	return True;
}

Bool string::DelExcept(const char* list, int count /*= -1*/)
{
	// prepare number of characters in the list
	if (list == NULL) count = 0;
	if (count < 0) count = StrLen(list);
	if (count == 0)
	{
		this->Empty();
		return True;
	}
	count--; // index of last character in the list

	// empty string
	int len = this->m_Data->len;
	if (len == 0) return True;

	// copy buffer before write
	if (!this->CopyWrite()) return False;

	// delete characters
	char* dst = this->m_Data->str;
	char* src = dst;
	const char* lst = list;
	char ch;
	int i;
	for (; len > 0; len--)
	{
		ch = *src++;
		for (i = count; i >= 0; i--)
		{
			if (ch == lst[i])
			{
				*dst++ = ch;
				break;
			}
		}
	}

	// resize buffer
	if (src != dst) return this->SetLength(dst - this->m_Data->str);
	return True;
}

// find and delete all words (returns False on memory error)
//  length = text length or -1=auto ASCIIZ
Bool string::DelWord(string text)
{
	// prepare text length
	int length = text.m_Data->len;
	if (length == 0) return True;

	// find and delete words
	int pos = 0;
	for (;;)
	{
		pos = this->Find(text, pos);
		if (pos < 0) break;
		if (!this->Delete(pos, length)) return False;
	}
	return True;
}

Bool string::DelWord(const char* text, int length /*= -1*/)
{
	// prepare text length
	if (text == NULL) length = 0;
	if (length < 0) length = StrLen(text);
	if (length == 0) return True;

	// find and delete words
	int pos = 0;
	for (;;)
	{
		pos = this->Find(text, pos, length);
		if (pos < 0) break;
		if (!this->Delete(pos, length)) return False;
	}
	return True;
}

// trim white spaces from the begin od the string (returns False on memory error)
Bool string::TrimLeft()
{
	// find start of the text
	char* src = this->m_Data->str;
	char ch;
	do { ch = *src++; } while ((ch > 0) && (ch <= (char)32));

	// delete white spaces
	int len = (int)(src - this->m_Data->str - 1);
	if (len > 0) return this->Delete(0, len);
	return True;
}

// trim white spaces from the end of the string (returns False on memory error)
Bool string::TrimRight()
{
	// find end of the text
	int len = this->m_Data->len;
	char* src = this->m_Data->str + len;
	for (; len > 0; len--)
	{
		src--;
		if ((u32)*src > (u32)32) break;
	}

	// set new text length
	if (len != this->m_Data->len) return SetLength(len);
	return True;
}

// trim white spaces from begin and end of the string (returns False on memory error)
Bool string::Trim()
{
	return this->TrimRight() && this->TrimLeft();
}

// get number of rows of multi-line text (search LF)
int string::RowsNum() const
{
	const char* src = this->m_Data->str;
	int len = this->m_Data->len;
	int rows = 0;

	// add last row without LF
	if ((len > 0) && (src[len - 1] != CH_LF)) rows = 1;

	// calculate number of rows
	for (; len > 0; len--)
	{
		if (*src++ == CH_LF) rows++;
	}
	return rows;
}

// get one row from multi-line text (search LF, trim CR from ends; returns False on memory error)
string string::Row(int row) const
{
	int len = this->m_Data->len;
	int start = 0;	// start index of current row
	int next = 0;	// start index of next row
	const char* src = this->m_Data->str;

	// find the row
	for (; next < len; next++)
	{
		if (*src == CH_LF)
		{
			row--;
			if (row < 0) break;
			start = next + 1;
		}
		src++;
	}

	// row not found, use empty row
	if (row > 0) start = next;

	// trim CR from start and end
	src = this->m_Data->str;
	while ((start < next) && (src[start] == CH_CR)) start++;
	while ((next > start) && (src[next-1] == CH_CR)) next--;

	// get text
	return this->Mid(start, next-start);
}

#if USE_PRINT		// 1=use Formatted print, 0=not used (lib_print.*)

// set formatted print string into text, with argument list (returns text length or 0 on error)
//  - fmt and destination text cannot be the same
u32 string::PrintArg(const char* fmt, va_list args)
{
	// delete destination text
	this->Empty();

	// prepare text length
	u32 n = NulPrintArg(fmt, args);
	if (n == 0) return 0;

	// create new buffer
	this->Detach(); // detach old text
	if (!this->NewBuffer(n)) return 0;

	// print text
	MemPrintArg(this->m_Data->str, this->m_Data->len+1, fmt, args);

	return n;
}

// set formatted print string into text, with variadic arguments
//  - fmt and destination text cannot be the same
u32 string::Print(const char* fmt, ...)
{
	u32 n;
	va_list args;
	va_start(args, fmt);
	n = this->PrintArg(fmt, args);
	va_end(args);
	return n;
}

// add formatted print string into text, with argument list (returns text length or 0 on error)
//  - fmt and destination text cannot be the same
u32 string::AddPrintArg(const char* fmt, va_list args)
{
	// prepare text length
	u32 n = NulPrintArg(fmt, args);
	if (n == 0) return 0;

	// resize buffer
	u32 oldlen = this->m_Data->len;
	if (!this->SetLength(n + oldlen)) return 0;

	// print text
	MemPrintArg(&this->m_Data->str[oldlen], n+1, fmt, args);

	return n;
}

// add formatted print string into text, with variadic arguments
//  - fmt and destination text cannot be the same
u32 string::AddPrint(const char* fmt, ...)
{
	u32 n;
	va_list args;
	va_start(args, fmt);
	n = this->AddPrintArg(fmt, args);
	va_end(args);
	return n;
}

#endif // USE_PRINT

#if USE_FAT		// 1=use FAT file system, 0=not used (lib_fat.*)

// load file (returns False on error)
//  Loaded text may be shorter than required length.
//  SD card must be mounted.
Bool string::LoadFile(sFile* file, int size)
{
	// create new buffer
	if (size < 0) size = 0;
	this->Detach(); // detach old text
	if (!this->NewBuffer(size)) return False;

	// read file
	int n = FileRead(file, this->m_Data->str, size);
	if (n < size) this->m_Data->str[n] = 0;

	// NUL correction
	this->CheckNul();
	return True;
}

// load file with name (returns False on error)
//  SD card must be mounted.
Bool string::LoadFileName(const char* name)
{
	// empty string
	this->Empty();

	// open file
	sFile file;
	if (!FileOpen(&file, name)) return False;

	// get file size
	int size = file.size;

	// load file
	Bool res = this->LoadFile(&file, size);

	// close file
	FileClose(&file);
	return res;
}

// save file (returns False if text could not be saved entirely)
//  SD card must be mounted.
Bool string::SaveFile(sFile* file) const
{
	// text size
	int size = this->m_Data->len;

	// write file
	int n = FileWrite(file, this->m_Data->str, size);
	return n == size;
}

// save file with name (returns False on error)
//  Existing file will be deleted and new one will be created.
//  SD card must be mounted.
Bool string::SaveFileName(const char* name) const
{
	// delete and create file
	sFile file;
	FileDelete(name);
	if (!FileCreate(&file, name)) return False;

	// save file
	Bool res = this->SaveFile(&file);

	// close file
	FileClose(&file);
	return res;
}

#endif // USE_FAT

// set operators
const string& string::operator= (const string& text)
{
	this->DetachAttach(text.m_Data);
	return *this;
}

const string& string::operator= (const char* text)
{
	this->Set(text);
	return *this;
}

const string& string::operator= (char ch)
{
	this->Set(ch);
	return *this;
}

// add operators
const string& string::operator+= (string text)
{
	this->Add(text);
	return *this;
}

const string& string::operator+= (const char* text)
{
	this->Add(text);
	return *this;
}

const string& string::operator+= (char ch)
{
	this->Add(ch);
	return *this;
}

// compare operators
Bool operator== (const string& text1, const string& text2)
{
	if (text1.m_Data == text2.m_Data) return True;
	int len = text1.m_Data->len;
	return (len == text2.m_Data->len) &&
		(memcmp(text1.m_Data->str, text2.m_Data->str, len) == 0);
}

Bool operator== (const char* text1, const string& text2)
{
	int len = StrLen(text1);
	return (len == text2.m_Data->len) &&
		(memcmp(text1, text2.m_Data->str, len) == 0);
}

Bool operator== (const string& text1, const char* text2)
{
	int len = StrLen(text2);
	return (len == text1.m_Data->len) &&
		(memcmp(text1.m_Data->str, text2, len) == 0);
}

Bool operator== (char ch, const string& text)
{
	return ((text.m_Data->len == 1) && (text.m_Data->str[0] == ch)) ||
		((text.m_Data->len == 0) && (ch == 0));
}

Bool operator== (const string& text, char ch)
{
	return ((text.m_Data->len == 1) && (text.m_Data->str[0] == ch)) ||
		((text.m_Data->len == 0) && (ch == 0));
}

Bool operator!= (const string& text1, const string& text2)
{
	if (text1.m_Data == text2.m_Data) return False;
	int len = text1.m_Data->len;
	return (len != text2.m_Data->len) ||
		(memcmp(text1.m_Data->str, text2.m_Data->str, len) != 0);
}

Bool operator!= (const char* text1, const string& text2)
{
	int len = StrLen(text1);
	return (len != text2.m_Data->len) ||
		(memcmp(text1, text2.m_Data->str, len) != 0);
}

Bool operator!= (const string& text1, const char* text2)
{
	int len = StrLen(text2);
	return (len != text1.m_Data->len) ||
		(memcmp(text1.m_Data->str, text2, len) != 0);
}

Bool operator!= (char ch, const string& text)
{
	int len = text.m_Data->len;	// get text length
	if (ch == 0) return len > 0;	// character 0 is empty string
	if (len != 1) return True;
	return text.m_Data->str[0] != ch;
}

Bool operator!= (const string& text, char ch)
{
	int len = text.m_Data->len;	// get text length
	if (ch == 0) return len > 0;	// character 0 is empty string
	if (len != 1) return True;
	return text.m_Data->str[0] != ch;
}

Bool operator< (const string& text1, const string& text2)
{
	if (text1.m_Data == text2.m_Data) return False;
	int len = (text1.m_Data->len < text2.m_Data->len) ? text1.m_Data->len : text2.m_Data->len;
	int cmp = memcmp(text1.m_Data->str, text2.m_Data->str, len);
	if (cmp < 0) return True;
	if (cmp > 0) return False;
	return text1.m_Data->len < text2.m_Data->len;
}

Bool operator< (const char* text1, const string& text2)
{
	int len1 = StrLen(text1);
	int len = (len1 < text2.m_Data->len) ? len1 : text2.m_Data->len;
	int cmp = memcmp(text1, text2.m_Data->str, len);
	if (cmp < 0) return True;
	if (cmp > 0) return False;
	return len1 < text2.m_Data->len;
}

Bool operator< (const string& text1, const char* text2)
{
	int len2 = StrLen(text2);
	int len = (text1.m_Data->len < len2) ? text1.m_Data->len :len2;
	int cmp = memcmp(text1.m_Data->str, text2, len);
	if (cmp < 0) return True;
	if (cmp > 0) return False;
	return text1.m_Data->len < len2;
}

Bool operator< (char ch, const string& text)
{
	int len = text.m_Data->len;	// get text length
	if (ch == 0) return len > 0;	// character 0 is empty string
	if (len == 0) return False;	// empty string is always False
	if (len > 1) return True;	// long string is always True
	return ch < text.m_Data->str[0]; // compare character
}

Bool operator< (const string& text, char ch)
{
	int len = text.m_Data->len;	// get text length
	if (ch == 0) return False;	// character 0 is empty string, always False ("" < "" false, "x" < "" false)
	if (len == 0) return True;	// empty string is always True ("" < "x" true)
	if (len > 1) return False;	// long string is always False
	return text.m_Data->str[0] < ch; // compare character
}

#endif // USE_TEXT
