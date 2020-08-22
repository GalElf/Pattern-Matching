
String Pattern Matching Algorithm

===Description ===

Program files:
slist.c - file that implementaion a Linked list and it's Actions.
funcation:
	slist_init - initialize a new list.
	slist_destroy - de-allocate the memory hold by the list.
	slist_append - add node in the end of the list.
	slist_prepend - add node in the start of the list.
	slist_pop_first - remove the first node in the list and return his data.
	slist_append_list - merging 2 list, when are list2 insert in the end of list1.

----------

pattern_matching.c - file that implementaion string searching, creating DFA that representing the patterns, with that searching in the text.
funcation:
	pm_init - initializes the fsm parameters.
	pm_goto_get - return the _transitions state.
	pm_goto_set - connect edge between from_state to to_state and the symbol as the edge.
	pm_addstring - add new pattern into the pm that was created.
	pm_makeFSM - setting up the failrue transitions in the fsm.
	pm_fsm_search - 
	pm_destroy - de-allocate the memory hold by the fsm.
private function:
	pmDestroyAllState - use the slist_destroy to release all the list, an free allocate memory.


