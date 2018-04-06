#include "os.h"

void init_list_node(struct s_list_node *node)
{
	node->prev = node;
	node->next = node;
}

/*
 * This method is used to determine if the list is empty or not
 */
int list_empty(const struct s_list_node *head)
{
	return (head->next == head);
}

/*
 * This method is used to remove the node from current list
 */
int32_t list_del(struct s_list_node *node)
{
	int32_t val = -1;

	if (node != NULL)
	{
		node->prev->next = node->next;
		node->next->prev = node->prev;

		val = 0;
	}

	return val;
}
/*
 * This method is used to add an new node to head in list
 */
int32_t list_add(struct s_list_node *head, struct s_list_node *node)
{
	int32_t val = -1;

	if (node != NULL && head != NULL)
	{
		node->prev = head->next->prev;
		node->next = head->next;

		head->next->prev = node;
		head->next = node;

		val = 0;
	}

	return val;
}

/*
 * This method is used to add an new node to tail in list
 */
int32_t list_add_tail(struct s_list_node *head, struct s_list_node *node)
{
	int32_t val = -1;

	if (node != NULL && head != NULL)
	{
		node->prev = head->prev;
		node->next = head->prev->next;

		head->prev->next = node;
		head->prev = node;

		val = 0;
	}

	return val;
}
