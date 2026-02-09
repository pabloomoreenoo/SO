#include <stdio.h>
#include <unistd.h> /* for getopt() */
#include <stdlib.h> /* for EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>
#include "defs.h"

/**
 * @brief  Make a copy of existing string allocating memory accordingly
 *
 * @param original
 * @return new string that is a clone of original
 **/
static inline char* clone_string(char* original)
{
	char* copy;
	copy=malloc(strlen(original)+1);
	strcpy(copy,original);
	return copy;
}


/**
 * @brief Parse /etc/passwd file and returns its contents as an array of entries.
 * The number of entries in the array is returned vie the nr_entries parameter.
 *
 * @param options
 * @param nr_entries
 * @return array of entries (passwd_entry_t*) and entry count (nr_entries parameter)
 */
passwd_entry_t* parse_passwd(struct options* options, int* nr_entries)
{
	FILE* passwd;
	char line[MAX_PASSWD_LINE+1];
	passwd_entry_t* entries;
	passwd_entry_t* cur_entry;
	int line_count=0;
	char* token;
	char* lineptr;
	token_id_t token_id;
	int entry_idx;
	int entry_count=0;
	int cur_line;

	// al modificarlo ponemos options-> porque es un puntero
	if ((passwd=fopen(options->input_file,"r"))==NULL) {    // (1) apertura del archivo con permiso de lectura
		fprintf(stderr, "/etc/passwd could not be opened: ");
		perror(NULL);
		return NULL;
	}

	/* Figure out number of lines */
	while (fgets(line, MAX_PASSWD_LINE + 1, passwd) != NULL){ // (2) en este bucle contamos las lineas
		line_count++;
		/* Discard lines that begin with # */
		if (line[0]!='#')
			entry_count++;
	}

	/* Rewind position indicator*/
	fseek(passwd,0,SEEK_SET);   // (3) Una vez contadas las lineas rebobinamos al principio

	entries=malloc(sizeof(passwd_entry_t)*entry_count);  // (4) reservar la memoria necesaria
	/* zero fill the array of structures */
	memset(entries,0,sizeof(passwd_entry_t)*entry_count);

	/* Parse file */
	entry_idx=0;  // contador de la entrada actual, la inicializamos a 0
	cur_line=1;
	while (fgets(line, MAX_PASSWD_LINE + 1, passwd) != NULL) {  // (5) en este segundo bucle procesamos las lineas
		
		/* Discard lines that begin with # */
		if (line[0]=='#'){  // (6) descartamos las lineas de comentario
			cur_line++;
			continue;
		}

		/* Point to the beginning of the line */
		lineptr=line;  // (7) inicializamos lineptr
		token_id=LOGIN_NAME_IDX; // (8) inicializar token_id
		cur_entry=&entries[entry_idx]; // (9) apuntar a la entrada actual  

		while((token = strsep(&lineptr, ":"))!=NULL) {   // (10) bucle interno para el strsep
			switch(token_id) {         // (11) switch case para los casos
			case LOGIN_NAME_IDX:
				strcpy(cur_entry->login_name,token);
				break;
			case ENCRYPTED_PASS_IDX:
				cur_entry->optional_encrypted_passwd=strdup(token);
				break;
			case UID_IDX:
				if (sscanf(token,"%d",&cur_entry->uid)!=1) {  // (12) sscanf para los enteros
					fprintf(stderr, "Warning: Wrong format for UID field in line %d of /etc/passwd\n",cur_line);
					cur_entry->uid=0;
				}

				break;
			case GID_IDX:
				if (sscanf(token,"%d",&cur_entry->gid)!=1) {
					fprintf(stderr, "Warning: Wrong format for GID field in line %d of /etc/passwd\n",cur_line);
					cur_entry->gid=0;
				}
				break;
			case USER_NAME_IDX:
				cur_entry->user_name=strdup(token);
				break;
			case USER_HOME_IDX:
				cur_entry->user_home=strdup(token);
				break;
			case USER_SHELL_IDX:
				/* remove new line from token */
				token[strlen(token)-1]='\0';   // (13) eliminar salto de linea
				cur_entry->user_shell=strdup(token);
				break;
			default:
				break;
			}
			token_id++;      // (14) incrementar token_id
		}
		if (token_id!=NR_FIELDS_PASSWD) {     // (15) comprobacion de errores
			fprintf(stderr, "Could not process all tokens from line %d of /etc/passwd\n",entry_idx+1);
			return NULL;
		}
		cur_line++;   // (16) incrementar contadores
		entry_idx++;
	}
	(*nr_entries)=entry_count;  // (17) devolver el numero de entradas
	return entries;           // (18) devolver el array de entradas
}

/**
 * @brief Free up the array of entries, including the 
 * memory of dynamically allocated strings
 * 
 * @param entries 
 * @param nr_entries 
 */
static void free_entries(passwd_entry_t* entries, int nr_entries)
{                                                      // su objetivo es liberar toda la memoria que se reservo dinamicamente 
	int i=0;                                          // para la funcion parse_passwd
	passwd_entry_t* entry;

	for (i=0; i<nr_entries; i++) {
		entry=&entries[i]; /* Point to current entry */
		free(entry->optional_encrypted_passwd);
		free(entry->user_name);
		free(entry->user_home);
		free(entry->user_shell);
	}

	free(entries);
}

/**
 * @brief This function takes care of invoking the parser, 
 * and then displays the contents of /etc/passwd based on the 
 * selected output mode
 * 
 * @param options 
 * @return int 
 */
static int show_passwd(struct options* options)    // llama a parse_passwd para obtener los datos del archivo /etc/passwd
{                                                 // imprime los datos en la salida especificada y libera la memoria
	int nr_entries;
	int i;
	passwd_entry_t* entries=parse_passwd(options,&nr_entries);

	if (!entries)
		return EXIT_FAILURE;

	for (i=0; i<nr_entries; i++) {
		passwd_entry_t* e=&entries[i]; /* Point to current entry */
		switch (options->output_mode) {
		case VERBOSE_MODE:
			fprintf(options->outfile,"[Entry #%d]\n",i);
			fprintf(options->outfile,"\tlogin=%s\n\tenc_pass=%s\n\t"
			        "uid=%d\n\tgid=%d\n\tuser_name=%s\n\t"
			        "home=%s\n\tshell=%s\n",
			        e->login_name, e->optional_encrypted_passwd,
			        e->uid, e->gid,e->user_name,
			        e->user_home, e->user_shell);
			break;
		case PIPE_MODE:
			fprintf(options->outfile,"|%s|%s|%d|%d|%s|%s|%s|\n",
			        e->login_name, e->optional_encrypted_passwd,
			        e->uid, e->gid,e->user_name,
			        e->user_home, e->user_shell);
			break; 
		case CSV_MODE:
			fprintf(options->outfile, "%s,%s,%d,%d,%s,%s,%s\n",
				e->login_name, e->optional_encrypted_passwd,
				e->uid, e->gid, e->user_name,
				e->user_home, e->user_shell);
		}

	}
	free_entries(entries,nr_entries);
	return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
	int retCode, opt;
	struct options options;

	/* Initialize default values for options */
	options.outfile=stdout;
	options.output_mode=VERBOSE_MODE;
	options.input_file = "/etc/passwd";  // inicializamos al valor por defecto

	/* Parse command-line options */
	while((opt = getopt(argc, argv, "hvpo:i:c")) != -1) {
		switch(opt) {
		case 'h':
			fprintf(stderr,"Usage: %s [ -h | -v | -p | -o <output_file> | -i <input_file> | -c]\n",argv[0]);
			exit(0);
		case 'v':
			options.output_mode=VERBOSE_MODE;
			break;
		case 'p':
			options.output_mode=PIPE_MODE;
			break;
		case 'o':
			if ((options.outfile=fopen(optarg,"wx"))==NULL) {
				fprintf(stderr, "The output file %s could not be opened: ",
				        optarg);
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			break;
		case 'i':

			options.input_file = optarg; 	

		    /*if((options.input_file=fopen(optarg, "r")) == NULL){
				fprintf(stderr, "The input file %s could not be opened: ",
					optarg);
			perror(NULL);
			exit(EXIT_FAILURE);
			}*/                     // no hace falta porque ya hace esta comprobacion en la funcion parse_passwd, pero bien planteado

			break;
		case 'c':
			options.output_mode = CSV_MODE; 
			break; 

		default:
			exit(EXIT_FAILURE);
		}
	}

	retCode=show_passwd(&options);
	exit(retCode);
}
