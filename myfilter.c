#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include "mypattern.h"
#include "myfilter.h"
#include "myutils.h"


#define MAX_PARSE_STATES    1024
#define MAX_BUF_SIZE  (5*1024)

#define MATCH_ALL_CHAR  '.'

static parse_state_t parse_states[MAX_PARSE_STATES];
static int state_count = 0;

static char buffer[MAX_BUF_SIZE];

static int binary_file = 0;


int handle_existing_states(unsigned char, int count);

int can_transition(unsigned char, unsigned int, int count);


/* Filters input and prints out lines that matches the pattern . 
For binary file a string indicating pattern was matched is printed.*/
int filter_input(void) {
    size_t char_read;
    int count = 0;
    int matched = 0;
    int total_matched = 0;
    unsigned char ch;

    int has_ending_rule = parse_has_ending_rule();

    my_memset(buffer,0, MAX_BUF_SIZE);

    while(1) {
        char_read = fread(&ch,sizeof(unsigned char),1,stdin);
        if (char_read < 1) { /* ENd of file */
            if (matched) {
                total_matched += matched;
                if (!binary_file) {
                    printf("%s",buffer);
                }
            }
            break;
        }

        if (ch < 32 && ch != 10) {
            binary_file = 1;
        }

        if (ch == 10) { /* New line */
            if (matched > 0 ){
                buffer[count] = ch;
                total_matched += matched;
                if (!binary_file) {
                    printf("%s",buffer);
                }
                matched = 0;
            }
            count = 0;
            my_memset(buffer,0, MAX_BUF_SIZE);
            continue;
        }
        
        if (has_ending_rule) {
            matched = 0;
        }

        if (count < MAX_BUF_SIZE){
            buffer[count++] = ch;
        }
        matched += handle_existing_states(ch,count);
    }
    if (binary_file && total_matched > 0) {
        printf("Binary file (standard input) matches\n");
    }

    return total_matched;
}


/* Transitionals all activite state to the next applicable
state i.e active, end (accept) or reject state.  
Additionally cleans up rejected / end states to ensure 
that the buffer does not overflow */
int handle_existing_states(unsigned char uch, int count) {
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


/* Return true if the state engine can transition from the current state
to the next state based on the input character. Else it returns false.
count is to determine the start of the line */
int can_transition(unsigned char uch, unsigned int in_index, int count) {
    int index = in_index;
    if (index < 0){
        return 1;
    }
    unsigned size = my_states[index].size;
    if (my_states[index].type == TYPE_START) {
        if (count != 1) {
            return 0;
        }
    }
    if ((my_states[index].type == TYPE_REGULAR) || (my_states[index].type == TYPE_SPECIAL)
        || (my_states[index].type == TYPE_START) || (my_states[index].type == TYPE_END)) {
        for (unsigned k = 0; k < size; k++){
            if ((my_states[index].ch[k] == MATCH_ALL_CHAR) && (my_states[index].type != TYPE_SPECIAL)){
                return 1;
            }

            if (my_states[index].ch[k] == uch){
                return 1;
            }
        }
        return 0;
    } else if (my_states[index].type == TYPE_INVERTED) {
        for (unsigned k = 0; k < size; k++){
            if (my_states[index].ch[k] == MATCH_ALL_CHAR) {
                return 0;
            }

            if (my_states[index].ch[k] == uch){
                return 0;
            }
        }
        return 1;
    }
    return 0;
}