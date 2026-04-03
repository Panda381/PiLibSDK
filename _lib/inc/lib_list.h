
// ****************************************************************************
//
//                             Doubly Linked List
//
// ****************************************************************************
// 32-bit pointers are also used on 64-bit system.

#if USE_LIST		// 1=use Doubly Linked List, 0=not used (lib_list.*)

#ifndef _LIB_LIST_H
#define _LIB_LIST_H

// === List entry

// list entry (size 8 bytes ... required by memory allocator)
// - Pointers are u32 even on 64-bit system.
typedef struct
{
	// pointer to next list entry sListEntry*
	u32	next;

	// pointer to previous list entry sListEntry*
	u32	prev;

} sListEntry;
STATIC_ASSERT(sizeof(sListEntry) == 8, "Incorrect sListEntry!");

// get pointer to next list entry
INLINE sListEntry* ListEntryNext(sListEntry* entry) { return (sListEntry*)(uintptr_t)entry->next; }

// get pointer to previous list entry
INLINE sListEntry* ListEntryPrev(sListEntry* entry) { return (sListEntry*)(uintptr_t)entry->prev; }

// retype pointer to the entry to u32 format
INLINE u32 ListEntryTo32(const sListEntry* entry) { return (u32)(uintptr_t)entry; }

// add new list entry after this entry
void ListEntryAddAfter(sListEntry* thisentry, sListEntry* newentry);

// add new list entry before this entry
void ListEntryAddBefore(sListEntry* thisentry, sListEntry* newentry);

// remove entry from the list
void ListEntryRemove(sListEntry* entry);

// get base structure from list entry
//   entry = pointer to list entry
//   base = base structure type
//   member = name of member variable of list entry in its owner
#define BASEFROMLIST(entry, base, member) ((base*)((char*)(entry) - \
		(((char*)&(((base*)NULL)->member)) - (char*)NULL)))

// === Simple list

// simple list
typedef struct
{
	// sentinel node
	sListEntry	head;

} sList;
STATIC_ASSERT(sizeof(sList) == 8, "Incorrect sList!");

// initialize list
void ListInit(sList* list);

// check if list is empty (check if it points to itself)
Bool ListIsEmpty(const sList* list);

// get head of the list - used as stop mark when walking through the list
INLINE sListEntry* ListGetHead(sList* list) { return &list->head; }

// get first list entry
sListEntry* ListGetFirst(sList* list);

// get last list entry
sListEntry* ListGetLast(sList* list);

// add new entry into start of list
void ListAddFirst(sList* list, sListEntry* entry);

// add new entry into end of list
void ListAddLast(sList* list, sListEntry* entry);

// to add new entry after entry in the list - use ListEntryAddAfter
// to add new entry before entry in the list - use ListEntryAddBefore
// to remove entry from the list - use ListEntryRemove

// walking through the simple list in forward direction
//	entry = variable of sListEntry*
//	list = simple list sList*
#define LISTFOREACH(entry, list) \
  for(entry = ListGetFirst(list); entry != ListGetHead(list); entry = ListEntryNext(entry))

// walking through the simple list in backward direction
//	entry = variable of sListEntry*
//	list = simple list sList*
#define LISTFOREACHBACK(entry, list) \
	for(entry = ListGetLast(list); entry != ListGetHead(list); entry = ListEntryPrev(entry))

// safe walking through the simple list (entry can be deleted)
//	entry = variable of sListEntry*
//	next = temporary variable of next sListEntry*
//	list = simple list sList*
#define LISTFOREACHSAFE(entry, next, list) \
  for(entry = ListGetFirst(list), next = ListEntryNext(entry); entry != ListGetHead(list); entry = next, next = ListEntryNext(entry))

// === List with count

// list with count
typedef struct
{
	// list head - sentinel node
	sListEntry	head;

	// number of entries
	int		num;

} sNumList;

// initialize list with count
void NumListInit(sNumList* list);

// check if list with count is empty
Bool NumListIsEmpty(const sNumList* list);

// get head of the list - used as stop mark when walking through the list
INLINE sListEntry* NumListGetHead(sNumList* list) { return &list->head; }

// get first list entry
sListEntry* NumListGetFirst(sNumList* list);

// get last list entry
sListEntry* NumListGetLast(sNumList* list);

// add new entry into start of list with count
void NumListAddFirst(sNumList* list, sListEntry* entry);

// add new entry into end of list with count
void NumListAddLast(sNumList* list, sListEntry* entry);

// add new entry into list with count, after old_entry
void NumListAddAfter(sNumList* list, sListEntry* oldentry, sListEntry* newentry);

// add new entry into list with count, before old_entry
void NumListAddBefore(sNumList* list, sListEntry* oldentry, sListEntry* newentry);

// remove entry from the list with count
void NumListRemove(sNumList* list, sListEntry* entry);

// walking through the list with count in forward direction
//	entry = variable of sListEntry*
//	numlist = list with count sNumList*
#define NUMLISTFOREACH(entry, numlist) \
  for(entry = NumListGetFirst(numlist); entry != NumListGetHead(numlist); entry = ListEntryNext(entry))

// walking through the list with count in backward direction
//	entry = variable of sListEntry*
//	numlist = list with count sNumList*
#define NUMLISTFOREACHBACK(entry, numlist) \
	for(entry = NumListGetLast(numlist); entry != NumListGetHead(numlist); entry = ListEntryPrev(entry))

// safe walking through the list with count (entry can be deleted)
//	entry = variable of sListEntry*
//	next = temporary variable of next sListEntry*
//	numlist = list with count sNumList*
#define NUMLISTFOREACHSAFE(entry, next, numlist) \
  for(entry = NumListGetFirst(numlist), next = ListEntryNext(entry); entry != NumListGetHead(numlist); entry = next, next = ListEntryNext(entry))

#endif // _LIB_LIST_H

#endif // USE_LIST
