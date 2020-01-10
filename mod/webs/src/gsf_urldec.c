#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>

#include "gsf_urldec.h"

static unsigned char hexchars[] = "0123456789ABCDEF";
/**
 * 16进制数转换成10进制数
 * 如：0xE4=14*16+4=228
 */
static int url_htoi(char *s)
{
    int value;
    int c;

    c = ((unsigned char *)s)[0];
    if (isupper(c))
        c = tolower(c);
    value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;

    c = ((unsigned char *)s)[1];
    if (isupper(c))
        c = tolower(c);
    value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;

    return (value);
}


char *gsf_url_encode(char const *s, int len, int *new_length)
{
    register unsigned char c;
    unsigned char *to, *start;
    unsigned char const *from, *end;
    
    from = (unsigned char *)s;
    end  = (unsigned char *)s + len;
    start = to = (unsigned char *) calloc(1, 3*len+1);

    while (from < end) 
    {
        c = *from++;

        if (c == ' ') 
        {
            *to++ = '+';
        } 
        else if ((c < '0' && c != '-' && c != '.') ||
                 (c < 'A' && c > '9') ||
                 (c > 'Z' && c < 'a' && c != '_') ||
                 (c > 'z')) 
        {
            to[0] = '%';
            to[1] = hexchars[c >> 4];//将2进制转换成16进制表示
            to[2] = hexchars[c & 15];//将2进制转换成16进制表示
            to += 3;
        }
        else 
        {
            *to++ = c;
        }
    }
    *to = 0;
    if (new_length) 
    {
        *new_length = to - start;
    }
    return (char *) start;
}


int gsf_url_decode(char *str, int len)
{
    char *dest = str;
    char *data = str;

    while (len--) 
    {
        if (*data == '+') 
        {
            *dest = ' ';
        }
        else if (*data == '%' && len >= 2 && isxdigit((int) *(data + 1)) && isxdigit((int) *(data + 2))) 
        {
            *dest = (char) url_htoi(data + 1);
            data += 2;
            len -= 2;
        } 
        else 
        {
            *dest = *data;
        }
        data++;
        dest++;
    }
    *dest = '\0';
    return dest - str;
}
#if 0
int main(int argc, char ** argv)
{
    char *str = "<中>",*new_str;
    int len = 0,new_len = 0,old_len = 0;
    len = strlen(str);
    new_str = gsf_url_encode(str,len,&new_len);
    printf("new string : %s,new length : %d\n",new_str,new_len);
    old_len = gsf_url_decode(new_str,new_len);
    printf("old string : %s,old length : %d\n",new_str,old_len);
    return 0;
}
#endif