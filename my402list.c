#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include "cs402.h"

#include "my402list.h"


/* ----------------------- main() ----------------------- */

//    My402List list;
//    memset(&list, 0, sizeof(My402List));
//    (void)My402ListInit(&list);

//1
int  My402ListLength(My402List* lp) {
    return lp->num_members;
}
//2
int  My402ListEmpty(My402List* lp) {
    if(lp->num_members == 0) {
	return TRUE;
    }
    return FALSE;
}

//5
//If list is empty, just add obj to the list. Otherwise add obj after Last()
//PRE: lp is a valid list
//so problems happen on ep
int  My402ListAppend(My402List* lp, void* obj) {
    
    //allocate & initialize memory
    My402ListElem *ep;

    //if((memset(&elmt, 0, sizeof(My402ListElem))) == NULL) {
    if((ep = malloc(sizeof(My402ListElem))) == NULL) {
	(void)fprintf(stderr, "ERROR: memset failed");
	return FALSE;
    }

    ep->obj = obj;

    if(My402ListEmpty(lp)) {
	ep->prev = &lp->anchor;
	ep->next = &lp->anchor;
	lp->anchor.prev = ep;
	lp->anchor.next = ep;
    }
    else {
	// get the pointer to the last My402ListElem
	// elmt beacomes the new Last()

	My402ListElem *lastp = My402ListLast(lp);
	My402ListElem *anchorp = &(lp->anchor);

	ep->next = anchorp;
	ep->prev = lastp;

	lastp->next = ep;
	anchorp->prev = ep;
    }

    lp->num_members++;
    return TRUE;
}

//6
//If list is empty, just add obj to the list, Otherwise add obj before First()
int  My402ListPrepend(My402List* lp, void* obj) {
    //allocate & initialize memory
    My402ListElem *ep;

    if((ep = malloc(sizeof(My402ListElem))) == NULL) {
	(void)fprintf(stderr, "ERROR: memset failed");
	return FALSE;
    }

    ep->obj = obj;

    if(My402ListEmpty(lp)) {
	ep->prev = &lp->anchor;
	ep->next = &lp->anchor;
	lp->anchor.prev = ep;
	lp->anchor.next = ep;
    }
    else {

	My402ListElem *firstp = My402ListFirst(lp);
	My402ListElem *anchorp = &(lp->anchor);

	ep->prev = anchorp;
	ep->next = firstp;

	firstp->prev = ep;
	anchorp->next = ep;
    }

    lp->num_members++;
    return TRUE;
}

//7
//Unlink and delete elem from the list
//do not delete the object pointed by elem
//do not check if elem is on the list
//PRE: lp is not empty, ep is on the list
void My402ListUnlink(My402List* lp, My402ListElem* ep) {
    if(My402ListEmpty(lp)) {
	return;
    }
    //pointer to the previous element
    My402ListElem* prevp = ep->prev;
    //pointer to the next element
    My402ListElem* nextp = ep->next;
    //seperate element
    if(prevp == NULL || nextp == NULL) {
        return;
    }

    prevp->next = nextp;
    nextp->prev = prevp;
    free(ep);
    lp->num_members--;
}

//8
//Unlink and delete all elements from the list and make the list empty
//do not delete the objects pointed to be the list elements
void My402ListUnlinkAll(My402List* lp) {
    while(My402ListEmpty(lp) == FALSE) {
	My402ListUnlink(lp, My402ListFirst(lp));
    }
}

//13
//Insert obj between elem and elem->next.
//if elem is NULL, same as Append()
//return TRUE successfully, FALSE otherwise
//do not check elem is on the list 
//PRE: lp is not empty, ep is on the list
int  My402ListInsertAfter(My402List* lp, void* obj, My402ListElem* ep) {
    if(ep == NULL) {
	return My402ListAppend(lp, obj);
    }

    //allocate & initialize memory
    My402ListElem *newp;

    if((newp = malloc(sizeof(My402ListElem))) == NULL) {
	(void)fprintf(stderr, "ERROR: InsertAfter malloc failed");
	return FALSE;
    }

    newp->obj = obj;
    newp->next = ep->next;
    newp->prev = ep;

    ep->next->prev = newp;
    ep->next = newp;
    lp->num_members++;
    return TRUE;
}

//14
//Insert obj between elem and elem->prev.
//if elem is NULL, same as Prepend()
//return TRUE successfully, FALSE otherwise
//do not check elem is on the list 
//PRE: lp is not empty, ep is on the list
int  My402ListInsertBefore(My402List* lp, void* obj, My402ListElem* ep) {
    if(ep == NULL) {
	return My402ListPrepend(lp, obj);
    }

    //allocate & initialize memory
    My402ListElem *newp;

    if((newp = malloc(sizeof(My402ListElem))) == NULL) {
	(void)fprintf(stderr, "ERROR: InsertBefore malloc failed");
	return FALSE;
    }

    newp->obj = obj;
    newp->next = ep;
    newp->prev = ep->prev;

    ep->prev->next = newp;
    ep->prev = newp;
    lp->num_members++;
    return TRUE;
}

//4
//return the first list element or NULL if the list is empty
My402ListElem *My402ListFirst(My402List* lp) {
    if(My402ListEmpty(lp)) {
	return NULL;
    }
    return lp->anchor.next;
}
//3
//return the last list element or NULL if the list is empty
My402ListElem *My402ListLast(My402List* lp) {
    if(My402ListEmpty(lp)) {
	return NULL;
    }
    return lp->anchor.prev;
}
//12
//Return elem->next or NULL if elem is the last item on the list
//do not check if elem is on the list
//PRE: lp is not empty, ep is on the list
My402ListElem *My402ListNext(My402List* lp, My402ListElem* ep) {
    //pointer to the last element
    if(ep == My402ListLast(lp)) {
	return NULL;
    }
    return ep->next;
}

//11
//Return elem->prev or NULL if elem is the first item on the list
//do not check if elem is on the list
//PRE: lp is not empty, ep is on the list
My402ListElem *My402ListPrev(My402List* lp, My402ListElem* ep) {

    //pointer to the first element
    if(ep == My402ListFirst(lp)) {
	return NULL;
    }
    return ep->prev;

}

//10
//Return the element such that elem->obj == obj
//Return NULL if no such element 
My402ListElem *My402ListFind(My402List* lp, void* obj) {

    //start from the first element
    My402ListElem *ep = My402ListFirst(lp);
    while(ep != &lp->anchor) {
	if(ep->obj == obj) {
	    return ep;
	}
	ep = ep->next;
    }
    return NULL;
}

//9
//Initialize the list into an empty list. Return TRUE if well
//return FALSE if there's an errer initializing the list
//could be an invalid list
int My402ListInit(My402List* lp) {
    if(lp == NULL) {
	return FALSE;
    }
    My402ListUnlinkAll(lp);

    lp->anchor.obj = NULL;

    return TRUE;
}
