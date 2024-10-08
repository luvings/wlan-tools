/*
 * lib/object.c		Generic Cacheable Object
 *
 *	This library is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Lesser General Public
 *	License as published by the Free Software Foundation version 2.1
 *	of the License.
 *
 * Copyright (c) 2003-2008 Thomas Graf <tgraf@suug.ch>
 */

/**
 * @ingroup cache
 * @defgroup object Object
 * @{
 */

#include <netlink-local.h>
#include <netlink/netlink.h>
#include <netlink/cache.h>
#include <netlink/object.h>
#include <netlink/utils.h>

static inline struct nl_object_ops *obj_ops(struct nl_object *obj)
{
	if (!obj->ce_ops)
		BUG();

	return obj->ce_ops;
}

/**
 * @name Object Creation/Deletion
 * @{
 */

/**
 * Allocate a new object of kind specified by the operations handle
 * @arg ops		cache operations handle
 * @return The new object or NULL
 */
struct nl_object *nl_object_alloc(struct nl_object_ops *ops)
{
	struct nl_object *new;

	if (ops->oo_size < sizeof(*new))
		BUG();

	new = calloc(1, ops->oo_size);

	if (!new)
		return NULL;

	new->ce_refcnt = 1;
	nl_init_list_head(&new->ce_list);

	new->ce_ops = ops;

	if (ops->oo_constructor)
		ops->oo_constructor(new);

	NL_DBG(4, "Allocated new object %p\n", new);

	return new;
}

/**
 * Allocate a new object of kind specified by the name
 * @arg kind		name of object type
 * @return The new object or nULL
 */
int nl_object_alloc_name(const char *kind, struct nl_object **result)
{
	struct nl_cache_ops *ops;

	ops = nl_cache_ops_lookup(kind);

	if (!ops)
		return -NLE_OPNOTSUPP;

	if (!(*result = nl_object_alloc(ops->co_obj_ops)))
		return -NLE_NOMEM;

	return 0;
}

struct nl_derived_object {
	NLHDR_COMMON
	char data;
};

/**
 * Allocate a new object and copy all data from an existing object
 * @arg obj		object to inherite data from
 * @return The new object or NULL.
 */
struct nl_object *nl_object_clone(struct nl_object *obj)
{
	struct nl_object *new;
	struct nl_object_ops *ops = obj_ops(obj);
	int doff = offsetof(struct nl_derived_object, data);
	int size;

	new = nl_object_alloc(ops);

	if (!new)
		return NULL;

	size = ops->oo_size - doff;

	if (size < 0)
		BUG();

	new->ce_ops = obj->ce_ops;
	new->ce_msgtype = obj->ce_msgtype;
	new->ce_mask = obj->ce_mask;

	if (size)
		memcpy((void *)new + doff, (void *)obj + doff, size);

	if (ops->oo_clone) {
		if (ops->oo_clone(new, obj) < 0) {
			nl_object_free(new);
			return NULL;
		}

	} else if (size && ops->oo_free_data)
		BUG();

	return new;
}

/**
 * Free a cacheable object
 * @arg obj		object to free
 *
 * @return 0 or a negative error code.
 */
void nl_object_free(struct nl_object *obj)
{
	struct nl_object_ops *ops = obj_ops(obj);

	if (obj->ce_refcnt > 0)
		NL_DBG(1, "Warning: Freeing object in use...\n");

	if (obj->ce_cache)
		nl_cache_remove(obj);

	if (ops->oo_free_data)
		ops->oo_free_data(obj);

	free(obj);

	NL_DBG(4, "Freed object %p\n", obj);
}

/** @} */

/**
 * @name Reference Management
 * @{
 */

/**
 * Acquire a reference on a object
 * @arg obj		object to acquire reference from
 */
void nl_object_get(struct nl_object *obj)
{
	obj->ce_refcnt++;
	NL_DBG(4, "New reference to object %p, total %d\n",
	       obj, obj->ce_refcnt);
}

/**
 * Release a reference from an object
 * @arg obj		object to release reference from
 */
void nl_object_put(struct nl_object *obj)
{
	if (!obj)
		return;

	obj->ce_refcnt--;
	NL_DBG(4, "Returned object reference %p, %d remaining\n",
	       obj, obj->ce_refcnt);

	if (obj->ce_refcnt < 0)
		BUG();

	if (obj->ce_refcnt <= 0)
		nl_object_free(obj);
}

/**
 * Check whether this object is used by multiple users
 * @arg obj		object to check
 * @return true or false
 */
int nl_object_shared(struct nl_object *obj)
{
	return obj->ce_refcnt > 1;
}

/** @} */

/**
 * @name Marks
 * @{
 */

/**
 * Add mark to object
 * @arg obj		Object to mark
 */
void nl_object_mark(struct nl_object *obj)
{
	obj->ce_flags |= NL_OBJ_MARK;
}

/**
 * Remove mark from object
 * @arg obj		Object to unmark
 */
void nl_object_unmark(struct nl_object *obj)
{
	obj->ce_flags &= ~NL_OBJ_MARK;
}

/**
 * Return true if object is marked
 * @arg obj		Object to check
 * @return true if object is marked, otherwise false
 */
int nl_object_is_marked(struct nl_object *obj)
{
	return (obj->ce_flags & NL_OBJ_MARK);
}

/** @} */

/**
 * @name Utillities
 * @{
 */

/**
 * Dump this object according to the specified parameters
 * @arg obj		object to dump
 * @arg params		dumping parameters
 */
void nl_object_dump(struct nl_object *obj, struct nl_dump_params *params)
{
	dump_from_ops(obj, params);
}

/**
 * Check if the identifiers of two objects are identical
 * @arg a		an object
 * @arg b		another object of same type
 *
 * @return true if both objects have equal identifiers, otherwise false.
 */
int nl_object_identical(struct nl_object *a, struct nl_object *b)
{
	struct nl_object_ops *ops = obj_ops(a);
	int req_attrs;

	/* Both objects must be of same type */
	if (ops != obj_ops(b))
		return 0;

	req_attrs = ops->oo_id_attrs;

	if (req_attrs == ~0)
		req_attrs = a->ce_mask & b->ce_mask;

	/* Both objects must provide all required attributes to uniquely
	 * identify an object */
	if ((a->ce_mask & req_attrs) != req_attrs ||
	                (b->ce_mask & req_attrs) != req_attrs)
		return 0;

	/* Can't judge unless we can compare */
	if (ops->oo_compare == NULL)
		return 0;

	return !(ops->oo_compare(a, b, req_attrs, 0));
}

/**
 * Compute bitmask representing difference in attribute values
 * @arg a		an object
 * @arg b		another object of same type
 *
 * The bitmask returned is specific to an object type, each bit set represents
 * an attribute which mismatches in either of the two objects. Unavailability
 * of an attribute in one object and presence in the other is regarded a
 * mismatch as well.
 *
 * @return Bitmask describing differences or 0 if they are completely identical.
 */
uint32_t nl_object_diff(struct nl_object *a, struct nl_object *b)
{
	struct nl_object_ops *ops = obj_ops(a);

	if (ops != obj_ops(b) || ops->oo_compare == NULL)
		return UINT_MAX;

	return ops->oo_compare(a, b, ~0, 0);
}

/**
 * Match a filter against an object
 * @arg obj		object to check
 * @arg filter		object of same type acting as filter
 *
 * @return 1 if the object matches the filter or 0
 *           if no filter procedure is available or if the
 *           filter does not match.
 */
int nl_object_match_filter(struct nl_object *obj, struct nl_object *filter)
{
	struct nl_object_ops *ops = obj_ops(obj);

	if (ops != obj_ops(filter) || ops->oo_compare == NULL)
		return 0;

	return !(ops->oo_compare(obj, filter, filter->ce_mask,
	                         LOOSE_COMPARISON));
}

/**
 * Convert bitmask of attributes to a character string
 * @arg obj		object of same type as attribute bitmask
 * @arg attrs		bitmask of attribute types
 * @arg buf		destination buffer
 * @arg len		length of destination buffer
 *
 * Converts the bitmask of attribute types into a list of attribute
 * names separated by comas.
 *
 * @return destination buffer.
 */
char *nl_object_attrs2str(struct nl_object *obj, uint32_t attrs,
                          char *buf, size_t len)
{
	struct nl_object_ops *ops = obj_ops(obj);

	if (ops->oo_attrs2str != NULL)
		return ops->oo_attrs2str(attrs, buf, len);

	else {
		memset(buf, 0, len);
		return buf;
	}
}

/**
 * Return list of attributes present in an object
 * @arg obj		an object
 * @arg buf		destination buffer
 * @arg len		length of destination buffer
 *
 * @return destination buffer.
 */
char *nl_object_attr_list(struct nl_object *obj, char *buf, size_t len)
{
	return nl_object_attrs2str(obj, obj->ce_mask, buf, len);
}

/** @} */

/**
 * @name Attributes
 * @{
 */

int nl_object_get_refcnt(struct nl_object *obj)
{
	return obj->ce_refcnt;
}

struct nl_cache *nl_object_get_cache(struct nl_object *obj)
{
	return obj->ce_cache;
}

/** @} */

/** @} */
