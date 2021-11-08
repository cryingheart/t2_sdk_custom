#ifndef _LIST_H
#define _LIST_H

#include <stddef.h>
#include <kernel/cdefs.h>

struct list_head {
    struct list_head *next;
    struct list_head *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define DECLARE_LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

static inline void
list_init(struct list_head *list)
{
    list->next = list;
    list->prev = list;
}

/**
 * __list_add - Insert a new entry between two known consecutive entries.
 */
static inline void __list_add(struct list_head *entry,
			      struct list_head *before,
			      struct list_head *after)
{
    entry->next = after;
    entry->prev = before;
    before->next = entry;
    after->prev = entry;
}

/**
 * Add a new entry
 *
 * Insert a new entry after the specified head.  This is good for implementing
 * stacks.
 *
 * @param head List head to add it after
 * @param entry New entry to be added
 */
static inline void list_add(struct list_head *head, struct list_head *entry)
{
    __list_add(entry, head, head->next);
}

/**
 * Add a new entry
 *
 * Insert a new entry before the specified head.  This is useful for
 * implementing queues.
 *
 * @param head List head to add it before
 * @param entry New entry to be added
 */
static inline void list_add_tail(struct list_head *head, struct list_head *entry)
{
    __list_add(entry, head->prev, head);
}

static inline void
list_del(struct list_head *entry)
{
    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;
    entry->next = entry->prev = NULL;
}

/**
 * Tests whether list is the last entry in list head
 *
 * @param head The head of the list
 * @param entry The entry to test
 */
static inline int
list_is_last(const struct list_head *head, const struct list_head *entry)
{
    return entry->next == head;
}

/**
 * Tests whether a list is empty
 *
 * @param head The list to test.
 */
static inline int
list_empty(const struct list_head *head)
{
    return head->next == head;
}

/**
 * Move 'list' inserting it between 'before' and 'after.
 */
static inline void
list_move(const struct list_head *list, struct list_head *before, struct list_head *after)
{
    struct list_head *first = list->next;
    struct list_head *last = list->prev;

    first->prev = before;
    before->next = first;

    last->next = after;
    after->prev = last;
}

/**
 * Join 'list' and 'head'.
 *
 * head < - list + head
 */
static inline void list_join(const struct list_head *list, struct list_head *head)
{
    if (!list_empty(list))
        list_move(list, head, head->next);
}

/**
 * list_join_tail - join 'list' and 'head'
 *
 * head < - head + list
 */
static inline void list_join_tail(struct list_head *list, struct list_head *head)
{
    if (!list_empty(list))
        list_move(list, head->prev, head);
}

/**
 * list_join_init - join two lists and reinit 'list.
 *
 * head < - list + head
 * list < - (empty)
 */
static inline void list_join_init(struct list_head *list, struct list_head *head)
{
    if (!list_empty(list)) {
        list_move(list, head, head->next);
        list_init(list);
    }
}

/**
 * Get the struct for this entry
 *
 * @param ptr   The &struct list_entry pointer.
 * @param type  The type of the struct this is embedded in.
 * @param member The name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/**
 * Iterate over a list
 *
 * @param pos	The &struct list_entry to use as a loop cursor.
 * @param head	The head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * Iterate over a list backwards
 *
 * @param pos The &struct list_entry to use as a loop cursor.
 * @param head The head for your list.
 */
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * Iterate over a list safe against removal of list entry
 *
 * @param pos The &struct list_entry to use as a loop cursor.
 * @param n Another &struct list_entry to use as temporary storage
 * @param head The head for your list.
 */
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/**
 * Iterate over list of given type
 *
 * @param pos The type * to use as a loop cursor.
 * @param head The head for your list.
 * @param member Name of the list_struct within the struct.
 */
#define list_for_each_entry(pos, head, member)				\
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     &pos->member != (head);                                	\
	     pos = list_entry(pos->member.next, typeof(*pos), member))

/**
 * Iterate backwards over list of given type.
 *
 * @param pos The type * to use as a loop cursor.
 * @param head The head for your list.
 * @param member The name of the list_struct within the struct.
 */
#define list_for_each_entry_reverse(pos, head, member)			\
	for (pos = list_entry((head)->prev, typeof(*pos), member);	\
	     &pos->member != (head);                                    \
	     pos = list_entry(pos->member.prev, typeof(*pos), member))

#endif /* _LIST_H */

