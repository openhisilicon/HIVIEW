/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

/**
 * @defgroup osal_string osal_string
 */
#ifndef __OSAL_STRING_H__
#define __OSAL_STRING_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define OSAL_BASE_DEC 10
#define OSAL_BASE_HEX 16

/**
 * @ingroup osal_string
 * @brief Compare memory areas.
 *
 * @par Description:
 * This API is used to compare the size bytes of the memory areas buf1 and buf2.
 *
 * @param buf1 [in] the memory areas to be compared.
 * @param buf2 [in] the memory areas to be compared.
 * @param size [in] the size of the memory areas.
 *
 * @return Returns an integer less than, equal to, or greater than zero if the first n bytes of buf1 is found,
 * respectively, to be less than, to match, or be greater than the first n bytes of buf2.
 *
 * @par Support System:
 * linux liteos.
 *
 * @attention the implementation is in libc, the parameters should be valid as calling libc interface.
 */
int osal_memncmp(const void *buf1, const void *buf2, unsigned long size);

/**
 * @ingroup osal_string
 * @brief Compare two strings.
 *
 * @par Description:
 * This API is used to compare two strings.
 *
 * @param s1 [in] the string to be compared.
 * @param s2 [in] the string to be compared.
 *
 * @return Returns an integer less than, equal to, or greater than zero if s1 is found, respectively, to be less than,
 * to match, or be greater than s2.
 *
 * @par Support System:
 * linux liteos freertos.
 *
 * @attention the implementation is in libc, the parameters should be valid as calling libc interface.
 */
int osal_strcmp(const char *s1, const char *s2);

/**
 * @ingroup osal_string
 * @brief Compare two strings.
 *
 * @par Description:
 * This API is used to compare first size bytes of two strings.
 *
 * @param s1 [in] the string to be compared.
 * @param s2 [in] the string to be compared.
 * @param size [in] the size of the string.
 *
 * @return Returns an integer less than, equal to, or greater than zero if the first size bytes of s1 is found,
 * respectively, to be less than, to match, or be greater than s2.
 *
 * @par Support System:
 * linux liteos.
 *
 * @attention the implementation is in libc, the parameters should be valid as calling libc interface.
 */
int osal_strncmp(const char *s1, const char *s2, unsigned long size);

/**
 * @ingroup osal_string
 * @brief Compare two strings ignoring case.
 *
 * @par Description:
 * This API is used to compare strings s1 and s2 byte-by-byte, ignoring the case of the characters.
 *
 * @param s1 [in] the string to be compared.
 * @param s2 [in] the string to be compared.
 *
 * @return Returns an integer less than, equal to, or greater than zero if s1 is found,
 * respectively, to be less than, to match, or be greater than s2.
 *
 * @par Support System:
 * linux liteos freertos.
 *
 * @attention the implementation is in libc, the parameters should be valid as calling libc interface.
 */
int osal_strcasecmp(const char *s1, const char *s2);

/**
 * @ingroup osal_string
 * @brief Compare two strings ignoring case.
 *
 * @par Description:
 * This API is used to compare first size bytes of strings s1 and s2 byte-by-byte, ignoring the case of the characters.
 *
 * @param s1 [in] the string to be compared.
 * @param s2 [in] the string to be compared.
 * @param size [in] the size of the string.
 *
 * @return Returns an integer less than, equal to, or greater than zero if the first size bytes of s1 is found,
 * respectively, to be less than, to match, or be greater than s2.
 *
 * @par Support System:
 * linux liteos.
 *
 * @attention the implementation is in libc, the parameters should be valid as calling libc interface.
 */
int osal_strncasecmp(const char *s1, const char *s2, unsigned long size);

/**
 * @ingroup osal_string
 * @brief Locate character in string.
 *
 * @par Description:
 * This API is used to locate character in string.
 *
 * @param s [in] the string to be searched.
 * @param n [in] the character to search for.
 *
 * @return Returns a pointer to the first occurrence of the character n in the string s.
 *
 * @par Support System:
 * linux liteos.
 *
 * @attention the implementation is in libc, the parameters should be valid as calling libc interface.
 */
char *osal_strchr(const char *s, int n);

/**
 * @ingroup osal_string
 * @brief Find a character in a length limited string.
 *
 * @par Description:
 * This API is used to find a character in a length limited string.
 *
 * @param s [in] the string to be searched.
 * @param count [in] the number of characters to be searched.
 * @param c [in] the character to search for.
 *
 * @return Returns a pointer to the first occurrence of the character c in the string s.
 *
 * @par Support System:
 * linux liteos.
 */
char *osal_strnchr(const char *s, int count, int c);

/**
 * @ingroup osal_string
 * @brief Find the last occurrence of a character in a string.
 *
 * @par Description:
 * This API is used to find a character in a length limited string.
 *
 * @param s [in] the string to be searched.
 * @param c [in] the character to search for.
 *
 * @return Returns a pointer to the last occurrence of the character c in the string s.
 *
 * @par Support System:
 * linux liteos.
 *
 * @attention the implementation is in libc, the parameters should be valid as calling libc interface.
 */
char *osal_strrchr(const char *s, int c);

/**
 * @ingroup osal_string
 * @brief Locate a substring.
 *
 * @par Description:
 * This API is used to locate a substring.
 *
 * @param s1 [in] the string to be searched.
 * @param s2 [in] the substring to search for.
 *
 * @return Returns a pointer to the first occurrence of the substring s2 in the string s1.
 *
 * @par Support System:
 * linux liteos.
 *
 * @attention the implementation is in libc, the parameters should be valid as calling libc interface.
 */
char *osal_strstr(const char *s1, const char *s2);

/**
 * @ingroup osal_string
 * @brief Locate a substring in a length limited string.
 *
 * @par Description:
 * This API is used to locate a substring in a length limited string.
 *
 * @param s1 [in] the string to be searched.
 * @param s2 [in] the substring to search for.
 * @param len [in] the length of string.
 *
 * @return Returns a pointer to the first occurrence of the substring s2 in the string s1.
 *
 * @par Support System:
 * linux liteos.
 */
char *osal_strnstr(const char *s1, const char *s2, int len);

/**
 * @ingroup osal_string
 * @brief Calculate the length of a string.
 *
 * @par Description:
 * This API is used to calculate the length of a string.
 *
 * @param s [in] the string to be calculated.
 *
 * @return Returns the number of characters in the string pointed to by s.
 *
 * @par Support System:
 * linux liteos freertos.
 *
 * @attention the implementation is in libc, the parameters should be valid as calling libc interface.
 */
unsigned int osal_strlen(const char *s);

/**
 * @ingroup osal_string
 * @brief Determine the length of a fixed-size string.
 *
 * @par Description:
 * This API is used to determine the length of a fixed-size string.
 *
 * @param s [in] the string to be calculated.
 * @param size [in] the length of string.
 *
 * @return Returns the number of characters in the string pointed to by s, excluding the terminating null byte ('\0'),
 * but at most size. In doing this, strnlen() looks only at the first size characters in the string pointed to by s
 * and never beyond s+size.
 *
 * @par Support System:
 * linux liteos.
 *
 * @attention the implementation is in libc, the parameters should be valid as calling libc interface.
 */
unsigned int osal_strnlen(const char *s, unsigned int size);

/**
 * @ingroup osal_string
 * @brief Search a string for any of a set of bytes.
 *
 * @par Description:
 * This API is used to search a string for any of a set of bytes.
 *
 * @param s1 [in] The string to be searched.
 * @param s2 [in] The characters to search for.
 *
 * @return Returns a pointer to the byte in s1 that matches one of the bytes in s2, or NULL if no such byte is found.
 *
 * @par Support System:
 * linux liteos.
 *
 * @attention the implementation is in libc, the parameters should be valid as calling libc interface.
 */
char *osal_strpbrk(const char *s1, const char *s2);

/**
 * @ingroup osal_string
 * @brief Extract token from string.
 *
 * @par Description:
 * This API is used to extract token from string.
 *
 * @param s [in/out] The string to be searched.
 * @param ct [in] The characters to search for.
 *
 * @return Returns a pointer to the token, that is, it returns the original value of *s.
 *
 * @par Support System:
 * linux liteos.
 *
 * @attention the implementation is in libc, the parameters should be valid as calling libc interface.
 */
char *osal_strsep(char **s, const char *ct);

/**
 * @ingroup osal_string
 * @brief Get length of a prefix substring.
 *
 * @par Description:
 * This API is used to calculate the length of the initial substring of s which only contain letters in accept.
 *
 * @param s [in] The string to be searched.
 * @param accept [in] The characters to search for.
 *
 * @return Returns the number of bytes in the initial segment of s which consist only of bytes from accept.
 *
 * @par Support System:
 * linux liteos.
 *
 * @attention the implementation is in libc, the parameters should be valid as calling libc interface.
 */
unsigned int osal_strspn(const char *s, const char *accept);

/**
 * @ingroup osal_string
 * @brief Get length of a prefix substring.
 *
 * @par Description:
 * This API is used to calculate the length of the initial substring of s which does not contain letters in reject.
 *
 * @param s [in] The string to be searched.
 * @param reject [in] The string to avoid.
 *
 * @return Returns the number of bytes in the initial segment of s which are not in the string reject.
 *
 * @par Support System:
 * linux liteos.
 *
 * @attention the implementation is in libc, the parameters should be valid as calling libc interface.
 */
unsigned int osal_strcspn(const char *s, const char *reject);

/**
 * @ingroup osal_string
 * @brief Find a character in an area of memory.
 *
 * @par Description:
 * This API is used to find a character in an area of memory.
 *
 * @param addr [in] The memory area.
 * @param c [in] The byte to search for.
 * @param size [in] The size of the area.
 *
 * @return Returns the address of the first occurrence of c, or 1 byte past the area if c is not found.
 *
 * @par Support System:
 * linux liteos.
 */
void *osal_memscan(void *addr, int c, int size);

/**
 * @ingroup osal_string
 * @brief Compare two areas of memory.
 *
 * @par Description:
 * This API is used to compare two areas of memory.
 *
 * @param cs [in] One area of memory.
 * @param ct [in] Another area of memory.
 * @param count [in] The size of the area.
 *
 * @return Returns an integer less than, equal to, or greater than zero if the first count bytes of cs is found,
 * respectively, to be less than, to match, or be greater than the first count bytes of ct.
 *
 * @par Support System:
 * linux liteos freertos.
 *
 * @attention the implementation is in libc, the parameters should be valid as calling libc interface.
 */
int osal_memcmp(const void *cs, const void *ct, int count);

/**
 * @ingroup osal_string
 * @brief Find a character in an area of memory.
 *
 * @par Description:
 * This API is used to find a character in an area of memory.
 *
 * @param s [in] The memory area.
 * @param c [in] The byte to search for.
 * @param n [in] The size of the area.
 *
 * @return Returns the address of the first occurrence of c, or NULL if c is not found.
 *
 * @par Support System:
 * linux liteos.
 *
 * @attention the implementation is in libc, the parameters should be valid as calling libc interface.
 */
void *osal_memchr(const void *s, int c, int n);

/**
 * @ingroup osal_string
 * @brief Find an unmatching character in an area of memory.
 *
 * @par Description:
 * This API is used to find an unmatching character in an area of memory.
 *
 * @param start [in] The memory area.
 * @param c [in] Find a character other than c.
 * @param bytes [in] The size of the area.
 *
 * @return Returns the address of the first character other than c, or NULL if the whole buffer contains just c.
 *
 * @par Support System:
 * linux liteos.
 */
void *osal_memchr_inv(const void *start, int c, int bytes);

/**
 * @ingroup osal_string
 * @brief Convert a string to an unsigned long long integer.
 *
 * @par Description:
 * The strtoull() function converts the initial part of the string in cp to an unsigned long long int value according to
 * the given base, which must be between 2 and 36 inclusive, or be the special value 0.
 * The string may begin with an arbitrary amount of white space (as determined by isspace(3)) followed by a single
 * optional '+' or '-' sign. If base is zero or 16, the string may then include a "0x" prefix, and the number will be
 * read in base 16; otherwise, a zero base is taken as 10 (decimal) unless the next character is '0', in which case
 * it is taken as 8 (octal).
 * The remainder of the string is converted to an unsigned long long int value in the obvious manner, stopping
 * at the first character which is not a valid digit in the given base.
 * (In bases above 10, the letter 'A' in either uppercase or lowercase represents 10, 'B' represents 11, and so forth,
 * with 'Z' representing 35.)
 * If endp is not NULL, strtoull() stores the address of the first invalid character in *endp.
 * If there were no digits at all, strtoull() stores the original value of cp in *endp (and returns 0).
 * In particular, if *cp is not '\0' but **endp is '\0' on return, the entire string is valid.
 *
 * @param cp [in] The string to be converted.
 * @param endp [in/out] Stores the result.
 * @param base [in] The base of conversion.
 *
 * @return Returns either the result of the conversion or, if there was a leading minus sign, the negation of the result
 * of the conversion represented as an unsigned value, unless the original (nonnegated) value would overflow; in
 * the latter case, strtoull() returns ULLONG_MAX and sets errno to ERANGE.
 *
 * @par Support System:
 * linux.
 */
unsigned long long osal_strtoull(const char *cp, char **endp, unsigned int base);

/**
 * @ingroup osal_string
 * @brief Convert a string to an unsigned long integer.
 *
 * @par Description:
 * The strtoul() function converts the initial part of the string in cp to an unsigned long int value according to
 * the given base, which must be between 2 and 36 inclusive, or be the special value 0.
 * The string may begin with an arbitrary amount of white space (as determined by isspace(3)) followed by a single
 * optional '+' or '-' sign. If base is zero or 16, the string may then include a "0x" prefix, and the number will be
 * read in base 16; otherwise, a zero base is taken as 10 (decimal) unless the next character is '0', in which case
 * it is taken as 8 (octal).
 * The remainder of the string is converted to an unsigned long int value in the obvious manner, stopping
 * at the first character which is not a valid digit in the given base.
 * (In bases above 10, the letter 'A' in either uppercase or lowercase represents 10, 'B' represents 11, and so forth,
 * with 'Z' representing 35.)
 * If endp is not NULL, osal_strtoul() stores the address of the first invalid character in *endp.
 * If there were no digits at all, osal_strtoul() stores the original value of cp in *endp (and returns 0).
 * In particular, if *cp is not '\0' but **endp is '\0' on return, the entire string is valid.
 *
 * @param cp [in] The string to be converted.
 * @param endp [in/out] Stores the result.
 * @param base [in] The base of conversion.
 *
 * @return Returns either the result of the conversion or, if there was a leading minus sign, the negation of the result
 * of the conversion represented as an unsigned value, unless the original (nonnegated) value would overflow; in
 * the latter case, osal_strtoul() returns ULONG_MAX and sets errno to ERANGE.
 *
 * @par Support System:
 * linux liteos.
 *
 * @attention the implementation is in libc, the parameters should be valid as calling libc interface.
 */
unsigned long osal_strtoul(const char *cp, char **endp, unsigned int base);

/**
 * @ingroup osal_string
 * @brief Convert a string to a long integer.
 *
 * @par Description:
 * The strtoll() function converts the initial part of the string in cp to a long int value according to
 * the given base, which must be between 2 and 36 inclusive, or be the special value 0.
 * The string may begin with an arbitrary amount of white space (as determined by isspace(3)) followed by a single
 * optional '+' or '-' sign. If base is zero or 16, the string may then include a "0x" prefix, and the number will be
 * read in base 16; otherwise, a zero base is taken as 10 (decimal) unless the next character is '0', in which case
 * it is taken as 8 (octal).
 * The remainder of the string is converted to a long int value in the obvious manner, stopping at the first
 * character which is not a valid digit in the given base.
 * (In bases above 10, the letter 'A' in either uppercase or lowercase represents 10, 'B' represents 11, and so forth,
 * with 'Z' representing 35.)
 * If endp is not NULL, strtol() stores the address of the first invalid character in *endp.
 * If there were no digits at all, strtol() stores the original value of cp in *endp (and returns 0).
 * In particular, if *cp is not '\0' but **endp is '\0' on return, the entire string is valid.
 *
 * @param cp [in] The string to be converted.
 * @param endp [in/out] Stores the result.
 * @param base [in] The base of conversion.
 *
 * @return Returns either the result of the conversion or, if there was a leading minus sign, the negation of the result
 * of the conversion represented as an unsigned value, unless the original (nonnegated) value would overflow; in
 * the latter case, strtol() returns LONG_MAX and sets errno to ERANGE.
 *
 * @par Support System:
 * linux liteos.
 *
 * @attention the implementation is in libc, the parameters should be valid as calling libc interface.
 */
long osal_strtol(const char *cp, char **endp, unsigned int base);

/**
 * @ingroup osal_string
 * @brief Convert a string to a long long integer.
 *
 * @par Description:
 * The strtoll() function converts the initial part of the string in cp to a long long int value according to
 * the given base, which must be between 2 and 36 inclusive, or be the special value 0.
 * The string may begin with an arbitrary amount of white space (as determined by isspace(3)) followed by a single
 * optional '+' or '-' sign. If base is zero or 16, the string may then include a "0x" prefix, and the number will be
 * read in base 16; otherwise, a zero base is taken as 10 (decimal) unless the next character is '0', in which case
 * it is taken as 8 (octal).
 * The remainder of the string is converted to a long long int value in the obvious manner, stopping at the first
 * character which is not a valid digit in the given base.
 * (In bases above 10, the letter 'A' in either uppercase or lowercase represents 10, 'B' represents 11, and so forth,
 * with 'Z' representing 35.)
 * If endp is not NULL, strtoll() stores the address of the first invalid character in *endp.
 * If there were no digits at all, strtoll() stores the original value of cp in *endp (and returns 0).
 * In particular, if *cp is not '\0' but **endp is '\0' on return, the entire string is valid.
 *
 * @param cp [in] The string to be converted.
 * @param endp [in/out] Stores the result.
 * @param base [in] The base of conversion.
 *
 * @return Returns either the result of the conversion or, if there was a leading minus sign, the negation of the result
 * of the conversion represented as an unsigned value, unless the original (nonnegated) value would overflow; in
 * the latter case, strtoll() returns LLONG_MAX and sets errno to ERANGE.
 *
 * @par Support System:
 * linux.
 */
long long osal_strtoll(const char *cp, char **endp, unsigned int base);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __OSAL_STRING_H__ */