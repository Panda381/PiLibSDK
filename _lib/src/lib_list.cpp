
// ****************************************************************************
//
//                             Doubly Linked List
//
// ****************************************************************************

#include "../../includes.h"	// includes

#if USE_LIST		// 1=use Doubly Linked List, 0=not used (lib_list.*)

// === List entry

// add new list entry after this entry
void ListEntryAddAfter(sListEntry* thisentry, sListEntry* newentry)
{
	sListEntry* next = ListEntryNext(thisentry);
	next->prev = ListEntryTo32(newentry);
	newentry->next = ListEntryTo32(next);
	newentry->prev = ListEntryTo32(thisentry);
	thisentry->next = ListEntryTo32(newentry);
}

// add new list entry before this entry
void ListEntryAddBefore(sListEntry* thisentry, sListEntry* newentry)
{
	sListEntry* prev = ListEntryPrev(thisentry);
	prev->next = ListEntryTo32(newentry);
	newentry->prev = ListEntryTo32(prev);
	newentry->next = ListEntryTo32(thisentry);
	thisentry->prev = ListEntryTo32(newentry);
}

// remove entry from the list
void ListEntryRemove(sListEntry* entry)
{
	sListEntry* next = ListEntryNext(entry);
	sListEntry* prev = ListEntryPrev(entry);
	next->prev = ListEntryTo32(prev);
	prev->next = ListEntryTo32(next);
}

// === Simple list

// initialize simple list
void ListInit(sList* list)
{
	list->head.next = ListEntryTo32(&list->head);
	list->head.prev = ListEntryTo32(&list->head);
}

// check if list is empty (check if it points to itself)
Bool ListIsEmpty(const sList* list)
{
	return list->head.next == ListEntryTo32(&list->head);
}

// get first list entry
sListEntry* ListGetFirst(sList* list)
{
	return ListEntryNext(&list->head);
}

// get last list entry
sListEntry* ListGetLast(sList* list)
{
	return ListEntryPrev(&list->head);
}

// add new entry into start of list
void ListAddFirst(sList* list, sListEntry* entry)
{
	ListEntryAddAfter(&list->head, entry);
}	

// add new entry into end of list
void ListAddLast(sList* list, sListEntry* entry)
{
	ListEntryAddBefore(&list->head, entry);
}

// === List with count

// initialize list with count
void NumListInit(sNumList* list)
{
	list->head.next = ListEntryTo32(&list->head);
	list->head.prev = ListEntryTo32(&list->head);
	list->num = 0;
}

// check if list with count is empty
Bool NumListIsEmpty(const sNumList* list)
{
	return list->num == 0;
}

// get first list entry
sListEntry* NumListGetFirst(sNumList* list)
{
	return ListEntryNext(&list->head);
}

// get last list entry
sListEntry* NumListGetLast(sNumList* list)
{
	return ListEntryPrev(&list->head);
}

// add new entry into start of list with count
void NumListAddFirst(sNumList* list, sListEntry* entry)
{
	ListEntryAddAfter(&list->head, entry);
	list->num++;
}	

// add new entry into end of list with count
void NumListAddLast(sNumList* list, sListEntry* entry)
{
	ListEntryAddBefore(&list->head, entry);
	list->num++;
}

// add new entry into list with count, after old_entry
void NumListAddAfter(sNumList* list, sListEntry* oldentry, sListEntry* newentry)
{
	ListEntryAddAfter(oldentry, newentry);
	list->num++;
}

// add new entry into list with count, before old_entry
void NumListAddBefore(sNumList* list, sListEntry* oldentry, sListEntry* newentry)
{
	ListEntryAddBefore(oldentry, newentry);
	list->num++;
}

// remove entry from the list
void NumListRemove(sNumList* list, sListEntry* entry)
{
	ListEntryRemove(entry);
	list->num--;
}

#endif // USE_LIST
