/*
 * Copyright 2003 - 2014 Stonestreet One.
 * All Rights Reserved.
 * Author:  Rory Sledge
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname    Description of Modification
 *   --------  -----------    ------------------------------------------------
 *   05/21/03  R. Sledge      Initial creation.
 *   07/01/06  D. Lange       Updated with new API based on Bit Rate.
 ****************************************************************************
 */

/**
 *  @file SBCAPI.h
 *
 *  @brief Stonestreet One Subband Codec Interface Prototypes and
 *      Constants. To use any of the following API's, include the following declaration in your application.
 *
 *  @code
 *  #include "SS1SBC.h"
 *  @endcode
 *  ============================================================================
 */
#ifndef __SBCAPIH__
#define __SBCAPIH__

#include "BTAPITyp.h"           /*! Bluetooth API Type Definitions. */
#include "BTTypes.h"            /*! Bluetooth Type Definitions/Constants. */



    /*! Success Return Codes. */
#define SBC_PROCESSING_COMPLETE                                  (0)
#define SBC_PROCESSING_DATA                                      (1)

   /*! Error Return Codes. */
#define SBC_ERROR_INVALID_PARAMETER                              (-1)
#define SBC_ERROR_INSUFFICIENT_RESOURCES                         (-2)
#define SBC_ERROR_NOT_INITIALIZED                                (-3)
#define SBC_ERROR_UNKNOWN_ERROR                                  (-4)

   /*! The following enumerated type represents the supported Sampling
    * Frequency types for the Subband Codec.
    */
typedef enum
{
  sf16kHz,
  sf32kHz,
  sf441kHz,
  sf48kHz
} SBC_Sampling_Frequency_t;

   /*! The following enumerated type represents the supported Block Size
    * types for the Subband Codec.
    */
typedef enum
{
   bsFour,
   bsEight,
   bsTwelve,
   bsSixteen,
   bsFifteen
} SBC_Block_Size_t;

   /*! The following enumerated type represents the supported Channel
    * Mode types for the Subband Codec.
    */
typedef enum
{
   cmMono,
   cmDualChannel,
   cmStereo,
   cmJointStereo
} SBC_Channel_Mode_t;

   /*! The following enumerated type represents the supported Allocation
    * Method types for the Subband Codec.
    */
typedef enum
{
   amLoudness,
   amSNR
} SBC_Allocation_Method_t;

   /*! The following enumerated type represents the supported Number of
    * Subbands for the Subband Codec.
    */
typedef enum
{
   sbFour,
   sbEight
} SBC_Subbands_t;

   /*! The follow type definitions represent the Encoder Handle Type to
    * be used to represent an initialized encoder.
    */
typedef void *Encoder_t;

   /*! The follow type definitions represent the Decoder Handle Type to
    * be used to represent an initialized decoder.
    */
typedef void *Decoder_t;

   /*! The following constants represent the defined parameters to
    * specify modified SBC (mSBC) to support Wide Band Speech (WBS).
    */
#define SBC_MSBC_WBS_SAMPLE_FREQUENCY                            (sf16kHz)
#define SBC_MSBC_WBS_BLOCK_SIZE                                  (bsFifteen)
#define SBC_MSBC_WBS_CHANNEL_MODE                                (cmMono)
#define SBC_MSBC_WBS_ALLOCATION_METHOD                           (amLoudness)
#define SBC_MSBC_WBS_SUBBANDS                                    (sbEight)
#define SBC_MSBC_WBS_MAXIMUM_BIT_RATE                            (60800)

   /*! The following structure type represents the Encoder Configuration
    * information required by the Subband Codec when encoding data.
    * \note To support modified SBC (mSBC), the following parameters
    *          *MUST* be specified:
    *             - sf16kHz
    *             - bsFifteen
    *             - cmMono
    *             - amLoudness
    *             - sbEight
    *             - 60,800 (maximum bit rate)
    * \note Constants exist for the above.  These constants begin
    *          with the following prefix:
    *
    *             SBC_MSBC_WBS_
    *
    */
typedef struct _tagSBC_Encode_Configuration_t
{
   SBC_Sampling_Frequency_t SamplingFrequency;
   SBC_Block_Size_t         BlockSize;
   SBC_Channel_Mode_t       ChannelMode;
   SBC_Allocation_Method_t  AllocationMethod;
   SBC_Subbands_t           Subbands;
   DWord_t                  MaximumBitRate;
} SBC_Encode_Configuration_t;

   /*! The following structure type represents the data to be Encoded.
    * \note The Data Length size members specify the size in
    *          Word_t's NOT unsigned char's!
    */
typedef struct _tagSBC_Encode_Data_t
{
   Word_t       *LeftChannelDataPtr;
   Word_t       *RightChannelDataPtr;
   unsigned int  ChannelDataLength;
   unsigned int  UnusedChannelDataLength;
} SBC_Encode_Data_t;

   /*! The following structure type represents the Encoded Bit Stream
    * Data returned when a frame of SBC data has been successfully
    * encoded.
    */
typedef struct _tagSBC_Encode_Bit_Stream_Data_t
{
   unsigned int   BitStreamDataSize;
   unsigned int   BitStreamDataLength;
   unsigned char *BitStreamDataPtr;
} SBC_Encode_Bit_Stream_Data_t;

   /*! The following structure type represents the Decoder Configuration
    * information which was parsed from the SBC frame and used in the
    * decoding process.
    * \note If a modified SBC (mSBC) frame is decoded, it will have
    *          the following parameters:
    *             - sf16kHz
    *             - bsFifteen
    *             - cmMono
    *             - amLoudness
    *             - sbEight
    *             - 60,800 (maximum bit rate)
    *             - 26     (bit pool)
    *             - 57     (frame length)
    * \note Constants exist for the above (aside from the calculated
    *          Bit Pool and Frame Length values).  These constants
    *          begin with the following prefix:
    *
    *             SBC_MSBC_WBS_
    *
    */
typedef struct _tagSBC_Decode_Configuration_t
{
   SBC_Sampling_Frequency_t SamplingFrequency;
   SBC_Block_Size_t         BlockSize;
   SBC_Channel_Mode_t       ChannelMode;
   SBC_Allocation_Method_t  AllocationMethod;
   SBC_Subbands_t           Subbands;
   DWord_t                  BitRate;
   unsigned int             BitPool;
   unsigned int             FrameLength;
} SBC_Decode_Configuration_t;

   /*! The following structure type represents the Decoded Data returned
    * when a frame of SBC data has been successfully decoded.
    * Information pertaining the configuration associated with this data
    * will exist in the Decode Configuration Structure.
    * \note The Data Size and Data Length size members specify the
    *          size in Word_t's NOT unsigned char's!
    */
typedef struct _tagSBC_Decode_Data_t
{
   unsigned int  ChannelDataSize;
   Word_t       *LeftChannelDataPtr;
   unsigned int  LeftChannelDataLength;
   Word_t       *RightChannelDataPtr;
   unsigned int  RightChannelDataLength;
} SBC_Decode_Data_t;

   /*! @brief The following function is a utility function that exists to
    * determine the Bit Pool size (in bits) required to hold an SBC
    * Frame encoded with the specified information.
    * @param EncodeConfiguration Pointer to the configuration that will
    * be used to calculate the required bit pool size (in number of bits).
    * @return This function returns a positive, non-zero value if the bit pool size (in bits)
    * was able to be determined. This function returns a negative
    * return error code if there was an error.
    */
BTPSAPI_DECLARATION int BTPSAPI SBC_CalculateEncoderBitPoolSize(SBC_Encode_Configuration_t *EncodeConfiguration);

#ifdef INCLUDE_SBC_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SBC_CalculateEncoderBitPoolSize_t)(SBC_Encode_Configuration_t *EncodeConfiguration);
#endif

   /*! @brief The following function is a utility function that exists to
    * determine the size (in bytes) required to hold an SBC Frame
    * encoded with the specified information.
    * @param EncodeConfiguration Pointer to the configuration that will
    * be used to calculate the required encoded frame size (in number of bytes).
    * @return This function returns a
    * positive, non-zero value if the frame size (in bytes) was able to
    * be determined. This function returns a negative return error code
    * if there was an error.
    * \note If the MaximumBitRate member of the EncodeConfiguration
    *          is less than or equal to 512, then the parameter is taken
    *          to be the actual Bit Pool value instead of a Bit Rate.
    * \note This function can be used to determine the maximum
    *          buffer size that will be required to encode an SBC
    *          Frame (Bit stream) into (programatically).
    */
BTPSAPI_DECLARATION int BTPSAPI SBC_CalculateEncoderFrameLength(SBC_Encode_Configuration_t *EncodeConfiguration);

#ifdef INCLUDE_SBC_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SBC_CalculateEncoderFrameLength_t)(SBC_Encode_Configuration_t *EncodeConfiguration);
#endif

   /*! @brief The following function is a utility function that exists to
    * determine the actual Bit Rate (in bits) for an SBC Bit Stream
    * encoded with the specified information.
    * @param EncodeConfiguration Pointer to the configuration
    * that will be used to calculate the actual encoded bit rate (in number of bits).
    * @return This function returns a
    * positive, non-zero value if the Bit Rate (in bits) was able to be
    * determined. This function returns a negative return error code if
    * there was an error.
    * \note If the MaximumBitRate member of the EncodeConfiguration
    *          is less than or equal to 512, then the parameter is taken
    *          to be the actual Bit Pool value instead of a Bit Rate.
    * \note This function can be used to determine the actual Bit
    *          Rate that will be required to encode an SBC Frame (Bit
    *          stream) into (programatically).
    * \note The value returned by this function can (and will almost
    *          always be) different from the value that is used as the
    *          Bit Rate parameter in the Encode Configuration structure.
    *          This is because the SBC encoder utilizes Bit Pool values
    *          that govern the actual Bit Rate (as opposed to the Bit
    *          Rate parameter).
    */
BTPSAPI_DECLARATION long BTPSAPI SBC_CalculateEncoderBitRate(SBC_Encode_Configuration_t *EncodeConfiguration);

#ifdef INCLUDE_SBC_API_PROTOTYPES
   typedef long (BTPSAPI *PFN_SBC_CalculateEncoderBitRate_t)(SBC_Encode_Configuration_t *EncodeConfiguration);
#endif

   /*! @brief The following function is responsible for Initializing an SBC
    * Encoder. @param EncodeConfiguration The first parameter to this function
    * is a pointer to the configuration to use during the encoding process.
    * @return This function will return a valid encoder handle if success or NULL if an error
    * occurs.
    * \note The SBC_Change_Encoder_Configuration() function can be
    *          used to alter the current encoder parameters.  Please
    *          note that the configuration should remain constant during
    *          an individual frame of SBC data.  If the configuration
    *          changes during the encoding of an SBC Frame any existing
    *          data will be flushed and encoding of a new frame will
    *          begin.
    * \note The data required for an entire SBC Frame will be equal
    *          to:
    *             Block Size * Number Subbands
    *          The value represents the number of samples required to
    *          generate a single SBC Frame.
    * \note If the MaximumBitRate member of the EncodeConfiguration
    *          is less than or equal to 512, then the parameter is taken
    *          to be the actual Bit Pool value instead of a Bit Rate.
    */
BTPSAPI_DECLARATION Encoder_t BTPSAPI SBC_Initialize_Encoder(SBC_Encode_Configuration_t *EncodeConfiguration);

#ifdef INCLUDE_SBC_API_PROTOTYPES
   typedef Encoder_t (BTPSAPI *PFN_SBC_Initialize_Encoder_t)(SBC_Encode_Configuration_t *EncodeConfiguration);
#endif

   /*! @brief The following function is responsible for freeing all resources
    * that were previously allocated for an SBC Encoder
    * @param EncoderHandle  Encoder Handle of the Encoder to cleanup.
    * This value was returned from a successful call to the SBC_Initialize_Encoder() function.
    */
BTPSAPI_DECLARATION void BTPSAPI SBC_Cleanup_Encoder(Encoder_t EncoderHandle);

#ifdef INCLUDE_SBC_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_SBC_Cleanup_Encoder_t)(Encoder_t EncoderHandle);
#endif

   /*! @brief The following function is responsible for changing the parameters
    * that are used to encode a SBC Stream.
    * @param EncoderHandle   Encoder Handle of the Encoder to cleanup.
    * This value was returned from a successful call to the SBC_Initialize_Encoder() function.
    * @param EncodeConfiguration Pointer to the configuration that will
    * be used for all subsequent bit stream encoding (unless explicitly changed).
    * The encoding parameters should remain constant during the encoding of a
    * single frame of SBC data.  See EncodeConfiguration parameter description
    * of SBC_Initialize_Encoder for a detailed structure definition.
    * @return This function returns a positive, non zero
    * value, if successful or a negative return value if there was an
    * error.  The successful return value represents the required size
    * of a single SBC Frame (equivalent return value from the
    * SBC_CalculateEncoderFrameLength() function.
    * \note If the MaximumBitRate member of the EncodeConfiguration
    *          is less than or equal to 512, then the parameter is taken
    *          to be the actual Bit Pool value instead of a Bit Rate.
    */
BTPSAPI_DECLARATION int BTPSAPI SBC_Change_Encoder_Configuration(Encoder_t EncoderHandle, SBC_Encode_Configuration_t *EncodeConfiguration);

#ifdef INCLUDE_SBC_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SBC_Change_Encoder_Configuration_t)(Encoder_t EncoderHandle, SBC_Encode_Configuration_t *EncodeConfiguration);
#endif

   /*! @brief The following function is responsible for Encoding Audio Data into
    * an SBC Frame.
    * @param EncoderHandle Encoder Handle of the Encoder stream that is
    * to encode the specified data.  This value was returned from a successful
    * call to the SBC_Initialize_Encoder() function.
    * @param EncodeData Pointer to an Encode Data structure, which holds
    * the data to be encoded.  The Right Channel Data is not required in the case
    * when encoding data with the Channel Mode in the Encode Configuration
    * Structure set to Mono.
    * @param EncodedBitStreamData Pointer to the encoded SBC Frame buffer
    * information. On input the size and data pointer member must be valid.
    * If the return value of this function indicates that a SBC Audio Frame was
    * processed then the contents of the data stream buffer (and length member)
    * will specify the encoded SBC bit stream data, otherwise the members of this
    * structure will be undefined.
    * @return This function returns SBC_PROCESSING_COMPLETE when a complete frame of SBC data has been
    * encoded.  If more audio data is required before a complete frame
    * can be produced this function will return SBC_PROCESSING_DATA.  If
    * an error occurs this function will return a negative error code
    * return value.
    * \note If the Left and Right Channel Data pointers point to
    *          buffers that are one sample offsets of each other, then
    *          this function will correctly interleave the left and
    *          right channels in the correct locations in the specified
    *          buffers.  For this module to correctly interleave the
    *          data the Left and Right Channel Data Pointers *MUST* be
    *          offset by a single location (i.e. LeftChannelDataPtr[1]
    *          equals RightChannelDataPtr[0]).  This is only applicable
    *          for stereo streams.
    */
BTPSAPI_DECLARATION int BTPSAPI SBC_Encode_Data(Encoder_t EncoderHandle, SBC_Encode_Data_t *EncodeData, SBC_Encode_Bit_Stream_Data_t *EncodedBitStreamData);

#ifdef INCLUDE_SBC_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SBC_Encode_Data_t)(Encoder_t EncoderHandle, SBC_Encode_Data_t *EncodeData, SBC_Encode_Bit_Stream_Data_t *EncodedBitStreamData);
#endif

   /*! @brief The following function is a utility function that exists to
    * determine the total size (in bytes) of the current frame SBC frame
    * bit stream pointer.  This is useful for extracting individual SBC
    * frames from an SBC bit stream.
    * @param BitStreamDataLength Specifies the length (in bytes) of the bit stream
    * data that is pointed to by the next parameter.
    * @param BitStreamDataPtr Specifies a pointer to a buffer that contains the
    * actual SBC bit stream.  This pointer should point to the start of an SBC
    * frame (i.e. the first element in this buffer should be the SBC frame sync word).
    * @return This function returns zero if the
    * specified bit stream data pointer does not immediately point to an
    * SBC frame OR there are not enough bytes specified in the buffer
    * (given by the first parameter) to determine if valid SBC frame
    * exists. This function returns a non-zero number which represents
    * the the size (in bytes) of the SBC frame pointed to by the SBC
    * frame buffer.
    * \note This function will return zero if there are not enough
    *          bytes specified in the buffer to hold the SBC frame data.
    */
BTPSAPI_DECLARATION int BTPSAPI SBC_CalculateDecoderFrameSize(unsigned int BitStreamDataLength, Byte_t *BitStreamDataPtr);

#ifdef INCLUDE_SBC_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SBC_CalculateDecoderFrameSize_t)(unsigned int BitStreamDataLength, Byte_t *BitStreamDataPtr);
#endif

   /*! @brief The following function is a utility function that exists to parse
    * and return the Decode Configuration of an SBC frame.  This could
    * be used, for example, to determine the: number of bytes in the SBC
    * frame, determine the number of channels in the frame, or determine
    * the number of samples in the frame.
    * @param BitStreamDataLength Specifies the length (in bytes) of the bit
    * stream data that is pointed to by the next parameter.
    * @param BitStreamDataPtr Specifies a pointer to a buffer that contains
    * the actual SBC bit stream.  This pointer should point to the start of
    * an SBC frame (i.e. the first element in this buffer should be the SBC frame sync word).
    * @param DecodeConfiguration Pointer to structure to return decoded
    * configuration structure (if this function returns success.
    * The information in this structure specifies the decoded information about
    * the PCM data that was returned.
    * @return This function returns a negative value if the specified bit stream data
    * pointer does not immediately point to an SBC frame, there are not
    * enough bytes specified in the buffer (given by the first parameter)
    * to determine if a valid SBC frame exists, or if there
    * are not enough bytes specified in the buffer to hold the SBC frame
    * data. This function returns a non-zero number which represents
    * the number of audio samples, for all channels, that are stored in
    * the SBC frame.
    */
BTPSAPI_DECLARATION int BTPSAPI SBC_CalculateDecodeConfiguration(unsigned int BitStreamDataLength, Byte_t *BitStreamDataPtr, SBC_Decode_Configuration_t *DecodeConfiguration);

#ifdef INCLUDE_SBC_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SBC_CalculateDecodeConfiguration_t)(unsigned int BitStreamDataLength, Byte_t *BitStreamDataPtr, SBC_Decode_Configuration_t *DecodeConfiguration);
#endif

   /*! @brief The following function is responsible for Initializing an SBC
    * Decoder. @return This function will return a valid decoder handle if
    * success or NULL if an error occurs.
    */
BTPSAPI_DECLARATION Decoder_t BTPSAPI SBC_Initialize_Decoder(void);

#ifdef INCLUDE_SBC_API_PROTOTYPES
   typedef Decoder_t (BTPSAPI *PFN_SBC_Initialize_Decoder_t)(void);
#endif

   /*! @brief The following function is responsible for freeing all resources
    * that were previously allocated for an SBC Decoder. @param DecoderHandle The only
    * parameter to this function is the decoder handle of the decoder to
    * cleanup.
    */
BTPSAPI_DECLARATION void BTPSAPI SBC_Cleanup_Decoder(Decoder_t DecoderHandle);

#ifdef INCLUDE_SBC_API_PROTOTYPES
   typedef void (BTPSAPI *PFN_SBC_Cleanup_Decoder_t)(Decoder_t DecoderHandle);
#endif

   /*! @brief The following function is responsible for Decoding SBC Bit Streams
    * into Audio Data.
    * @param DecoderHandle Decoder Handle of the Decoder stream that
    * is to decode the specified data. This value was returned
    * from a successful call to the SBC_Initialize_Decoder() function.
    * @param DataLength Bit stream data buffer length specified in bytes.
    * @param DataPtr Pointer to actual bit stream to be decoded.
    * This buffer must be (at least) the size specified by the DataLength parameter.
    * @param DecodeConfiguration Pointer to decode configuration
    * structure.  The contents of this structure are valid when this
    * function indicates that it has decoded a SBC frame (SBC_PROCESSING_COMPLETE is returned).  The information in this structure specifies the decoded information about the PCM data that was returned.
    * @param DecodedData Pointer to a Decode Data structure that
    * will hold the data that is decoded (only when the return value
    * of this function indicates that an SBC Frame was decoded).
    * The Right Channel Data is not used in the case when SBC bit
    * streams that specify Mono as the Channel Mode (specified in the
    * Decode Configuration Structure).
    * @param UnusedDataLength Pointer to the amount of data that
    * was passed to this function but was not required for the complete
    * processing of the current frame of data. Any unused data should
    * be passed back to the decoder in the next call of this function.
    * This parameter is changed to reflect the number of bytes from the
    * input buffer that have been consumed when this function call is successful.
    * The caller should repeatedly call this function until this parameter is
    * returned as zero (or an error occurs).
    * @return This function returns SBC_PROCESSING_COMPLETE when a complete frame of SBC data
    * has been decoded.  Upon the completion of processing a frame of
    * audio data the Decode Configuration parameters and Decoded Data
    * will be set with the information and data decoded from the audio
    * frame.  If more bit stream data is required before the audio data
    * can be completely decoded this function will return
    * SBC_PROCESSING_DATA.  If an error occurs this function will return
    * a negative error code return value.
    * \note If the Left and Right Channel Data pointers point to
    *          buffers that are one sample offsets of each other, then
    *          this function will correctly interleave the left and
    *          right channels in the correct locations in the specified
    *          buffers.  For this module to correctly interleave the
    *          data the Left and Right Channel Data Pointers *MUST* be
    *          offset by a single location (i.e. LeftChannelDataPtr[1]
    *          equals RightChannelDataPtr[0]).  This is only applicable
    *          for stereo streams.
    */
BTPSAPI_DECLARATION int BTPSAPI SBC_Decode_Data(Decoder_t DecoderHandle, unsigned int DataLength, unsigned char *DataPtr, SBC_Decode_Configuration_t *DecodeConfiguration, SBC_Decode_Data_t *DecodedData, unsigned int *UnusedDataLength);

#ifdef INCLUDE_SBC_API_PROTOTYPES
   typedef int (BTPSAPI *PFN_SBC_Decode_Data_t)(Decoder_t DecoderHandle, unsigned int DataLength, unsigned char *DataPtr, SBC_Decode_Configuration_t *DecodeConfiguration, SBC_Decode_Data_t *DecodedData, unsigned int *UnusedDataLength);
#endif

#endif
