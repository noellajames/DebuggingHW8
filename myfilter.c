#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include "mypattern.h"
#include "myfilter.h"
#include "myutils.h"
#include "handleexistingstates.h"

#define MAX_PARSE_STATES    1024
#define MAX_BUF_SIZE  (5*1024)

#define MATCH_ALL_CHAR  '.'

static char buffer[MAX_BUF_SIZE];

static int binary_file = 0;

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
        matched += handle_existing_states(ch,count,parse_states,state_count);
    }
    if (binary_file && total_matched > 0) {
        printf("Binary file (standard input) matches\n");
    }

    return total_matched;
}
