#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include "mypattern.h"
#include "myfilter.h"
#include "myutils.h"
#include "cantransition.h"
#include "handleexistingstates.h"
/* Transitionals all activite state to the next applicable
state i.e active, end (accept) or reject state.  
Additionally cleans up rejected / end states to ensure 
that the buffer does not overflow */
int handle_existing_states(unsigned char uch, int count, parse_state_t* parse_states, int state_count) {
    int i;
    int matched = 0;
    int empty_slots = 0;


    //printf("1 - state_count %d \n", state_count);
    //("%c - %u is found\n",uch,uch);


    for(i = 0 ; i < state_count; i++) {
        //printf("1\n");
        if (parse_states[i].inuse == FREE) {
            continue;
        }
        int index = parse_states[i].index;
        //printf("2 -index %d\n",index);
        if (index == END_STATE) {
            continue;
        }
        if (index >= stack_pos){
            //printf("For parse_state %d, we have invalid index %ud\n",i, index);
            parse_states[i].inuse = FREE;
            continue;
        }
        if (can_transition(uch, index,count)) {
            parse_states[i].index = my_states[index].next_state;
            //printf("3a  %d\n", parse_states[i].index);
        } else {
            //printf("3b\n");
            parse_states[i].index = REJECT_STATE;
        }
    }


    for (i = 0; i < state_count; i++){
        if (parse_states[i].index == END_STATE) {
            //printf("Matched for %d\n",i);
            matched++;
        }
        if ( (parse_states[i].index == END_STATE) || (parse_states[i].index == REJECT_STATE)){
            empty_slots++;
        }
    }



    int j = 0;            
    for (i = state_count - 1; i >=0; i--) {
        if ( (parse_states[i].index != END_STATE) && (parse_states[i].index != REJECT_STATE)){
            for ( ; j < state_count ; j++){
                if ( (parse_states[j].index == END_STATE) || (parse_states[j].index == REJECT_STATE)){
                    parse_states[j].index = parse_states[i].index;
                    parse_states[j].inuse = parse_states[i].inuse;
                    parse_states[i].inuse = FREE;
                    state_count--;
                    break;
                }
            }
        }
    }    

    if ( (state_count == 1) && 
        ((parse_states[0].index == END_STATE) || (parse_states[0].index == REJECT_STATE))) {
        state_count = 0;
        parse_states[0].inuse = FREE;
    }

    if (empty_slots == state_count) { /* All the slots are empty */
        for (int i = 0 ; i < state_count ; i++) {
            parse_states[i].inuse = FREE;
        }
        state_count = 0;
    }

    if (can_transition(uch,1,count)){
        parse_states[state_count].inuse = USED;
        parse_states[state_count].index = my_states[1].next_state;
        state_count++;
    } 

    return matched;
}
