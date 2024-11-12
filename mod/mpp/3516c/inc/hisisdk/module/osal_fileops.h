/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

/**
 * @defgroup osal_fileops osal_fileops
 */
#ifndef __OSAL_FILEOPS_H__
#define __OSAL_FILEOPS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define OSAL_O_RDONLY 00000000
#define OSAL_O_WRONLY 00000001
#define OSAL_O_RDWR 00000002
#define OSAL_O_ACCMODE 00000003

#define OSAL_O_CREAT 00000100
#define OSAL_O_EXCL 00000200
#define OSAL_O_TRUNC 00001000
#define OSAL_O_APPEND 00002000
#define OSAL_O_CLOEXEC 02000000

#define OSAL_SEEK_SET 0
#define OSAL_SEEK_CUR 1
#define OSAL_SEEK_END 2

/**
 * @ingroup osal_fileops
 * @brief open file and return file pointer.
 * @param file [in] the filename of file
 * @param flag [in] the opreation flag example: OSAL_O_CREAT
 * @param mode [in] the mode example: OSAL_O_RDONLY
 * @return void* / null
 *
 * @par Support System:
 * linux liteos seliteos freertos.
 */
void *osal_klib_fopen(const char *file, int flags, int mode);

/**
 * @ingroup osal_fileops
 * @brief close file.
 * @param filp [in] the result of osal_klib_fopen
 * @par Support System:
 * linux liteos seliteos freertos.
 */
void osal_klib_fclose(void *filp);

/**
 * @ingroup osal_fileops
 * @brief kernel write function.
 * @param buf [in] the buffer you want to write
 * @param size [in] the size of buffer
 * @param filp [in] the result of osal_klib_fopen
 * @return The length that has been written.
 *
 * @par Support System:
 * linux liteos seliteos freertos.
 */
int osal_klib_fwrite(const char *buf, unsigned long size, void *filp);

/**
 * @ingroup osal_fileops
 * @brief Kernel read function.
 * @param buf [out] the buffer you want to read in
 * @param size [in] the size of buffer
 * @param filp [in] the result of osal_klib_fopen
 * @return The length that has been read.
 *
 * @par Support System:
 * linux liteos seliteos freertos.
 */
int osal_klib_fread(char *buf, unsigned long size, void *filp);

/**
 * @ingroup osal_fileops
 * @brief Perform a fsync or fdatasync on a file
 * @param filp [in] the result of osal_klib_fopen
 * @return The value 0 indicates success, and other values indicate failure.
 * @par Support System:
 * linux liteos.
 */
void osal_klib_fsync(void *filp);

/**
 * @ingroup osal_fileops
 * @brief Kernel file seek function.
 * @param offset [in] the new position of file
 * @param whence [in] the method of lseek,example:OSAL_SEEK_SET
 * @param filp [in] the result of osal_klib_fopen
 * @par Support System:
 * linux liteos seliteos freertos.
 */
int osal_klib_fseek(long long offset, int whence, void *filp);

/**
 * @ingroup osal_fileops
 * @brief Removes a file or directory.
 * @param path [in] the file path of file
 * @attention If removing a directory, the directory must be empty.
 *
 * @return The value 0 indicates success, and other values indicate failure.
 *
 * @par Support System:
 * seliteos.
 */
int osal_klib_unlink(const char *path);

/**
 * @ingroup osal_fileops
 * @brief Truncates the size of the file to the specified size.
 * @param filp [in] the result of osal_klib_fopen
 * @param len [in] the len of ftruncate
 * @return The value 0 indicates success, and other values indicate failure.
 *
 * @par Support System:
 * seliteos.
 */
int osal_klib_ftruncate(void *filp, unsigned long len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __OSAL_FILEOPS_H__ */