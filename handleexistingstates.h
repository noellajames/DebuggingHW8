#ifndef _HANDLE_EXISTING_STATES_H
#define _HANDLE_EXISTING_STATES_H

#define FREE    0
#define USED    1

typedef struct parse_state {
    unsigned int inuse;
    int index;
}parse_state_t, *parse_state_p ;


int handle_existing_states(unsigned char, int);


#endif
