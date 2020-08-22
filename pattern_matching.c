#include <stdio.h>
#include <string.h>
#include "slist.h"
#include "pattern_matching.h"

// private declaration function
void pmDestroyAllState(pm_state_t *pm);

/* ---------------------------------------------------- */

// Initializes the fsm parameters, this function should init zero state
int pm_init(pm_t *pmAuto)
{
    if (pmAuto == NULL)
    { // check if the pm has not been allocate
        return -1;
    }
    pmAuto->newstate = 1;
    pmAuto->zerostate = (pm_state_t *)malloc(sizeof(pm_state_t));
    if (pmAuto->zerostate == NULL)
    {
        printf("pmAuto->zerostate allocation was failed");
        return -1;
    }
    pmAuto->zerostate->id = 0;
    pmAuto->zerostate->depth = 0;
    pmAuto->zerostate->fail = NULL;
    pmAuto->zerostate->output = (slist_t *)malloc(sizeof(slist_t));
    if (pmAuto->zerostate->output == NULL)
    {
        printf("pmAuto->zerostate->output allocation was failed");
        return -1;
    }
    slist_init(pmAuto->zerostate->output);
    pmAuto->zerostate->_transitions = (slist_t *)malloc(sizeof(slist_t));
    if (pmAuto->zerostate->_transitions == NULL)
    {
        printf("pmAuto->zerostate->_transitions allocation was failed");
        return -1;
    }
    slist_init(pmAuto->zerostate->_transitions);
    return 0;
}

// this function return the _transitions state
pm_state_t *pm_goto_get(pm_state_t *state, unsigned char symbol)
{
    if (state == NULL)
    {
        return NULL;
    }
    slist_node_t *node = slist_head(state->_transitions);
    pm_labeled_edge_t *edge;
    while (node != NULL)
    {
        edge = (pm_labeled_edge_t *)slist_data(node);
        if (edge->label == symbol)
        { // check if the chae exist
            return edge->state;
        }
        node = slist_next(node);
    }
    return NULL;
}

// this function connect edge between fromstate to tostate and the symbol as the edge
int pm_goto_set(pm_state_t *from_state, unsigned char symbol, pm_state_t *to_state)
{
    if (from_state == NULL || to_state == NULL)
    {
        return -1;
    }
    pm_labeled_edge_t *edge = (pm_labeled_edge_t *)malloc(sizeof(pm_labeled_edge_t));
    if (edge == NULL)
    {
        printf("edge allocation was failed");
        return -1;
    }
    edge->label = symbol;
    edge->state = to_state;
    slist_append(from_state->_transitions, (void *)edge); // add the edge to the _transitions list
    return 0;
}

// add new pattern into the pm that was created
int pm_addstring(pm_t *pmAuto, unsigned char *str, size_t n)
{
    if (pmAuto == NULL || pmAuto->zerostate == NULL)
    { // check if the pm was allocate and created
        return -1;
    }
    if (strlen(str) != n)
    { // check if the data of the string was correct
        printf("The size that was enter was worng\n");
        return -1;
    }
    pm_state_t *currState = pmAuto->zerostate;
    pm_state_t *nextState = pm_goto_get(currState, str[0]);
    int i = 1;
    while (i <= n)
    {
        if (nextState != NULL)
        {
            currState = nextState;
            nextState = pm_goto_get(currState, str[i]);
            if (i == n)
            {
                slist_append(currState->output, str);
                return 0;
            }
        }
        else
        { // from here the rest of the char dont exists and creating new state and edge with the rest of the chars
            pm_state_t *newState = (pm_state_t *)malloc(sizeof(pm_state_t));
            if (newState == NULL)
            {
                printf("newState allocation was failed");
                return -1;
            }
            printf("Allocating state %d\n", pmAuto->newstate);
            // initialize the new state
            newState->id = pmAuto->newstate++;
            newState->depth = currState->depth++;
            newState->fail = NULL;
            newState->output = (slist_t *)malloc(sizeof(slist_t));
            if (newState->output == NULL)
            {
                printf("newState->output allocation was failed");
                return -1;
            }
            slist_init(newState->output);
            newState->_transitions = (slist_t *)malloc(sizeof(slist_t));
            if (newState->_transitions == NULL)
            {
                printf("newState->_transitions allocation was failed");
                return -1;
            }
            slist_init(newState->_transitions);

            if (i == n)
            { // if we arrive to the last char we fill his output
                slist_append(newState->output, str);
            }
            pm_goto_set(currState, str[i - 1], newState);
            printf("%d->%c->%d\n", currState->id, str[i - 1], newState->id);
            currState = newState;
        }
        i++;
    }
    return 0;
}

// creating and setting up the failrue transition
int pm_makeFSM(pm_t *pmAuto)
{
    if (pmAuto == NULL || pmAuto->zerostate == NULL)
    {
        return -1;
    }
    if (slist_head(pmAuto->zerostate->_transitions) == NULL)
    {
        return -1;
    }                                                    // check that the DFA has created before creating the failrue transitions
    slist_t *queue = (slist_t *)malloc(sizeof(slist_t)); // queue created by using slist
    if (queue == NULL)
    {
        printf("queue allocation was failed");
        return -1;
    } // check queue allocation memory
    slist_init(queue);

    slist_node_t *currNode = slist_head(pmAuto->zerostate->_transitions); // create pointer for the transitions of zerostate
    pm_labeled_edge_t *edge;
    while (currNode != NULL) //adding to the queue depth 1 states
    {
        edge = (pm_labeled_edge_t *)slist_data(currNode); // get data from the cuurent node
        edge->state->fail = pmAuto->zerostate;            // update the failrue transitions to the zerostate
        slist_append(queue, (void *)edge->state);         // insertind the state into the queue
        currNode = slist_next(currNode);
        printf("Setting f(%d) = %d\n", edge->state->id, edge->state->fail->id);
    }
    while (slist_head(queue) != NULL)
    {
        pm_state_t *r = slist_pop_first(queue); // remove state from the queue
        currNode = slist_head(r->_transitions); // create pointer to the head of the _transitions list
        while (currNode != NULL)
        {
            edge = (pm_labeled_edge_t *)slist_data(currNode);
            slist_append(queue, (void *)edge->state);
            pm_state_t *tempState = r->fail; // create pointer for to fail
            int skip = 0;
            while (pm_goto_get(tempState, edge->label) == NULL) //verify if there is edge from the fail of the father and his father...
            {                                                   //if their is no edge from the state and he is the zerostate
                if (tempState == pmAuto->zerostate)
                {
                    edge->state->fail = pmAuto->zerostate;
                    skip = 1;
                    break;
                }
                tempState = tempState->fail;
            }
            if (skip == 0)
            {
                edge->state->fail = pm_goto_get(tempState, edge->label);
                slist_append_list(edge->state->output, edge->state->fail->output); // merge the 2 output of the state
            }
            printf("Setting f(%d) = %d\n", edge->state->id, edge->state->fail->id);
            currNode = slist_next(currNode);
            skip = 0;
        }
    }
    free(queue);
    return 0;
}

// this funcation search for matches in the string
slist_t *pm_fsm_search(pm_state_t *state, unsigned char *str, size_t n)
{
    if (state == NULL || str == NULL || strlen(str) != n)
    { // check if the data that the function get are fit to function to work properly
        return NULL;
    }
    if (slist_head(state->_transitions) == NULL || ((pm_labeled_edge_t *)slist_data(slist_head(state->_transitions)))->state->fail == NULL)
    { // check if the user use pm_makeFSM and pm_addstring to create the DFA
        return NULL;
    }

    slist_t *matched_list = (slist_t *)malloc(sizeof(slist_t)); // create the list to enter the match
    if (matched_list == NULL)
    { // check list allocation memory
        printf("matched_list allocation was failed");
        return NULL;
    }
    slist_init(matched_list);
    int i = 0;
    pm_state_t *tempState = state;
    while (i < n)
    {
        pm_state_t *checkChar = pm_goto_get(tempState, str[i]);
        while (checkChar == NULL && i <= n)
        {
            if (tempState->fail == NULL)
            {
                i++;
            }
            else
            {
                tempState = tempState->fail;
            }
            checkChar = pm_goto_get(tempState, str[i]);
        }
        if (i >= n)
        {
            break;
        }
        tempState = pm_goto_get(tempState, str[i]);
        if (tempState->output->size > 0)
        {
            slist_node_t *temp = slist_head(tempState->output);
            while (temp != NULL)
            { // create all the matchs
                pm_match_t *match = (pm_match_t *)malloc(sizeof(pm_match_t));
                match->pattern = (char *)temp->data;
                match->start_pos = i - strlen(match->pattern) + 1;
                match->end_pos = match->start_pos + strlen(match->pattern) - 1;
                match->fstate = tempState;
                slist_append(matched_list, (void *)match);
                printf("Pattern: %s, start at: %d, ends at: %d, last state = %d\n", match->pattern, match->start_pos, match->end_pos, match->fstate->id);
                temp = slist_next(temp);
                if (temp == NULL && pm_goto_get(tempState, str[i + 1]) == NULL)
                {
                    tempState = tempState->fail;
                }
            }
        }
        i++;
    }
    return matched_list;
}

// this function release all allocate memory
void pm_destroy(pm_t *pmAuto)
{
    if (pmAuto == NULL || pmAuto->zerostate == NULL)
    {
        return;
    } // check that the DFA has created before creating the failrue transitions
    if (pmAuto->zerostate->_transitions->size == 0)
    {
        pmDestroyAllState(pmAuto->zerostate); // destroy zerostate and all the other state
        return;
    } // there is no transitions to relase

    slist_t *queue = (slist_t *)malloc(sizeof(slist_t)); // queue created by using slist
    if (queue == NULL)
    {
        printf("queue allocation was failed");
        return;
    } // check queue allocation memory
    slist_init(queue);
    slist_node_t *temp = slist_head(pmAuto->zerostate->_transitions);
    while (temp != NULL)
    {
        slist_append(queue, (void *)((pm_labeled_edge_t *)slist_data(temp))->state);
        temp = slist_next(temp);
    } // enter all the state into the queue
    while (slist_head(queue) != NULL)
    { // check if was left any state
        pm_state_t *state = slist_pop_first(queue);
        temp = slist_head(state->_transitions);
        while (temp != NULL)
        { // enter all the state that was left into the queue
            slist_append(queue, (void *)((pm_labeled_edge_t *)slist_data(temp))->state);
            temp = slist_next(temp);
        }
        pmDestroyAllState(state); // destroy zerostate and all the other state
    }
    pmDestroyAllState(pmAuto->zerostate); // destroy zerostate and all the other state
    free(queue);
    return;
}

// private function that destroy the zerostate and all the other state
void pmDestroyAllState(pm_state_t *pm)
{
    slist_destroy(pm->_transitions, SLIST_FREE_DATA);
    slist_destroy(pm->output, SLIST_LEAVE_DATA);
    free(pm->_transitions);
    free(pm->output);
    free(pm);
}
