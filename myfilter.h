#ifndef _MY_FILTER_H
#define _MY_FILTER_H

#define FREE    0
#define USED    1


typedef struct parse_state {
    unsigned int inuse;
    int index;
}parse_state_t, *parse_state_p ;



int filter_input(void);

#endif