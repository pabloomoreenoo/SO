#include <stdio.h>
#include <unistd.h> /* for getopt() */
#include <stdlib.h> /* for EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>
#include "defs.h"


/* Assume lines in the text file are no larger than 100 chars */
#define MAXLEN_LINE_FILE 100

int print_text_file(char *path)
{
	/* To be completed (part A) */
	int entry_idx; 
	char *token;
	token_id_t token_id; 
	student_t *student; 
	char line[MAX_PASSWD_LINE]; 
	char *lineptr; 

	FILE *file = fopen(path, "rb"); 

	if(file == NULL){
		perror("Error abriendo el archivo"); 
		exit(EXIT_FAILURE); 
	}

	int num_records; 
	fscanf(file, "%d\n", &num_records); 
	int curr_line = 0; 

	while (fgets(line, MAX_PASSWD_LINE, file) != NULL) {

		if(line[0] == '#'){
			curr_line++; 
		}

		lineptr = line; 

		fprintf(stdout, "[Entry #=%d]\n", curr_line); 


		token_id = 0; 
		while((token = strsep(&lineptr, ":"))!=NULL) { 
			fprintf(stdout, "TokenId = %d", token_id); 
			fprintf(stdout, "  Token =%s\n", token); 
			
			//switch(token_id){

			//}
			token_id++;
		}

		curr_line++; 
	}



	return 0;
}

int print_binary_file(char *path)
{
	/* To be completed  (part B) */
	int entry_idx; 
	token_id_t token; 
	student_t student; 




	return 0;
}


int write_binary_file(char *input_file, char *output_file)
{
	/* To be completed  (part C) */
	return 0;
}

int main(int argc, char *argv[])
{
	int ret_code, opt;
	struct options options;

	/* Initialize default values for options */
	options.input_file = NULL;
	options.output_file = NULL;
	options.action = NONE_ACT;
	ret_code = 0;

	/* Parse command-line options (incomplete code!) */
	while ((opt = getopt(argc, argv, "hi:p")) != -1)
	{
		switch (opt)
		{
		case 'h':
			fprintf(stderr, "Usage: %s [ -h | -p | -i <file>]\n", argv[0]);
			exit(EXIT_SUCCESS);
		case 'i':
			options.input_file = optarg;
			break;
		case 'p':
			options.action = PRINT_TEXT_ACT; 
			break; 
		
		/**
		 **  To be completed ...
		 **/

		default:
			exit(EXIT_FAILURE);
		}
	}

	if (options.input_file == NULL)
	{
		fprintf(stderr, "Must specify one record file as an argument of -i\n");
		exit(EXIT_FAILURE);
	}

	switch (options.action)
	{
	case NONE_ACT:
		fprintf(stderr, "Must indicate one of the following options: -p, -o, -b \n");
		ret_code = EXIT_FAILURE;
		break;
	case PRINT_TEXT_ACT:
		/* Part A */
		ret_code = print_text_file(options.input_file);
		break;
	case WRITE_BINARY_ACT:
		/* Part B */
		ret_code = write_binary_file(options.input_file, options.output_file);
		break;
	case PRINT_BINARY_ACT:
		/* Part C */
		ret_code = print_binary_file(options.input_file);
		break;
	default:
		break;
	}
	exit(ret_code);
}
