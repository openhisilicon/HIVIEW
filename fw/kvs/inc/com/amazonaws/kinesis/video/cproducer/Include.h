/**
 * Main public include file
 */
#ifndef __KINESIS_VIDEO_PRODUCER_INCLUDE__
#define __KINESIS_VIDEO_PRODUCER_INCLUDE__

#pragma once

#ifdef  __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////
// Public headers
////////////////////////////////////////////////////
#include <com/amazonaws/kinesis/video/client/Include.h>
#include <com/amazonaws/kinesis/video/common/Include.h>

////////////////////////////////////////////////////
// Status return codes
////////////////////////////////////////////////////
#define STATUS_PRODUCER_BASE                                                        0x15000000
#define STATUS_STOP_CALLBACK_CHAIN                                                  STATUS_PRODUCER_BASE + 0x00000001
#define STATUS_MAX_CALLBACK_CHAIN                                                   STATUS_PRODUCER_BASE + 0x00000002
#define STATUS_INVALID_PLATFORM_CALLBACKS_VERSION                                   STATUS_PRODUCER_BASE + 0x00000003
#define STATUS_INVALID_PRODUCER_CALLBACKS_VERSION                                   STATUS_PRODUCER_BASE + 0x00000004
#define STATUS_INVALID_STREAM_CALLBACKS_VERSION                                     STATUS_PRODUCER_BASE + 0x00000005
#define STATUS_INVALID_AUTH_CALLBACKS_VERSION                                       STATUS_PRODUCER_BASE + 0x00000006
#define STATUS_INVALID_API_CALLBACKS_VERSION                                        STATUS_PRODUCER_BASE + 0x00000007
#define STATUS_INVALID_DESCRIBE_STREAM_RETURN_JSON                                  STATUS_PRODUCER_BASE + 0x0000000f
#define STATUS_MAX_USER_AGENT_NAME_POSTFIX_LEN_EXCEEDED                             STATUS_PRODUCER_BASE + 0x00000013
#define STATUS_MAX_CUSTOM_USER_AGENT_LEN_EXCEEDED                                   STATUS_PRODUCER_BASE + 0x00000014
#define STATUS_INVALID_ENDPOINT_CACHING_PERIOD                                      STATUS_PRODUCER_BASE + 0x00000016
#define STATUS_DUPLICATE_PRODUCER_CALLBACK_FREE_FUNC                                STATUS_PRODUCER_BASE + 0x00000019
#define STATUS_DUPLICATE_STREAM_CALLBACK_FREE_FUNC                                  STATUS_PRODUCER_BASE + 0x0000001a
#define STATUS_DUPLICATE_AUTH_CALLBACK_FREE_FUNC                                    STATUS_PRODUCER_BASE + 0x0000001b
#define STATUS_DUPLICATE_API_CALLBACK_FREE_FUNC                                     STATUS_PRODUCER_BASE + 0x0000001c
#define STATUS_FILE_LOGGER_INDEX_FILE_TOO_LARGE                                     STATUS_PRODUCER_BASE + 0x0000001d
#define STATUS_STREAM_BEING_SHUTDOWN                                                STATUS_PRODUCER_BASE + 0x0000001e
#define STATUS_CLIENT_BEING_SHUTDOWN                                                STATUS_PRODUCER_BASE + 0x0000001f

/**
 * Maximum callbacks in the processing chain
 */
#define MAX_CALLBACK_CHAIN_COUNT                                                20

/**
 * Default number of the callbacks in the chain
 */
#define DEFAULT_CALLBACK_CHAIN_COUNT                                            5

////////////////////////////////////////////////////
// Main defines
////////////////////////////////////////////////////
/**
 * Current versions for the public structs
 */
#define PRODUCER_CALLBACKS_CURRENT_VERSION                                      0
#define PLATFORM_CALLBACKS_CURRENT_VERSION                                      0
#define STREAM_CALLBACKS_CURRENT_VERSION                                        0
#define AUTH_CALLBACKS_CURRENT_VERSION                                          0
#define API_CALLBACKS_CURRENT_VERSION                                           0

////////////////////////////////////////////////////
// Extra callbacks definitions
////////////////////////////////////////////////////
/**
 * Frees platform callbacks
 *
 * @param 1 PUINT64 - Pointer to custom handle passed by the caller.
 *
 * @return STATUS code of the operations
 */
typedef STATUS (*FreePlatformCallbacksFunc)(PUINT64);

/**
 * Frees producer callbacks
 *
 * @param 1 PUINT64 - Pointer to custom handle passed by the caller.
 *
 * @return STATUS code of the operations
 */
typedef STATUS (*FreeProducerCallbacksFunc)(PUINT64);

/**
 * Frees stream callbacks
 *
 * @param 1 PUINT64 - Pointer to custom handle passed by the caller.
 *
 * @return STATUS code of the operations
 */
typedef STATUS (*FreeStreamCallbacksFunc)(PUINT64);

/**
 * Frees auth callbacks
 *
 * @param 1 PUINT64 - Pointer to custom handle passed by the caller.
 *
 * @return STATUS code of the operations
 */
typedef STATUS (*FreeAuthCallbacksFunc)(PUINT64);

/**
 * Frees API callbacks
 *
 * @param 1 PUINT64 - Pointer to custom handle passed by the caller.
 *
 * @return STATUS code of the operations
 */
typedef STATUS (*FreeApiCallbacksFunc)(PUINT64);

////////////////////////////////////////////////////
// Main structure declarations
////////////////////////////////////////////////////
/**
 * The Platform specific callbacks
 */
typedef struct __PlatformCallbacks PlatformCallbacks;
struct __PlatformCallbacks {
    // Version
    UINT32 version;

    // Custom data to be passed back to the caller
    UINT64 customData;

    // The callback functions
    GetCurrentTimeFunc getCurrentTimeFn;
    GetRandomNumberFunc getRandomNumberFn;
    CreateMutexFunc createMutexFn;
    LockMutexFunc lockMutexFn;
    UnlockMutexFunc unlockMutexFn;
    TryLockMutexFunc tryLockMutexFn;
    FreeMutexFunc freeMutexFn;
    CreateConditionVariableFunc createConditionVariableFn;
    SignalConditionVariableFunc signalConditionVariableFn;
    BroadcastConditionVariableFunc broadcastConditionVariableFn;
    WaitConditionVariableFunc waitConditionVariableFn;
    FreeConditionVariableFunc freeConditionVariableFn;
    LogPrintFunc logPrintFn;

    // Specialized cleanup callback
    FreePlatformCallbacksFunc freePlatformCallbacksFn;
};
typedef struct __PlatformCallbacks* PPlatformCallbacks;

/**
 * The Producer object specific callbacks
 */
typedef struct __ProducerCallbacks ProducerCallbacks;
struct __ProducerCallbacks {
    // Version
    UINT32 version;

    // Custom data to be passed back to the caller
    UINT64 customData;

    // The callback functions
    StorageOverflowPressureFunc storageOverflowPressureFn;
    ClientReadyFunc clientReadyFn;
    ClientShutdownFunc clientShutdownFn;

    // Specialized cleanup callback
    FreeProducerCallbacksFunc freeProducerCallbacksFn;
};
typedef struct __ProducerCallbacks* PProducerCallbacks;

/**
 * The Stream specific callbacks
 */
typedef struct __StreamCallbacks StreamCallbacks;
struct __StreamCallbacks {
    // Version
    UINT32 version;

    // Custom data to be passed back to the caller
    UINT64 customData;

    // The callback functions
    StreamUnderflowReportFunc streamUnderflowReportFn;
    BufferDurationOverflowPressureFunc bufferDurationOverflowPressureFn;
    StreamLatencyPressureFunc streamLatencyPressureFn;
    StreamConnectionStaleFunc streamConnectionStaleFn;
    DroppedFrameReportFunc droppedFrameReportFn;
    DroppedFragmentReportFunc droppedFragmentReportFn;
    StreamErrorReportFunc streamErrorReportFn;
    FragmentAckReceivedFunc fragmentAckReceivedFn;
    StreamDataAvailableFunc streamDataAvailableFn;
    StreamReadyFunc streamReadyFn;
    StreamClosedFunc streamClosedFn;
    StreamShutdownFunc streamShutdownFn;

    // Specialized cleanup callback
    FreeStreamCallbacksFunc freeStreamCallbacksFn;
};
typedef struct __StreamCallbacks* PStreamCallbacks;

/**
 * The Authentication specific callbacks
 */
typedef struct __AuthCallbacks AuthCallbacks;
struct __AuthCallbacks {
    // Version
    UINT32 version;

    // Custom data to be passed back to the caller
    UINT64 customData;

    // The callback functions
    GetSecurityTokenFunc getSecurityTokenFn;
    GetDeviceCertificateFunc getDeviceCertificateFn;
    DeviceCertToTokenFunc deviceCertToTokenFn;
    GetDeviceFingerprintFunc getDeviceFingerprintFn;
    GetStreamingTokenFunc getStreamingTokenFn;

    // Specialized cleanup callback
    FreeAuthCallbacksFunc freeAuthCallbacksFn;
};
typedef struct __AuthCallbacks* PAuthCallbacks;

/**
 * The KVS backend specific callbacks
 */
typedef struct __ApiCallbacks ApiCallbacks;
struct __ApiCallbacks {
    // Version
    UINT32 version;

    // Custom data to be passed back to the caller
    UINT64 customData;

    // The callback functions
    CreateStreamFunc createStreamFn;
    DescribeStreamFunc describeStreamFn;
    GetStreamingEndpointFunc getStreamingEndpointFn;
    PutStreamFunc putStreamFn;
    TagResourceFunc tagResourceFn;
    CreateDeviceFunc createDeviceFn;

    // Specialized cleanup callback
    FreeApiCallbacksFunc freeApiCallbacksFn;
};
typedef struct __ApiCallbacks* PApiCallbacks;

/**
 * Type of caching implementation to use with the callbacks provider
 */
typedef enum {
    /**
     *  No caching. The callbacks provider will make backend API calls every time PIC requests.
     */
    API_CALL_CACHE_TYPE_NONE,

    /**
     * The backend API calls are emulated for all of the API calls with the exception of GetStreamingEndpoint
     * and PutMedia (for actual streaming). The result of the GetStreamingEndpoint is cached and the cached
     * value is returned next time PIC makes a request to call GetStreamingEndpoint call.
     *
     * NOTE: The stream should be pre-existing as DescribeStream API call will be emulated and will
     * return a success with stream being Active.
     */
    API_CALL_CACHE_TYPE_ENDPOINT_ONLY,

    /**
     * Cache all of the backend API calls with the exception of PutMedia (for actual data streaming).
     * In this mode, the actual backend APIs will be called once and the information will be cached.
     * The cached result will be returned afterwards when the PIC requests the provider to make the
     * backend API call.
     *
     * This mode is the recommended mode for most of the streaming scenarios and is the default in the samples.
     *
     * NOTE: This mode is most suitable for streaming scenarios when the stream is not dynamically deleted
     * or modified by another entity so the cached value of the API calls are static.
     *
     * NOTE: This mode will work for non-pre-created streams as the first call will not be cached for
     * DescribeStream API call and if the stream is not created then the state machine will attempt
     * to create it by calling CreateStream API call which would evaluate to the actual backend call
     * to create the stream as it's not cached.
     */
    API_CALL_CACHE_TYPE_ALL,
} API_CALL_CACHE_TYPE;

////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////

/**
 * Creates a default callbacks provider based on static AWS credentials
 *
 * NOTE: The caller is responsible for releasing the structure by calling
 * the corresponding {@link freeCallbackProvider} API.
 *
 * @param - PCHAR - IN - Access Key Id
 * @param - PCHAR - IN - Secret Key
 * @param - PCHAR - IN/OPT - Session Token
 * @param - UINT64 - IN - Expiration of the token. MAX_UINT64 if non-expiring
 * @param - PCHAR - IN/OPT - AWS region
 * @param - PCHAR - IN/OPT - CA Cert Path
 * @param - PCHAR - IN/OPT - User agent name (Use NULL)
 * @param - PCHAR - IN/OUT - Custom user agent to be used in the API calls
 * @param - PClientCallbacks* - OUT - Returned pointer to callbacks provider
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS createDefaultCallbacksProviderWithAwsCredentials(PCHAR, PCHAR, PCHAR, UINT64, PCHAR, PCHAR, PCHAR, PCHAR, PClientCallbacks*);

/**
 * Creates a default callbacks provider that uses iot certificate as auth method.
 *
 * NOTE: The caller is responsible for releasing the structure by calling
 * the corresponding {@link freeCallbackProvider} API.
 *
 * @param - PCHAR - IN - IoT endpoint to use for the auth
 * @param - PCHAR - IN - Credential cert path
 * @param - PCHAR - IN - Private key path
 * @param - PCHAR - IN/OPT - CA Cert path
 * @param - PCHAR - IN - Role alias name
 * @param - PCHAR - IN - IoT Thing name
 * @param - PCHAR - IN/OPT - AWS region
 * @param - PCHAR - IN/OPT - User agent name (Use NULL)
 * @param - PCHAR - IN/OPT - Custom user agent to be used in the API calls
 * @param - PClientCallbacks* - OUT - Returned pointer to callbacks provider
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS createDefaultCallbacksProviderWithIotCertificate(PCHAR, PCHAR, PCHAR, PCHAR, PCHAR, PCHAR, PCHAR, PCHAR, PCHAR, PClientCallbacks*);

/**
 * Creates a default callbacks provider that uses file-based certificate as auth method.
 *
 * NOTE: The caller is responsible for releasing the structure by calling
 * the corresponding {@link freeCallbackProvider} API.
 *
 * @param - PCHAR - IN - Credential file path
 * @param - PCHAR - IN/OPT - AWS region
 * @param - PCHAR - IN/OPT - CA Cert path
 * @param - PCHAR - IN/OPT - User agent name (Use NULL)
 * @param - PCHAR - IN/OPT - Custom user agent to be used in the API calls
 * @param - PClientCallbacks* - OUT - Returned pointer to callbacks provider
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS createDefaultCallbacksProviderWithFileAuth(PCHAR, PCHAR, PCHAR, PCHAR, PCHAR, PClientCallbacks*);

/**
 * Creates a default callbacks provider that uses auth callbacks as auth method.
 *
 * NOTE: The caller is responsible for releasing the structure by calling
 * the corresponding {@link freeCallbackProvider} API.
 *
 * @param - PAuthCallbacks - IN - Auth Callback for the auth
 * @param - PCHAR - IN/OPT - AWS region
 * @param - PCHAR - IN/OPT - CA Cert path
 * @param - PCHAR - IN/OPT - User agent name (Use NULL)
 * @param - PCHAR - IN/OPT - Custom user agent to be used in the API calls
 * @param - PClientCallbacks* - OUT - Returned pointer to callbacks provider
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS createDefaultCallbacksProviderWithAuthCallbacks(PAuthCallbacks, PCHAR, PCHAR, PCHAR, PCHAR, PClientCallbacks*);

/**
 * Releases and frees the callbacks provider structure.
 *
 * NOTE: The caller is responsible for releasing the structure by calling
 * the corresponding free API.
 *
 * @param - PClientCallbacks* - IN/OUT - Pointer to callbacks provider to free
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS freeCallbacksProvider(PClientCallbacks*);

/**
 * Sets the Platform callbacks.
 *
 * NOTE: The existing Platform callbacks are overwritten.
 *
 * @param - PClientCallbacks - IN - Pointer to client callbacks
 * @param - @PPlatformCallbacks - IN - Pointer to platform callbacks to set
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS setPlatformCallbacks(PClientCallbacks, PPlatformCallbacks);

/**
 * Appends Producer callbacks
 *
 * NOTE: The callbacks are appended at the end of the chain.
 *
 * @param - PClientCallbacks - IN - Pointer to client callbacks
 * @param - PProducerCallbacks - IN - Pointer to producer callbacks to add
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS addProducerCallbacks(PClientCallbacks, PProducerCallbacks);

/**
 * Appends Stream callbacks
 *
 * NOTE: The callbacks are appended at the end of the chain.
 *
 * @param - PClientCallbacks - IN - Pointer to client callbacks
 * @PStreamCallbacks - IN - Pointer to stream callbacks to use
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS addStreamCallbacks(PClientCallbacks, PStreamCallbacks);

/**
 * Appends Auth callbacks
 *
 * NOTE: The callbacks are appended at the end of the chain.
 *
 * @param - PClientCallbacks - IN - Pointer to client callbacks
 * @PAuthCallbacks - IN - Pointer to auth callbacks to use
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS addAuthCallbacks(PClientCallbacks, PAuthCallbacks);

/**
 * Appends Api callbacks
 *
 * NOTE: The callbacks are appended at the end of the chain.
 *
 * @param - PClientCallbacks - IN - Pointer to client callbacks
 * @PApiCallbacks - IN - Pointer to api callbacks to use
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS addApiCallbacks(PClientCallbacks, PApiCallbacks);

/**
 * Creates Stream Info for RealTime Streaming Scenario using default values.
 *
 * NOTE: The caller is responsible for releasing the structure by calling
 * the corresponding free API.
 *
 * @param - PCHAR - IN/OPT - stream name
 * @param - UINT64 - IN - retention in 100ns time unit
 * @param - UINT64 - IN - buffer duration in 100ns time unit
 * @param - PStreamInfo* - OUT - Constructed object
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS createRealtimeVideoStreamInfoProvider(PCHAR, UINT64, UINT64, PStreamInfo*);

/**
 * Creates Stream Info for Offline Video Streaming Scenario.
 *
 * NOTE: The caller is responsible for releasing the structure by calling
 * the corresponding free API.
 *
 * @param - PCHAR - IN/OPT - stream name
 * @param - UINT64 - IN - retention in 100ns time unit. Should be greater than 0.
 * @param - UINT64 - IN - buffer duration in 100ns time unit
 * @param - PStreamInfo* - OUT - Constructed object
 *
 * @return - STATUS code of the execution
*/
PUBLIC_API STATUS createOfflineVideoStreamInfoProvider(PCHAR, UINT64, UINT64, PStreamInfo*);

/**
 * Creates Stream Info for RealTime Audio Video Streaming Scenario
 *
 * NOTE: The caller is responsible for releasing the structure by calling
 * the corresponding free API.
 *
 * @param - PCHAR - IN/OPT - stream name
 * @param - UINT64 - IN - retention in 100ns time unit
 * @param - UINT64 - IN - buffer duration in 100ns time unit
 * @param - PStreamInfo* - OUT - Constructed object
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS createRealtimeAudioVideoStreamInfoProvider(PCHAR, UINT64, UINT64, PStreamInfo*);

/**
 * Creates Stream Info for Offline Audio Video Streaming Scenario
 *
 * NOTE: The caller is responsible for releasing the structure by calling
 * the corresponding free API.
 *
 * @param - PCHAR - IN/OPT - stream name
 * @param - UINT64 - IN - retention in 100ns time unit
 * @param - UINT64 - IN - buffer duration in 100ns time unit
 * @param - PStreamInfo* - OUT - Constructed object
 *
 * @return - STATUS code of the execution
*/
PUBLIC_API STATUS createOfflineAudioVideoStreamInfoProvider(PCHAR, UINT64, UINT64, PStreamInfo*);

/**
 * Configure streaminfo based on given storage size and average bitrate
 * Will change buffer duration, stream latency duration.
 *
 * @param - UINT32 - Storage size in bytes
 * @param - UINT64 - Total average bitrate for all tracks. Unit: bits per second
 * @param - UINT32 - Total number of streams per kinesisVideoStreamClient
 * @param - PStreamInfo
 * @return - STATUS code of the execution
*/
PUBLIC_API STATUS setStreamInfoBasedOnStorageSize(UINT32, UINT64, UINT32, PStreamInfo);

/*
 * Frees the StreamInfo provider object.
 *
 * @param - PStreamInfo* - IN/OUT - StreamInfo provider object to be freed
 */
PUBLIC_API STATUS freeStreamInfoProvider(PStreamInfo*);

/**
 * Create deviceInfo with DEFAULT_STORAGE_SIZE amount of storage.
 *
 * NOTE: The caller is responsible for releasing the structure by calling
 * the corresponding free API.
 *
 * @param - PDeviceInfo* - OUT - pointer to PDeviceInfo that will point to the created deviceInfo
 *
 * @return - STATUS - status of operation
 */
PUBLIC_API STATUS createDefaultDeviceInfo(PDeviceInfo*);

/**
 * Frees the DeviceInfo provider object.
 *
 * @param - PDeviceInfo* - IN/OUT - pointer to PDeviceInfo provider that will be freed
 *
 * @return - STATUS - status of operation
 */
PUBLIC_API STATUS freeDeviceInfo(PDeviceInfo*);

/**
 * Can be called after createDefaultDeviceInfo, set the storage size to storageSize.
 *
 * @param - PDeviceInfo - IN - pointer to the target object
 * @param - UINT64 - IN - Buffer storage size in bytes
 *
 * @return - STATUS - status of operation
 */
PUBLIC_API STATUS setDeviceInfoStorageSize(PDeviceInfo, UINT64);

/**
 *  Can be called after createDefaultDeviceInfo, calculate the storage size based on averageBitRate and bufferDuration,
 *  the call initDeviceInfoStorageInMemoryFixedSize to set storage size.
 *
 * @param - PDeviceInfo - IN - pointer to the target object
 * @param - UINT64 - IN - bitrate in bits per second
 * @param - UINT64 - IN - buffer duration in 100ns
 *
 * @return - STATUS - status of operation
 */
PUBLIC_API STATUS setDeviceInfoStorageSizeBasedOnBitrateAndBufferDuration(PDeviceInfo, UINT64, UINT64);

/*
 * Creates the Iot Credentials auth callbacks
 *
 * NOTE: The caller is responsible for releasing the structure by calling
 * the corresponding free API.
 *
 * @param - PCallbacksProvider - IN - Pointer to callback provider
 * @param - PCHAR - IN - iot credentials endpoint
 * @param - PCHAR - IN - kvs iot certificate file path
 * @param - PCHAR - IN - private key file path
 * @param - PCHAR - IN - CA cert path
 * @param - PCHAR - IN - iot role alias
 * @param - PCHAR - IN - IoT thing name
 *
 * @param - PAuthCallbacks* - OUT - Pointer to pointer to AuthCallback struct
 *
 * @return - STATUS - status of operation
 */
PUBLIC_API STATUS createIotAuthCallbacks(PClientCallbacks, PCHAR, PCHAR, PCHAR, PCHAR, PCHAR, PCHAR, PAuthCallbacks*);

/*
 * Frees the Iot Credential auth callbacks
 *
 * @param - PAuthCallbacks* - IN/OUT - pointer to AuthCallback provider object
 *
 * @return - STATUS - status of operation
 */
PUBLIC_API STATUS freeIotAuthCallbacks(PAuthCallbacks*);

/*
 * Creates the File Credentials auth callbacks
 *
 * NOTE: The caller is responsible for releasing the structure by calling
 * the corresponding free API.
 *
 * @param - PCallbacksProvider - IN - Pointer to callback provider
 * @param - PCHAR - IN - file path for the credentials file
 * @param - PAuthCallbacks* - OUT - Pointer to pointer to AuthCallback struct
 *
 * @return - STATUS - status of operation
 */
PUBLIC_API STATUS createFileAuthCallbacks(PClientCallbacks, PCHAR, PAuthCallbacks*);

/*
 * Frees the File Credential auth callbacks
 *
 * @param - PAuthCallbacks* - IN/OUT - pointer to AuthCallback provider object
 *
 * @return - STATUS - status of operation
 */
PUBLIC_API STATUS freeFileAuthCallbacks(PAuthCallbacks*);

/*
 * Creates a Static Credentials auth callbacks
 *
 * NOTE: The caller is responsible for releasing the structure by calling
 * the corresponding free API.
 *
 * @param - PCallbacksProvider - IN - Pointer to callback provider
 * @param - PCHAR - IN - Access key id
 * @param - PCHAR - IN - Secret key
 * @param - PCHAR - IN/OPT - Session token
 * @param - UINT64 - IN - Expiration absolute time in 100ns
 * @param - PAuthCallbacks* - OUT - Pointer to pointer to AuthCallback struct
 *
 * @return - STATUS - status of operation
 */
PUBLIC_API STATUS createStaticAuthCallbacks(PClientCallbacks, PCHAR, PCHAR, PCHAR, UINT64, PAuthCallbacks*);

/*
 * Frees the Static Credential auth callback
 *
 * @param - PAuthCallbacks* - IN/OUT - pointer to AuthCallback provider object
 *
 * @return - STATUS - status of operation
 */
PUBLIC_API STATUS freeStaticAuthCallbacks(PAuthCallbacks*);

/**
 * Creates StreamCallbacks struct
 *
 * NOTE: The caller is responsible for releasing the structure by calling
 * the corresponding free API.
 *
 * @param - PStreamCallbacks* - OUT - Constructed object
 *
 * @return - STATUS code of the execution
*/
PUBLIC_API STATUS createStreamCallbacks(PStreamCallbacks*);

/**
 * Frees StreamCallbacks struct
 *
 * @param - PStreamCallbacks* - IN/OUT - the object to be freed
 *
 * @return - STATUS code of the execution
*/
PUBLIC_API STATUS freeStreamCallbacks(PStreamCallbacks*);

/**
 * Creates a stream callbacks object allowing continuous retry on failures,
 * reset logic on staleness and latency.
 *
 * https://docs.aws.amazon.com/kinesisvideostreams/latest/dg/producer-reference-callbacks.html
 *
 * @param - PCallbacksProvider - IN - Pointer to callback provider
 * @param - PStreamCallbacks* - OUT - Constructed object
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS createContinuousRetryStreamCallbacks(PClientCallbacks, PStreamCallbacks*);

/**
 * Frees a previously constructed continuous stream callbacks object
 *
 * @param - PStreamCallbacks* - IN/OUT/OPT - Object to be destroyed.
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS freeContinuousRetryStreamCallbacks(PStreamCallbacks*);

/**
 * Create abstract callback provider that can hook with other callbacks
 *
 * @param - UINT32 - IN - Length of callback provider calling chain
 * @param - API_CALL_CACHE_TYPE - IN - Backend API call caching mode
 * @param - UINT64 - IN - Cached endpoint update period
 * @param - PCHAR - IN/OPT - AWS region
 * @param - PCHAR - IN - Specific Control Plane Uri as endpoint to be called
 * @param - PCHAR - IN/OPT - CA Cert path
 * @param - PCHAR - IN/OPT - User agent name (Use NULL)
 * @param - PCHAR - IN/OPT - Custom user agent to be used in the API calls
 * @param - PClientCallbacks* - OUT - Returned pointer to callbacks provider
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS createAbstractDefaultCallbacksProvider(UINT32, API_CALL_CACHE_TYPE, UINT64, PCHAR, PCHAR, PCHAR, PCHAR, PCHAR, PClientCallbacks*);

/**
 * Use file logger instead of default logger which log to stdout. The underlying objects are automatically freed
 * when PClientCallbacks is freed.
 *
 * @param - PClientCallbacks - IN - The callback provider whose logPrintFn will be replaced with file logger log printing function
 * @param - UINT64 - IN - Size of string buffer in file logger. When the string buffer is full the logger will flush everything into a new file
 * @param - UINT64 - IN - Max number of log file. When exceeded, the oldest file will be deleted when new one is generated
 * @param - PCHAR - IN - Directory in which the log file will be generated
 * @param - BOOL - IN - print log to std out too
 *
 * @return - STATUS code of the execution
 */
PUBLIC_API STATUS addFileLoggerPlatformCallbacksProvider(PClientCallbacks, UINT64, UINT64, PCHAR, BOOL);



#ifdef  __cplusplus
}
#endif
#endif  /* __KINESIS_VIDEO_PRODUCER_INCLUDE__ */
