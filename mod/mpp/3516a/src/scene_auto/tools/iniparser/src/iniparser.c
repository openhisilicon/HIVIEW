
/*-------------------------------------------------------------------------*/
/**
   @file    iniparser.c
   @author  N. Devillard
   @brief   Parser for ini files.
*/
/*--------------------------------------------------------------------------*/
/*---------------------------- Includes ------------------------------------*/
#include <ctype.h>
#include <stdio.h>
#include <syslog.h>
#include "iniparser.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*---------------------------- Defines -------------------------------------*/
#define ASCIILINESZ         (8192)
#define INI_INVALID_KEY     ((char*)-1)
#define CNOSK               0xFFFF

/*---------------------------------------------------------------------------
                        Private to this module
 ---------------------------------------------------------------------------*/
/**
 * This enum stores the status for each parsed line (internal use only).
 */
typedef enum _line_status_
{
    LINE_UNPROCESSED,
    LINE_ERROR,
    LINE_EMPTY,
    LINE_COMMENT,
    LINE_SECTION,
    LINE_VALUE
} line_status ;


/*-------------------------------------------------------------------------*/
/**
  @brief    Skip blanks until the first non-blank character.
  @param    s   String to parse.
  @return   Pointer to char inside given string.

  This function returns a pointer to the first non-blank character in the
  given string.
 */
/*--------------------------------------------------------------------------*/

char *strskp(char *s)
{
    char *skip = s;
    if (s == NULL)
    {
        return NULL ;
    }
    while (isspace((int)*skip) && *skip)
    {
        skip++;
    }
    return skip ;
}


/*-------------------------------------------------------------------------*/
/**
  @brief    Remove blanks at the beginning and the end of a string.
  @param    str  String to parse and alter.
  @return   unsigned New size of the string.
 */
/*--------------------------------------------------------------------------*/
unsigned strstrip(char *s)
{
    char *last = NULL ;
    char *dest = s;

    if (s == NULL)
    {
        return 0;
    }

    last = s + strlen(s);
    while (isspace((int)*s) && *s)
    {
        s++;
    }
    while (last > s)
    {
        if (!isspace((int) * (last - 1)))
        {
            break ;
        }
        last -- ;
    }
    *last = (char)0;

    memmove(dest, s, last - s + 1);
    return last - s;
}

char *pseek = NULL;

int iniparser_add_entry(
    dictionary *d,
    const char *sec,
    const char *key,
    const char *val)
{
    char longkey[2 * ASCIILINESZ + 1];

    if ((d == NULL) || (sec == NULL))
    {
        return -1;
    }
    /* Make a key as section:keyword */
    if (key != NULL)
    {
        sprintf(longkey, "%s:%s", sec, key);
    }
    else
    {
        strcpy(longkey, sec);
    }
    /* Add (key,val) to dictionary */
    if (0 != dictionary_set(d, longkey, val))
    {
        return 1;
    }

    return 0;
}


/*-------------------------------------------------------------------------*/
/**
  @brief    Get number of sections in a dictionary
  @param    d   Dictionary to examine
  @return   int Number of sections found in dictionary

  This function returns the number of sections found in a dictionary.
  The test to recognize sections is done on the string stored in the
  dictionary: a section name is given as "section" whereas a key is
  stored as "section:key", thus the test looks for entries that do not
  contain a colon.

  This clearly fails in the case a section name contains a colon, but
  this should simply be avoided.

  This function returns -1 in case of error.
 */
/*--------------------------------------------------------------------------*/
int iniparser_getnsec(const dictionary *d)
{
    int i ;
    int nsec ;

    if (d == NULL)
    {
        return -1 ;
    }
    nsec = 0 ;
    for (i = 0 ; i < d->size ; i++)
    {
        if (d->key[i] == NULL)
        {
            continue ;
        }
        if (strchr(d->key[i], ':') == NULL)
        {
            nsec ++ ;
        }
    }
    return nsec ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get name for section n in a dictionary.
  @param    d   Dictionary to examine
  @param    n   Section number (from 0 to nsec-1).
  @return   Pointer to char string

  This function locates the n-th section in a dictionary and returns
  its name as a pointer to a string statically allocated inside the
  dictionary. Do not free or modify the returned string!

  This function returns NULL in case of error.
 */
/*--------------------------------------------------------------------------*/
const char *iniparser_getsecname(const dictionary *d, int n)
{
    int i ;
    int foundsec ;

    if (d == NULL || n < 0)
    {
        return NULL ;
    }
    foundsec = 0 ;
    for (i = 0 ; i < d->size ; i++)
    {
        if (d->key[i] == NULL)
        {
            continue ;
        }
        if (strchr(d->key[i], ':') == NULL)
        {
            foundsec++ ;
            if (foundsec > n)
            {
                break ;
            }
        }
    }
    if (foundsec <= n)
    {
        return NULL ;
    }
    return d->key[i] ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Dump a dictionary to an opened file pointer.
  @param    d   Dictionary to dump.
  @param    f   Opened file pointer to dump to.
  @return   void

  This function prints out the contents of a dictionary, one element by
  line, onto the provided file pointer. It is OK to specify @c stderr
  or @c stdout as output files. This function is meant for debugging
  purposes mostly.
 */
/*--------------------------------------------------------------------------*/
void iniparser_dump(const dictionary *d, FILE *f)
{
    int     i ;

    if (d == NULL || f == NULL)
    {
        return ;
    }
    for (i = 0 ; i < d->size ; i++)
    {
        if (d->key[i] == NULL)
        {
            continue ;
        }
        if (d->val[i] != NULL)
        {
            fprintf(f, "[%s]=[%s]\n", d->key[i], d->val[i]);
        }
        else
        {
            fprintf(f, "[%s]=UNDEF\n", d->key[i]);
        }
    }
    return ;
}

static unsigned char iniparser_save_comment(const dictionary *d, FILE *f, const unsigned int hash)
{
    int     ci;
    unsigned char flag = 0; /* comment line flag */

    for (ci = 0; ci < d->commSize; ci++)
    {
        if (d->commPlace[ci] == 0)
        {
            continue;
        }
        if (d->commHash[ci] == hash)
        {
            flag = 1;
            switch (d->commPlace[ci])
            {
                case 1:
                {
                    fprintf(f, "%s", d->comment[ci] ? d->comment[ci] : " ");
                    break;
                }
                case 2:
                {
                    fseek(f, -1, SEEK_CUR);
                    fprintf(f, "%s\n", d->comment[ci] ? d->comment[ci] : " ");
                    break;
                }
                case 3:
                {
                    fprintf(f, "%50s%s", " ", d->comment[ci] ? d->comment[ci] : " ");
                    break;
                }
                case 4:
                {
                    fprintf(f, "\r\n");
                    break;
                }
                /*case 5:
                {
                    fprintf(f, "%48s\n", " ");
                    break;
                }*/
                default:
                    break;
            }
        }
    }
    return flag;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Save a dictionary to a loadable ini file
  @param    d   Dictionary to dump
  @param    f   Opened file pointer to dump to
  @return   void

  This function dumps a given dictionary into a loadable ini file.
  It is Ok to specify @c stderr or @c stdout as output files.
 */
/*--------------------------------------------------------------------------*/
void iniparser_dump_ini(const dictionary *d, FILE *f)
{
    int          i ;
    int          nsec ;
    const char *secname ;
    if (d == NULL || f == NULL)
    {
        return ;
    }

    nsec = iniparser_getnsec(d);
    if (nsec < 1)
    {
        /* No section in file: dump all keys as they are */
        /* save comment and space Line*/
        iniparser_save_comment(d, f, CNOSK);

        for (i = 0 ; i < d->size ; i++)
        {
            if (d->key[i] == NULL)
            {
                continue ;
            }
            /*fprintf(f, "%s = %s\n", d->key[i], d->val[i]);*/
            /* save comment and space Line*/
            fprintf(f, "%-30s = \"%-15s\"\n", d->key[i], d->val[i]);
            fprintf(f, "%-30s = \"%-15s\"\n", d->key[i],
                    d->val[i] ? d->val[i] : " ");
            /* save comment and space Line*/
            iniparser_save_comment(d, f, d->hash[i]);
        }
        return ;
    }

    /* save comment and space Line*/
    iniparser_save_comment(d, f, CNOSK);

    for (i = 0 ; i < nsec ; i++)
    {
        secname = iniparser_getsecname(d, i) ;
        iniparser_dumpsection_ini(d, secname, f);
    }
    /*fprintf(f, "\n");*/
    return ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Save a dictionary section to a loadable ini file
  @param    d   Dictionary to dump
  @param    s   Section name of dictionary to dump
  @param    f   Opened file pointer to dump to
  @return   void

  This function dumps a given section of a given dictionary into a loadable ini
  file.  It is Ok to specify @c stderr or @c stdout as output files.
 */
/*--------------------------------------------------------------------------*/
void iniparser_dumpsection_ini(const dictionary *d, const char *s, FILE *f)
{
    int     j ;
    int hash;
    char    keym[ASCIILINESZ + 1];
    int     seclen ;

    if (d == NULL || f == NULL)
    {
        return ;
    }
    if (! iniparser_find_entry(d, s))
    {
        return ;
    }

    seclen  = (int)strlen(s);
    /*fprintf(f, "\n[%s]\n", s);*/

    fprintf(f, "[%-48s\n", s);
    fseek(f, 0 - (49 - strlen(s)), SEEK_CUR);
    fprintf(f, "%c", ']');

    fseek(f, 0, SEEK_END);
    /* save comment and space Line*/
    hash = dictionary_hash(s);
    iniparser_save_comment(d, f, hash);
    sprintf(keym, "%s:", s);
    for (j = 0 ; j < d->size ; j++)
    {
        if (d->key[j] == NULL)
        {
            continue ;
        }
        if (!strncmp(d->key[j], keym, seclen + 1))
        {
            fprintf(f,
                    "%-30s = \"%-15s\"\n",
                    d->key[j] + seclen + 1,
                    d->val[j] ? d->val[j] : "");
            /* save comment and space Line*/
            iniparser_save_comment(d, f, d->hash[j]);
        }
    }
    /*fprintf(f, "\n");*/
    return ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the number of keys in a section of a dictionary.
  @param    d   Dictionary to examine
  @param    s   Section name of dictionary to examine
  @return   Number of keys in section
 */
/*--------------------------------------------------------------------------*/
int iniparser_getsecnkeys(const dictionary *d, const char *s)
{
    int     seclen, nkeys ;
    char    keym[ASCIILINESZ + 1];
    int j ;

    nkeys = 0;

    if (d == NULL)
    {
        return nkeys;
    }
    if (! iniparser_find_entry(d, s))
    {
        return nkeys;
    }

    seclen  = (int)strlen(s);
    sprintf(keym, "%s:", s);

    for (j = 0 ; j < d->size ; j++)
    {
        if (d->key[j] == NULL)
        {
            continue ;
        }
        if (!strncmp(d->key[j], keym, seclen + 1))
        {
            nkeys++;
        }
    }

    return nkeys;

}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the number of keys in a section of a dictionary.
  @param    d    Dictionary to examine
  @param    s    Section name of dictionary to examine
  @param    keys Already allocated array to store the keys in
  @return   The pointer passed as `keys` argument or NULL in case of error

  This function queries a dictionary and finds all keys in a given section.
  The keys argument should be an array of pointers which size has been
  determined by calling `iniparser_getsecnkeys` function prior to this one.

  Each pointer in the returned char pointer-to-pointer is pointing to
  a string allocated in the dictionary; do not free or modify them.
 */
/*--------------------------------------------------------------------------*/
const char **iniparser_getseckeys(const dictionary *d, const char *s, const char **keys)
{
    int i, j, seclen ;
    char keym[ASCIILINESZ + 1];

    if (d == NULL || keys == NULL)
    {
        return NULL;
    }
    if (! iniparser_find_entry(d, s))
    {
        return NULL;
    }

    seclen  = (int)strlen(s);
    sprintf(keym, "%s:", s);

    i = 0;

    for (j = 0 ; j < d->size ; j++)
    {
        if (d->key[j] == NULL)
        {
            continue ;
        }
        if (!strncmp(d->key[j], keym, seclen + 1))
        {
            keys[i] = d->key[j];
            printf("keys[i]:%s\n", keys[i]);
            i++;
        }
    }

    return keys;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key
  @param    d       Dictionary to search
  @param    key     Key string to look for
  @param    def     Default value to return if key not found.
  @return   pointer to statically allocated character string

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the pointer passed as 'def' is returned.
  The returned char pointer is pointing to a string allocated in
  the dictionary, do not free or modify it.
 */
/*--------------------------------------------------------------------------*/
const char *iniparser_getstring(const dictionary *d, const char *key, const char *def)
{
    /*const char * lc_key ;*/
    const char *sval ;
    /*char tmp_str[ASCIILINESZ+1];*/

    if (d == NULL || key == NULL)
    {
        return def ;
    }

    /*lc_key = strlwc(key, tmp_str, sizeof(tmp_str));*/
    sval = dictionary_get(d, key, def);
    return sval ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key, convert to an int
  @param    d Dictionary to search
  @param    key Key string to look for
  @param    notfound Value to return in case of error
  @return   integer

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.

  Supported values for integers include the usual C notation
  so decimal, octal (starting with 0) and hexadecimal (starting with 0x)
  are supported. Examples:

  "42"      ->  42
  "042"     ->  34 (octal -> decimal)
  "0x42"    ->  66 (hexa  -> decimal)

  Warning: the conversion may overflow in various ways. Conversion is
  totally outsourced to strtol(), see the associated man page for overflow
  handling.

  Credits: Thanks to A. Becker for suggesting strtol()
 */
/*--------------------------------------------------------------------------*/
int iniparser_getint(const dictionary *d, const char *key, int notfound)
{
    const char *str ;

    str = iniparser_getstring(d, key, INI_INVALID_KEY);
    if (str == INI_INVALID_KEY)
    {
        return notfound ;
    }
    return (int)strtol(str, NULL, 0);
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key, convert to a double
  @param    d Dictionary to search
  @param    key Key string to look for
  @param    notfound Value to return in case of error
  @return   double

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.
 */
/*--------------------------------------------------------------------------*/
double iniparser_getdouble(const dictionary *d, const char *key, double notfound)
{
    const char *str ;

    str = iniparser_getstring(d, key, INI_INVALID_KEY);
    if (str == INI_INVALID_KEY)
    {
        return notfound ;
    }
    return atof(str);
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key, convert to a boolean
  @param    d Dictionary to search
  @param    key Key string to look for
  @param    notfound Value to return in case of error
  @return   integer

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.

  A true boolean is found if one of the following is matched:

  - A string starting with 'y'
  - A string starting with 'Y'
  - A string starting with 't'
  - A string starting with 'T'
  - A string starting with '1'

  A false boolean is found if one of the following is matched:

  - A string starting with 'n'
  - A string starting with 'N'
  - A string starting with 'f'
  - A string starting with 'F'
  - A string starting with '0'

  The notfound value returned if no boolean is identified, does not
  necessarily have to be 0 or 1.
 */
/*--------------------------------------------------------------------------*/
int iniparser_getboolean(const dictionary *d, const char *key, int notfound)
{
    int          ret ;
    const char *c ;

    c = iniparser_getstring(d, key, INI_INVALID_KEY);
    if (c == INI_INVALID_KEY)
    {
        return notfound ;
    }
    if (c[0] == 'y' || c[0] == 'Y' || c[0] == '1' || c[0] == 't' || c[0] == 'T')
    {
        ret = 1 ;
    }
    else if (c[0] == 'n' || c[0] == 'N' || c[0] == '0' || c[0] == 'f' || c[0] == 'F')
    {
        ret = 0 ;
    }
    else
    {
        ret = notfound ;
    }
    return ret;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Finds out if a given entry exists in a dictionary
  @param    ini     Dictionary to search
  @param    entry   Name of the entry to look for
  @return   integer 1 if entry exists, 0 otherwise

  Finds out if a given entry exists in the dictionary. Since sections
  are stored as keys with NULL associated values, this is the only way
  of querying for the presence of sections in a dictionary.
 */
/*--------------------------------------------------------------------------*/
int iniparser_find_entry(const dictionary *ini, const char *entry)
{
    int found = 0 ;
    if (iniparser_getstring(ini, entry, INI_INVALID_KEY) != INI_INVALID_KEY)
    {
        found = 1 ;
    }
    return found ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Set an entry in a dictionary.
  @param    ini     Dictionary to modify.
  @param    entry   Entry to modify (entry name)
  @param    val     New value to associate to the entry.
  @return   int 0 if Ok, -1 otherwise.

  If the given entry can be found in the dictionary, it is modified to
  contain the provided value. If it cannot be found, the entry is created.
  It is Ok to set val to NULL.
 */
/*--------------------------------------------------------------------------*/
int iniparser_set(dictionary *ini, const char *entry, const char *val)
{
    /*char tmp_str[ASCIILINESZ+1];
    return dictionary_set(ini, strlwc(entry, tmp_str, sizeof(tmp_str)), val) ;*/
    return dictionary_set(ini, entry, val) ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Delete an entry in a dictionary
  @param    ini     Dictionary to modify
  @param    entry   Entry to delete (entry name)
  @return   void

  If the given entry can be found, it is deleted from the dictionary.
 */
/*--------------------------------------------------------------------------*/
void iniparser_unset(dictionary *ini, const char *entry)
{
    /*char tmp_str[ASCIILINESZ+1];
    dictionary_unset(ini, strlwc(entry, tmp_str, sizeof(tmp_str)));*/
    dictionary_unset(ini, entry);
}
#if 0
/*-------------------------------------------------------------------------*/
/**
  @brief    Load a single line from an INI file
  @param    input_line  Input line, may be concatenated multi-line input
  @param    section     Output space to store section
  @param    key         Output space to store key
  @param    value       Output space to store value
  @return   line_status value
 */
/*--------------------------------------------------------------------------*/
static line_status iniparser_line(
    const char *input_line,
    char *section,
    char *key,
    char *value)
{
    line_status sta ;
    char *line = NULL;
    size_t      len ;

    line = _strdup(input_line);
    len = strstrip(line);

    sta = LINE_UNPROCESSED ;
    if (len < 1)
    {
        /* Empty line */
        sta = LINE_EMPTY ;
    }
    else if (line[0] == '#' || line[0] == ';')
    {
        /* Comment line */
        sta = LINE_COMMENT ;
    }
    else if (line[0] == '[' && line[len - 1] == ']')
    {
        /* Section name */
        sscanf(line, "[%[^]]", section);
        strstrip(section);
        strlwc(section, section, len);
        sta = LINE_SECTION ;
    }
    else if (sscanf (line, "%[^=] = \"%[^\"]\"", key, value) == 2
             ||  sscanf (line, "%[^=] = '%[^\']'",   key, value) == 2
             ||  sscanf (line, "%[^=] = %[^;#]",     key, value) == 2)
    {
        /* Usual key=value, with or without comments */
        strstrip(key);
        strlwc(key, key, len);
        strstrip(value);
        /*
         * sscanf cannot handle '' or "" as empty values
         * this is done here
         */
        if (!strcmp(value, "\"\"") || (!strcmp(value, "''")))
        {
            value[0] = 0 ;
        }
        sta = LINE_VALUE ;
    }
    else if (sscanf(line, "%[^=] = %[;#]", key, value) == 2
             ||  sscanf(line, "%[^=] %[=]", key, value) == 2)
    {
        /*
         * Special cases:
         * key=
         * key=;
         * key=#
         */
        strstrip(key);
        strlwc(key, key, len);
        value[0] = 0 ;
        sta = LINE_VALUE ;
    }
    else
    {
        /* Generate syntax error */
        sta = LINE_ERROR ;
    }

    free(line);
    return sta ;
}
#endif

/* read ini file to memory */
static char *iniparser_open(const char *ininame)
{
    FILE *ini;
    long fsize;
    char *buf;
    size_t result;
    if ((ini = fopen(ininame, "rb")) == NULL)
    {
        return NULL ;
    }
    /*get file size*/
    fseek (ini, 0, SEEK_END);
    fsize = ftell (ini);
    rewind (ini);
    buf = (char *)malloc(sizeof(char) * fsize + 1);
    if (buf == NULL)
    {
        printf("iniparser malloc fail\n");
        fclose(ini);
        return NULL ;
    }

    result = fread(buf, 1, fsize, ini);
    if ((long)result != fsize)
    {
        printf("read inifile fail\n");
        fclose(ini);
        return NULL;
    }
    fclose(ini);
    buf[fsize] = '\0';
    return buf;/* buf should be free by caller */
}

char *iniparser_gets(char *s, int n,  char *stream)
{
    register int c = 0;
    register char *cs;
    if (pseek == NULL)
    {
        pseek = stream;
    }
    cs = s;
    while (--n > 0 && (c = *pseek++) != '\0')
        if ((*cs++ =  c) == '\n')
        {
            break;
        }
    *cs = '\0';
    return (c == '\0' && cs == s) ? NULL : s ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Parse an ini file and return an allocated dictionary object
  @param    ininame Name of the ini file to read.
  @return   Pointer to newly allocated dictionary

  This is the parser for ini files. This function is called, providing
  the name of the file to be read. It returns a dictionary object that
  should not be accessed directly, but through accessor functions
  instead.

  The returned dictionary must be freed using iniparser_freedict().
 */
/*--------------------------------------------------------------------------*/
dictionary *iniparser_load(const char *ininame)
{
    /*FILE * in ;*/

    char line    [ASCIILINESZ + 1] ;
    char section [ASCIILINESZ + 1] ;
    char key     [ASCIILINESZ + 1] ;
    char tmp     [ASCIILINESZ + 1] ;
    char val     [ASCIILINESZ + 1] ;
    char comment    [ASCIILINESZ + 1];

    int  len ;
    int  lineno = 0 ;
    char *where ;
    char *pstream;
    dictionary *dict ;
    int last = 0;

    if ((pstream = iniparser_open(ininame)) == NULL)
    {
        fprintf(stderr, "iniparser: cannot open %s\n", ininame);
        return NULL ;
    }
    dict = dictionary_new(0) ;
    if (!dict)
    {
        /*fclose(in);*/
        free(pstream);
        return NULL ;
    }
    memset(line,    0, ASCIILINESZ);
    memset(section, 0, ASCIILINESZ);
    memset(key,     0, ASCIILINESZ);
    memset(tmp,     0, ASCIILINESZ);
    memset(val,     0, ASCIILINESZ);
    memset(comment,     0, ASCIILINESZ);
    while (iniparser_gets(line + last, ASCIILINESZ - last, pstream) != NULL)
    {
        lineno++ ;
        where = line;   /* Skip leading spaces */
        comment[0] = 0;
        len = (int)strlen(line) - 1;
        if (len == 0)
        {
            continue;
        }
        /* Safety check against buffer overflows */
        if (line[len] != '\n' && (len == (ASCIILINESZ - 1)))
        {
            fprintf(stderr,
                    "iniparser: input line too long in %s (%d):%s\n",
                    ininame,
                    lineno,
                    line);
#if HI_OS_TYPE == HI_OS_LINUX
            syslog(LOG_NOTICE, "iniparser: input line too long in %s (%d)", ininame, lineno);
#endif
            memset(line, 0, ASCIILINESZ);
            while (iniparser_gets(line, ASCIILINESZ, pstream))
            {
                len = (int)strlen(line) - 1;
                if (line[len] != '\n')
                {
                    memset(line, 0, ASCIILINESZ);
                    continue;
                }
                else
                {
                    memset(line, 0, ASCIILINESZ);
                    break;
                }
            }
            continue;
        }

        /*2016/09/09 c0027017 modify: detect multi-line*/
        /* Get rid of \n and spaces at end of line */
        while ((len >= 0) &&
               ((line[len] == '\n') || (line[len] == '\r')))
        {
            line[len] = 0 ;
            len-- ;
        }
        /* Detect multi-line */
        //printf("len = %dict\n", len);
        //printf("line[%dict] = %c, %dict\n", len, line[len], line[len]);
        if (line[len] == '\\')
        {
            /* Multi-line value */
            last = len ;
            continue ;
        }
        else
        {
            last = 0 ;
        }


        /* save comment and space Line*/
        /*if (*where==';' || *where=='#' || *where==0)*/
        if (*where == ';' || *where == '#') /*; comment*/
        {
            strcpy(comment, where);
            /*no space commnet line*/
            iniparser_add_comment(dict, section, key, comment, CL_NSBC);
        }
        else
        {
            where = strskp(line); /* Skip leading spaces */
            /*if((sscanf (where, " ; %[^\n]", comment) == 1)
                ||(sscanf (where, " # %[^\n]", comment) == 1))*//*   ; comment*/
            if (*where == ';' || *where == '#')
            {
                strcpy(comment, where);
                /*comment line with space*/
                iniparser_add_comment(dict, section, key, comment, CL_CLINE);
                last = 0;
                continue ; /* Comment lines */
            }
            else
            {
                if ((sscanf(where, "[%[^]]] %[^\n]", section, comment) == 2)
                    || (sscanf(where, "[%[^]]", section) == 1)) /*[sec];comment*/
                {
                    /* Valid section name */
                    key[0] = 0;
                    /*strcpy(section, strlwc(section, tmp, sizeof(tmp)));*/
                    iniparser_add_entry(dict, section, NULL, NULL);
                    if (comment[0] != 0)
                    {
                        /*The comment at key line's right*/
                        iniparser_add_comment(dict, section, key, comment, CL_AKLR);
                    }
                }
                else /*key = value ;comment*/
                {
                    /*2006/03/13 blair modify : save comment and space Line*/
                    if (sscanf(where, "%[^=] = \"%[^\"]\" %[^\n]", key, val,
                               comment) == 3
                        ||  sscanf(where, "%[^=] = \"%[^\"]\"", key, val) == 2
                        ||  sscanf(where, "%[^=] = '%[^\']' %[^\n]", key, val,
                                   comment) == 3
                        ||  sscanf(where, "%[^=] = '%[^\']'",   key, val) == 2
                        ||  sscanf(where, "%[^=] = %[^;] %[^\n]", key, val,
                                   comment) == 3
                        ||  sscanf(where, "%[^=] = %[^;]", key, val ) == 2
                        ||  sscanf(where, "%[^=] = %[^\n]", key,
                                   comment ) == 2
                        ||  sscanf(where, "%[^=] = ", key) == 1)
                    {
                        /*strcpy(key, strlwc(key, tmp, sizeof(tmp)));*/
                        /*
                         * sscanf cannot handle "" or '' as empty value,
                         * this is done here
                         */
                        if (!strcmp(val, "\"\"") || !strcmp(val, "''"))
                        {
                            val[0] = (char)0;
                        }
                        else
                        {
                            strstrip(val);
                        }
                        strstrip(key);
                        iniparser_add_entry(dict, section, key, val);
                        if (comment[0] != 0)
                        {
                            /*The comment at key line's right*/
                            iniparser_add_comment(dict, section, key, comment, CL_AKLR);
                        }
                    }
                    else
                    {
                        comment[0] = 0;
                        /*Space Line*/
                        iniparser_add_comment(dict, section, key, comment, CL_SLINE);
                    }
                }
            }
        }
        last = 0;
    }
    pseek = NULL;
    free(pstream);

    return dict ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    add comment
  @param  ini     Dictionary to modify
  @param  sec    section string
  @param  key    Key string
  @param  comment    comment string
  @return  0 success, -1 failed

  save comment and space Line.
 */
/*--------------------------------------------------------------------------*/
int iniparser_add_comment(
    dictionary          *d,
    const char          *sec,
    const char          *key,
    const char          *comment,
    const unsigned char place)
{
    char        longkey[1024] = {0};
    unsigned    hash;
    int         i;

    if ((d == NULL) || (sec == NULL)  || (place < 1))
    {
        return -1;
    }

    /* Make a key as section:keyword */
    if (sec[0] != 0)
    {
        if (key[0] != 0)
        {
            snprintf(longkey, 1024, "%s:%s", sec, key);
        }
        else
        {
            strncpy(longkey, sec, 1024);
        }
    }
    else
    {
        if (key[0] != 0)
        {
            snprintf(longkey, 1024, "%s", key);
        }
        else
        {
            longkey[0] = 0;
        }
    }

    /* Compute hash for this key */
    if (longkey[0] != '\0')
    {
        hash = dictionary_hash(longkey);
    }
    else
    {
        hash = CNOSK;
    }

    /* Add a new value */
    /* See if dictionary needs to grow */
    if (d->commN == d->commSize)
    {
        /* Reached maximum size: reallocate blackboard */
        d->comment  = (char **)mem_double((void *)d->comment, d->commSize * sizeof(char *));
        d->commHash = (unsigned *)mem_double((void *)d->commHash, d->commSize * sizeof(unsigned));
        d->commPlace = (unsigned char *)mem_double((void *)d->commPlace, d->commSize * sizeof(unsigned char));

        /* Double size */
        d->commSize *= 2 ;
    }

    /* Insert comment in the first empty slot */
    for (i = 0 ; i < d->commSize ; i++)
    {
        if (d->commPlace[i] == 0)
        {
            /* Add comment here */
            break ;
        }
    }
    d->commHash[i] = hash;
    d->commPlace[i] = place;
    d->comment[i] ? free(d->comment[i]), d->comment[i] = NULL : NULL;
    d->comment[i]  = (comment[0] != 0) ? xstrdup(comment) : NULL;
    d->commN ++ ;
    return 0;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Free all memory associated to an ini dictionary
  @param    d Dictionary to free
  @return   void

  Free all memory associated to an ini dictionary.
  It is mandatory to call this function before the dictionary object
  gets out of the current context.
 */
/*--------------------------------------------------------------------------*/
void iniparser_freedict(dictionary *d)
{
    dictionary_del(d);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
