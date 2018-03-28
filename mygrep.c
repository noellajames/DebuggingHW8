#include <stdio.h>
#include <stdlib.h>
#include <error.h>

#include "myutils.h"
#include "mypattern.h"
#include "myfilter.h"

/*
                                User Commands
NAME
    mygrep - prints lines matching a pattern

SYNOPSIS
    cat [FILE] | ./mygrep [OPTIONS] PATTERN(INFILE)
    echo STDIN | ./mygrep PATTERN

EXAMPLES
    cat test1.txt | ./mygrep -f pattern1.txt
    echo 'abcd' | ./mygrep 'ab'

DESCRIPTION
    mygrep searchs the named input file for lines containing a match to the 
    given PATTERN. By default, mygrep prints the matching lines.

OPTIONS
    -f <filename>, --file=<filename>,
    Obtain patterns  from  FILE,  one  per  line.   The  empty  file
    contains  zero  patterns, and therefore matches nothing.  (-f is
    specified by POSIX.)

PATTERN
    Patterns used in mygrep are regular expressions, which are patterns that 
    describes a set of strings. The regular expression special characters that 
    are used in the regular expression patterns in mygrep are  
    ( .  ^ $ [ ... ]  [ - ] ) . Other characters are not included in this 
    version of mygrep. 

IMPLEMENTATION:
    The implementation of this grep takes in the arguments passed in through 
    the command line and parses them. If there are less than 2 arguments, 
    mygrep immediately throws an error. If the first argument that's passed in 
    is a -f or a --file, that means we are reading from a file and not from 
    STDIN. Therefore, we store the file name that is passed in as the next 
    argument. If a file exists, then there is a pattern that exists in the 
    file, so we call the read_pattern_from_file method and store the regex 
    expression that passed in. If the pattern is an octal string, we must 
    convert it to a proper regex expression, so we call 
    convert_string_with_octal_to_char. Then, we use the pattern that was 
    passed in to call the parse_pattern method located in mypattern.c that 
    parses the regex expression.
    The parse_pattern method iterates through the entire pattern and counts 
    the number of brackets in the pattern. It checks to see if there's special 
    characters and alls of of these characters that must fix the regex to the 
    my_states array (a array of states structs that hold type, size, and char 
    array). This parsed pattern is used to filter through our input and find 
    matches to print at the end.
    In the end, we call filter_input in myfilter.c, which filters input and 
    prints out lines that matches the pattern. For a binary file a string 
    indicating pattern was matched is printed. 

AUTHOR
    Noella James, 
    EID nj4729, 
    CS Login njames96

*/

/*Buffer to hold the pattern read from a file*/
static char file_pattern[MAX_INPUT_STRING];

/* Prints usages and exits */
void print_usage(void) {
    fprintf(stderr,"usage: mygrep <pattern> or mygrep -f <pattern-file> or mygrep --filename=<pattern-file>\n");
    exit(1);    
}


/* Reads the pattern from the file */

void read_pattern_from_file(char * filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == 0){
        fprintf(stderr,"Unable to open file %s\n",filename);
        exit(1);
    }
    fgets(file_pattern,MAX_INPUT_STRING,fp);
    fclose(fp);
    for (int i = 0; i < MAX_INPUT_STRING; i++){
        if (file_pattern[i] == '\n') {
            file_pattern[i] = 0;
            break;
        }
    }
}

/* coverts a set of octal digits to a char */
unsigned char octal_char(int digit1, int digit2, int digit3) {
    unsigned char ch = (unsigned char) ( digit1 * 8 * 8 + digit2 * 8 + digit3);
    //printf("converted octal %ud\n",ch);
    return ch;
}

/* Converts a pattern containing octal strings to regular pattern */
void convert_string_with_octal_to_char(char *s) {
    unsigned char t_buf[MAX_INPUT_STRING];
    int len = my_strlen(s);
    int i;
    int j = 0;
    my_memset(t_buf,0,MAX_INPUT_STRING);
    for (i = 0; i < len; i++) {
        /*CHeck if within bounds and is ocutal number between 0 to 7 */
        if ( (s[i] == '\\') && (i+3 < len) &&
            (s[i+1] >= '0' && s[i+1] <= '7') &&
            (s[i+2] >= '0' && s[i+2] <= '7') &&
            (s[i+3] >= '0' && s[i+3] <= '7')) {
            t_buf[j] = octal_char(s[i+1] - '0',s[i+2] - '0',s[i+3] - '0');
            i += 3;
        } else {
            t_buf[j] = s[i];
        }
        j++;
    }

    /* Copy the converted buffer */
    my_memset(s,0,len);
    len = j; 
    for (i = 0; i < len; i++){
        s[i] = t_buf[i];
    }
}

/* Main program */
int main(int argc, char** argv){
    int i;
    char * file_name = 0;
    char * pattern;

    my_memset(file_pattern,0,MAX_INPUT_STRING);

    parse_init();
    if (argc < 2) {
        print_usage();
    }
    /* Handle mygrep -f <pattern_file> */
    if ((argv[1][0] == '-') && (argv[1][1] == 'f')) {
        if (argc < 3) {
            print_usage();
        }
        file_name = argv[2];
    }
    /* handle --file=<pattern file > */
    if (my_strncmp(argv[1],"--file=",7) == 0) {
        file_name = &argv[1][7];
    }

    if (file_name != 0) {
        read_pattern_from_file(file_name);
        pattern = file_pattern;

    } else {
        pattern = argv[1];
    }

    //printf("Pattern is %s\n", pattern);
    convert_string_with_octal_to_char(pattern);
    //printf("Pattern2 is %s\n", pattern);
    parse_pattern(pattern);
    //parse_debug_states();

    int matched = filter_input();

    exit(0);
}