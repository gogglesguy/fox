/********************************************************************************
*                                                                               *
*                  U n d o / R e d o - a b l e   C o m m a n d                  *
*                                                                               *
*********************************************************************************
* Copyright (C) 2000,2026 by Jeroen van der Zijp.   All Rights Reserved.        *
*********************************************************************************
* This library is free software; you can redistribute it and/or modify          *
* it under the terms of the GNU Lesser General Public License as published by   *
* the Free Software Foundation; either version 3 of the License, or             *
* (at your option) any later version.                                           *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                 *
* GNU Lesser General Public License for more details.                           *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public License      *
* along with this program.  If not, see <http://www.gnu.org/licenses/>          *
********************************************************************************/
#ifndef FXUNDOLIST_H
#define FXUNDOLIST_H

#ifndef FXOBJECT_H
#include "FXObject.h"
#endif

namespace FX {


// Declarations
class FXUndoList;
class FXCommandGroup;


/**
* Base class for undoable commands records.  Each undo records all
* the information necessary to undo, as well as redo, a given operation.
* Since commands are derived from FXObject, subclassed commands
* may be able to implement their behaviour by sending messages
* (like ID_SETINTVALUE, etc) as well as simple API calls to
* objects to be modified.
* Small, incremental undo commands may sometimes be consolidated
* into larger ones, by merging consecutive commands.  A merge
* is effected by calling the mergeWith() API after first establishing
* that merging is possible with canMerge().
* When commands are merged, the incoming (new) command is deleted and
* not added to the undo list.  In some cases, the incoming command may
* completely cancel the existing commands, causing both to be deleted.
* In that case the prior command will be removed from the undo list.
* To allow UndoList to manage memory, your subclassed Command should
* re-implement size() and return the amount of memory used by your
* undo command.  This way, bookkeeping periodically pare down undo
* commands to keep memory usage in check.
*/
class FXAPI FXCommand : public FXObject {
  FXDECLARE_ABSTRACT(FXCommand)
private:
  FXival refs;
private:
  FXCommand(const FXCommand& org);
  FXCommand &operator=(const FXCommand&);
protected:
  FXCommand():refs(0){}
public:

  // Return reference count
  FXival nrefs() const { return refs; }

  // Increment reference count
  void ref(){ ++refs; }

  // Decrement reference count
  void unref(){ if(--refs<=0) delete this; }

  /**
  * Undo this command.
  */
  virtual void undo() = 0;

  /**
  * Redo this command.
  */
  virtual void redo() = 0;

  /**
  * Return the size of the information in the undo record.
  * The undo list may be periodically trimmed to limit memory
  * usage to a certain limit.
  * For proper accounting, value returned should include the
  * size of the command record itself as well as any data linked
  * from it.
  */
  virtual FXuval size() const;

  /**
  * Name of the undo command to be shown on a button;
  * for example, "Undo Delete".
  */
  virtual FXString undoName() const;

  /**
  * Name of the redo command to be shown on a button;
  * for example, "Redo Delete".
  */
  virtual FXString redoName() const;

  /**
  * Return true if this command could possibly be merged with a
  * previous undo command. This is useful to combine e.g. multiple
  * consecutive single-character text changes into a single block change.
  * The default implementation returns false.
  */
  virtual FXbool canMerge() const;

  /**
  * Called by the undo system to try and merge the new incoming command
  * with this command.  When overriding this API, return:
  *
  *   0  if incoming command could not be merged into this command,
  *   1  if incoming command could be merged into this command,
  *   2  if incoming command completely cancels effect of this command.
  *
  * The default implementation returns 0.
  */
  virtual FXuint mergeWith(FXCommand* command);
  };


// A marked pointer to a command
typedef FXMarkedPtr<FXCommand> FXCommandPtr;

// Array of marked command pointers
typedef FXArray<FXCommandPtr> FXCommandArray;

// Stack of command group pointers
typedef FXPtrListOf<FXCommandGroup> FXCommandStack;


/**
* Group of undoable commands.
* Complicated manipulations may sometimes be more effectively implemented as
* a combination of multiple sub-commands.
* The CommandGroup implements this concept by invoking each the sub-commands
* undo() and redo() in the proper order.
* For example, a network editing program may decompose a "Delete Node"
* operation into a number of connection changes followed by a unconnected
* node removal operation.
* Note that some sub-commands may themselves be also CommandGroup items.
* You start a new CommandGroup by creating an instance of CommandGroup
* cg and calling undolist.begin(ch), and end it by calling undolist.end().
* In between, additional CommandGroup items may be created as well,
* keeping in mind that the inner CommandGroup must be closed with
* undolist.end() before adding to the outer CommandGroup.
* They are strictly nested.
*/
class FXAPI FXCommandGroup : public FXCommand {
  FXDECLARE(FXCommandGroup)
  friend class FXUndoList;
private:
  FXCommandArray  command;
private:
  FXCommandGroup(const FXCommandGroup&);
  FXCommandGroup &operator=(const FXCommandGroup&);
public:

  /// Construct initially empty undo command group
  FXCommandGroup();

  /// Return true if empty
  FXbool empty() const;

  /// Called before adding command
  virtual FXbool cut();

  /// Undo whole command group
  virtual void undo() override;

  /// Redo whole command group
  virtual void redo() override;

  /// Return the size of the command group
  virtual FXuval size() const override;

  /// Clear list
  virtual void clear();

  /// Delete undo command and sub-commands
  virtual ~FXCommandGroup();
  };


/**
* The UndoList class manages a list of undoable commands.
*
* When performing an undo, the document is regressed from its current
* state to an earlier state.  Likewise, performing a redo, a document
* is advanced from an earlier state to a later state.
*
* A document state may be marked, i.e. a special designated state may
* be identified.  Typically, a freshly loaded document is marked as "clean".
* Also, any time a document is saved to disk, one can mark that state
* as "clean".
*
* You can go back to the marked state by invoking revert().
* The revert() API will call a sequence of undo's or redo's, depending
* on whether one need to go to older or newer state of the document.
*
* UndoList can directly receive SEL_UPDATE and SEL_COMMAND
* messages from widgets.  For example, sending ID_UNDO command
* will invoke the undo() API and cause an undo operation.
*
* Alternate history, when enabled, allows one to revert to any prior
* state, even back to the path not taken.
* Supposed we've issued a sequence of commands such as:
*
*   A - B - C - D - E - F
*
* If issue undos until we're back at C, and then do commands G,H,I:
*
*   A - B - C - G - H - I
*
* Prior to "alternate history" support, we would have permanently lost
* access to D,E,F.  With alternate hostory, however, the undo-buffer
* would look as follows:
*                           _   _   _
*   A - B - C - D - E - F - F - E - D - G - H - I
*
* Where overbar indicates a reversed-redo command; notice that the
* sequence:
*               _   _   _
*   D - E - F - F - E - D
*
* has no net effect, it is a sequences of redo's followed by a
* sequence [in reverse order] of reversed-redo's.
*
* Implementation detail: an redo and the corresponding reversed-redo
* are one and the same record; the special marked pointer type keeps
* track of the reversals.  Thus, while the undo-buffer can grow quite
* a lot, the major amount of memory, which lives in the redo-records
* themselves, may be heavily reused.
*/
class FXAPI FXUndoList : public FXCommandGroup {
  FXDECLARE(FXUndoList)
private:
  FXCommandStack groups;        // Command group stack
  FXuval         space;         // Total memory in the undo commands
  FXint          undocount;     // Number of undo records
  FXint          redocount;     // Number of redo records
  FXint          working;       // Currently busy with undo or redo
  FXint          marker;        // Marker value
  FXbool         markset;       // Mark is set
  FXbool         alternate;     // Keep alternate history
private:
  FXUndoList(const FXUndoList&);
  FXUndoList &operator=(const FXUndoList&);
public:
  long onCmdUndo(FXObject*,FXSelector,void*);
  long onUpdUndo(FXObject*,FXSelector,void*);
  long onCmdRedo(FXObject*,FXSelector,void*);
  long onUpdRedo(FXObject*,FXSelector,void*);
  long onCmdRevert(FXObject*,FXSelector,void*);
  long onUpdRevert(FXObject*,FXSelector,void*);
  long onCmdUndoAll(FXObject*,FXSelector,void*);
  long onCmdRedoAll(FXObject*,FXSelector,void*);
  long onUpdUndoCount(FXObject*,FXSelector,void*);
  long onUpdRedoCount(FXObject*,FXSelector,void*);
  long onCmdAltHistory(FXObject*,FXSelector,void*);
  long onUpdAltHistory(FXObject*,FXSelector,void*);
  long onCmdDumpStats(FXObject*,FXSelector,void*);
  long onCmdTrimMark(FXObject*,FXSelector,void*);
  long onCmdTrimSize(FXObject*,FXSelector,void*);
  long onCmdTrimWrinkles(FXObject*,FXSelector,void*);
  long onCmdTrimCount(FXObject*,FXSelector,void*);
  long onCmdClear(FXObject*,FXSelector,void*);
  long onUpdClear(FXObject*,FXSelector,void*);
  long onUpdSize(FXObject*,FXSelector,void*);
public:
  enum{
    ID_REVERT=FXWindow::ID_LAST,
    ID_UNDO,
    ID_REDO,
    ID_UNDO_ALL,
    ID_REDO_ALL,
    ID_UNDO_COUNT,
    ID_REDO_COUNT,
    ID_ALT_HISTORY,
    ID_SIZE,
    ID_CLEAR,
    ID_TRIM_MARK,
    ID_TRIM_COUNT_1K,
    ID_TRIM_COUNT_10K,
    ID_TRIM_COUNT_100K,
    ID_TRIM_COUNT_1M,
    ID_TRIM_COUNT_10M,
    ID_TRIM_COUNT_100M,
    ID_TRIM_COUNT_1G,
    ID_TRIM_COUNT=ID_TRIM_COUNT_1K,
    ID_TRIM_SIZE_1K,
    ID_TRIM_SIZE_10K,
    ID_TRIM_SIZE_100K,
    ID_TRIM_SIZE_1M,
    ID_TRIM_SIZE_10M,
    ID_TRIM_SIZE_100M,
    ID_TRIM_SIZE_1G,
    ID_TRIM_SIZE=ID_TRIM_SIZE_10M,
    ID_TRIM_ALT_1K,
    ID_TRIM_ALT_10K,
    ID_TRIM_ALT_100K,
    ID_TRIM_ALT_1M,
    ID_TRIM_ALT_10M,
    ID_TRIM_ALT_100M,
    ID_TRIM_ALT_1G,
    ID_TRIM_ALT=ID_TRIM_ALT_10M,
    ID_DUMP_STATS,
    ID_LAST
    };
public:

  /**
  * Make new empty undo list, initially unmarked.
  */
  FXUndoList();

  /**
  * Return true if currently inside undo or redo operation; this
  * is useful to avoid generating another undo command while inside
  * an undo operation.
  */
  FXbool busy() const { return (working!=0); }

  /**
  * Mark the current state of the undo list, which is initially unmarked.
  * There can be only one active mark at any time.  Call mark() at any
  * time when you know your document to be "clean"; for example when you
  * save the document to disk.
  * If you don't need to undo past this marked point, consider calling
  * trimMark() to delete all undo commands for states prior to the mark.
  */
  void mark();

  /**
  * Unmark the marked state.
  */
  void unmark();

  /**
  * Check if the current state was marked, if the application has returned
  * to the previously marked state.
  */
  FXbool marked() const;

  /**
  * Add undo command, executing it if desired. The new command will be merged
  * with the previous command if merge is true and we're not at a marked position
  * and the commands are mergeable.  Otherwise the new command will be appended
  * after the last undo command in the currently active undo group.
  * If the new command is successfully merged, it will be deleted.
  */
  FXbool add(FXCommand* cmd,FXbool doit=false,FXbool merge=true);

  /**
  * Begin undo command sub-group. This begins a new group of commands that
  * are treated as a single command.  Must eventually be followed by a
  * matching end() after recording the sub-commands.  The new sub-group
  * will be appended to its parent group's undo list when end() is called.
  */
  FXbool begin(FXCommandGroup *command);

  /**
  * Abort the current command sub-group being compiled.  All commands
  * already added to the sub-groups undo list will be discarded.
  * Intermediate command groups will be left intact.
  */
  FXbool abort();

  /**
  * End undo command sub-group.  If the sub-group is still empty, it will
  * be deleted; otherwise, the sub-group will be added as a new command
  * into parent group.
  * A matching begin() must have been called previously.
  */
  FXbool end();

  /**
  * If alternate history mode is in effect, remember the alternate history.
  * All commands on the redo-list are paired up with their inverses, then
  * transferred to the undo list.  A sequence of commands:
  *                    __     __ __ __
  *   C1 C2 C3 ... CN  CN ... C3 C2 C1
  *
  * Has no effect on the state of the document; however, moving backwards
  * and forwards allows every state in the edit-history of the document
  * to be reached.
  *
  * If alternate history is not in effect, simply delete the commands
  * on the redo-list.
  *
  * This is automatically invoked when a new undo command is added, and
  * not typically called by the user directly.
  */
  virtual FXbool cut() override;

  /**
  * Undo last command. This will move the command to the redo list.
  */
  virtual void undo() override;

  /**
  * Redo next command. This will move the command back to the undo list.
  */
  virtual void redo() override;

  /**
  * Undo all recorded commands.
  */
  void undoAll();

  /**
  * Redo all recorded commands.
  */
  void redoAll();

  /**
  * Revert to marked state; this will perform a sequence of undos
  * (or redos), and return the document to the marked state.
  */
  void revert();

  /**
  * Can we undo more commands.
  */
  FXbool canUndo() const;

  /**
  * Can we redo more commands.
  */
  FXbool canRedo() const;

  /**
  * Can revert to marked.
  */
  FXbool canRevert() const;

  /**
  * Current top undo command.
  */
  FXCommand* current() const;

  /**
  * Return name of the first undo command available; if no
  * undo command available this will return the empty string.
  */
  virtual FXString undoName() const override;

  /**
  * Return name of the first redo command available; if no
  * Redo command available this will return the empty string.
  */
  virtual FXString redoName() const override;

  /// Number of undo records
  FXint undoCount() const { return undocount; }

  /// Number of redo records
  FXint redoCount() const { return redocount; }

  /**
  * Return total amount of space used by undo/redo commands.
  */
  virtual FXuval size() const override;

  /**
  * Trim undo list down, starting from the oldest commands,
  * until reaching the marked ("clean" state) point.
  * If no mark was set this does nothing.
  */
  void trimMark();

  /**
  * Trim undo list down, starting from the oldest commands, until
  * total memory used drops below size sz.
  * Call this periodically to prevent the undo-list from growing
  * beyond a certain maximum amount of memory.
  */
  void trimSize(FXuval sz);

  /**
  * Trim undo list down, starting from the oldest commands.
  * Unlike trimSize(), trimWrinkles() will try bring down space
  * usage by linearizing wrinkles in the command-history, i.e.
  * removing matching command-pairs in sequences like:
  *                    __     __ __ __
  *   C1 C2 C3 ... CN  CN ... C3 C2 C1
  *                    __
  * starting from CN - CN moving outward, until the desired aount
  * of space is reclaimed, or until all such "wrinkles" of alternate
  * history have been removed and only linear edit history remains.
  *
  * If alternate history is not in effect, or there is no alternate
  * history to be removed, this command reclaims no space at all.
  */
  void trimWrinkles(FXuval sz);

  /**
  * Trim undo list down, starting from the oldest commands, until
  * no more than nc commands are left in the undo list.
  * Call this periodically to prevent the undo-list from growing
  * beyond a certain number of records.
  */
  void trimCount(FXint nc);

  /**
  * Clear all undo information, releasing all commands
  * and markers.
  */
  virtual void clear() override;

  /**
  * Enable or disable alternate history mode.
  * In alternate history mode, adding a new command after performing a
  * number of undo's will remember the alternate history, and allow a
  * sequence of undo's and redo's to navigate back through this alternate
  * history.
  */
  void setAlternateHistory(FXbool flag){ alternate=flag; }

  /**
  * Returns true if alternate history mode is in effect.
  */
  FXbool getAlternateHistory() const { return alternate; }

  /// Dump statistics
  void dumpStats();

  /// Destroy
  virtual ~FXUndoList();
  };

}

#endif
