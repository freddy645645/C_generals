

//#include "unp.h"

#define isEmptyList(x) (x.prev==x.next)
#define notEmptyList(x) (x.prev==x.next)

#define InitList(x) (x.prev=x.next=&x)
#define push_backList(L,x)  (x->prev=L.prev, x->next=&L, L.prev->next=x, L.prev=x)
#define push_frontList(L,x) (x->prev=&L, x->next=L.next, L.next->prev=x, L.next=x)
#define pop_backList(L,x)  (assert(notEmptyList(L)), x=L.prev, L.prev->prev->next=&L, L.prev=L.prev->prev)
#define pop_frontList(L,x) (assert(notEmptyList(L)), x=L.next, L.next->next->prev=&L, L.next=L.next->next)





