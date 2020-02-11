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

// Script Command Sequences

//	-- jweier

// this include must remain at the top of every Icarus CPP file
#include "icarus/icarus.hpp"

#include <cassert>

CSequence::CSequence( void )
{
	m_numCommands	= 0;
	m_flags			= 0;
	m_iterations	= 1;

	m_parent		= nullptr;
	m_return		= nullptr;
}

CSequence::~CSequence( void )
{
	Delete();
}

CSequence *CSequence::Create( void )
{
	CSequence *seq = new CSequence;

	//TODO: Emit warning
	assert(seq);
	if ( seq == nullptr )
		return nullptr;

	seq->SetFlag( SQ_COMMON );

	return seq;
}

void CSequence::Delete( void )
{
	block_l::iterator	bi;
	sequence_l::iterator si;

	//Notify the parent of the deletion
	if ( m_parent )
	{
		m_parent->RemoveChild( this );
	}

	//Clear all children
	if ( m_children.size() > 0 )
	{
		/*for ( iterSeq = m_childrenMap.begin(); iterSeq != m_childrenMap.end(); iterSeq++ )
		{
			(*iterSeq).second->SetParent( nullptr );
		}*/

		for ( si = m_children.begin(); si != m_children.end(); ++si )
		{
			(*si)->SetParent( nullptr );
		}
	}
	m_children.clear();

	//Clear all held commands
	for ( bi = m_commands.begin(); bi != m_commands.end(); ++bi )
	{
		delete (*bi);	//Free() handled internally
	}

	m_commands.clear();
}

void CSequence::AddChild( CSequence *child )
{
	assert( child );
	if ( child == nullptr )
		return;

	m_children.insert( m_children.end(), child );
}

void CSequence::RemoveChild( CSequence *child )
{
	assert( child );
	if ( child == nullptr )
		return;

	//Remove the child
	m_children.remove( child );
}

bool CSequence::HasChild( CSequence *sequence )
{
	sequence_l::iterator	ci;

	for ( ci = m_children.begin(); ci != m_children.end(); ++ci )
	{
		if ( (*ci) == sequence )
			return true;

		if ( (*ci)->HasChild( sequence ) )
			return true;
	}

	return false;
}

void CSequence::SetParent( CSequence *parent )
{
	m_parent = parent;

	if ( parent == nullptr )
		return;

	//Inherit the parent's properties (this avoids messy tree walks later on)
	if ( parent->m_flags & SQ_RETAIN )
		m_flags |= SQ_RETAIN;

	if ( parent->m_flags & SQ_PENDING )
		m_flags |= SQ_PENDING;
}

CBlock *CSequence::PopCommand( int type )
{
	CBlock	*command = nullptr;

	//Make sure everything is ok
	assert( (type == POP_FRONT) || (type == POP_BACK) );

	if ( m_commands.empty() )
		return nullptr;

	switch ( type )
	{
	case POP_FRONT:

		command = m_commands.front();
		m_commands.pop_front();
		m_numCommands--;

		return command;
		break;

	case POP_BACK:

		command = m_commands.back();
		m_commands.pop_back();
		m_numCommands--;

		return command;
		break;
	}

	//Invalid flag
	return nullptr;
}

int CSequence::PushCommand( CBlock *block, int type )
{
	//Make sure everything is ok
	assert( (type == PUSH_FRONT) || (type == PUSH_BACK) );
	assert( block );

	switch ( type )
	{
	case PUSH_FRONT:

		m_commands.push_front( block );
		m_numCommands++;

		return true;
		break;

	case PUSH_BACK:

		m_commands.push_back( block );
		m_numCommands++;

		return true;
		break;
	}

	//Invalid flag
	return false;
}

void CSequence::SetFlag( int flag )
{
	m_flags |= flag;
}

void CSequence::RemoveFlag( int flag, bool children )
{
	m_flags &= ~flag;

	if ( children )
	{
		sequence_l::iterator	si;

		for ( si = m_children.begin(); si != m_children.end(); ++si )
		{
			(*si)->RemoveFlag( flag, true );
		}
	}
}

int CSequence::HasFlag( int flag )
{
	return (m_flags & flag);
}

void CSequence::SetReturn ( CSequence *sequence )
{
	assert( sequence != this );
	m_return = sequence;
}

CSequence *CSequence::GetChildByIndex( int iIndex )
{
	if ( iIndex < 0 || iIndex >= (int)m_children.size() )
		return nullptr;

	sequence_l::iterator iterSeq = m_children.begin();
	for ( int i = 0; i < iIndex; i++  )
	{
		++iterSeq;
	}
	return (*iterSeq);
}

int CSequence::SaveCommand( CBlock *block )
{
	unsigned char	flags;
	int				numMembers, bID, size;
	CBlockMember	*bm;

	//Save out the block ID
	bID = block->GetBlockID();
	(m_owner->GetInterface())->I_WriteSaveData( INT_ID('B','L','I','D'), &bID, sizeof ( bID ) );

	//Save out the block's flags
	flags = block->GetFlags();
	(m_owner->GetInterface())->I_WriteSaveData( INT_ID('B','F','L','G'), &flags, sizeof ( flags ) );

	//Save out the number of members to read
	numMembers = block->GetNumMembers();
	(m_owner->GetInterface())->I_WriteSaveData( INT_ID('B','N','U','M'), &numMembers, sizeof ( numMembers ) );

	for ( int i = 0; i < numMembers; i++ )
	{
		bm = block->GetMember( i );

		//Save the block id
		bID = bm->GetID();
		(m_owner->GetInterface())->I_WriteSaveData( INT_ID('B','M','I','D'), &bID, sizeof ( bID ) );

		//Save out the data size
		size = bm->GetSize();
		(m_owner->GetInterface())->I_WriteSaveData( INT_ID('B','S','I','Z'), &size, sizeof( size ) );

		//Save out the raw data
		(m_owner->GetInterface())->I_WriteSaveData( INT_ID('B','M','E','M'), bm->GetData(), size );
	}

	return true;
}

int CSequence::Save( void ) {
	return false;
}

int CSequence::Load( void ) {
	return false;
}
