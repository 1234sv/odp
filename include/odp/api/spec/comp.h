/* Copyright (c) 2017, Linaro Limited
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

/**
 * @file
 *
 * ODP Compression
 */

#ifndef ODP_API_COMP_H_
#define ODP_API_COMP_H_

#include <odp/visibility_begin.h>
#include <odp/api/support.h>
#include <odp/api/packet.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup odp_compression ODP COMP
 *  ODP Compression is an API set to do compression+hash or decompression+hash
 *  operations on data. Hash is calculated on plaintext.
 *
 *  if opcode = ODP_COMP_COMPRESS, then it will apply hash and then compress,
 *  if opcode = ODP_COMP_DECOMPRESS, then it will decompress and then apply
 *  hash.
 *  Independent hash-only operations are not supported. Implementation should
 *  perform hash along with valid compression algo.
 *  Macros, enums, types and operations to utilize compression interface.
 *  @{
 */

/**
 * @def ODP_COMP_SESSION_INVALID
 * Invalid session handle
 */

/**
 * @typedef odp_comp_session_t
 * Compression/Decompression session handle
 */

/**
 * Compression API operation mode
 */
typedef enum {
	/** Synchronous, return results immediately */
	ODP_COMP_SYNC,
	/** Asynchronous, return results via event queue */
	ODP_COMP_ASYNC
} odp_comp_op_mode_t;

/**
 * Comp API operation type.
 *
 */
typedef enum {
	/** Compress */
	ODP_COMP_OP_COMPRESS,
	/** Decompress */
	ODP_COMP_OP_DECOMPRESS
} odp_comp_op_t;

/**
 * Comp API hash algorithm
 *
 */
typedef enum {
	/** ODP_COMP_HASH_ALG_NONE - No hash algorithm selected. */
	ODP_COMP_HASH_ALG_NONE,
	/** ODP_COMP_HASH_ALG_SHA1 - SHA-1 hash algorithm. */
	ODP_COMP_HASH_ALG_SHA1,
	/** ODP_COMP_HASH_ALG_SHA256 - SHA-2 hash algorithm
	* 256-bit digest length.
	*/
	ODP_COMP_HASH_ALG_SHA256
} odp_comp_hash_alg_t;

/**
 * Comp API compression algorithm
 *
 */
typedef enum {
	/** No algorithm specified.
	 * Means no compression, output == input.
	 * if provided, no operation (compression/decompression or hash)
	 * applied on input. Added for testing purpose.
	 */
	ODP_COMP_ALG_NULL,
	/** DEFLATE - RFC1951 */
	ODP_COMP_ALG_DEFLATE,
	/** ZLIB - RFC1950 */
	ODP_COMP_ALG_ZLIB,
	/** LZS */
	ODP_COMP_ALG_LZS
} odp_comp_alg_t;

/**
 * Comp API session creation return code
 *
 */
typedef enum {
	/** Session created */
	ODP_COMP_SES_CREATE_ERR_NONE,
	/** Creation failed, no resources */
	ODP_COMP_SES_CREATE_ERR_ENOMEM,
	/** Creation failed, bad compression params */
	ODP_COMP_SES_CREATE_ERR_INV_COMP,
	/** Creation failed, bad hash params */
	ODP_COMP_SES_CREATE_ERR_INV_HASH,
	/** Creation failed,requested configuration not supported*/
	ODP_COMP_SES_CREATE_ERR_NOT_SUPPORTED
} odp_comp_ses_create_err_t;

/**
 * Comp API operation return codes
 *
 */
typedef enum {
	/** Operation completed successfully*/
	ODP_COMP_ERR_NONE,
	/** Operation paused due to insufficient output buffer.
	*
	* This is not an error condition. On seeing this situation,
	* Implementation should maintain context of in-progress operation and
	* application should call packet processing API again with valid
	* output buffer but no other alteration to operation params
	* (odp_comp_op_param_t).
	*
	* if using async mode, application should either make sure to
	* provide sufficient output buffer size OR maintain relevant
	* context (or ordering) information with respect to each input packet
	* en-queued for processing.
	*
	*/
	ODP_COMP_ERR_OUT_OF_SPACE,
	/** Invalid user data pointers*/
	ODP_COMP_ERR_DATA_PTR,
	/** Invalid input data size*/
	ODP_COMP_ERR_DATA_SIZE,
	/**  Compression and/or hash Algo failure*/
	ODP_COMP_ERR_ALGO_FAIL,
	/** Error if operation has been requested in an invalid state */
	ODP_COMP_ERR_INV_STATE,
	/** Error if API does not support any of the operational parameter. */
	ODP_COMP_ERR_NOT_SUPPORTED,
	/** Error if session is invalid. */
	ODP_COMP_ERR_INV_SESS
} odp_comp_err_t;

/**
 * Comp API enumeration for preferred compression level/speed. Applicable
 * only for compression operation not decompression.
 * Value provided defines a trade-off between speed and compression ratio.
 *
 * If compression level == ODP_COMP_LEVEL_MIN, output will be produced at
 * fastest possible rate,
 *
 * If compression level == ODP_COMP_LEVEL_MAX, output will be highest possible
 * compression,
 *
 * compression level == ODP_COMP_LEVEL_DEFAULT means implementation will use
 * its default choice of compression level.
 *
 */
typedef enum {
	/* Use implementation default */
	ODP_COMP_LEVEL_DEFAULT = 0,
	/** Minimum compression (fastest in speed) */
	ODP_COMP_LEVEL_MIN,
	/** Maximum compression (slowest in speed) */
	ODP_COMP_LEVEL_MAX,
} odp_comp_level_t;

/**
 * Comp API enumeration for huffman encoding. Valid for compression operation.
 *
 */
typedef enum {
	/** use implementation default to choose between compression codes  */
	ODP_COMP_HUFFMAN_CODE_DEFAULT = 0,
	/** use fixed huffman codes */
	ODP_COMP_HUFFMAN_CODE_FIXED,
	/** use dynamic huffman coding */
	ODP_COMP_HUFFMAN_CODE_DYNAMIC,
} odp_comp_huffman_code_t;

/**
 * Hash algorithms in a bit field structure
 *
 */
typedef union odp_comp_hash_algos_t {
	/** hash algorithms */
	struct {
		/** ODP_COMP_HASH_ALG_SHA1 */
		uint32_t sha1  : 1;

		/** ODP_COMP_HASH_ALG_SHA256 */
		uint32_t sha256 : 1;

	} bit;

	/** All bits of the bit field structure
	 *
	 * This field can be used to set/clear all flags, or bitwise
	 * operations over the entire structure.
	 */
	uint32_t all_bits;
} odp_comp_hash_algos_t;

/**
 * Comp algorithms in a bit field structure
 *
 */
typedef union odp_comp_algos_t {
	/** Compression algorithms */
	struct {
		/** ODP_COMP_ALG_NULL */
		uint32_t null       : 1;

		/** ODP_COMP_ALG_DEFLATE */
		uint32_t deflate    : 1;

		/** ODP_COMP_ALG_ZLIB */
		uint32_t zlib       : 1;

		/** ODP_COMP_ALG_LZS */
		uint32_t lzs        :1;
	} bit;

	/** All bits of the bit field structure
	 * This field can be used to set/clear all flags, or bitwise
	 * operations over the entire structure.
	 */
	uint32_t all_bits;
} odp_comp_algos_t;

/**
 * Compression Interface Capabilities
 *
 */
typedef struct odp_comp_capability_t {
	/** Maximum number of  sessions */
	uint32_t max_sessions;

	/** Supported compression algorithms */
	odp_comp_algos_t comp_algos;

	/** Supported hash algorithms. */
	odp_comp_hash_algos_t hash_algos;

	/** Support type for synchronous operation mode (ODP_COMP_SYNC).
	 *  User should set odp_comp_session_param_t:mode based on
	 *  support level as indicated by this param.
	 */
	odp_support_t sync;

	/** Support type for asynchronous operation mode (ODP_COMP_ASYNC).
	 *  User should set odp_comp_session_param_t:mode param based on
	 *  support level as indicated by this param.
	 */
	odp_support_t async;
} odp_comp_capability_t;

/**
 * Hash algorithm capabilities
 *
 */
typedef struct odp_comp_hash_alg_capability_t {
	/** Digest length in bytes */
	uint32_t digest_len;
} odp_comp_hash_alg_capability_t;

/**
 * Compression algorithm capabilities structure for each algorithm.
 *
 */
typedef struct odp_comp_algo_capability_t {
	/** Enumeration indicating alg support for dictionary load */
	odp_support_t support_dict;

	/** Optional Maximum length of dictionary supported
	 *   by implementation of the algorithm.
	 *
	 *   Invalid if support_dict == ODP_SUPPORT_NO.
	 *
	 *   Implementation use dictionary of length less than or equal to value
	 *   indicated by dict_len. if set to 0 and if support_dict ==
	 *   ODP_SUPPORT_YES, then implementation will use dictionary length
	 *   less than or equal to user input length in odp_comp_set_dict()
	 *   and update used dictionary length at output of the call.
	 *
	 */
	uint32_t dict_len;

	/* Maximum compression level supported by implementation of this algo.
	 *  Indicates number of compression levels supported by implementation,
	 *
	 * where,
	 *
	 * 1 means fastest compression i.e. output produced at
	 * best possible speed at the expense of compression quality, and
	 *
	 * max_level means best compression i.e.output produced is best possible
	 * compressed content at the expense of speed.
	 *
	 * Example, if max_level = 4 , it means algorithm supports four levels
	 * of compression from value 1 up to 4. User can set this value from
	 * 1 (fastest compression) to 4 (best compression).
	 * See RFC1950 for an example explanation to level.
	 *
	 * Value 0 mean implementation use its default value.
	 *
	 */
	uint32_t max_level;

	/* Supported hash algorithms */
	odp_comp_hash_algos_t hash_algo;
} odp_comp_algo_capability_t;

/**
 * Comp API dictionary type
 * Consists of pointer to byte buffer. length of dictionary
 * indicated by length parameter.
 */
typedef struct odp_comp_dict_t {
	/** pointer to byte array */
	uint8_t *buf;
	/** length of the dictionary. */
	uint32_t len;
} odp_comp_dict_t;

/**
 * Comp API algorithm specific parameters
 *
 */
typedef struct odp_comp_algo_param_t {
	/** struct for defining deflate algorithm parameters.
	* Also initialized by other deflate based algorithms , ex. ZLIB
	*/
	struct comp_alg_def_param {
		/** compression level where
		 * ODP_COMP_LEVEL_MIN <= level <= ODP_COMP_LEVEL_MAX
		 */
		odp_comp_level_t level;
		/** huffman code to use */
		odp_comp_huffman_code_t comp_code;
	} deflate;

	/** struct for defining zlib algorithm parameters.
	*/
	struct comp_alg_zlib_param {
			/** deflate algo params */
			struct comp_alg_def_param def;
	} zlib;
} odp_comp_algo_param_t;

 /**
 * Comp API session creation parameters
 *
 */
typedef struct odp_comp_session_param_t {
	/** Compress vs. Decompress operation */
	odp_comp_op_t op;

	/** Sync vs Async mode
	 *
	 * When mode = ODP_COMP_SYNC, odp_comp_xxx()
	 * should be called.
	 *
	 * When mode = ODP_COMP_ASYNC, odp_comp_xxx_enq()
	 * should be called.
	 *
	 * Use odp_comp_capability() for supported mode.
	 *
	 */
	odp_comp_op_mode_t mode;

	/** Compression algorithm
	 *
	 *  Use odp_comp_capability() for supported algorithms.
	 */
	odp_comp_alg_t comp_algo;

	/** Hash algorithm
	 *
	 *  Use odp_comp_alg_capability() for supported hash algo for
	 *  compression algo given as comp_alg. Implementation should not
	 *  support hash only operation on data. output should always contain
	 *  data + hash.
	 *
	 */
	odp_comp_hash_alg_t hash_algo;

	/** parameters specific to compression */
	odp_comp_algo_param_t alg_param;

	/** Async mode completion event queue
	 *
	 * When mode = ODP_COMP_ASYNC, user should wait on ODP_EVENT_PACKET
	 * with subtype ODP_EVENT_PACKET_COMP on this queue.
	 *
	 * By default, implementation enques completion events in-order-of
	 * request submission and thus queue is considered ordered.
	 *
	 * Please note, behavior could be changed or enhanced
	 * to queue event in-order-of their completion to enable
	 * performance-oriented application to leverage hw offered parallelism.
	 * However, this will be subject to application requirement and more
	 * explicit defined use-case.
	 *
	 */
	odp_queue_t compl_queue;
} odp_comp_session_param_t;

/**
 * Comp API per packet operation result
 *
 */
typedef struct odp_comp_packet_result_t {
	/** Operation Return Code */
	odp_comp_err_t err;
} odp_comp_packet_result_t;

/**
 * Comp packet API per packet operation parameters
 */
typedef struct odp_comp_packet_op_param_t {
	/** Session handle from creation */
	odp_comp_session_t session;

	/** Boolean indicating End of data, where
	 *
	 *   true : last chunk
	 *
	 *   false: more to follow
	 *
	 * If set to true, indicates this is the last chunk of
	 * data which was being processed in stateful mode. After processing
	 * of last chunk of data is complete (i.e.call return with any
	 * error code except ODP_COMP_ERR_OUT_OF_SPACE),
	 * implementation should move algorithm to stateless mode
	 * for next of batch of operation i.e. reset history,
	 * insert 'End of Block' marker into compressed data stream, if
	 * supported by algo.(See deflate/zlib for interpretation of
	 * stateless/stateful).
	 *
	 * For stateless compressions (ex ipcomp), last should be set to 'true'
	 * for every input packet processing call.
	 *
	 * For compression + hash, digest will be available after
	 * last chunk is processed completely.
	 */
	odp_bool_t last;

	/** Input data range */
	odp_packet_data_range_t indata_range;

	/** Output packet data range. Indicates where
	* processed packet output will be written.
	*/
} odp_comp_packet_op_param_t;

/**
 * Query comp capabilities
 *
 * Output comp capabilities on success.
 *
 * @param[out] capa   Pointer to capability structure for output
 *
 * @retval 0 on success
 * @retval <0 on failure
 */
int odp_comp_capability(odp_comp_capability_t *capa);

/**
 * Query supported compression algorithm capabilities
 *
 * Output algorithm capabilities.
 *
 * @param      comp     Compression algorithm
 * @param[out] capa     Array of capability structures for output
 * @param      num     Maximum number of capability structures to output
 *
 * @return Number of capability structures for the algorithm. If this is larger
 *         than 'num', only 'num' first structures were output and application
 *         may call the function again with a larger value of 'num'.
 * @retval <0 on failure
 */
int odp_comp_alg_capability(odp_comp_alg_t comp,
			    odp_comp_alg_capability_t capa[], int num);

 /**
  * Query supported hash algorithm capabilities
  *
  * Outputs all supported configuration options for the algorithm.
  *
  * @param      hash     Hash algorithm
  * @param[out] capa     Array of capability structures for output
  * @param      num     Maximum number of capability structures to output
  *
  * @return Number of capability structures for the algorithm. If this is larger
  *	    than 'num', only 'num' first structures were output and application
  *	    may call the function again with a larger value of 'num'.
  * @retval <0 on failure
  */
int odp_comp_hash_alg_capability(odp_comp_hash_alg_t hash,
				 odp_comp_hash_alg_capability_t capa[],
				 int num);

 /**
  * Initialize comp session parameters
  *
  * Initialize an odp_comp_session_param_t to its default values for
  * all fields.
  *
  * @param param   Pointer to odp_comp_session_param_t to be initialized
  */
void odp_comp_session_param_init(odp_comp_session_param_t *param);

 /**
  * Compression session creation
  *
  * Create a comp session according to the session parameters. Use
  * odp_comp_session_param_init() to initialize parameters into their
  * default values.
  *
  * @param param             Session parameters
  * @param session           Created session else ODP_COMP_SESSION_INVALID
  * @param status            Failure code if unsuccessful
  *
  * @retval 0 on success
  * @retval <0 on failure
  */
int odp_comp_session_create(odp_comp_session_param_t *param,
			    odp_comp_session_t *session,
			    odp_comp_ses_create_err_t *status);

 /**
  * Comp session destroy
  *
  * Destroy an unused session. Result is undefined if session is being used
  * (i.e. asynchronous operation is in progress).
  *
  * @param session           Session handle
  *
  * @retval 0 on success
  * @retval <0 on failure
  */
int odp_comp_session_destroy(odp_comp_session_t session);

/**
 * Comp set dictionary
 *
 * Should be called when there is no operation in progress i.e.
 * before initiating processing of first chunk of data and
 * after processing of last chunk of data is complete.
 *
 * @param session           Session handle
 * @param dict[in,out]      Pointer to dictionary.
 *                          implementation should update length of dictionary
 *                          used at output.
 * @retval 0 on success
 * @retval <0 on failure
 *
 * @note:
 * Application should call odp_comp_alg_capability() to query 'support_dict'
 * before making call to this API.
 */
int odp_comp_set_dict(odp_comp_session_t session,
		      odp_comp_dict_t *dict);

/**
 * Comp packet operation synchronous
 *
 * Performs the SYNC compression/decompression operations on the packets.
 *
 * If session is created in ODP_COMP_SYNC mode, this call wait for operation
 * to complete.
 *
 * If session is created in ODP_COMP_ASYNC mode, behavior is undefined.
 *
 * for compression + hash, call returns with hash appended to the end of
 * last processed chunk of data.
 *
 * User should compute processed data len = total output len - digest_len, where
 * digest_len queried through odp_comp_hash_alg_capability().
 *
 * Caller should initialize pkt_out with valid output
 * packet handles. All arrays should be of num_pkt size.
 *
 * In case of partially processed array i.e. when
 * number of packets returned < num_pkt, application should call
 * odp_comp_result() on 1st failed packet (i.e. n+1th packet where
 * n=number of packets returned) to ensure if error code is
 * ODP_COMP_ERR_OUT_OF_SPACE.
 *
 * In case of ODP_COMP_ERR_OUT_OF_SPACE, application should keep on calling
 * odp_comp_op() API with more output buffer until call returns
 * with num_pkt or with result as ODP_COMP_ERR_NONE or any other
 * failure code.
 *
 * @param         pkt_in   Packets to be processed
 * @param[in,out] pkt_out  Packet handle array specifying resulting packets
 * @param         param    Operation parameters array
 * @param         num_pkt  Number of packets to be processed
 *
 * @return Number of input packets consumed (0 ... num_pkt)
 * @retval <0 on failure
 */
int odp_comp_compress(const odp_packet_t pkt_in[],
		      odp_packet_t pkt_out[],
		      odp_comp_packet_op_param_t param[],
		      int num_pkt);

/**
 * Comp packet operation synchronous
 *
 * Performs the SYNC decompression operations on the packets.
 *
 * If session is created in ODP_COMP_SYNC mode, this call wait for operation
 * to complete and update result at output
 *
 * If session is created in ODP_COMP_ASYNC mode, behavior of this call
 * is undefined.
 *
 * For decompression + hash, call returns with hash appended to the end of
 * last processed chunk of data.User should compute processed data len =
 * total output len - digest_len, where digest_len queried through
 * odp_comp_hash_alg_capability().
 *
 * Caller should initialize pkt_out with valid output
 * packet handles. All arrays should be of num_pkt size.
 * In case of partially processed array i.e. when
 * number of packets returned < num_pkt, application should call
 * odp_comp_result() on 1st failed packet (i.e. n+1th packet where
 * n=number of packets returned) to ensure if error code is
 * ODP_COMP_ERR_OUT_OF_SPACE.
 *
 * In case of ODP_COMP_ERR_OUT_OF_SPACE, application should keep on calling
 * odp_comp_op() API with more output buffer until call returns
 * with num_pkt or with result as ODP_COMP_ERR_NONE or any other
 * failure code.
 *
 * @param         pkt_in   Packets to be processed
 * @param[in,out] pkt_out  Packet handle array specifying resulting packets
 * @param         param    Operation parameters array
 * @param         num_pkt  Number of packets to be processed
 *
 * @return Number of input packets consumed (0 ... num_pkt)
 * @retval <0 on failure
 */
int odp_comp_decompress(const odp_packet_t pkt_in[],
			odp_packet_t pkt_out[],
			odp_comp_packet_op_param_t param[],
			int num_pkt);

/**
 * Comp packet operation asynchronous
 *
 * Performs the ASYNC compression operations on the packets.
 * If session is created in ODP_COMP_ASYNC mode, event will be queued
 * to completion queue. Application should monitor ODP_EVENT_PACKET with
 * subtype ODP_EVENT_PACKET_COMP on queue.
 *
 * If session is created in ODP_COMP_SYNC mode, behavior is undefined.
 *
 * Caller should initialize pkt_out with valid output
 * packet handles.All arrays should be of num_pkt size. Resulting packets
 * are returned through events.
 *
 * In case of partially processed array i.e. when
 * number of packets returned < num_pkt, application should call
 * odp_comp_result() on 1st failed packet (i.e. n+1th packet where
 * n=number of packets returned) to ensure if error code is
 * ODP_COMP_ERR_OUT_OF_SPACE.
 *
 * In case of ODP_COMP_ERR_OUT_OF_SPACE, application should keep on calling
 * odp_comp_op() API with more output buffer until call returns
 * with num_pkt or with result as ODP_COMP_ERR_NONE or any other
 * failure code.
 *
 * For compression + hash, call returns with hash appended to the end of
 * last processed chunk of data.
 *
 * User should compute processed data len = total output len - digest_len, where
 * digest_len queried through odp_comp_hash_alg_capability().
 *
 * Please note it is always recommended that application using async mode,
 * provide sufficiently large buffer size to avoid ODP_COMP_ERR_OUT_OF_SPACE.
 * Else it is recommended that application maintain relevant context
 * with respect to each input processing request to correctly identify
 * its corresponding enqueued event.
 *
 *
 * @param pkt_in   Packets to be processed
 * @param pkt_out  Packet handle array specifying resulting packets
 * @param param    Operation parameters array
 * @param num_pkt  Number of packets to be processed
 *
 * @return Number of input packets consumed (0 ... num_pkt)
 * @retval <0 on failure
 */
int odp_comp_compress_enq(const odp_packet_t pkt_in[],
			  odp_packet_t pkt_out[],
			  odp_comp_packet_op_param_t param[],
			  int num_pkt);

/**
 * Comp packet operation
 *
 * Performs the ASYNC decompression operations on the packet array.
 *
 * If session is created in ODP_COMP_ASYNC mode, result will be queued
 * to completion queue. Application should monitor ODP_EVENT_PACKET
 * with subtype ODP_EVENT_PACKET_COMP on queue.
 *
 * If session is created in ODP_COMP_SYNC mode, behavior is undefined.
 *
 * for decompression+hash, call returns with hash appended to the end of
 * last processed chunk of data.
 *
 * User should compute processed data len = total output length - digest_len,
 * where digest_len queried through odp_comp_hash_alg_capability().
 *
 * Caller should initialize pkt_out with valid output
 * packet handles.All arrays should be of num_pkt size. Resulting packets
 * are returned through events.
 * In case of partially processed array i.e. when
 * number of packets returned < num_pkt, application should call
 * odp_comp_result() on 1st failed packet (i.e. n+1th packet where
 * n=number of packets returned) to ensure if error code is
 * ODP_COMP_ERR_OUT_OF_SPACE.
 * In case of ODP_COMP_ERR_OUT_OF_SPACE, application should keep on calling
 * odp_comp_op() API with more output buffer until call returns
 * with num_pkt or with result as ODP_COMP_ERR_NONE or any other
 * failure code.
 *
 * Please note it is always recommended that application using async mode,
 * provide sufficiently large buffer size to avoid ODP_COMP_ERR_OUT_OF_SPACE.
 * Else it is recommended that application maintain required context
 * to associate event to its respective input.
 *
 * @param pkt_in   Packets to be processed
 * @param pkt_out  Packet handle array specifying resulting packets
 * @param param    Operation parameters array
 * @param num_pkt  Number of packets to be processed
 *
 * @return Number of input packets consumed (0 ... num_pkt)
 * @retval <0 on failure
 */
int odp_comp_decompress_enq(const odp_packet_t pkt_in[],
			    odp_packet_t pkt_out[],
			    odp_crypto_packet_op_param_t param[],
			    int num_pkt);

 /**
 * Get compression/decompression operation results from an processed packet.
 *
 * Successful compression/decompression operations produce
 * packets which contain operation result metadata. This function copies the
 * operation results from an processed packet. Event subtype of this kind
 * of packet is ODP_EVENT_PACKET_COMP. Results are undefined if input packet
 * has not be processed by compression/decompression call.
 *
 * @param[out]    result  Pointer to operation result for output
 * @param	    packet  An processed packet (ODP_EVENT_PACKET_COMP)
 *
 * @retval  0     On success
 * @retval <0     On failure
 *
 * @see odp_comp_compress_enq(), odp_comp_decompress_enq(),
 *	   odp_comp_packet_from_event()
 */
int odp_comp_result(odp_comp_packet_result_t *result,
		    odp_packet_t packet);

 /**
  * Convert processed packet event to packet handle
  *
  * Get packet handle corresponding to processed packet event. Event subtype
  * must be ODP_EVENT_PACKET_COMP. compression/decompression operation
  * results can be examined with odp_comp_result().
  *
  * @param ev	    Event handle
  *
  * @return Valid Packet handle on success,
  *	    ODP_PACKET_INVALID on failure
  *
  * @see odp_event_subtype(), odp_comp_result()
  *
  * @ Example Usage
  * odp_event_t ev = odp_queue_deque(comple_q);
  * odp_event_subtype_t subtype;
  * if(ODP_PACKET_EVENT == odp_event_types(ev, &subtype)) {
  * if(subtype == ODP_PACKET_EVENT_COMP) {
  *  pkt = odp_comp_packet_from_event(ev);
  *  odp_comp_op_result_t res;
  *  odp_comp_result(&res, packet);
  * }
  * }
  */
odp_packet_t odp_comp_packet_from_event(odp_event_t event);

 /**
  * Convert processed packet handle to event
  *
  * The packet handle must be an output of an compression/decompression
  * operation.
  *
  * @param pkt	    Packet handle from odp_comp_compress_enq()/
  *		    odp_comp_decomp_enq()
  *
  * @return Event handle
  */
odp_event_t odp_comp_packet_to_event(odp_packet_t pkt);

/**
 * Get printable value for an odp_comp_session_t
 *
 * @param hdl  odp_comp_session_t handle to be printed
 * @return     uint64_t value that can be used to print/display this
 *             handle
 *
 * @note This routine is intended to be used for diagnostic purposes
 * to enable applications to generate a printable value that represents
 * an odp_comp_session_t handle.
 */
uint64_t odp_comp_session_to_u64(odp_comp_session_t hdl);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#include <odp/visibility_end.h>
#endif