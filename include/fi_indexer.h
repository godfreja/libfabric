/*
 * Copyright (c) 2011 Intel Corporation.  All rights reserved.
 * Copyright (c) 2016 Cisco Systems, Inc .  All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#if !defined(INDEXER_H)
#define INDEXER_H

#include "config.h"

#include <sys/types.h>

/*
 * Indexer - to find a structure given an index.  Synchronization
 * must be provided by the caller.  Caller must initialize the
 * indexer by setting free_list and size to 0.
 */

union ofi_idx_entry {
	void *item;
	int   next;
};

#define OFI_IDX_INDEX_BITS 16
#define OFI_IDX_ENTRY_BITS 10
#define OFI_IDX_ENTRY_SIZE (1 << OFI_IDX_ENTRY_BITS)
#define OFI_IDX_ARRAY_SIZE (1 << (OFI_IDX_INDEX_BITS - OFI_IDX_ENTRY_BITS))
#define OFI_IDX_MAX_INDEX  ((1 << OFI_IDX_INDEX_BITS) - 1)

struct indexer
{
	union ofi_idx_entry *array[OFI_IDX_ARRAY_SIZE];
	int		 free_list;
	int		 size;
};

#define ofi_idx_array_index(index) (index >> OFI_IDX_ENTRY_BITS)
#define ofi_idx_entry_index(index) (index & (OFI_IDX_ENTRY_SIZE - 1))

int ofi_idx_insert(struct indexer *idx, void *item);
void *ofi_idx_remove(struct indexer *idx, int index);
void ofi_idx_replace(struct indexer *idx, int index, void *item);
void ofi_idx_reset(struct indexer *idx);

static inline void *ofi_idx_at(struct indexer *idx, int index)
{
	return (idx->array[ofi_idx_array_index(index)] + ofi_idx_entry_index(index))->item;
}

/*
 * Index map - associates a structure with an index.  Synchronization
 * must be provided by the caller.  Caller must initialize the
 * index map by setting it to 0.
 */

struct index_map
{
	void **array[OFI_IDX_ARRAY_SIZE];
	int count[OFI_IDX_ARRAY_SIZE];
};

int ofi_idm_set(struct index_map *idm, int index, void *item);
void *ofi_idm_clear(struct index_map *idm, int index);
void ofi_idm_reset(struct index_map *idm);

static inline void *ofi_idm_at(struct index_map *idm, int index)
{
	void **entry;
	entry = idm->array[ofi_idx_array_index(index)];
	return entry[ofi_idx_entry_index(index)];
}

static inline void *ofi_idm_lookup(struct index_map *idm, int index)
{
	return ((index <= OFI_IDX_MAX_INDEX) && idm->array[ofi_idx_array_index(index)]) ?
		ofi_idm_at(idm, index) : NULL;
}

#endif /* INDEXER_H */
