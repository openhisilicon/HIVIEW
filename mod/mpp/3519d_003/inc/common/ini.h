/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef INI_H_INCLUDED
#define INI_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ini_check_ret_with_return(ret, result, info) do { \
    if ((ret) != 0) { \
        printf("%s failed, ret %d\n\n", (info), (ret)); \
        return (result); \
    } \
} while (0)

typedef struct {
    int             n;     /* number of dictionary entries */
    unsigned int size;  /* storage size */
    char        **key;   /* list of string keys */
    char        **val;   /* list of string values */
    unsigned int *hash;  /* list of hash values for key */
} ini_dictionary;

typedef enum {
    FILELINE_EMPTY,  /* The line is empty */
    FILELINE_ERROR,  /* There is an error on the line */
    FILELINE_UNPROCESSED, /* The line has not been processed */
    FILELINE_COMMENT,  /* the line is a comment */
    FILELINE_SECTION,  /* This line is the section name */
    FILELINE_VALUE  /* The row has value */
} file_line_status;

typedef enum {
    ERROR_SECFUNC, /* Failed to execute safe function */
    ERROR_NULLPOINT, /* The input parameter has a null pointer */
    ERROR_NEWDICT, /* Failed to create new dictionary pointer */
    ERROR_EXPANDDICT, /* Failed to expand dictionary size */
    ERROR_ADDVAL, /* Failed to add value to dictionary */
    ERROR_OPENFILE, /* fail to open the file */
    ERROR_FILELINE, /* There is an error in a line of the file */
    ERROR_FILEERROR /* A line of the file is not standardized,not be processed */
} error_id;

/* Internal interface */
void str_to_lower(char *input_str);
unsigned int remove_spaces(char *str, int len);
ini_dictionary *new_ini_info_dict();
void free_ini_info_dict(ini_dictionary *ini_info_dict);
unsigned int hash_for_key(const char *key);
const char *get_val_from_dict(const ini_dictionary *ini_info_dict, const char *key, const char *not_found);
int modify_dict_val(ini_dictionary *ini_info_dict, const char *key, unsigned int key_hash, const char *val);
int expand_space_of_dict(ini_dictionary *ini_info_dict);
int add_val_to_dict(ini_dictionary *ini_info_dict, const char *key, unsigned int key_hash, const char *val);
int set_val_for_dict(ini_dictionary *ini_info_dict, const char *key, const char *val);
int find_ini_content(const ini_dictionary *ini_info_dict, const char *content);

/* External Interface */
ini_dictionary *ini_process_file(const char *ini_file_name);
int ini_get_num_of_keys(const ini_dictionary *ini_info_dict, const char *section_name);
const char **ini_get_keys_from_sec(const ini_dictionary *ini_info_dict,
    const char *section_name, const char **keys);
const char *ini_get_string(const ini_dictionary *ini_info_dict, const char *key, const char *not_found);
int ini_get_bool(const ini_dictionary *ini_info_dict, const char *key, int not_found);
long int ini_get_long_int(const ini_dictionary *ini_info_dict, const char *key, long int not_found);
int ini_get_int(const ini_dictionary *ini_info_dict, const char *key, int not_found);
double ini_get_double(const ini_dictionary *ini_info_dict, const char *key, double not_found);
error_id ini_get_error_id();

#ifdef __cplusplus
}
#endif

#endif

