/**
 * Main public include file
 */
#ifndef __KINESIS_VIDEO_COMMON_INCLUDE__
#define __KINESIS_VIDEO_COMMON_INCLUDE__

#pragma once

#ifdef  __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////
// Public headers
////////////////////////////////////////////////////
#include <com/amazonaws/kinesis/video/client/Include.h>
#ifndef JSMN_HEADER
#define JSMN_HEADER
#endif
#include <com/amazonaws/kinesis/video/common/jsmn.h>

////////////////////////////////////////////////////
// Status return codes
////////////////////////////////////////////////////

// This section is done for backward compat. We shouldn't add to it. New status should be added to common base section
#define STATUS_COMMON_PRODUCER_BASE                                                 0x15000000
#define STATUS_INVALID_AWS_CREDENTIALS_VERSION                                      STATUS_COMMON_PRODUCER_BASE + 0x00000008
#define STATUS_MAX_REQUEST_HEADER_COUNT                                             STATUS_COMMON_PRODUCER_BASE + 0x00000009
#define STATUS_MAX_REQUEST_HEADER_NAME_LEN                                          STATUS_COMMON_PRODUCER_BASE + 0x0000000a
#define STATUS_MAX_REQUEST_HEADER_VALUE_LEN                                         STATUS_COMMON_PRODUCER_BASE + 0x0000000b
#define STATUS_INVALID_API_CALL_RETURN_JSON                                         STATUS_COMMON_PRODUCER_BASE + 0x0000000c
#define STATUS_CURL_INIT_FAILED                                                     STATUS_COMMON_PRODUCER_BASE + 0x0000000d
#define STATUS_CURL_LIBRARY_INIT_FAILED                                             STATUS_COMMON_PRODUCER_BASE + 0x0000000e
#define STATUS_HMAC_GENERATION_ERROR                                                STATUS_COMMON_PRODUCER_BASE + 0x00000010
#define STATUS_IOT_FAILED                                                           STATUS_COMMON_PRODUCER_BASE + 0x00000011
#define STATUS_MAX_ROLE_ALIAS_LEN_EXCEEDED                                          STATUS_COMMON_PRODUCER_BASE + 0x00000012
#define STATUS_INVALID_USER_AGENT_LENGTH                                            STATUS_COMMON_PRODUCER_BASE + 0x00000015
#define STATUS_IOT_EXPIRATION_OCCURS_IN_PAST                                        STATUS_COMMON_PRODUCER_BASE + 0x00000017
#define STATUS_IOT_EXPIRATION_PARSING_FAILED                                        STATUS_COMMON_PRODUCER_BASE + 0x00000018
#define STATUS_MAX_IOT_THING_NAME_LENGTH                                            STATUS_COMMON_PRODUCER_BASE + 0x0000001e
#define STATUS_IOT_CREATE_LWS_CONTEXT_FAILED                                        STATUS_COMMON_PRODUCER_BASE + 0x0000001f
#define STATUS_INVALID_CA_CERT_PATH                                                 STATUS_COMMON_PRODUCER_BASE + 0x00000020
#define STATUS_FILE_CREDENTIAL_PROVIDER_OPEN_FILE_FAILED                            STATUS_COMMON_PRODUCER_BASE + 0x00000022
#define STATUS_FILE_CREDENTIAL_PROVIDER_INVALID_FILE_LENGTH                         STATUS_COMMON_PRODUCER_BASE + 0x00000023
#define STATUS_FILE_CREDENTIAL_PROVIDER_INVALID_FILE_FORMAT                         STATUS_COMMON_PRODUCER_BASE + 0x00000024

// Continue errors from the new common base
#define STATUS_COMMON_BASE                                                          0x16000000

////////////////////////////////////////////////////
// Main defines
////////////////////////////////////////////////////

/**
 * Environment variable to enable file logging. Run export AWS_ENABLE_FILE_LOGGING=TRUE to enable file
 * logging
 */
#define ENABLE_FILE_LOGGING                                                     ((PCHAR) "AWS_ENABLE_FILE_LOGGING")

/**
 * Max region name
 */
#define MAX_REGION_NAME_LEN                                                     128

/**
 * Max user agent string
 */
#define MAX_USER_AGENT_LEN                                                      256

/**
 * Max custom user agent string
 */
#define MAX_CUSTOM_USER_AGENT_LEN                                               128

/**
 * Max custom user agent name postfix string
 */
#define MAX_CUSTOM_USER_AGENT_NAME_POSTFIX_LEN                                  32

/**
 * Default Video track ID to be used
 */
#define DEFAULT_VIDEO_TRACK_ID                                                  1

/**
 * Default Audio track ID to be used
 */
#define DEFAULT_AUDIO_TRACK_ID                                                  2

/*
 * Max access key id length https://docs.aws.amazon.com/STS/latest/APIReference/API_Credentials.html
 */
#define MAX_ACCESS_KEY_LEN                                                      128

/*
 * Max secret access key length
 */
#define MAX_SECRET_KEY_LEN                                                      128

/*
 * Max session token string length
 */
#define MAX_SESSION_TOKEN_LEN                                                   2048

/*
 * Max expiration string length
 */
#define MAX_EXPIRATION_LEN                                                      128

/*
 * Max role alias length https://docs.aws.amazon.com/iot/latest/apireference/API_UpdateRoleAlias.html
 */
#define MAX_ROLE_ALIAS_LEN                                                      128

/**
 * Max string length for IoT thing name
 */
#define MAX_IOT_THING_NAME_LEN                                                  MAX_STREAM_NAME_LEN

/**
 * Default period for the cached endpoint update
 */
#define DEFAULT_ENDPOINT_CACHE_UPDATE_PERIOD                                    (40 * HUNDREDS_OF_NANOS_IN_A_MINUTE)

/**
 * Sentinel value indicating to use default update period
 */
#define ENDPOINT_UPDATE_PERIOD_SENTINEL_VALUE                                   0

/**
 * Max period for the cached endpoint update
 */
#define MAX_ENDPOINT_CACHE_UPDATE_PERIOD                                        (24 * HUNDREDS_OF_NANOS_IN_AN_HOUR)

/**
 * AWS credential environment variable name
 */
#define ACCESS_KEY_ENV_VAR                                                      ((PCHAR) "AWS_ACCESS_KEY_ID")
#define SECRET_KEY_ENV_VAR                                                      ((PCHAR) "AWS_SECRET_ACCESS_KEY")
#define SESSION_TOKEN_ENV_VAR                                                   ((PCHAR) "AWS_SESSION_TOKEN")
#define DEFAULT_REGION_ENV_VAR                                                  ((PCHAR) "AWS_DEFAULT_REGION")
#define CACERT_PATH_ENV_VAR                                                     ((PCHAR) "AWS_KVS_CACERT_PATH")
#define DEBUG_LOG_LEVEL_ENV_VAR                                                 ((PCHAR) "AWS_KVS_LOG_LEVEL")

#ifdef CMAKE_DETECTED_CACERT_PATH
#define DEFAULT_KVS_CACERT_PATH                                                 KVS_CA_CERT_PATH
#else
#define DEFAULT_KVS_CACERT_PATH                                                 EMPTY_STRING
#endif

// Protocol scheme names
#define HTTPS_SCHEME_NAME                       "https"
#define WSS_SCHEME_NAME                         "wss"

// Max header name length in chars
#define MAX_REQUEST_HEADER_NAME_LEN             128

// Max header value length in chars
#define MAX_REQUEST_HEADER_VALUE_LEN            2048

// Max header count
#define MAX_REQUEST_HEADER_COUNT                200

// Max delimiter characters when packing headers into a string for printout
#define MAX_REQUEST_HEADER_OUTPUT_DELIMITER     5

// Max request header length in chars including the name/value, delimiter and null terminator
#define MAX_REQUEST_HEADER_STRING_LEN           (MAX_REQUEST_HEADER_NAME_LEN + MAX_REQUEST_HEADER_VALUE_LEN + 3)

// Literal definitions of the request verbs
#define HTTP_REQUEST_VERB_GET_STRING            (PCHAR) "GET"
#define HTTP_REQUEST_VERB_PUT_STRING            (PCHAR) "PUT"
#define HTTP_REQUEST_VERB_POST_STRING           (PCHAR) "POST"

// Schema delimiter string
#define SCHEMA_DELIMITER_STRING                 (PCHAR) "://"

// Default canonical URI if we fail to get anything from the parsing
#define DEFAULT_CANONICAL_URI_STRING            (PCHAR) "/"

// HTTP status OK
#define HTTP_STATUS_CODE_OK                         200

// HTTP status Request timed out
#define HTTP_STATUS_CODE_REQUEST_TIMEOUT            408

/**
 * Maximal length of the credentials file
 */
#define MAX_CREDENTIAL_FILE_LEN                     MAX_AUTH_LEN

/**
 * Default AWS region
 */
#define DEFAULT_AWS_REGION                          "us-west-2"

/**
 * Control plane prefix
 */
#define CONTROL_PLANE_URI_PREFIX                    "https://"

/**
 * KVS service name
 */
#define KINESIS_VIDEO_SERVICE_NAME                  "kinesisvideo"

/**
 * Control plane postfix
 */
#define CONTROL_PLANE_URI_POSTFIX                   ".amazonaws.com"

/**
 * Default user agent name
 */
#define DEFAULT_USER_AGENT_NAME                     "AWS-SDK-KVS"

/**
 * Max number of tokens in the API return JSON
 */
#define MAX_JSON_TOKEN_COUNT                        100

/**
 * Max parameter JSON string len which will be used for preparing the parameterized strings for the API calls.
 */
#define MAX_JSON_PARAMETER_STRING_LEN               (10 * 1024)

/**
 * Current versions for the public structs
 */
#define AWS_CREDENTIALS_CURRENT_VERSION             0

/**
 * Buffer length for the error to be stored in
 */
#define CALL_INFO_ERROR_BUFFER_LEN                  256

/**
 * Parameterized string for each tag pair
 */
#define TAG_PARAM_JSON_TEMPLATE                     "\n\t\t\"%s\": \"%s\","

/**
 * Low speed limits in bytes per duration
 */
#define DEFAULT_LOW_SPEED_LIMIT                     30

/**
 * Low speed limits in 100ns for the amount of bytes per this duration
 */
#define DEFAULT_LOW_SPEED_TIME_LIMIT                (30 * HUNDREDS_OF_NANOS_IN_A_SECOND)

// Header delimiter for requests and it's size
#define REQUEST_HEADER_DELIMITER                    ((PCHAR) ": ")
#define REQUEST_HEADER_DELIMITER_SIZE               (2 * SIZEOF(CHAR))

/*
 * Default SSL port
 */
#define DEFAULT_SSL_PORT_NUMBER                     443

/*
 * Default non-SSL port
 */
#define DEFAULT_NON_SSL_PORT_NUMBER                 8080

/**
 * AWS service Request id header name
 */
#define KVS_REQUEST_ID_HEADER_NAME                  "x-amzn-RequestId"

////////////////////////////////////////////////////
// Main structure declarations
////////////////////////////////////////////////////

/**
 * Types of verbs
 */
typedef enum {
    HTTP_REQUEST_VERB_GET,
    HTTP_REQUEST_VERB_POST,
    HTTP_REQUEST_VERB_PUT
} HTTP_REQUEST_VERB;

/**
 * Request SSL certificate type Not specified, "DER", "PEM", "ENG"
 */
typedef enum {
    SSL_CERTIFICATE_TYPE_NOT_SPECIFIED,
    SSL_CERTIFICATE_TYPE_DER,
    SSL_CERTIFICATE_TYPE_PEM,
    SSL_CERTIFICATE_TYPE_ENG,
} SSL_CERTIFICATE_TYPE;

/**
 * AWS Credentials declaration
 */
typedef struct __AwsCredentials AwsCredentials;
struct __AwsCredentials {
    // Version
    UINT32 version;

    // Size of the entire structure in bytes including the struct itself
    UINT32 size;

    // Access Key ID - NULL terminated
    PCHAR accessKeyId;

    // Length of the access key id - not including NULL terminator
    UINT32 accessKeyIdLen;

    // Secret Key - NULL terminated
    PCHAR secretKey;

    // Length of the secret key - not including NULL terminator
    UINT32 secretKeyLen;

    // Session token - NULL terminated
    PCHAR sessionToken;

    // Length of the session token - not including NULL terminator
    UINT32 sessionTokenLen;

    // Expiration in absolute time in 100ns.
    UINT64 expiration;

    // The rest of the data might follow the structure
};
typedef struct __AwsCredentials* PAwsCredentials;

/**
 * Request Header structure
 */
typedef struct __RequestHeader RequestHeader;
struct __RequestHeader {
    // Request header name
    PCHAR pName;

    // Header name length
    UINT32 nameLen;

    // Request header value
    PCHAR pValue;

    // Header value length
    UINT32 valueLen;
};
typedef struct __RequestHeader* PRequestHeader;

/**
 * Request info structure
 */
typedef struct __RequestInfo RequestInfo;
struct __RequestInfo {
    // Indicating whether the request is being terminated
    volatile ATOMIC_BOOL terminating;

    // HTTP verb
    HTTP_REQUEST_VERB verb;

    // Body of the request.
    // NOTE: In streaming mode the body will be NULL
    // NOTE: The body will follow the main struct
    PCHAR body;

    // Size of the body in bytes
    UINT32 bodySize;

    // The URL for the request
    CHAR url[MAX_URI_CHAR_LEN + 1];

    // CA Certificate path to use - optional
    CHAR certPath[MAX_PATH_LEN + 1];

    // SSL Certificate file path to use - optional
    CHAR sslCertPath[MAX_PATH_LEN + 1];

    // SSL Certificate private key file path to use - optional
    CHAR sslPrivateKeyPath[MAX_PATH_LEN + 1];

    // One of the following types "DER", "PEM", "ENG"
    SSL_CERTIFICATE_TYPE certType;

    // Region
    CHAR region[MAX_REGION_NAME_LEN + 1];

    // Current time when request was created
    UINT64 currentTime;

    // Call completion timeout
    UINT64 completionTimeout;

    // Connection completion timeout
    UINT64 connectionTimeout;

    // Call after time
    UINT64 callAfter;

    // Low-speed limit
    UINT64 lowSpeedLimit;

    // Low-time limit
    UINT64 lowSpeedTimeLimit;

    // AWS Credentials
    PAwsCredentials pAwsCredentials;

    // Request headers
    PSingleList pRequestHeaders;
};
typedef struct __RequestInfo* PRequestInfo;

/**
* Call Info structure
*/
typedef struct __CallInfo CallInfo;
struct __CallInfo {
    // Original request info
    PRequestInfo pRequestInfo;

    // HTTP status code of the execution
    UINT32 httpStatus;

    // Execution result
    SERVICE_CALL_RESULT callResult;

    // Error buffer for curl calls
    CHAR errorBuffer[CALL_INFO_ERROR_BUFFER_LEN + 1];

    // Response Headers list
    PStackQueue pResponseHeaders;

    // Request ID if specified
    PRequestHeader pRequestId;

    // Buffer to write the data to - will be allocated. Buffer is freed by a caller.
    PCHAR responseData;

    // Response data size
    UINT32 responseDataLen;
};
typedef struct __CallInfo* PCallInfo;

typedef struct __AwsCredentialProvider *PAwsCredentialProvider;

/**
 * Function returning AWS credentials
 */
typedef STATUS (*GetCredentialsFunc)(PAwsCredentialProvider, PAwsCredentials*);

/**
 * Abstract base for the credential provider
 */
typedef struct __AwsCredentialProvider AwsCredentialProvider;
struct __AwsCredentialProvider {
    // Get credentials function which will be overwritten by different implementations
    GetCredentialsFunc getCredentialsFn;
};

////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////

/**
 * Creates an AWS credentials object
 *
 * @param - PCHAR - IN - Access Key Id
 * @param - UINT32 - IN - Access Key Id Length excluding NULL terminator or 0 to calculate
 * @param - PCHAR - IN - Secret Key
 * @param - UINT32 - IN - Secret Key Length excluding NULL terminator or 0 to calculate
 * @param - PCHAR - IN/OPT - Session Token
 * @param - UINT32 - IN/OPT - Session Token Length excluding NULL terminator or 0 to calculate
 * @param - UINT64 - IN - Expiration in 100ns absolute time
 * @param - PAwsCredentials* - OUT - Constructed object
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS createAwsCredentials(PCHAR, UINT32, PCHAR, UINT32, PCHAR, UINT32, UINT64, PAwsCredentials*);

/**
 * Frees an Aws credentials object
 *
 * @param - PAwsCredentials* - IN/OUT - Object to be destroyed.
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS freeAwsCredentials(PAwsCredentials*);

/**
 * Deserialize an AWS credentials object, adapt the accessKey/secretKey/sessionToken pointer
 * to offset following the AwsCredential structure
 *
 * @param - PBYTE - IN - Token to be deserialized.
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS deserializeAwsCredentials(PBYTE);

/**
 * Creates a Static AWS credential provider object
 *
 * @param - PCHAR - IN - Access Key Id
 * @param - UINT32 - IN - Access Key Id Length excluding NULL terminator or 0 to calculate
 * @param - PCHAR - IN - Secret Key
 * @param - UINT32 - IN - Secret Key Length excluding NULL terminator or 0 to calculate
 * @param - PCHAR - IN/OPT - Session Token
 * @param - UINT32 - IN/OPT - Session Token Length excluding NULL terminator or 0 to calculate
 * @param - UINT64 - IN - Expiration in 100ns absolute time
 * @param - PAwsCredentialProvider* - OUT - Constructed AWS credentials provider object
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS createStaticCredentialProvider(PCHAR, UINT32, PCHAR, UINT32, PCHAR, UINT32, UINT64, PAwsCredentialProvider*);

/**
 * Frees a Static Aws credential provider object
 *
 * @param - PAwsCredentialProvider* - IN/OUT - Object to be destroyed.
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS freeStaticCredentialProvider(PAwsCredentialProvider*);

/**
 * Creates an IoT based AWS credential provider object using libCurl
 *
 * @param - PCHAR - IN - IoT endpoint
 * @param - PCHAR - IN - Cert file path
 * @param - PCHAR - IN - Private key file path
 * @param - PCHAR - IN/OPT - CA cert file path
 * @param - PCHAR - IN - Role alias
 * @param - PCHAR - IN - IoT thing name
 * @param - PAwsCredentialProvider* - OUT - Constructed AWS credentials provider object
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS createCurlIotCredentialProvider(PCHAR, PCHAR, PCHAR, PCHAR, PCHAR, PCHAR, PAwsCredentialProvider*);

/**
 * Creates an IoT based AWS credential provider object using libWebSockets
 *
 * @param - PCHAR - IN - IoT endpoint
 * @param - PCHAR - IN - Cert file path
 * @param - PCHAR - IN - Private key file path
 * @param - PCHAR - IN/OPT - CA cert file path
 * @param - PCHAR - IN - Role alias
 * @param - PCHAR - IN - IoT thing name
 * @param - PAwsCredentialProvider* - OUT - Constructed AWS credentials provider object
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS createLwsIotCredentialProvider(PCHAR, PCHAR, PCHAR, PCHAR, PCHAR, PCHAR, PAwsCredentialProvider*);

/**
 * Creates an IoT based AWS credential provider object with time function which is based on libCurl
 *
 * @param - PCHAR - IN - IoT endpoint
 * @param - PCHAR - IN - Cert file path
 * @param - PCHAR - IN - Private key file path
 * @param - PCHAR - IN/OPT - CA cert file path
 * @param - PCHAR - IN - Role alias
 * @param - PCHAR - IN - IoT thing name
 * @param - GetCurrentTimeFunc - IN - Custom current time function
 * @param - UINT64 - IN - Time function custom data
 * @param - PAwsCredentialProvider* - OUT - Constructed AWS credentials provider object
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS createCurlIotCredentialProviderWithTime(PCHAR, PCHAR, PCHAR, PCHAR, PCHAR, PCHAR,
        GetCurrentTimeFunc, UINT64,
        PAwsCredentialProvider*);

/**
 * Creates an IoT based AWS credential provider object with time function which is based on libWebSockets
 *
 * @param - PCHAR - IN - IoT endpoint
 * @param - PCHAR - IN - Cert file path
 * @param - PCHAR - IN - Private key file path
 * @param - PCHAR - IN/OPT - CA cert file path
 * @param - PCHAR - IN - Role alias
 * @param - PCHAR - IN - IoT thing name
 * @param - GetCurrentTimeFunc - IN - Custom current time function
 * @param - UINT64 - IN - Time function custom data
 * @param - PAwsCredentialProvider* - OUT - Constructed AWS credentials provider object
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS createLwsIotCredentialProviderWithTime(PCHAR, PCHAR, PCHAR, PCHAR, PCHAR, PCHAR,
        GetCurrentTimeFunc, UINT64,
        PAwsCredentialProvider*);

/**
 * Frees an IoT based Aws credential provider object
 *
 * @param - PAwsCredentialProvider* - IN/OUT - Object to be destroyed.
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS freeIotCredentialProvider(PAwsCredentialProvider*);

/**
 * Creates a File based AWS credential provider object
 *
 * @param - PCHAR - IN - Credentials file path
 * @param - PAwsCredentialProvider* - OUT - Constructed AWS credentials provider object
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS createFileCredentialProvider(PCHAR, PAwsCredentialProvider*);

/**
 * Creates a File based AWS credential provider object
 *
 * @param - PCHAR - IN - Credentials file path
 * @param - GetCurrentTimeFunc - IN - Current time function
 * @param - UINT64 - IN - Time function custom data
 * @param - PAwsCredentialProvider* - OUT - Constructed AWS credentials provider object
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS createFileCredentialProviderWithTime(PCHAR, GetCurrentTimeFunc, UINT64, PAwsCredentialProvider*);

/**
 * Frees a File based Aws credential provider object
 *
 * @param - PAwsCredentialProvider* - IN/OUT - Object to be destroyed.
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS freeFileCredentialProvider(PAwsCredentialProvider*);

/**
 * Creates a Request Info object
 *
 * @param - PCHAR - IN - URL of the request
 * @param - PCHAR - IN/OPT - Body of the request
 * @param - PCHAR - IN - Region
 * @param - PCHAR - IN/OPT - CA Certificate path/file
 * @param - PCHAR - IN/OPT - SSL Certificate path/file
 * @param - PCHAR - IN/OPT - SSL Certificate private key file path
 * @param - SSL_CERTIFICATE_TYPE - IN/OPT - SSL certificate file type
 * @param - PCHAR - IN/OPT - User agent string
 * @param - UINT64 - IN - Connection timeout
 * @param - UINT64 - IN - Completion timeout
 * @param - UINT64 - IN/OPT - Low speed limit
 * @param - UINT64 - IN/OPT - Low speed time limit
 * @param - PAwsCredentials - IN/OPT - Credentials to use for the call
 * @param - PRequestInfo* - IN/OUT - The newly created object
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS createRequestInfo(PCHAR, PCHAR, PCHAR, PCHAR, PCHAR, PCHAR,
                                    SSL_CERTIFICATE_TYPE, PCHAR,
                                    UINT64, UINT64, UINT64, UINT64,
                                    PAwsCredentials, PRequestInfo*);

/**
 * Frees a Request Info object
 *
 * @param - PRequestInfo* - IN/OUT - The object to release
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS freeRequestInfo(PRequestInfo*);

/**
 * Signs a request by appending SigV4 headers
 *
 * @param - PRequestInfo - IN/OUT request info for signing
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS signAwsRequestInfo(PRequestInfo);

/**
 * Signs a request by appending SigV4 query param
 *
 * @param - PRequestInfo - IN/OUT request info for signing
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS signAwsRequestInfoQueryParam(PRequestInfo);

/**
 * Gets a request host string
 *
 * @param - PCHAR - IN - Request URL
 * @param - PCHAR* - OUT - The request host start character. NULL on error.
 * @param - PCHAR* - OUT - The request host end character. NULL on error.
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS getRequestHost(PCHAR, PCHAR*, PCHAR*);

/**
 * Compares JSON strings taking into account the type
 *
 * @param - PCHAR - IN - JSON string being parsed
 * @param - jsmntok_t* - IN - Jsmn token to match
 * @param - jsmntype_t - IN - Jsmn token type to match
 * @param - PCHAR - IN - Token name to match
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API BOOL compareJsonString(PCHAR, jsmntok_t*, jsmntype_t, PCHAR);

/**
 * Converts the timestamp string to time
 *
 * @param - PCHAR - IN - String to covert
 * @param - UINT64 - IN - Current time
 * @param - PUINT64 - IN/OUT - Converted time
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS convertTimestampToEpoch(PCHAR, UINT64, PUINT64);

/**
 * Creates a user agent string
 *
 * @param - PCHAR - IN - User agent name
 * @param - PCHAR - IN - Custom user agent string
 * @param - UINT32 - IN - Length of the string
 * @param - PCHAR - OUT - Combined user agent string
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS getUserAgentString(PCHAR, PCHAR, UINT32, PCHAR);

/**
 * Checks whether the request URL requires a secure connection
 *
 * @param - PCHAR - IN - Request URL
 * @param - PBOOL - OUT - returned value
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS requestRequiresSecureConnection(PCHAR, PBOOL);

/**
 * Sets a header in the request info
 *
 * @param - PRequestInfo - IN - Request Info object
 * @param - PCHAR - IN - Header name
 * @param - UINT32 - IN/OPT - Header name length. Calculated if 0
 * @param - PCHAR - IN - Header value
 * @param - UINT32 - IN/OPT - Header value length. Calculated if 0
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS setRequestHeader(PRequestInfo, PCHAR, UINT32, PCHAR, UINT32);

/**
 * Removes a header from the headers list if exists
 *
 * @param - PRequestInfo - IN - Request Info object
 * @param - PCHAR - IN - Header name to check and remove
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS removeRequestHeader(PRequestInfo, PCHAR);

/**
* Removes and deletes all headers
*
* @param - PRequestInfo - IN - Request Info object
*
* @return - STATUS code of the execution
*/
PUBLIC_API STATUS removeRequestHeaders(PRequestInfo);

/**
 * Creates a request header
 *
 * @param - PCHAR - IN - Header name
 * @param - UINT32 - IN - Header name length
 * @param - PCHAR - IN - Header value
 * @param - UINT32 - IN - Header value length
 * @param - PRequestHeader* - OUT - Resulting object
 *
 * @return
 */
PUBLIC_API STATUS createRequestHeader(PCHAR, UINT32, PCHAR, UINT32, PRequestHeader*);

/**
 * Convenience method to convert HTTP statuses to SERVICE_CALL_RESULT status.
 *
 * @param - UINT32 - http_status the HTTP status code of the call
 *
 * @return The HTTP status translated into a SERVICE_CALL_RESULT value.
 */
PUBLIC_API SERVICE_CALL_RESULT getServiceCallResultFromHttpStatus(UINT32);

/**
 * Releases the CallInfo allocations
 *
 * @param - PCallInfo - IN - Call info object to release
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS releaseCallInfo(PCallInfo);

/**
 * Creates a file based logger object and installs the global logger callback function
 *
 * @param - UINT64 - IN - Size of string buffer in file logger. When the string buffer is full the logger will flush everything into a new file
 * @param - UINT64 - IN - Max number of log file. When exceeded, the oldest file will be deleted when new one is generated
 * @param - PCHAR - IN - Directory in which the log file will be generated
 * @param - BOOL - IN - Whether to print log to std out too
 * @param - BOOL - IN - Whether to set global logger function pointer
 * @param - logPrintFunc* - OUT/OPT - Optional function pointer to be returned to the caller that contains the main function for actual output
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS createFileLogger(UINT64, UINT64, PCHAR, BOOL, BOOL, logPrintFunc*);

/**
 * Frees the static file logger object and resets the global logging function if it was
 * previously set by the create function.
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS freeFileLogger();

/**
 * Helper macros to be used in pairs at the application start and end
 */
#define SET_FILE_LOGGER()               createFileLogger(FILE_LOGGER_STRING_BUFFER_SIZE, FILE_LOGGER_LOG_FILE_COUNT, (PCHAR) FILE_LOGGER_LOG_FILE_DIRECTORY_PATH, TRUE, TRUE, NULL)
#define RESET_FILE_LOGGER()             freeFileLogger()

#ifdef  __cplusplus
}
#endif
#endif  /* __KINESIS_VIDEO_COMMON_INCLUDE__ */
