#ifndef __HI_UNF_CIPHER_H__
#define __HI_UNF_CIPHER_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */
/*************************** Structure Definition ****************************/
/** \addtogroup      CIPHER */
/** @{ */  /** <!-- [CIPHER] */

/** min length of encrypt, unit: byte */
#define HI_UNF_CIPHER_MIN_CRYPT_LEN      1

/** max length of encrypt, unit: byte */
#define HI_UNF_CIPHER_MAX_CRYPT_LEN      0xfffff

#define HI_UNF_CIPHER_MAX_RSA_KEY_LEN    (512)

/** Cipher work mode */
typedef enum hiHI_UNF_CIPHER_WORK_MODE_E
{
    HI_UNF_CIPHER_WORK_MODE_ECB,        /**<Electronic codebook (ECB) mode*/
    HI_UNF_CIPHER_WORK_MODE_CBC,        /**<Cipher block chaining (CBC) mode*/
    HI_UNF_CIPHER_WORK_MODE_CFB,        /**<Cipher feedback (CFB) mode*/
    HI_UNF_CIPHER_WORK_MODE_OFB,        /**<Output feedback (OFB) mode*/
    HI_UNF_CIPHER_WORK_MODE_CTR,        /**<Counter (CTR) mode*/
    HI_UNF_CIPHER_WORK_MODE_CCM,
    HI_UNF_CIPHER_WORK_MODE_GCM,
    HI_UNF_CIPHER_WORK_MODE_CBC_CTS,    /**<Cipher block chaining CipherStealing mode*/
    HI_UNF_CIPHER_WORK_MODE_BUTT    = 0xffffffff
}HI_UNF_CIPHER_WORK_MODE_E;

/** Cipher algorithm */
typedef enum hiHI_UNF_CIPHER_ALG_E
{
    HI_UNF_CIPHER_ALG_DES           = 0x0,  /**< Data encryption standard (DES) algorithm */
    HI_UNF_CIPHER_ALG_3DES          = 0x1,  /**< 3DES algorithm */
    HI_UNF_CIPHER_ALG_AES           = 0x2,  /**< Advanced encryption standard (AES) algorithm */
    HI_UNF_CIPHER_ALG_BUTT          = 0x3
}HI_UNF_CIPHER_ALG_E;

/** Key length */
typedef enum hiHI_UNF_CIPHER_KEY_LENGTH_E
{
    HI_UNF_CIPHER_KEY_AES_128BIT    = 0x0,  /**< 128-bit key for the AES algorithm */
    HI_UNF_CIPHER_KEY_AES_192BIT    = 0x1,  /**< 192-bit key for the AES algorithm */
    HI_UNF_CIPHER_KEY_AES_256BIT    = 0x2,  /**< 256-bit key for the AES algorithm */
    HI_UNF_CIPHER_KEY_DES_3KEY      = 0x2,  /**< Three keys for the DES algorithm */
    HI_UNF_CIPHER_KEY_DES_2KEY      = 0x3,  /**< Two keys for the DES algorithm */
}HI_UNF_CIPHER_KEY_LENGTH_E;

/** Cipher bit width */
typedef enum hiHI_UNF_CIPHER_BIT_WIDTH_E
{
    HI_UNF_CIPHER_BIT_WIDTH_64BIT   = 0x0,  /**< 64-bit width */
    HI_UNF_CIPHER_BIT_WIDTH_8BIT    = 0x1,  /**< 8-bit width */
    HI_UNF_CIPHER_BIT_WIDTH_1BIT    = 0x2,  /**< 1-bit width */
    HI_UNF_CIPHER_BIT_WIDTH_128BIT  = 0x3,  /**< 128-bit width */
}HI_UNF_CIPHER_BIT_WIDTH_E;

/** Cipher control parameters */
typedef struct hiHI_UNF_CIPHER_CTRL_CHANGE_FLAG_S
{
    HI_U32   bit1IV:1;              /**< Initial Vector change or not */
    HI_U32   bitsResv:31;           /**< Reserved */
}HI_UNF_CIPHER_CTRL_CHANGE_FLAG_S;

/** Encryption/Decryption type selecting */
typedef enum hiHI_UNF_CIPHER_KEY_SRC_E
{
    HI_UNF_CIPHER_KEY_SRC_USER       = 0x0,  /**< User Key*/
    HI_UNF_CIPHER_KEY_SRC_EFUSE_0,          /**< Efuse Key 0*/
    HI_UNF_CIPHER_KEY_SRC_EFUSE_1,          /**< Efuse Key 1*/
    HI_UNF_CIPHER_KEY_SRC_EFUSE_2,          /**< Efuse Key 2*/
    HI_UNF_CIPHER_KEY_SRC_EFUSE_3,          /**< Efuse Key 3*/
    HI_UNF_CIPHER_KEY_SRC_KLAD_1,           /**< KLAD Key 1*/
    HI_UNF_CIPHER_KEY_SRC_KLAD_2,           /**< KLAD Key 2*/
    HI_UNF_CIPHER_KEY_SRC_KLAD_3,           /**< KLAD Key 3*/
    HI_UNF_CIPHER_KEY_SRC_BUTT,
}HI_UNF_CIPHER_KEY_SRC_E;

/** Encryption/Decryption type selecting */
typedef enum
{
    HI_UNF_CIPHER_KLAD_TARGET_AES       = 0x0,  /**< Klad for AES*/
    HI_UNF_CIPHER_KLAD_TARGET_RSA,              /**< Klad for RSA*/
    HI_UNF_CIPHER_KLAD_TARGET_BUTT,
}HI_UNF_CIPHER_KLAD_TARGET_E;

typedef struct hiUNF_CIPHER_CCM_INFO_S
{
    HI_U8  u8Nonce[16];
    HI_U8 *pu8Aad;
    HI_U32 u32ALen;
    HI_U32 u32MLen;
    HI_U8  u8NLen;
    HI_U8  u8TLen;
    HI_U8  u8Reserve[2];
}HI_UNF_CIPHER_CCM_INFO_S;

typedef struct hiUNF_CIPHER_GCM_INFO_S
{
    HI_U8 *pu8Aad;
    HI_U32 u32ALen;
    HI_U32 u32MLen;
    HI_U32 u32IVLen;
}HI_UNF_CIPHER_GCM_INFO_S;

/** Structure of the cipher control information */
typedef struct hiHI_UNF_CIPHER_CTRL_S
{
    HI_U32 u32Key[8];                               /**< Key input */
    HI_U32 u32IV[4];                                /**< Initialization vector (IV) */
    HI_UNF_CIPHER_ALG_E enAlg;                      /**< Cipher algorithm */
    HI_UNF_CIPHER_BIT_WIDTH_E enBitWidth;           /**< Bit width for encryption or decryption */
    HI_UNF_CIPHER_WORK_MODE_E enWorkMode;           /**< Operating mode */
    HI_UNF_CIPHER_KEY_LENGTH_E enKeyLen;            /**< Key length */
    HI_UNF_CIPHER_CTRL_CHANGE_FLAG_S stChangeFlags; /**< control information exchange choices, we default all woulde be change except they have been in the choices */
    HI_UNF_CIPHER_KEY_SRC_E enKeySrc;             /**< Key source */
    union
    {
        HI_UNF_CIPHER_CCM_INFO_S stCCM;
        HI_UNF_CIPHER_GCM_INFO_S stGCM;
    }unModeInfo;
} HI_UNF_CIPHER_CTRL_S;

/** Cipher data */
typedef struct hiHI_UNF_CIPHER_DATA_S
{
    HI_U32 u32SrcPhyAddr;     /**< phy address of the original data */
    HI_U32 u32DestPhyAddr;    /**< phy address of the purpose data */
    HI_U32 u32ByteLength;     /**< cigher data length*/
} HI_UNF_CIPHER_DATA_S;

/** Hash algrithm type */
typedef enum hiHI_UNF_CIPHER_HASH_TYPE_E
{
    HI_UNF_CIPHER_HASH_TYPE_SHA1,
    HI_UNF_CIPHER_HASH_TYPE_SHA256,
    HI_UNF_CIPHER_HASH_TYPE_HMAC_SHA1,
    HI_UNF_CIPHER_HASH_TYPE_HMAC_SHA256,
    HI_UNF_CIPHER_HASH_TYPE_BUTT,
}HI_UNF_CIPHER_HASH_TYPE_E;

/** Hash init struct input */
typedef struct
{
    HI_U8 *pu8HMACKey;
    HI_U32 u32HMACKeyLen;
    HI_UNF_CIPHER_HASH_TYPE_E eShaType;
}HI_UNF_CIPHER_HASH_ATTS_S;

/** RSA encryption scheme*/
typedef enum hiHI_UNF_CIPHER_RSA_ENC_SCHEME_E
{ 
    HI_UNF_CIPHER_RSA_ENC_SCHEME_NO_PADDING,            /**< without padding */
    HI_UNF_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_0,          /**< PKCS#1 block type 0 padding*/
    HI_UNF_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_1,          /**< PKCS#1 block type 1 padding*/
    HI_UNF_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_2,          /**< PKCS#1 block type 2 padding*/
    HI_UNF_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA1,  /**< PKCS#1 RSAES-OAEP-SHA1 padding*/
    HI_UNF_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA256,/**< PKCS#1 RSAES-OAEP-SHA256 padding*/
    HI_UNF_CIPHER_RSA_ENC_SCHEME_RSAES_PKCS1_V1_5,      /**< PKCS#1 RSAES-PKCS1_V1_5 padding*/
    HI_UNF_CIPHER_RSA_SCHEME_BUTT,
}HI_UNF_CIPHER_RSA_ENC_SCHEME_E;

/** RSA signature algorithms*/
typedef enum hiHI_UNF_CIPHER_RSA_SIGN_SCHEME_E
{ 
    HI_UNF_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA1 = 0x100, /**< PKCS#1 RSASSA_PKCS1_V15_SHA1 signature*/
    HI_UNF_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA256,       /**< PKCS#1 RSASSA_PKCS1_V15_SHA256 signature*/
    HI_UNF_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA1,         /**< PKCS#1 RSASSA_PKCS1_PSS_SHA1 signature*/
    HI_UNF_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA256,       /**< PKCS#1 RSASSA_PKCS1_PSS_SHA256 signature*/
    HI_UNF_CIPHER_RSA_SIGN_SCHEME_BUTT,
}HI_UNF_CIPHER_RSA_SIGN_SCHEME_E;

/** RSA public key struct */
typedef struct
{
    HI_U8  *pu8N;              /**< point to public modulus  */
	HI_U8  *pu8E;			   /**< point to public exponent */
    HI_U16 u16NLen;            /**< length of public modulus */
    HI_U16 u16ELen;            /**< length of public exponent */
}HI_UNF_CIPHER_RSA_PUB_KEY_S;

/** RSA private key struct */
typedef struct
{
    HI_U8 *pu8N;                      /*!<  public modulus    */
    HI_U8 *pu8D;                      /*!<  private exponent  */
    HI_U16 u16NLen;                   /**< length of public modulus */
    HI_U16 u16DLen;                   /**< length of private exponent */
}HI_UNF_CIPHER_RSA_PRI_KEY_S;

/** RSA public key encryption struct input */
typedef struct
{
    HI_UNF_CIPHER_RSA_ENC_SCHEME_E enScheme;   /** RSA encryption scheme*/
    HI_UNF_CIPHER_RSA_PUB_KEY_S stPubKey;      /** RSA public key struct */
}HI_UNF_CIPHER_RSA_PUB_ENC_S;

/** RSA private key decryption struct input */
typedef struct
{
    HI_UNF_CIPHER_RSA_ENC_SCHEME_E enScheme; /** RSA encryption scheme */
    HI_UNF_CIPHER_RSA_PRI_KEY_S stPriKey;    /** RSA private key struct */
    HI_UNF_CIPHER_KEY_SRC_E enKeySrc;
}HI_UNF_CIPHER_RSA_PRI_ENC_S;

/** RSA signature struct input */
typedef struct
{
    HI_UNF_CIPHER_RSA_SIGN_SCHEME_E enScheme;  /** RSA signature scheme*/
    HI_UNF_CIPHER_RSA_PRI_KEY_S stPriKey;      /** RSA private key struct */
    HI_UNF_CIPHER_KEY_SRC_E enKeySrc;
 }HI_UNF_CIPHER_RSA_SIGN_S;

/** RSA signature verify struct input */
typedef struct
{
    HI_UNF_CIPHER_RSA_SIGN_SCHEME_E enScheme; /** RSA signature scheme*/
    HI_UNF_CIPHER_RSA_PUB_KEY_S stPubKey;     /** RSA public key struct */
 }HI_UNF_CIPHER_RSA_VERIFY_S;

/** @} */  /** <!-- ==== Structure Definition End ==== */


#define HI_UNF_CIPHER_Open(HI_VOID) HI_UNF_CIPHER_Init(HI_VOID);
#define HI_UNF_CIPHER_Close(HI_VOID) HI_UNF_CIPHER_DeInit(HI_VOID);

/******************************* API Declaration *****************************/
/** \addtogroup      CIPHER */
/** @{ */  /** <!-- [CIPHER] */
/* ---CIPHER---*/
/**
\brief  Init the cipher device.  
\attention \n
This API is used to start the cipher device.
\param N/A                                                                      
\retval ::HI_SUCCESS  Call this API successful.                                 
\retval ::HI_FAILURE  Call this API fails.                                      
\retval ::HI_ERR_CIPHER_FAILED_INIT  The cipher device fails to be initialized. 
\see \n
N/A
*/
HI_S32 HI_UNF_CIPHER_Init(HI_VOID);

/**
\brief  Deinit the cipher device.
\attention \n
This API is used to stop the cipher device. If this API is called repeatedly, HI_SUCCESS is returned, but only the first operation takes effect.

\param N/A                                                                      
\retval ::HI_SUCCESS  Call this API successful.                                 
\retval ::HI_FAILURE  Call this API fails.                                      
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.         
\see \n
N/A
*/
HI_S32 HI_UNF_CIPHER_DeInit(HI_VOID);

/**
\brief Obtain a cipher handle for encryption and decryption.

\param[in] cipher attributes                                                    
\param[out] phCipher Cipher handle                                              
\retval ::HI_SUCCESS Call this API successful.                                  
\retval ::HI_FAILURE Call this API fails.                                       
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.        
\retval ::HI_ERR_CIPHER_INVALID_POINT  The pointer is null.                     
\retval ::HI_ERR_CIPHER_FAILED_GETHANDLE  The cipher handle fails to be obtained, because there are no available cipher handles. 
\see \n
N/A
*/
HI_S32 HI_UNF_CIPHER_CreateHandle(HI_HANDLE* phCipher);

/**
\brief Destroy the existing cipher handle. 
\attention \n
This API is used to destroy existing cipher handles.

\param[in] hCipher Cipher handle                                                
\retval ::HI_SUCCESS  Call this API successful.                                 
\retval ::HI_FAILURE  Call this API fails.                                      
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.         
\see \n
N/A
*/
HI_S32 HI_UNF_CIPHER_DestroyHandle(HI_HANDLE hCipher);

/**
\brief Get the random number.

\attention \n
This API is used to obtain the random number from the hardware.

\param[out] pu32RandomNumber Point to the random number.                                        
\retval ::HI_SUCCESS  Call this API successful.                                                 
\retval ::HI_FAILURE  Call this API fails.                                                      

\see \n
N/A
*/
HI_S32 HI_UNF_CIPHER_GetRandomNumber(HI_U32 *pu32RandomNumber);

/**
\brief Configures the cipher control information.
\attention \n
Before encryption or decryption, you must call this API to configure the cipher control information.
The first 64-bit data and the last 64-bit data should not be the same when using TDES algorithm.

\param[in] hCipher Cipher handle.                                               
\param[in] pstCtrl Cipher control information.                                  
\retval ::HI_SUCCESS Call this API successful.                                  
\retval ::HI_FAILURE Call this API fails.                                       
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.         
\retval ::HI_ERR_CIPHER_INVALID_POINT  The pointer is null.                     
\retval ::HI_ERR_CIPHER_INVALID_PARA  The parameter is invalid.                 
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.                 
\see \n
N/A
*/
HI_S32 HI_UNF_CIPHER_ConfigHandle(HI_HANDLE hCipher, HI_UNF_CIPHER_CTRL_S* pstCtrl);

/**
\brief Performs encryption.

\attention \n
This API is used to perform encryption by using the cipher module.
The length of the encrypted data should be a multiple of 8 in TDES mode and 16 in AES mode. Besides, the length can not be bigger than 0xFFFFF.After this operation, the result will affect next operation.If you want to remove vector, you need to config IV(config pstCtrl->stChangeFlags.bit1IV with 1) by transfering HI_UNF_CIPHER_ConfigHandle.
\param[in] hCipher Cipher handle                                                
\param[in] u32SrcPhyAddr Physical address of the source data                    
\param[in] u32DestPhyAddr Physical address of the target data                   
\param[in] u32ByteLength   Length of the encrypted data                         
\retval ::HI_SUCCESS  Call this API successful.                                 
\retval ::HI_FAILURE  Call this API fails.                                      
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.         
\retval ::HI_ERR_CIPHER_INVALID_PARA  The parameter is invalid.                 
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.                  
\see \n
N/A
*/
HI_S32 HI_UNF_CIPHER_Encrypt(HI_HANDLE hCipher, HI_U32 u32SrcPhyAddr, HI_U32 u32DestPhyAddr, HI_U32 u32ByteLength);

/**
\brief Performs decryption.

\attention \n
This API is used to perform decryption by using the cipher module.
The length of the decrypted data should be a multiple of 8 in TDES mode and 16 in AES mode. Besides, the length can not be bigger than 0xFFFFF.After this operation, the result will affect next operation.If you want to remove vector, you need to config IV(config pstCtrl->stChangeFlags.bit1IV with 1) by transfering HI_UNF_CIPHER_ConfigHandle.
\param[in] hCipher Cipher handle.                                               
\param[in] u32SrcPhyAddr Physical address of the source data.                   
\param[in] u32DestPhyAddr Physical address of the target data.                  
\param[in] u32ByteLength Length of the decrypted data                          
\retval ::HI_SUCCESS Call this API successful.                                  
\retval ::HI_FAILURE Call this API fails.                                       
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.        
\retval ::HI_ERR_CIPHER_INVALID_PARA  The parameter is invalid.                 
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.                  
\see \n
N/A
*/
HI_S32 HI_UNF_CIPHER_Decrypt(HI_HANDLE hCipher, HI_U32 u32SrcPhyAddr, HI_U32 u32DestPhyAddr, HI_U32 u32ByteLength);

/**
\brief Encrypt multiple packaged data.
\attention \n
You can not encrypt more than 128 data package one time. When HI_ERR_CIPHER_BUSY return, the data package you send will not be deal, the custmer should decrease the number of data package or run cipher again.Note:When encrypting more than one packaged data, every one package will be calculated using initial vector configured by HI_UNF_CIPHER_ConfigHandle.Previous result will not affect the later result.
\param[in] hCipher cipher handle                                                                  
\param[in] pstDataPkg data package ready for cipher                                               
\param[in] u32DataPkgNum  number of package ready for cipher                                      
\retval ::HI_SUCCESS  Call this API successful.                                                   
\retval ::HI_FAILURE  Call this API fails.                                                        
\retval ::HI_ERR_CIPHER_NOT_INIT  cipher device have not been initialized                         
\retval ::HI_ERR_CIPHER_INVALID_PARA  parameter error                                             
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  handle invalid                                            
\retval ::HI_ERR_CIPHER_BUSY  hardware is busy, it can not deal with all data package once time   
\see \n
N/A
*/
HI_S32 HI_UNF_CIPHER_EncryptMulti(HI_HANDLE hCipher, HI_UNF_CIPHER_DATA_S *pstDataPkg, HI_U32 u32DataPkgNum);


/**
\brief Decrypt multiple packaged data.
\attention \n
You can not decrypt more than 128 data package one time.When HI_ERR_CIPHER_BUSY return, the data package you send will not be deal, the custmer should decrease the number of data package or run cipher again.Note:When decrypting more than one packaged data, every one package will be calculated using initial vector configured by HI_UNF_CIPHER_ConfigHandle.Previous result will not affect the later result.
\param[in] hCipher cipher handle                                                                 
\param[in] pstDataPkg data package ready for cipher                                             
\param[in] u32DataPkgNum  number of package ready for cipher                                     
\retval ::HI_SUCCESS  Call this API successful.                                                  
\retval ::HI_FAILURE  Call this API fails.                                                     
\retval ::HI_ERR_CIPHER_NOT_INIT  cipher device have not been initialized                        
\retval ::HI_ERR_CIPHER_INVALID_PARA  parameter error                                            
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  handle invalid                                           
\retval ::HI_ERR_CIPHER_BUSY  hardware is busy, it can not deal with all data package once time 
\see \n
N/A
*/
HI_S32 HI_UNF_CIPHER_DecryptMulti(HI_HANDLE hCipher, HI_UNF_CIPHER_DATA_S *pstDataPkg, HI_U32 u32DataPkgNum);

/**
\brief Encrypt multiple packaged data.
\attention \n
You can not encrypt more than 128 data package one time. When HI_ERR_CIPHER_BUSY return, the data package you send will not be deal, the custmer should decrease the number of data package or run cipher again.Note:When encrypting more than one packaged data, every one package will be calculated using initial vector configured by HI_UNF_CIPHER_ConfigHandle.Previous result will not affect the later result.
\param[in] hCipher cipher handle                                                                  
\param[in] pstCtrl Cipher control information.                                  
\param[in] pstDataPkg data package ready for cipher                                               
\param[in] u32DataPkgNum  number of package ready for cipher                                      
\retval ::HI_SUCCESS  Call this API successful.                                                   
\retval ::HI_FAILURE  Call this API fails.                                                        
\retval ::HI_ERR_CIPHER_NOT_INIT  cipher device have not been initialized                         
\retval ::HI_ERR_CIPHER_INVALID_PARA  parameter error                                             
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  handle invalid                                            
\retval ::HI_ERR_CIPHER_BUSY  hardware is busy, it can not deal with all data package once time   
\see \n
N/A
*/
HI_S32 HI_UNF_CIPHER_EncryptMultiEx(HI_HANDLE hCipher, HI_UNF_CIPHER_CTRL_S* pstCtrl, HI_UNF_CIPHER_DATA_S *pstDataPkg, HI_U32 u32DataPkgNum);

/**
\brief Decrypt multiple packaged data.
\attention \n
You can not decrypt more than 128 data package one time.When HI_ERR_CIPHER_BUSY return, the data package you send will not be deal, the custmer should decrease the number of data package or run cipher again.Note:When decrypting more than one packaged data, every one package will be calculated using initial vector configured by HI_UNF_CIPHER_ConfigHandle.Previous result will not affect the later result.
\param[in] hCipher cipher handle                                                                
\param[in] pstCtrl Cipher control information.                                  
\param[in] pstDataPkg data package ready for cipher                                              
\param[in] u32DataPkgNum  number of package ready for cipher                                    
\retval ::HI_SUCCESS  Call this API successful.                                                  
\retval ::HI_FAILURE  Call this API fails.                                                       
\retval ::HI_ERR_CIPHER_NOT_INIT  cipher device have not been initialized                        
\retval ::HI_ERR_CIPHER_INVALID_PARA  parameter error                                            
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  handle invalid                                           
\retval ::HI_ERR_CIPHER_BUSY  hardware is busy, it can not deal with all data package once time  
\see \n
N/A
*/
HI_S32 HI_UNF_CIPHER_DecryptMultiEx(HI_HANDLE hCipher, HI_UNF_CIPHER_CTRL_S* pstCtrl, HI_UNF_CIPHER_DATA_S *pstDataPkg, HI_U32 u32DataPkgNum);

/**
\brief get tag value.
\attention \n
You can get the tag value after encrypt/decrypt with CCM/GCM mode.
\param[in] hCipher cipher handle                                                                 
\param[out] pstTag TAG data                                                                      
\retval ::HI_SUCCESS  Call this API successful.                                                  
\retval ::HI_FAILURE  Call this API fails.                                                       
\retval ::HI_ERR_CIPHER_NOT_INIT  cipher device have not been initialized                        
\retval ::HI_ERR_CIPHER_INVALID_PARA  parameter error                                            
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  handle invalid                                           
\retval ::HI_ERR_CIPHER_BUSY  hardware is busy, it can not deal with all data package once time
\see \n
N/A
*/
HI_S32 HI_UNF_CIPHER_GetTag(HI_HANDLE hCipher,  HI_U8 *pstTag);

/**
\brief Encrypt the clean key data by KLAD.
\attention \n
N/A
\param[in] enRootKey klad root key.                   
\param[in] pu8CleanKey clean key.                     
\param[in] enTarget the module who to use this key.  
\param[out] pu8EcnryptKey encrypt key.                
\param[in] u32KeyLen clean key.                       
\retval ::HI_SUCCESS Call this API successful.                                  
\retval ::HI_FAILURE Call this API fails.                                       
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.         
\retval ::HI_ERR_CIPHER_INVALID_POINT  The pointer is null.                     
\retval ::HI_ERR_CIPHER_INVALID_PARA  The parameter is invalid.                
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.        
\see \n
N/A
*/
HI_S32 HI_UNF_CIPHER_KladEncryptKey(HI_UNF_CIPHER_KEY_SRC_E enRootKey, 
                                    HI_UNF_CIPHER_KLAD_TARGET_E enTarget, 
                                    HI_U8 *pu8CleanKey, HI_U8* pu8EcnryptKey, HI_U32 u32KeyLen);

/**
\brief Init the hash module, if other program is using the hash module, the API will return failure.

\attention \n
N/A

\param[in] pstHashAttr: The hash calculating structure input.                             
\param[out] pHashHandle: The output hash handle.                                        
\retval ::HI_SUCCESS  Call this API successful.                                           
\retval ::HI_FAILURE  Call this API fails.                                                 

\see \n
N/A
*/
HI_S32 HI_UNF_CIPHER_HashInit(HI_UNF_CIPHER_HASH_ATTS_S *pstHashAttr, HI_HANDLE *pHashHandle);

/**
\brief Calculate the hash, if the size of the data to be calculated is very big and the DDR ram is not enough, this API can calculate the data one block by one block. Attention: The input block length must be 64bytes alingned except for the last block.

\attention \n
N/A

\param[in] hHashHandl:  Hash handle.                                        
\param[in] pu8InputData:  The input data buffer.                           
\param[in] u32InputDataLen:  The input data length, attention: the block length input must be 64bytes aligned except the last block!      
\retval ::HI_SUCCESS  Call this API successful.                             
\retval ::HI_FAILURE  Call this API fails.                                  

\see \n
N/A
*/
HI_S32 HI_UNF_CIPHER_HashUpdate(HI_HANDLE hHashHandle, HI_U8 *pu8InputData, HI_U32 u32InputDataLen);



/**
\brief Get the final hash value, after calculate all of the data, call this API to get the final hash value and close the handle.If there is some reason need to interupt the calculation, this API should also be call to close the handle.

\attention \n
N/A

\param[in] hHashHandle:  Hash handle.                                       
\param[out] pu8OutputHash:  The final output hash value.                    

\retval ::HI_SUCCESS  Call this API successful.                             
\retval ::HI_FAILURE  Call this API fails.                             

\see \n
N/A
*/
HI_S32 HI_UNF_CIPHER_HashFinal(HI_HANDLE hHashHandle, HI_U8 *pu8OutputHash);

/**
\brief compute the hash module

\attention \n
N/A

\param[in] pstHashAttr: The hash calculating structure input.              
\param[in] u32DataPhyAddr:  Physical address of input data.                
\param[in] u32ByteLength:   The input data length                          
\param[out] pu8OutputHash:  The final output hash value.                   
\retval ::HI_SUCCESS  Call this API successful.                            
\retval ::HI_FAILURE  Call this API fails.                                 

\see \n
N/A
*/
HI_S32 HI_UNF_CIPHER_Hash(HI_UNF_CIPHER_HASH_ATTS_S *pstHashAttr, HI_U32 u32DataPhyAddr, HI_U32 u32ByteLength, HI_U8 *pu8OutputHash);

/**
\brief RSA encryption a plaintext with a RSA public key.

\attention \n
N/A

\param[in] pstRsaEnc:   encryption struct.                                   
\param[in] pu8Input    input data to be encryption                          
\param[out] u32InLen:   length of input data to be encryption                
\param[out] pu8Output  output data to be encryption                         
\param[out] pu32OutLen: length of output data to be encryption               

\retval ::HI_SUCCESS  Call this API successful.                         
\retval ::HI_FAILURE  Call this API fails.                              

\see \n
N/A
*/
HI_S32 HI_UNF_CIPHER_RsaPublicEnc(HI_UNF_CIPHER_RSA_PUB_ENC_S *pstRsaEnc, 
								  HI_U8 *pu8Input, HI_U32 u32InLen, 
								  HI_U8 *pu8Output, HI_U32 *pu32OutLen);

/**
\brief RSA decryption a ciphertext with a RSA private key.

\attention \n
N/A

\param[in] pstRsaDec:   decryption struct.                                   
\param[in] pu8Input    input data to be decryption                          
\param[out] u32InLen:   length of input data to be decryption                
\param[out] pu8Output  output data to be decryption                         
\param[out] pu32OutLen: length of output data to be decryption               

\retval ::HI_SUCCESS  Call this API successful.                         
\retval ::HI_FAILURE  Call this API fails.                             

\see \n
N/A
*/
HI_S32 HI_UNF_CIPHER_RsaPrivateDec(HI_UNF_CIPHER_RSA_PRI_ENC_S *pstRsaDec,								  
								   HI_U8 *pu8Input, HI_U32 u32InLen, 
								   HI_U8 *pu8Output, HI_U32 *pu32OutLen);

/**
\brief RSA encryption a plaintext with a RSA private key.

\attention \n
N/A

\param[in] pstRsaSign:   encryption struct.   
\param[in] pu8Input     input data to be encryption  
\param[out] u32InLen:   length of input data to be encryption 
\param[out] pu8Output   output data to be encryption              
\param[out] pu32OutLen: length of output data to be encryption   

\retval ::HI_SUCCESS  Call this API successful.           
\retval ::HI_FAILURE  Call this API fails.                 

\see \n
N/A
*/
HI_S32 HI_UNF_CIPHER_RsaPrivateEnc(HI_UNF_CIPHER_RSA_PRI_ENC_S *pstRsaEnc, 
							 	   HI_U8 *pu8Input, HI_U32 u32InLen, 
								   HI_U8 *pu8Output, HI_U32 *pu32OutLen);

/**
\brief RSA decryption a ciphertext with a RSA public key.

\attention \n
N/A

\param[in] pstRsaVerify:   decryption struct.                                
\param[in] pu8Input     input data to be decryption                          
\param[out] u32InLen:   length of input data to be decryption               
\param[out] pu8Output   output data to be decryption                        
\param[out] pu32OutLen: length of output data to be decryption               

\retval ::HI_SUCCESS  Call this API successful.                        
\retval ::HI_FAILURE  Call this API fails.                              

\see \n
N/A
*/
HI_S32 HI_UNF_CIPHER_RsaPublicDec(HI_UNF_CIPHER_RSA_PUB_ENC_S *pstRsaDec,
							   HI_U8 *pu8Input, HI_U32 u32InLen,
							   HI_U8 *pu8Output, HI_U32 *pu32OutLen);

/**
\brief RSA signature a context with appendix, where a signer's RSA private key is used.

\attention \n
N/A

\param[in] pstRsaSign:      signature struct.                                  
\param[in] pu8Input         input context to be signature, maybe null           
\param[in] u32InLen:        length of input context to be signature              
\param[in] pu8HashData      hash value of context,if NULL, let pu8HashData = Hash(context) automatically 
\param[out] pu8OutSign      output message of signature                          
\param[out] pu32OutSignLen: length of message of signature                      

\retval ::HI_SUCCESS  Call this API successful.                       
\retval ::HI_FAILURE  Call this API fails.                           

\see \n
N/A
*/								   
HI_S32 HI_UNF_CIPHER_RsaSign(HI_UNF_CIPHER_RSA_SIGN_S *pstRsaSign, 
							 HI_U8 *pu8InData, HI_U32 u32InDataLen,
							 HI_U8 *pu8HashData,
							 HI_U8 *pu8OutSign, HI_U32 *pu32OutSignLen);

/**
\brief RSA signature verification a context with appendix, where a signer's RSA public key is used.

\attention \n
N/A

\param[in] pstRsaVerify:    signature verification struct.                         
\param[in] pu8Input         input context to be signature verification, maybe null 
\param[in] u32InLen:        length of input context to be signature                
\param[in] pu8HashData      hash value of context,if NULL, let pu8HashData = Hash(context) automatically     
\param[in] pu8InSign        message of signature                                 
\param[in] pu32InSignLen:   length of message of signature                       

\retval ::HI_SUCCESS  Call this API successful.                         
\retval ::HI_FAILURE  Call this API fails.                              

\see \n
N/A
*/
HI_S32 HI_UNF_CIPHER_RsaVerify(HI_UNF_CIPHER_RSA_VERIFY_S *pstRsaVerify,
							   HI_U8 *pu8InData, HI_U32 u32InDataLen,
							   HI_U8 *pu8HashData,
							   HI_U8 *pu8InSign, HI_U32 u32InSignLen);

/** @} */  /** <!-- ==== API declaration end ==== */
	
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_UNF_CIPHER_H__ */
