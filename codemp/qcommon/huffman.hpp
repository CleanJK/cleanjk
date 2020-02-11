/*
===========================================================================
Copyright (C) 2000 - 2013, Raven Software, Inc.
Copyright (C) 2001 - 2013, Activision, Inc.
Copyright (C) 2013 - 2019, OpenJK contributors
Copyright (C) 2019 - 2020, CleanJoKe contributors

This file is part of the OpenJK source code.

OpenJK is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/

#pragma once

/* This is based on the Adaptive Huffman algorithm described in Sayood's Data
 * Compression book.  The ranks are not actually stored, but implicitly defined
 * by the location of a node within a doubly-linked list */

// ======================================================================
// DEFINE
// ======================================================================

#define NYT HMAX					/* NYT = Not Yet Transmitted */
#define INTERNAL_NODE (HMAX+1)
#define HMAX 256 /* Maximum symbol */

// ======================================================================
// STRUCT
// ======================================================================

typedef struct nodetype {
	struct	nodetype *left, *right, *parent; /* tree structure */
	struct	nodetype *next, *prev; /* doubly-linked list */
	struct	nodetype **head; /* highest ranked node in block */
	int		weight;
	int		symbol;
} node_t;

typedef struct huff_s {
	int			blocNode;
	int			blocPtrs;

	node_t*		tree;
	node_t*		lhead;
	node_t*		ltail;
	node_t*		loc[HMAX+1];
	node_t**	freelist;

	node_t		nodeList[768];
	node_t*		nodePtrs[768];
} huff_t;

typedef struct huffman_s {
	huff_t		compressor;
	huff_t		decompressor;
} huffman_t;

// ======================================================================
// EXTERN VARIABLE
// ======================================================================

extern huffman_t clientHuffTables;

// ======================================================================
// FUNCTION
// ======================================================================

int	Huff_getBit(byte* fout, int* offset);
void Huff_addRef(huff_t* huff, byte ch);
void Huff_Compress(msg_t* buf, int offset);
void Huff_Decompress(msg_t* buf, int offset);
void Huff_Init(huffman_t* huff);
void Huff_offsetReceive(node_t* node, int* ch, byte* fin, int* offset);
void Huff_offsetTransmit(huff_t* huff, int ch, byte* fout, int* offset);
void Huff_putBit(int bit, byte* fout, int* offset);
void MSG_shutdownHuffman();