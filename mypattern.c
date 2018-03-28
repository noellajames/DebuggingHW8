#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include "mypattern.h"
#include "myutils.h"

#define LPAREN '('
#define RPAREN ')'
#define LBRACK '['
#define RBRACK ']'
#define HYPHEN '-'
#define BEGIN  '^'
#define END    '$'



state_t my_states[MAX_INPUT_STRING];

unsigned int stack_pos = 0;

unsigned int paren_count = 0;

unsigned int brack_count = 0;

unsigned int start = 0;
unsigned int end = 0;
unsigned int reversed = 0;
unsigned int special = 0;

void parse_add_empty_match_block(void);
void parse_add_char_existing_slot(unsigned char);
void parse_add_char_existing_slot_fill(unsigned char);
void parse_update_start_end(void);


/* Initializes the parser and DFA used for parsing */
void parse_init(void) {
    my_memset((char *)my_states,0, sizeof (state_t) * MAX_INPUT_STRING);
    my_states[stack_pos].type = TYPE_START;
    my_states[stack_pos].next_state = END_STATE;
    stack_pos++;
}

/* Check if the pattern has $ at the end */
int parse_has_ending_rule(void) {
    if (my_states[stack_pos-1].type == TYPE_END) {
        return 1;
    }
    return 0;
}

/* Add's a character match pattern to the rule*/
void parse_add_char(unsigned char uch){
    my_states[stack_pos].type = TYPE_REGULAR;
    if (special) {
        my_states[stack_pos].type = TYPE_SPECIAL;
    } 
    my_states[stack_pos].next_state = END_STATE;
    my_states[stack_pos].size = 1;
    my_states[stack_pos].ch[0] = uch;
    my_states[stack_pos-1].next_state = stack_pos;
    stack_pos++;
    parse_update_start_end();
}

/* Creates an empty block - used for meta-char's [] */
void parse_add_empty_match_block(void){
    my_states[stack_pos].type = TYPE_REGULAR;
    my_states[stack_pos].next_state = END_STATE;
    my_states[stack_pos].size = 0;
    my_states[stack_pos-1].next_state = stack_pos;
    stack_pos++;
    parse_update_start_end();
}

/* Adds a character to the existing slow. used primairly with meta-chars[] */
void parse_add_char_existing_slot(unsigned char uch) {
    unsigned size = my_states[stack_pos-1].size;
    my_states[stack_pos-1].size = size + 1;
    my_states[stack_pos-1].ch[size] = uch;
    if (reversed) {
        my_states[stack_pos-1].type = TYPE_INVERTED;
    }
}

/* Marks start and end positon */
void parse_update_start_end(void) {
    if (start) {
        my_states[stack_pos-1].type = TYPE_START;
        start = 0;
    }
    if (end) {
        my_states[stack_pos-1].type = TYPE_END;
        end = 0;
    }
}

/* Added extened patterns like a-z or 0-9 */
void parse_add_char_existing_slot_fill(unsigned char uch) {
    unsigned size = my_states[stack_pos-1].size;
    if (size == 0) {
        fprintf(stderr,"Incorrect pattern: Incorrect hyphen placement\n");
        exit(1);        
    }
    unsigned char prev = my_states[stack_pos-1].ch[size-1];
    if (prev >= uch) {
        fprintf(stderr,"Incorrect pattern: character on right is lexigraphically less than the character on left\n");
        exit(1);          
    }
    for (unsigned char c = prev + 1 ; c <= uch; c++) {
        my_states[stack_pos-1].size = my_states[stack_pos-1].size + 1;
        my_states[stack_pos-1].ch[my_states[stack_pos-1].size - 1] = c;
    }
}

/* Parses the provided pattern */
int parse_pattern(char * pattern) {
    unsigned char * s = (unsigned char *) pattern;
    unsigned size = 0;

    while (*s != 0) {
        if (*s == LPAREN) {
            paren_count++;
        } else if ((*s == BEGIN) && (brack_count == 0)) {
            start = 1;
        } else if ((*s == BEGIN) && (brack_count != 0)) {
            reversed = 1;
        } else if ((*s == '\\') && (special == 0)) {
            special = 1;
        } else if (*s == END) {
            end = 1;
            parse_update_start_end();
        } else if (!special && (*s == RPAREN)) {
            if (paren_count == 0) {
                fprintf(stderr,"Incorrect pattern: Incorrect paranthesis placement\n");
                exit(1);
            }
            paren_count--;
        } else if  (!special && (*s == LBRACK)) {
            if (brack_count > 0){
                fprintf(stderr,"Incorrect pattern: Incorrect bracket placement\n");
                exit(1);
            }
            brack_count++;
            parse_add_empty_match_block();
        } else if  (!special && (*s == RBRACK)) {
            if (brack_count == 0){
                fprintf(stderr,"Incorrect pattern: Incorrect bracket placement\n");
                exit(1);
            }
            brack_count--;
            reversed = 0;
        } else if ( !special && (*s == HYPHEN) && (brack_count > 0) ) {
            s++;
            size++;
            if (*s == HYPHEN) {
                fprintf(stderr,"Incorrect pattern: Incorrect bracket placement\n");
                exit(1);
            }
            parse_add_char_existing_slot_fill(*s);
        }
        else {
            if (brack_count > 0) {
                parse_add_char_existing_slot(*s);
            } else {
                parse_add_char(*s);
            }
            special = 0;
        }
        s++;
        size++;
    }

    if (paren_count > 0) {
        fprintf(stderr,"Incorrect pattern: Incorrect paranthesis placement\n");
        exit(1);
    }
    if (brack_count > 0){
        fprintf(stderr,"Incorrect pattern: Incorrect bracket placement\n");
        exit(1);
    }

    return size;
}


/* Helper function to print out the DFA. Used primarily for debuggng */
void parse_debug_states(void) {
    for (unsigned i = 0;  i < stack_pos; i++) {
        printf("%ud , %ud, %ud , %d", i, my_states[i].type, my_states[i].size, my_states[i].next_state);
        for (unsigned j = 0; j < my_states[i].size; j++) {
            printf(" %c", my_states[i].ch[j]);
        }
        printf("\n");
    }
}