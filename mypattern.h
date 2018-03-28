#ifndef _MYPATTERN_H_
#define _MYPATTERN_H_



#define MAX_CHAR_COUNT 512
#define MAX_INPUT_STRING 1024


#define TYPE_START      (0x00000001)
#define TYPE_END        (0x00000010)
#define TYPE_SPECIAL    (0x00000100)
#define TYPE_REGULAR    (0x00100000)
#define TYPE_INVERTED   (0x01000000)

#define END_STATE       (-1)
#define REJECT_STATE    (-2)


typedef struct state {
    unsigned int type;
    unsigned int size;
    unsigned char ch[MAX_CHAR_COUNT];
    int next_state;
}state_t, *state_p ;


extern state_t my_states[MAX_INPUT_STRING];

extern unsigned int stack_pos;

void parse_init(void);
int parse_pattern(char *);
void parse_debug_states(void);
int parse_has_ending_rule(void);

#endif 