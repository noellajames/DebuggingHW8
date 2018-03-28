#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include "mypattern.h"
#include "myutils.h"
#include "cantransition.h"

#define MAX_PARSE_STATES    1024
#define MAX_BUF_SIZE  (5*1024)

#define MATCH_ALL_CHAR  '.'

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