//
//  Command.h
//
#pragma once
#include "../datastructures/List.h"

/*
===============================
Command
===============================
*/
class Command {
public:
	virtual void Execute() = 0;
	virtual void Undo() = 0;
	virtual void Redo() = 0;
};

/*
===============================
CommandHistory
===============================
*/
class CommandHistory {
public:
	CommandHistory() : m_currentNum( 0 ) {}

	void Execute( Command * cmd );
	void Undo();
	void Redo();

private:
	List< Command * >	m_commandStack;
	int					m_currentNum;
};

/*
===============================
CommandHistory::Execute
===============================
*/
void CommandHistory::Execute( Command * cmd ) {
	// Remove any elements in the redo portion of the history stack
	while ( m_commandStack.Num() > m_currentNum ) {
		delete m_commandStack[ m_commandStack.Num() - 1 ];
		m_commandStack.Remove( m_commandStack.Num() - 1 );
	}

	// Append the current number of commands executed
	m_commandStack.Append( cmd );
	m_currentNum++;

	// Execute the command
	cmd->Execute();

}

/*
===============================
CommandHistory::Undo
===============================
*/
void CommandHistory::Undo() {
	// Don't undo the history stack when there's nothing to undo
	if ( m_currentNum <= 0 ) {
		return;
	}

	// Undo the top command on the stack history
	Command * cmd = m_commandStack[ m_currentNum - 1 ];
	cmd->Undo();

	// Decrement the number of commands that have been executed
	m_currentNum--;
}

/*
===============================
CommandHistory::Redo
===============================
*/
void CommandHistory::Redo() {
	// Don't attempt to redo commands when there are no commands to redo
	if ( m_commandStack.Num() <= m_currentNum ) {
		return;
	}

	// Redo the first command available in the redo portion of the stack
	m_commandStack[ m_currentNum - 1 ]->Redo();

	// Increment the number of commands that have been executed
	m_currentNum++;
}