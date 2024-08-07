/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#ifndef INI_PARSER_H
#define INI_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <securec.h>
#include <limits.h>

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
static inline void str_to_lower(char *input_str, size_t buff_size);
static inline unsigned int remove_spaces(char *str, int len);
static inline ini_dictionary *new_ini_info_dict();
static inline void free_ini_info_dict(ini_dictionary *ini_info_dict);
static inline unsigned int hash_for_key(const char *key);
static inline const char *get_val_from_dict(const ini_dictionary *ini_info_dict,
    const char *key, const char *not_found);
static inline int modify_dict_val(ini_dictionary *ini_info_dict, const char *key,
    unsigned int key_hash, const char *val);
static inline int expand_space_of_dict(ini_dictionary *ini_info_dict);
static inline int add_val_to_dict(ini_dictionary *ini_info_dict, const char *key, unsigned int key_hash,
    const char *val);
static inline int set_val_for_dict(ini_dictionary *ini_info_dict, const char *key, const char *val);
static inline int find_ini_content(const ini_dictionary *ini_info_dict, const char *content);

/* External Interface */
static inline ini_dictionary *ini_process_file(const char *ini_file_name);
static inline int ini_get_num_of_keys(const ini_dictionary *ini_info_dict, const char *section_name);
static inline const char **ini_get_keys_from_sec(const ini_dictionary *ini_info_dict,
    const char *section_name, const char **keys);
static inline const char *ini_get_string(const ini_dictionary *ini_info_dict, const char *key,
    const char *not_found);
static inline int ini_get_bool(const ini_dictionary *ini_info_dict, const char *key, int not_found);
static inline long int ini_get_long_int(const ini_dictionary *ini_info_dict, const char *key, long int not_found);
static inline int ini_get_int(const ini_dictionary *ini_info_dict, const char *key, int not_found);
static inline double ini_get_double(const ini_dictionary *ini_info_dict, const char *key, double not_found);
static inline error_id ini_get_error_id();

#define MINSIZE 128  /* original step size for dictionary expansion */
#define MAXSIZE 8192 /* Maximum number of single line */
#define DOUBLE  2 /* Store dictinfo needs to increase the multiple of memory */
#define NOTFOUND ((char *) -1)
static error_id g_error_id;

/* Change string value from uppercase to all lowercase */
static inline void str_to_lower(char *input_str, size_t buff_size)
{
    unsigned int res_len = 0;
    if (input_str == NULL) {
        return;
    }
    size_t str_len = strnlen(input_str, buff_size);
    while (res_len < str_len && res_len < buff_size && input_str[res_len] != '\0') {
        input_str[res_len] = (char) tolower((int)input_str[res_len]);
        res_len++;
    }
}

/* Remove the beginning and last spaces of the string, and return the length of the string after removal */
static inline unsigned int remove_spaces(char *str, int len)
{
    unsigned int res;
    char *end_line = NULL;
    char *rem_res = str;
    int ret = 0;

    if (len < 0 || str == NULL) {
        return -1;
    }
    end_line = rem_res + strlen(rem_res);
    while (isspace((int)*rem_res)) {
        rem_res++;
    }
    /* Find non-blank characters from back to front */
    while (end_line > rem_res) {
        if (!isspace((int) *(end_line - 1))) { /* Stop if it's not a space */
            break;
        }
        end_line--;
    }
    *end_line = '\0';
	/* The destination buffer must be greater than or equal to the source buffe */
    if ((long)(strlen(str) + 1) < (end_line - rem_res + 1)) {
        printf("destination buffer samller than source buffer!\n");
        return -1;
    } else {
        ret = memmove_s(str, strlen(str) + 1, rem_res, end_line - rem_res + 1);
        if (ret != EOK) {
            printf("remove_spaces fail,ret:0x%x\n", ret);
            return -1;
        }
    }

    res = (unsigned int)(end_line - rem_res);
    return res;
}

/*
 * Function description: Create a new ini information object
 * Parameters: dictSize The size of the dictionary to be allocated
 */
static inline ini_dictionary *new_ini_info_dict()
{
    ini_dictionary *dict;
    dict = (ini_dictionary *)calloc(1, sizeof(ini_dictionary));
    if (dict == NULL) {
        g_error_id = ERROR_NEWDICT;
        return NULL;
    }

    dict->n = 0;
    dict->size = MINSIZE; /* Original step size for dictionary memory expansion */
    dict->key = (char **)calloc(dict->size, sizeof(char *));
    if (dict->key == NULL) {
        g_error_id = ERROR_NEWDICT;
        free(dict);
        return NULL;
    }
    dict->val = (char **)calloc(dict->size, sizeof(char *));
    if (dict->val == NULL) {
        g_error_id = ERROR_NEWDICT;
        free(dict->key);
        free(dict);
        return NULL;
    }
    dict->hash = (unsigned int *)calloc(dict->size, sizeof(unsigned int));
    if (dict->hash == NULL) {
        g_error_id = ERROR_NEWDICT;
        free(dict->val);
        free(dict->key);
        free(dict);
        return NULL;
    }
    return dict;
}

/* delete ini info object */
static inline void free_ini_info_dict(ini_dictionary *ini_info_dict)
{
    if (ini_info_dict == NULL) {
        return;
    }
    unsigned int i;
    free(ini_info_dict->hash);
    for (i = 0; i < ini_info_dict->size; i++) {
        if (ini_info_dict->val[i] != NULL) {
            free(ini_info_dict->val[i]);
            ini_info_dict->val[i] = NULL;
        }
        if (ini_info_dict->key[i] != NULL) {
            free(ini_info_dict->key[i]);
            ini_info_dict->key[i] = NULL;
        }
    }
    free(ini_info_dict->val);
    free(ini_info_dict->key);
    free(ini_info_dict);
    return;
}

/* Calculate the hash value of the string key */
static inline unsigned int hash_for_key(const char *key)
{
    size_t key_len;
    unsigned int res = 0;
    size_t i;
    int hash_val_1 = 10;
    int hash_val_2 = 3;
    if (key == NULL) {
        return 0;
    }
    key_len = strlen(key);
    /* res = (key + (key << 10)) ^ ((key + (key << 10)) >> 6) */
    for (i = 0; i < key_len; i++) {
        res += (unsigned int)key[i];
        res += (res << hash_val_1);
        res ^= (res >> 6); /* (res >> 6) */
    }
    /* res = (res + (res << 3)) ^ ((res + (res << 3)) >> 11)
            + ((res + (res << 3)) ^ ((res + (res << 3)) >> 11) << 15) */
    res += (res << hash_val_2); /* (res << 3) */
    res ^= (res >> (hash_val_1 + 1)); /* (res >> 11) */
    res += (res << (hash_val_1 + 5)); /* (res << 15) */
    return res;
}

/*
 * Function description: Get the value of the corresponding Key
 * Parameters: ini_info_dict The ini information dictionary to look up
               key  key value to look up
               not_found Default value to return when not found
 * Return value: val value corresponding to key
 */
static inline const char *get_val_from_dict(const ini_dictionary *ini_info_dict, const char *key, const char *not_found)
{
    unsigned int key_hash;
    unsigned int i;
    if (ini_info_dict == NULL || key == NULL) {
        return not_found;
    }
    key_hash = hash_for_key(key);
    for (i = 0; i < ini_info_dict->size; i++) {
        if (ini_info_dict->key[i] == NULL) {
            continue;
        }
        if (key_hash == ini_info_dict->hash[i]) {
            if (strcmp(ini_info_dict->key[i], key) == 0) {
                return ini_info_dict->val[i];
            }
        }
    }
    return not_found;
}

/* Modify the value of val */
static inline int modify_dict_val(ini_dictionary *ini_info_dict, const char *key,
    unsigned int key_hash, const char *val)
{
    unsigned int i;
    if (ini_info_dict == NULL || key == NULL || val == NULL) {
        return -1;
    }
    for (i = 0; i < ini_info_dict->size; i++) {
        if (ini_info_dict->key[i] == NULL) {
            continue;
        }
        /* When the hash is the same, modify it */
        if (key_hash == ini_info_dict->hash[i] && strcmp(key, ini_info_dict->key[i]) == 0) {
            if (ini_info_dict->val[i] != NULL) { /* First free the value pointed to by the previous pointer */
                free(ini_info_dict->val[i]);
                ini_info_dict->val[i] = NULL;
            }
            if (val != NULL) {
                ini_info_dict->val[i] = strdup(val);
            }
            return 0; /* value successfully modified */
        }
    }
    return -1;
}

/* Increase the size of the current dictionary */
static inline int expand_space_of_dict(ini_dictionary *ini_info_dict)
{
    char **new_dict_val;
    char **new_dict_key;
    unsigned int *new_dict_hash = NULL;
    int dsize = 0;
    int n = 2; /* Indicates the multiple of expansion */
    int ret0;
    int ret1;
    int ret2;
    if (ini_info_dict == NULL) {
        return -1;
    }
    dsize = ini_info_dict->size;
    new_dict_val = (char **)calloc(dsize * n, sizeof(*ini_info_dict->val));
    if (new_dict_val == NULL) {
        return -1;
    }
    new_dict_key = (char **)calloc(dsize * n, sizeof(*ini_info_dict->key));
    if (new_dict_key == NULL) {
        free(new_dict_val);
        return -1;
    }
    new_dict_hash = (unsigned int *)calloc(dsize * n, sizeof(*ini_info_dict->hash));
    if (new_dict_hash == NULL) {
        free(new_dict_val);
        free(new_dict_key);
        return -1;
    }
    /* The destination buffer size must be larger than the source buffer size */
    ret0 = memcpy_s(new_dict_val, dsize * n * sizeof(*ini_info_dict->val), ini_info_dict->val, dsize * sizeof(char *));
    ret1 = memcpy_s(new_dict_key, dsize * n * sizeof(*ini_info_dict->key), ini_info_dict->key, dsize * sizeof(char *));
    ret2 = memcpy_s(new_dict_hash, dsize * n * sizeof(*ini_info_dict->hash), ini_info_dict->hash,
        dsize * sizeof(unsigned int));
    if (ret0 != EOK || ret1 != EOK || ret2 != EOK) {
        free(new_dict_val);
        free(new_dict_key);
        free(new_dict_hash);
    }
    free(ini_info_dict->val);
    free(ini_info_dict->key);
    free(ini_info_dict->hash);
    ini_info_dict->size = dsize * n;
    ini_info_dict->val = new_dict_val;
    ini_info_dict->key = new_dict_key;
    ini_info_dict->hash = new_dict_hash;
    return 0;
}

/* Add the value of val to the dictionary */
static inline int add_val_to_dict(ini_dictionary *ini_info_dict, const char *key,
    unsigned int key_hash, const char *val)
{
    int insert_position;
    if (ini_info_dict == NULL || key == NULL || val == NULL) {
        return -1;
    }
    insert_position = ini_info_dict->n;
    if (insert_position >= (int)ini_info_dict->size) {
        if (expand_space_of_dict(ini_info_dict) != 0) {
            g_error_id = ERROR_EXPANDDICT;
            return -1;
        }
    }
    /* Find a place to insert */
    while (ini_info_dict->key[insert_position] != NULL) {
        insert_position++;
        if (insert_position == (int)ini_info_dict->size) {
            insert_position = 0;
        }
    }
    if (key != NULL) {
        ini_info_dict->key[insert_position] = strdup(key);
    }
    if (val != NULL) {
        ini_info_dict->val[insert_position] = strdup(val);
    } else {
        ini_info_dict->val[insert_position] = NULL;
    }
    ini_info_dict->hash[insert_position] = key_hash;
    ini_info_dict->n++;
    return 0;
}

/*
 * Function description: Set the value of the corresponding key
 * Parameters: ini_info_dict The ini information dictionary to be modified
               key The key value to be modified
               val value to set
 * Return value: 0 for success, -1 for failure
 */
static inline int set_val_for_dict(ini_dictionary *ini_info_dict, const char *key, const char *val)
{
    if (ini_info_dict == NULL || key == NULL) {
        g_error_id = ERROR_NULLPOINT;
        return -1;
    }
    unsigned int key_hash;
    key_hash = hash_for_key(key);
    if (ini_info_dict->n < 0) {
        return -1;
    }
    /* If there is a value in the current dictionary */
    if (ini_info_dict->n > 0) {
        /* If there is this key, it will return if the modification is successful */
        if (modify_dict_val(ini_info_dict, key, key_hash, val) == 0) {
            return 0;
        }
    }
    if (val != NULL) {
        if (add_val_to_dict(ini_info_dict, key, key_hash, val) != 0) {
            g_error_id = ERROR_ADDVAL;
            return -1;
        }
    }
    return 0;
}

/* Load a line from an Ini file, return the state of that line */
static inline file_line_status process_line(const char *fline, char *fsection, char *fkey, char *fvalue, int buff_size)
{
    file_line_status res = FILELINE_EMPTY;
    char *tmp_line = NULL;
    if (fline == NULL) {
        return res;
    }
    tmp_line = strdup(fline);
    if (tmp_line == NULL) {
        return res;
    }
    unsigned int line_len = remove_spaces(tmp_line, sizeof(char) * (MAXSIZE + 1));
    int num = 2; /* The number of formatted copies should be 2 */
    if (line_len < 1) {
        res = FILELINE_EMPTY;
    } else if (tmp_line[0] == '#' || tmp_line[0] == ';') {
        res = FILELINE_COMMENT;
    } else if (tmp_line[0] == '[' && tmp_line[line_len - 1] == ']') {
        int sec_ret = sscanf_s(tmp_line, "[%[^]]", fsection, buff_size);
        if (sec_ret != 1) {
            g_error_id = ERROR_SECFUNC;
            return res;
        }
        res = FILELINE_SECTION;
        remove_spaces(fsection, sizeof(char) * (MAXSIZE + 1));
        str_to_lower(fsection, buff_size);
    } else if (sscanf_s(tmp_line, "%[^=] = '%[^\']'", fkey, buff_size, fvalue, buff_size) == num ||
                sscanf_s(tmp_line, "%[^=] = \"%[^\"]\"", fkey, buff_size, fvalue, buff_size) == num) {
        /* Common quoted key=value */
        res = FILELINE_VALUE;
        remove_spaces(fkey, sizeof(char) * (MAXSIZE + 1));
        str_to_lower(fkey, buff_size);
    } else if (sscanf_s(tmp_line, "%[^=] = %[^;#]", fkey, buff_size, fvalue, buff_size) == num) {
        /* common unquoted key=value */
        res = FILELINE_VALUE;
        remove_spaces(fkey, sizeof(char) * (MAXSIZE + 1));
        str_to_lower(fkey, buff_size);
        remove_spaces(fvalue, sizeof(char) * (MAXSIZE + 1));
        /* When value is empty, i.e. '' or "" */
        if (strcmp(fvalue, "\"\"") == 0 || (strcmp(fvalue, "''") == 0)) {
            fvalue[0] = 0;
        }
    } else if (sscanf_s(tmp_line, "%[^=] %[=]", fkey, buff_size, fvalue, buff_size) == num ||
               sscanf_s(tmp_line, "%[^=] = %[;#]", fkey, buff_size, fvalue, buff_size) == num) {
        /* Special case: key=; key=# key= */
        res = FILELINE_VALUE;
        remove_spaces(fkey, sizeof(char) * (MAXSIZE + 1));
        str_to_lower(fkey, buff_size);
        fvalue[0] = 0;
    } else {
        res = FILELINE_ERROR;
    }
    free(tmp_line);
    return res;
}


/* Find if something is in a dictionary */
static inline int find_ini_content(const ini_dictionary *ini_info_dict, const char *content)
{
    int is_found = 0;
    if (ini_info_dict == NULL || content == NULL) {
        g_error_id = ERROR_NULLPOINT;
        return -1;
    }
    if (ini_get_string(ini_info_dict, content, NOTFOUND) != NOTFOUND) {
        is_found = 1;
    }
    return is_found;
}

typedef struct {
    char fline[MAXSIZE + 1];
    char fsection[MAXSIZE + 1];
    char fkey[MAXSIZE + 1];
    char dict_info[(MAXSIZE * DOUBLE) + DOUBLE];
    char fval[MAXSIZE + 1];
    char path[PATH_MAX];
    int last_line;
    int line_nums;
    int line_errs;
    int mem_err;
    int line_len, sec_ret;
    FILE *ini_file;
    file_line_status line_sta;
} ini_file_ctx;

static inline void process_line_status(ini_file_ctx *ctx, ini_dictionary *ini_info_dict)
{
    switch (ctx->line_sta) {
        case FILELINE_COMMENT:
        case FILELINE_EMPTY:
            break;

        case FILELINE_SECTION:
            ctx->mem_err = set_val_for_dict(ini_info_dict, ctx->fsection, NULL);
            break;

        case FILELINE_VALUE:
            ctx->sec_ret = sprintf_s(ctx->dict_info, sizeof(ctx->dict_info), "%s:%s", ctx->fsection, ctx->fkey);
            if (ctx->sec_ret == -1) {
                g_error_id = ERROR_SECFUNC;
                ctx->line_errs++;
                break;
            }
            ctx->mem_err = set_val_for_dict(ini_info_dict, ctx->dict_info, ctx->fval);
            break;

        case FILELINE_ERROR:
            g_error_id = ERROR_FILELINE;
            ctx->line_errs++;
            break;

        default:
            break;
    }
}

static inline int alloc_ctx_and_open_file(ini_file_ctx **ctx, ini_dictionary **ini_info_dict, const char *ini_file_name)
{
    if (ini_file_name == NULL || ctx == NULL || ini_info_dict == NULL) {
        printf("NULL pointer\n");
        return -1;
    }
    *ctx = (ini_file_ctx *)calloc(sizeof(ini_file_ctx), 1);
    if (*ctx == NULL) {
        printf("malloc file context %zu fail\n", sizeof(ini_file_ctx));
        return -1;
    }

    *ini_info_dict = new_ini_info_dict();
    if (*ini_info_dict == NULL) {
        printf("create dictionary fail\n");
        free(*ctx);
        return -1;
    }

    if (realpath(ini_file_name, (*ctx)->path) == NULL) {
        free_ini_info_dict(*ini_info_dict);
        free(*ctx);
        return -1;
    }

    (*ctx)->ini_file = fopen((*ctx)->path, "r");
    if ((*ctx)->ini_file == NULL) {
        printf("open ini file %s failed\n", ini_file_name);
        free_ini_info_dict(*ini_info_dict);
        free(*ctx);
        return -1;
    }
    return 0;
}
static inline void release_file_ctx(ini_file_ctx *ctx)
{
    (void)fclose(ctx->ini_file);
    free(ctx);
}

/* Parse the ini file and return the allocated information dictionary object */
static inline ini_dictionary *ini_process_file(const char *ini_file_name)
{
    ini_dictionary *ini_info_dict = NULL;
    ini_file_ctx *ctx = NULL;

    int ret = alloc_ctx_and_open_file(&ctx, &ini_info_dict, ini_file_name);
    ini_check_ret_with_return(ret, NULL, "alloc_ctx_and_open_file");

    while (fgets(ctx->fline + ctx->last_line, MAXSIZE - ctx->last_line, ctx->ini_file) != NULL) {
        ctx->line_nums++;
        ctx->line_len = (int)strlen(ctx->fline) - 1;
        if (ctx->line_len <= 0) {
            continue;
        } else if (ctx->line_len >= (MAXSIZE + 1)) {
            g_error_id = ERROR_FILELINE;
            goto fail_out;
        }

        if (ctx->fline[ctx->line_len] != '\n' && !feof(ctx->ini_file)) { /* Buffer Overflow Safety Check */
            g_error_id = ERROR_FILELINE;
            goto fail_out;
        }
        if (ctx->fline[ctx->line_len] == '\n') {
            ctx->fline[ctx->line_len--] = 0;
        }
        if (ctx->fline[ctx->line_len] == '\\') { /* Check if there are multiple lines */
            ctx->last_line = ctx->line_len;
            continue;
        } else {
            ctx->last_line = 0;
        }

        ctx->line_sta = process_line(ctx->fline, ctx->fsection, ctx->fkey, ctx->fval, MAXSIZE + 1);
        process_line_status(ctx, ini_info_dict);

        (void)memset_s(ctx->fline, sizeof(ctx->fline), 0, sizeof(ctx->fline));
        if (ctx->mem_err < 0) { /* If the entire Ini file has an incorrect set value, the entire file is skipped */
            g_error_id = ERROR_FILEERROR;
            break;
        }
        if ((MAXSIZE - ctx->last_line) < 0 || (MAXSIZE - ctx->last_line) > (MAXSIZE + 1)) {
            g_error_id = ERROR_FILELINE;
            goto fail_out;
        }
    }
    if (ctx->line_errs > 0) {
        free_ini_info_dict(ini_info_dict);
        ini_info_dict = NULL;
    }
    release_file_ctx(ctx);
    return ini_info_dict;
fail_out:
    free_ini_info_dict(ini_info_dict);
    release_file_ctx(ctx);
    return NULL;
}

/*
 * Function description: Get the number of keys in a section of the dictionary.
 * Parameters: ini_info_dict ini information pointer to be counted
               section_name name of section
 */
static inline int ini_get_num_of_keys(const ini_dictionary *ini_info_dict, const char *section_name)
{
    int num_keys = 0;
    int sec_len;
    char sec_low_name[MAXSIZE + 1];
    unsigned int i = 0;
    if (ini_info_dict == NULL || section_name == NULL) {
        g_error_id = ERROR_NULLPOINT;
        return num_keys;
    }
    if (find_ini_content(ini_info_dict, section_name) == 0) {
        return num_keys; /* If there is no such section, return 0 */
    }
    /* make section_name lowercase */
    while (i < MAXSIZE && section_name[i] != '\0') {
        sec_low_name[i] = (char)tolower((int)section_name[i]);
        i++;
    }
    sec_len = (int)strlen(section_name);
    sec_low_name[sec_len] = ':';
    /* Count how many keys the section has */
    for (i = 0; i < ini_info_dict->size; i++) {
        if (ini_info_dict->key[i] == NULL) {
            continue;
        }
        if (strncmp(ini_info_dict->key[i], sec_low_name, sec_len + 1) == 0) {
            num_keys++;
        }
    }
    return num_keys;
}

/*
 * Function description: Query the ini information dictionary to find all the keys in the given section
 *  Parameters: ini_info_dict ini information pointer to be counted
                section_name name of section
                keys secondary pointer to store all keys in the section
 */
static inline const char **ini_get_keys_from_sec(const ini_dictionary *ini_info_dict,
    const char *section_name, const char **keys)
{
    if (ini_info_dict == NULL || section_name == NULL || keys == NULL) {
        g_error_id = ERROR_NULLPOINT;
        return NULL;
    }
    if (find_ini_content(ini_info_dict, section_name) == 0) {
        return NULL;
    }

    int nums = 0;
    int sec_len = (int)strlen(section_name);
    char tmp_low_sec[MAXSIZE + 1];
    unsigned int i = 0;
    while (i < MAXSIZE && section_name[i] != '\0') {
        tmp_low_sec[i] = (char)tolower((int)section_name[i]);
        i++;
    }
    tmp_low_sec[sec_len] = ':';
    for (i = 0; i < ini_info_dict->size; i++) {
        if (ini_info_dict->key[i] == NULL) {
            continue;
        }
        if (strncmp(ini_info_dict->key[i], tmp_low_sec, sec_len + 1) == 0) {
            keys[nums] = ini_info_dict->key[i];
            nums++;
        }
    }
    return keys;
}

/* Get the string associated with the key value */
static inline const char *ini_get_string(const ini_dictionary *ini_info_dict, const char *key, const char *not_found)
{
    const char *res = NULL;
    const char *tmp_key;
    char tmp_low_key[MAXSIZE + 1];
    unsigned int i = 0;
    if (ini_info_dict == NULL || key == NULL) {
        g_error_id = ERROR_NULLPOINT;
        return NULL;
    }
    while (i < MAXSIZE && key[i] != '\0') {
        tmp_low_key[i] = (char)tolower((int)key[i]);
        i++;
    }
    tmp_low_key[i] = '\0';
    tmp_key = tmp_low_key;
    res = get_val_from_dict(ini_info_dict, tmp_key, not_found);
    return res;
}

/* Get the string associated with the key and convert it to a boolean */
static inline int ini_get_bool(const ini_dictionary *ini_info_dict, const char *key, int not_found)
{
    const char *val_str;
    int res;
    if (ini_info_dict == NULL || key == NULL) {
        g_error_id = ERROR_NULLPOINT;
        return not_found;
    }
    val_str = ini_get_string(ini_info_dict, key, NOTFOUND);
    if (val_str == NOTFOUND) {
        return not_found;
    }
    if (val_str[0] == 'T' || val_str[0] == 't' || val_str[0] == 'Y' || val_str[0] == 'y' || val_str[0] == '1') {
        res = 1;  /* returns 1 for true */
    } else if (val_str[0] == 'F' || val_str[0] == 'f' || val_str[0] == 'N' || val_str[0] == 'n' || val_str[0] == '0') {
        res = 0; /* returns 0 for false */
    } else {
        res = not_found; /* Otherwise, it is counted as not found */
    }
    return res;
}

/* Get the string associated with the key and convert it to long int */
static inline long int ini_get_long_int(const ini_dictionary *ini_info_dict, const char *key, long int not_found)
{
    long int res;
    const char *val_str;
    if (ini_info_dict == NULL || key == NULL) {
        g_error_id = ERROR_NULLPOINT;
        return not_found;
    }
    val_str = ini_get_string(ini_info_dict, key, NOTFOUND);
    if (val_str == NOTFOUND) {
        return not_found;
    }
    res = strtol(val_str, NULL, 0);
    return res;
}

/* Get the string associated with the key, convert to int */
static inline int ini_get_int(const ini_dictionary *ini_info_dict, const char *key, int not_found)
{
    long int val_long;
    if (ini_info_dict == NULL || key == NULL) {
        g_error_id = ERROR_NULLPOINT;
        return not_found;
    }
    val_long = ini_get_long_int(ini_info_dict, key, not_found);
    return (int)val_long;
}

/* Get the string associated with key and convert it to double */
static inline double ini_get_double(const ini_dictionary *ini_info_dict, const char *key, double not_found)
{
    const char *val_str;
    double res;
    if (ini_info_dict == NULL || key == NULL) {
        g_error_id = ERROR_NULLPOINT;
        return not_found;
    }
    val_str = ini_get_string(ini_info_dict, key, NOTFOUND);
    if (val_str == NOTFOUND) {
        return not_found;
    }
    res = strtod(val_str, NULL);
    return res;
}

static inline error_id ini_get_error_id()
{
    return g_error_id;
}

#endif
