#ifndef _LIST_H
#define _LIST_H

/*
 * Simple doubly circule linked list implementation.
 */

struct s_list_node
{
	struct s_list_node *prev;
	struct s_list_node *next;
};

void init_list_node(struct s_list_node *node);
int list_empty(const struct s_list_node *head);
int32_t list_del(struct s_list_node *node);
int32_t list_add(struct s_list_node *head, struct s_list_node *node);
int32_t list_add_tail(struct s_list_node *head, struct s_list_node *node);

#define container_of(ptr, type, member) ({                      \
	const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
	(type *)( (char *)__mptr - offsetof(type,member) );})

/**
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/**
 * list_first_entry - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

/**
 * list_for_each - iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 *  list_for_each_entry	-	iterate over list of given type
 *  @pos:	the type * to use as a loop cursor.
 *  @head:	the head for your list.
 *  @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry(pos, head, member)				\
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
		&pos->member != (head);					\
		pos = list_entry(pos->member.next, typeof(*pos), member))

#endif
