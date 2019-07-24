/*
 * Copyright 2003 - 2014 Stonestreet One.
 * All Rights Reserved.
 * Author:  Rory Sledge
 *** MODIFICATION HISTORY ****************************************************
 *
 *   mm/dd/yy  F. Lastname       Description of Modification
 *   --------  -----------       ---------------------------------------------
 *   07/02/03  R. Sledge         Initial creation.
 *****************************************************************************
 */

/**
 *  @file A2DPAPI.h
 *
 *  @brief Stonestreet One Bluetooth Stack Advanced Audio Distribution
 *      Profile Type Definitions and Constants.
 *
 *  @section A2DP Programming Interface
 *  The Advanced Audio Distribution Profile programming interface defines the protocols
 *  and procedures to be used to implement A2DP capabilities.  Because the
 *  Advanced Audio Distribution Profile is so heavily dependent on the
 *  Generic Audio/Video Distribution (GAVD) Profile which uses the
 *  Audio/Video Distribution Protocol (AVDTP), there are no additional functions
 *  required to implement this profile.  The application programmer can simply
 *  use the GAVD Profile API in conjunction with the constants and MACROs that
 *  are defined in the Advanced Audio Distribution Profile API header file to
 *  implement the Advanced Audio Distribution Profile.  It should be noted that
 *  most of the A2DP Specification deals with the implementation of the
 *  Subband CODEC (SBC).  Stonestreet One provides a Subband CODEC library
 *  that can be used by application developers to manager
 *  SBC stream encoding and decoding. The programmer should consult
 *  the GAVD and SBC documentation, in addition to the core Bluetopia documentation,
 *  when implementing Bluetooth applications that conform to
 *  the Advanced Audio Distribution Profile Specification.
 *
 *  The necessary SS1 Audio profile and Sub-System Headers can be included in an application
 *  as below.
 *
 *  @code
 *  #include "SS1BTA2D.h"
 *  @endcode
 *  ============================================================================
 */
#ifndef __A2DPAPIH__
#define __A2DPAPIH__

#include "SS1BTPS.h"
    /*! Bluetooth Stack API Prototypes/Constants. */
    /*! SDP Advanced Audio Distribution Profile Service Class UUID's.
    * The following MACRO is a utility MACRO that assigns the Advanced
    * Audio Distribution Profile Audio Source Service Class Bluetooth
    * Universally Unique Identifier
    * (ADVANCED_AUDIO_DISTRIBUTION_AUDIO_SOURCE_UUID_16) to the
    * specified UUID_16_t variable.  This MACRO accepts one parameter
    * which is the UUID_16_t variable that is to receive the
    * ADVANCED_AUDIO_DISTRIBUTION_AUDIO_SOURCE_UUID_16 Constant value.
    */
#define SDP_ASSIGN_ADVANCED_AUDIO_DISTRIBUTION_AUDIO_SOURCE_UUID_16(_x) ASSIGN_SDP_UUID_16((_x), 0x11, 0x0A)

   /*! The following MACRO is a utility MACRO that assigns the Advanced
    * Audio Distribution Profile Audio Source Service Class Bluetooth
    * Universally Unique Identifier
    * (ADVANCED_AUDIO_DISTRIBUTION_AUDIO_SOURCE_UUID_32) to the
    * specified UUID_32_t variable.  This MACRO accepts one parameter
    * which is the UUID_32_t variable that is to receive the
    * ADVANCED_AUDIO_DISTRIBUTION_AUDIO_SOURCE_UUID_32 Constant value.
    */
#define SDP_ASSIGN_ADVANCED_AUDIO_DISTRIBUTION_AUDIO_SOURCE_UUID_32(_x) ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x11, 0x0A)

   /*! The following MACRO is a utility MACRO that assigns the Advanced
    * Audio Distribution Profile Audio Source Service Class Bluetooth
    * Universally Unique Identifier
    * (ADVANCED_AUDIO_DISTRIBUTION_AUDIO_SOURCE_UUID_128) to the
    * specified UUID_128_t variable.  This MACRO accepts one parameter
    * which is the UUID_128_t variable that is to receive the
    * ADVANCED_AUDIO_DISTRIBUTION_AUDIO_SOURCE_UUID_128 Constant value.
    */
#define SDP_ASSIGN_ADVANCED_AUDIO_DISTRIBUTION_AUDIO_SOURCE_UUID_128(_x) ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x11, 0x0A, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /*! The following MACRO is a utility MACRO that assigns the Advanced
    * Audio Distribution Profile Audio Sink Service Class Bluetooth
    * Universally Unique Identifier
    * (ADVANCED_AUDIO_DISTRIBUTION_AUDIO_SINK_UUID_16) to the specified
    * UUID_16_t variable.  This MACRO accepts one parameter which is the
    * UUID_16_t variable that is to receive the
    * ADVANCED_AUDIO_DISTRIBUTION_AUDIO_SINK_UUID_16 Constant value.
    */
#define SDP_ASSIGN_ADVANCED_AUDIO_DISTRIBUTION_AUDIO_SINK_UUID_16(_x)   ASSIGN_SDP_UUID_16((_x), 0x11, 0x0B)

   /*! The following MACRO is a utility MACRO that assigns the Advanced
    * Audio Distribution Profile Audio Sink Service Class Bluetooth
    * Universally Unique Identifier
    * (ADVANCED_AUDIO_DISTRIBUTION_AUDIO_SINK_UUID_32) to the specified
    * UUID_32_t variable.  This MACRO accepts one parameter which is the
    * UUID_32_t variable that is to receive the
    * ADVANCED_AUDIO_DISTRIBUTION_AUDIO_SINK_UUID_32 Constant value.
    */
#define SDP_ASSIGN_ADVANCED_AUDIO_DISTRIBUTION_AUDIO_SINK_UUID_32(_x)   ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x11, 0x0B)

   /*! The following MACRO is a utility MACRO that assigns the Advanced
    * Audio Distribution Profile Audio Sink Service Class Bluetooth
    * Universally Unique Identifier
    * (ADVANCED_AUDIO_DISTRIBUTION_AUDIO_SINK_UUID_128) to the specified
    * UUID_128_t variable.  This MACRO accepts one parameter which is
    * the UUID_128_t variable that is to receive the
    * ADVANCED_AUDIO_DISTRIBUTION_AUDIO_SINK_UUID_128 Constant value.
    */
#define SDP_ASSIGN_ADVANCED_AUDIO_DISTRIBUTION_AUDIO_SINK_UUID_128(_x)  ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x11, 0x0B, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /* SDP Advanced Audio Distribution Profile UUID's.
    * The following MACRO is a utility MACRO that assigns the Advanced
    * Audio Distribution Profile Bluetooth Universally Unique Identifier
    * (ADVANCED_AUDIO_DISTRIBUTION_PROFILE_UUID_16) to the specified
    * UUID_16_t variable.  This MACRO accepts one parameter which is the
    * UUID_16_t variable that is to receive the
    * ADVANCED_AUDIO_DISTRIBUTION_PROFILE_UUID_16 Constant value.
    */
#define SDP_ASSIGN_ADVANCED_AUDIO_DISTRIBUTION_PROFILE_UUID_16(_x)      ASSIGN_SDP_UUID_16((_x), 0x11, 0x0D)

   /*! The following MACRO is a utility MACRO that assigns the Advanced
    * Audio Distribution Profile Bluetooth Universally Unique Identifier
    * (ADVANCED_AUDIO_DISTRIBUTION_PROFILE_UUID_32) to the specified
    * UUID_32_t variable.  This MACRO accepts one parameter which is the
    * UUID_32_t variable that is to receive the
    * ADVANCED_AUDIO_DISTRIBUTION_PROFILE_UUID_32 Constant value.
    */
#define SDP_ASSIGN_ADVANCED_AUDIO_DISTRIBUTION_PROFILE_UUID_32(_x)      ASSIGN_SDP_UUID_32((_x), 0x00, 0x00, 0x11, 0x0D)

   /*! The following MACRO is a utility MACRO that assigns the Advanced
    * Audio Distribution Profile Bluetooth Universally Unique Identifier
    * (ADVANCED_AUDIO_DISTRIBUTION_PROFILE_UUID_128) to the specified
    * UUID_128_t variable.  This MACRO accepts one parameter which is
    * the UUID_128_t variable that is to receive the
    * ADVANCED_AUDIO_DISTRIBUTION_PROFILE_UUID_128 Constant value.
    */
#define SDP_ASSIGN_ADVANCED_AUDIO_DISTRIBUTION_PROFILE_UUID_128(_x)     ASSIGN_SDP_UUID_128((_x), 0x00, 0x00, 0x11, 0x0D, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB)

   /*! The following definitions represent the Supported Features that
    * can be supported by the A2DP Audio Source.  These SDP Definitions
    * are used with the Supported Features Attribute.
    */
#define SDP_A2DP_SUPPORTED_FEATURES_AUDIO_SOURCE_PLAYER                 (0x0001)
#define SDP_A2DP_SUPPORTED_FEATURES_AUDIO_SOURCE_MICROPHONE             (0x0002)
#define SDP_A2DP_SUPPORTED_FEATURES_AUDIO_SOURCE_TUNER                  (0x0004)
#define SDP_A2DP_SUPPORTED_FEATURES_AUDIO_SOURCE_MIXER                  (0x0008)

   /*! The following definitions represent the Supported Features that
    * can be supported by the A2DP Audio Sink.  These SDP Definitions
    * are used with the Supported Features Attribute.
    */
#define SDP_A2DP_SUPPORTED_FEATURES_AUDIO_SINK_HEADPHONE                (0x0001)
#define SDP_A2DP_SUPPORTED_FEATURES_AUDIO_SINK_SPEAKER                  (0x0002)
#define SDP_A2DP_SUPPORTED_FEATURES_AUDIO_SINK_RECORDER                 (0x0004)
#define SDP_A2DP_SUPPORTED_FEATURES_AUDIO_SINK_AMPLIFIER                (0x0008)

   /*! The following constant defines the Profile Version Number that is
    * supported by this implementation (this is the value that is used
    * in the SDP Records for the actual Profile Version).
    */
#define A2DP_PROFILE_VERSION                                            (0x0102)

   /*! The following constants represent the defined error codes that can
    * be used if there is a Codec Specific Information Element error.
    * These codes can be used (in addition to the defined GAVD/AVDTP
    * error codes).  See section 5.1.3 of the A2DP Specification for
    * more details.
    */
#define A2DP_GAVD_ERROR_CODE_INVALID_CODEC_TYPE                         (0xC1)
#define A2DP_GAVD_ERROR_CODE_NOT_SUPPORTED_CODEC_TYPE                   (0xC2)
#define A2DP_GAVD_ERROR_CODE_INVALID_SAMPLING_FREQUENCY                 (0xC3)
#define A2DP_GAVD_ERROR_CODE_NOT_SUPPORTED_SAMPLING_FREQUENCY           (0xC4)
#define A2DP_GAVD_ERROR_CODE_INVALID_CHANNEL_MODE                       (0xC5)
#define A2DP_GAVD_ERROR_CODE_NOT_SUPPORTED_CHANNEL_MODE                 (0xC6)
#define A2DP_GAVD_ERROR_CODE_INVALID_SUBBANDS                           (0xC7)
#define A2DP_GAVD_ERROR_CODE_NOT_SUPPORTED_SUBBANDS                     (0xC8)
#define A2DP_GAVD_ERROR_CODE_INVALID_ALLOCATION_METHOD                  (0xC9)
#define A2DP_GAVD_ERROR_CODE_NOT_SUPPORTED_ALLOCATION_METHOD            (0xCA)
#define A2DP_GAVD_ERROR_CODE_INVALID_MINIMUM_BIT_POOL_VALUE             (0xCB)
#define A2DP_GAVD_ERROR_CODE_NOT_SUPPORTED_MINIMUM_BIT_POOL_VALUE       (0xCC)
#define A2DP_GAVD_ERROR_CODE_INVALID_MAXIMUM_BIT_POOL_VALUE             (0xCD)
#define A2DP_GAVD_ERROR_CODE_NOT_SUPPORTED_MAXIMUM_BIT_POOL_VALUE       (0xCE)
#define A2DP_GAVD_ERROR_CODE_INVALID_LAYER                              (0xCF)
#define A2DP_GAVD_ERROR_CODE_NOT_SUPPORTED_LAYER                        (0xD0)
#define A2DP_GAVD_ERROR_CODE_NOT_SUPPORTED_CRC                          (0xD1)
#define A2DP_GAVD_ERROR_CODE_NOT_SUPPORTED_MPF                          (0xD2)
#define A2DP_GAVD_ERROR_CODE_NOT_SUPPORTED_MBR                          (0xD3)
#define A2DP_GAVD_ERROR_CODE_INVALID_BIT_RATE                           (0xD4)
#define A2DP_GAVD_ERROR_CODE_NOT_SUPPORTED_BIT_RATE                     (0xD5)
#define A2DP_GAVD_ERROR_CODE_INVALID_OBJECT_TYPE                        (0xD6)
#define A2DP_GAVD_ERROR_CODE_NOT_SUPPORTED_OBJECT_TYPE                  (0xD7)
#define A2DP_GAVD_ERROR_CODE_INVALID_CHANNELS                           (0xD8)
#define A2DP_GAVD_ERROR_CODE_NOT_SUPPORTED_CHANNELS                     (0xD9)
#define A2DP_GAVD_ERROR_CODE_INVALID_VERSION                            (0xDA)
#define A2DP_GAVD_ERROR_CODE_NOT_SUPPORTED_VERSION                      (0xDB)
#define A2DP_GAVD_ERROR_CODE_NOT_SUPPORTED_MAXIMUM_SUL                  (0xDC)
#define A2DP_GAVD_ERROR_CODE_INVALID_BLOCK_LENGTH                       (0xDD)
#define A2DP_GAVD_ERROR_CODE_INVALID_CP_TYPE                            (0xE0)
#define A2DP_GAVD_ERROR_CODE_INVALID_CP_FORMAT                          (0xE1)

   /*! The following constants represent the defined Media Codec Type
    * values supported by A2DP and match the values as specified in
    * Bluetooth Assigned Numbers "Host Operating Environment".
    */
#define A2DP_MEDIA_CODEC_TYPE_SBC                                       (0x00)
#define A2DP_MEDIA_CODEC_TYPE_MPEG_1_2_AUDIO                            (0x01)
#define A2DP_MEDIA_CODEC_TYPE_MPEG_2_4_AUDIO                            (0x02)
#define A2DP_MEDIA_CODEC_TYPE_ATRAC                                     (0x04)
#define A2DP_MEDIA_CODEC_TYPE_NON_A2DP                                  (0xFF)

   /*! Bluetooth SIG Assigned Numbers Content Protection Types.
    */
#define A2DP_CONTENT_PROTECTION_TYPE_DTCP                               (0x00001)
#define A2DP_CONTENT_PROTECTION_TYPE_SCMS_T                             (0x00002)

   /*! The following constants represent the defined values used with the
    * SCMS-T Content Protection Mechanism.
    */
#define A2DP_CONTENT_PROTECTION_TYPE_SCMS_T_HEADER_LENGTH               (0x01)

#define A2DP_CONTENT_PROTECTION_TYPE_SCMS_T_COPY_ALLOWED                (0x00)
#define A2DP_CONTENT_PROTECTION_TYPE_SCMS_T_COPY_ONCE                   (0x02)
#define A2DP_CONTENT_PROTECTION_TYPE_SCMS_T_COPY_PROHIBITED             (0x03)

   /*! Subband Codec (SBC) Type Definitions/Constants.
    * The following structure represents the Codec Specific Information
    * Element for the subband codec.
    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagA2DP_SBC_Codec_Specific_Information_Element_t
{
   Byte_t SamplingFrequencyChannelMode;
   Byte_t BlockLengthSubbandsAllocationMethod;
   Byte_t MinimumBitPoolValue;
   Byte_t MaximumBitPoolValue;
} __PACKED_STRUCT_END__ A2DP_SBC_Codec_Specific_Information_Element_t;

#define A2DP_SBC_CODEC_SPECIFIC_INFORMATION_ELEMENT_SIZE (sizeof(A2DP_SBC_Codec_Specific_Information_Element_t))

   /*! The following constants represent the defined sampling frequencies
    * supported by the subband codec.  These values are as they would
    * appear in Octet0 of the Codec Specific Information Element for the
    * subbands codec.
    */
#define A2DP_SBC_SAMPLING_FREQUENCY_MASK                                (0xF0)
#define A2DP_SBC_SAMPLING_FREQUENCY_16_KHZ_VALUE                        (0x80)
#define A2DP_SBC_SAMPLING_FREQUENCY_32_KHZ_VALUE                        (0x40)
#define A2DP_SBC_SAMPLING_FREQUENCY_44_1_KHZ_VALUE                      (0x20)
#define A2DP_SBC_SAMPLING_FREQUENCY_48_KHZ_VALUE                        (0x10)

   /*! The following constants represent the defined channel modes
    * supported by the subband codec.  These values are as they would
    * appear in Octet0 of the Codec Specific Information Element for the
    * subbands codec.
    */
#define A2DP_SBC_CHANNEL_MODE_MASK                                      (0x0F)
#define A2DP_SBC_CHANNEL_MODE_MONO_VALUE                                (0x08)
#define A2DP_SBC_CHANNEL_MODE_DUAL_CHANNEL_VALUE                        (0x04)
#define A2DP_SBC_CHANNEL_MODE_STEREO_VALUE                              (0x02)
#define A2DP_SBC_CHANNEL_MODE_JOINT_STEREO_VALUE                        (0x01)

   /*! The following constants represent the defined block lengths
    * supported by the subband codec.  These values are as they would
    * appear in Octet1 of the Codec Specific Information Element for the
    * subbands codec.
    */
#define A2DP_SBC_BLOCK_LENGTH_MASK                                      (0xF0)
#define A2DP_SBC_BLOCK_LENGTH_FOUR_VALUE                                (0x80)
#define A2DP_SBC_BLOCK_LENGTH_EIGHT_VALUE                               (0x40)
#define A2DP_SBC_BLOCK_LENGTH_TWELVE_VALUE                              (0x20)
#define A2DP_SBC_BLOCK_LENGTH_SIXTEEN_VALUE                             (0x10)

   /*! The following constants represent the defined subbands supported
    * by the subband codec.  These values are as they would appear in
    * Octet1 of the Codec Specific Information Element for the subbands
    * codec.
    */
#define A2DP_SBC_SUBBANDS_MASK                                          (0x0C)
#define A2DP_SBC_SUBBANDS_FOUR_VALUE                                    (0x08)
#define A2DP_SBC_SUBBANDS_EIGHT_VALUE                                   (0x04)

   /*! The following constants represent the defined allocation methods
    * supported by the subband codec.  These values are as they would
    * appear in Octet1 of the Codec Specific Information Element for the
    * subband codec.
    */
#define A2DP_SBC_ALLOCATION_METHOD_MASK                                 (0x03)
#define A2DP_SBC_ALLOCATION_METHOD_SNR_VALUE                            (0x02)
#define A2DP_SBC_ALLOCATION_METHOD_LOUDNESS_VALUE                       (0x01)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * sampling frequency value into the specified Codec Specific
    * Information Element variable.  This MACRO accepts as it's first
    * parameter a pointer to the Codec Specific Information Element
    * structure. The second parameter is the actual sampling frequency
    * value to store into the Codec Specific Information Element
    * structure.
    */
#define A2DP_SBC_ASSIGN_SAMPLING_FREQUENCY(_x, _y)      (((A2DP_SBC_Codec_Specific_Information_Element_t *)(_x))->SamplingFrequencyChannelMode |= (Byte_t)((Byte_t)(_y) & A2DP_SBC_SAMPLING_FREQUENCY_MASK))

   /*! The following MACRO is a utility MACRO that exists to read the
    * sampling frequency value from the specified Codec Specific
    * Information Element variable.  This MACRO accepts as its only
    * parameter a pointer to the Codec Specific Information Element
    * structure.  This MACRO returns the sampling frequency value.
    */
#define A2DP_SBC_READ_SAMPLING_FREQUENCY(_x)            (((A2DP_SBC_Codec_Specific_Information_Element_t *)(_x))->SamplingFrequencyChannelMode & A2DP_SBC_SAMPLING_FREQUENCY_MASK)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * channel mode value into the specified Codec Specific Information
    * Element variable.  This MACRO accepts as it's first parameter a
    * pointer to the Codec Specific Information Element structure. The second parameter is the actual channel mode value to store into
    * the Codec Specific Information Element structure.
    */
#define A2DP_SBC_ASSIGN_CHANNEL_MODE(_x, _y)            (((A2DP_SBC_Codec_Specific_Information_Element_t *)(_x))->SamplingFrequencyChannelMode |= (Byte_t)((Byte_t)(_y) & A2DP_SBC_CHANNEL_MODE_MASK))

   /*! The following MACRO is a utility MACRO that exists to read the
    * channel mode value from the specified Codec Specific Information
    * Element variable.  This MACRO accepts as its only parameter a
    * pointer to the Codec Specific Information Element structure.  This
    * MACRO returns the channel mode value.
    */
#define A2DP_SBC_READ_CHANNEL_MODE(_x)                  (((A2DP_SBC_Codec_Specific_Information_Element_t *)(_x))->SamplingFrequencyChannelMode & A2DP_SBC_CHANNEL_MODE_MASK)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * block length value into the specified Codec Specific Information
    * Element variable.  This MACRO accepts as it's first parameter a
    * pointer to the Codec Specific Information Element structure. The second parameter is the actual block length value to store into
    * the Codec Specific Information Element structure.
    */
#define A2DP_SBC_ASSIGN_BLOCK_LENGTH(_x, _y)            (((A2DP_SBC_Codec_Specific_Information_Element_t *)(_x))->BlockLengthSubbandsAllocationMethod |= (Byte_t)((Byte_t)(_y) & A2DP_SBC_BLOCK_LENGTH_MASK))

   /*! The following MACRO is a utility MACRO that exists to read the
    * block length value from the specified Codec Specific Information
    * Element variable.  This MACRO accepts as its only parameter a
    * pointer to the Codec Specific Information Element structure.  This
    * MACRO returns the block length value.
    */
#define A2DP_SBC_READ_BLOCK_LENGTH(_x)                  (((A2DP_SBC_Codec_Specific_Information_Element_t *)(_x))->BlockLengthSubbandsAllocationMethod & A2DP_SBC_BLOCK_LENGTH_MASK)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * subbands value into the specified Codec Specific Information
    * Element variable.  This MACRO accepts as it's first parameter a
    * pointer to the Codec Specific Information Element structure. The second parameter is the actual subbands value to store into the
    * Codec Specific Information Element structure.
    */
#define A2DP_SBC_ASSIGN_SUBBANDS(_x, _y)                (((A2DP_SBC_Codec_Specific_Information_Element_t *)(_x))->BlockLengthSubbandsAllocationMethod |= (Byte_t)((Byte_t)(_y) & A2DP_SBC_SUBBANDS_MASK))

   /*! The following MACRO is a utility MACRO that exists to read the
    * subbands value from the specified Codec Specific Information
    * Element variable.  This MACRO accepts as its only parameter a
    * pointer to the Codec Specific Information Element structure.  This
    * MACRO returns the subbands value.
    */
#define A2DP_SBC_READ_SUBBANDS(_x)                      (((A2DP_SBC_Codec_Specific_Information_Element_t *)(_x))->BlockLengthSubbandsAllocationMethod & A2DP_SBC_SUBBANDS_MASK)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * allocation method value into the specified Codec Specific
    * Information Element variable.  This MACRO accepts as it's first
    * parameter a pointer to the Codec Specific Information Element
    * structure. The second parameter is the actual allocation method
    * value to store into the Codec Specific Information Element
    * structure.
    */
#define A2DP_SBC_ASSIGN_ALLOCATION_METHOD(_x, _y)       (((A2DP_SBC_Codec_Specific_Information_Element_t *)(_x))->BlockLengthSubbandsAllocationMethod |= (Byte_t)((Byte_t)(_y) & A2DP_SBC_ALLOCATION_METHOD_MASK))

   /*! The following MACRO is a utility MACRO that exists to read the
    * allocation method value from the specified Codec Specific
    * Information Element variable.  This MACRO accepts as its only
    * parameter a pointer to the Codec Specific Information Element
    * structure.  This MACRO returns the allocation method value.
    */
#define A2DP_SBC_READ_ALLOCATION_METHOD(_x)             (((A2DP_SBC_Codec_Specific_Information_Element_t *)(_x))->BlockLengthSubbandsAllocationMethod & A2DP_SBC_ALLOCATION_METHOD_MASK)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * minimum bit pool value into the specified Codec Specific
    * Information Element variable.  This MACRO accepts as it's first
    * parameter a pointer to the Codec Specific Information Element
    * structure. The second parameter is the actual minimum bit pool
    * value to store into the Codec Specific Information Element
    * structure.
    */
#define A2DP_SBC_ASSIGN_MINIMUM_BIT_POOL_VALUE(_x, _y)  (((A2DP_SBC_Codec_Specific_Information_Element_t *)(_x))->MinimumBitPoolValue = (Byte_t)(_y))

   /*! The following MACRO is a utility MACRO that exists to read the
    * minimum bit pool value value from the specified Codec Specific
    * Information Element variable.  This MACRO accepts as its only
    * parameter a pointer to the Codec Specific Information Element
    * structure.  This MACRO returns the minimum bit pool value.
    */
#define A2DP_SBC_READ_MINIMUM_BIT_POOL_VALUE(_x)        (((A2DP_SBC_Codec_Specific_Information_Element_t *)(_x))->MinimumBitPoolValue)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * maximum bit pool value into the specified Codec Specific
    * Information Element variable.  This MACRO accepts as it's first
    * parameter a pointer to the Codec Specific Information Element
    * structure. The second parameter is the actual maximum bit pool
    * value to store into the Codec Specific Information Element
    * structure.
    */
#define A2DP_SBC_ASSIGN_MAXIMUM_BIT_POOL_VALUE(_x, _y)  (((A2DP_SBC_Codec_Specific_Information_Element_t *)(_x))->MaximumBitPoolValue = (Byte_t)(_y))

   /*! The following MACRO is a utility MACRO that exists to read the
    * maximum bit pool value value from the specified Codec Specific
    * Information Element variable.  This MACRO accepts as its only
    * parameter a pointer to the Codec Specific Information Element
    * structure.  This MACRO returns the maximum bit pool value.
    */
#define A2DP_SBC_READ_MAXIMUM_BIT_POOL_VALUE(_x)        (((A2DP_SBC_Codec_Specific_Information_Element_t *)(_x))->MaximumBitPoolValue)

   /*! The following structure defines the structure of the Media Payload
    * for an SBC Encoded Stream.  This structure simply consists of the
    * header information followed by the SBC Payload data (which is
    * of variable length).
    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagA2DP_SBC_Media_Payload_t
{
   Byte_t Header;
   Byte_t VariableData[1];
} __PACKED_STRUCT_END__ A2DP_SBC_Media_Payload_t;

   /*! The following MACRO is provided to allow the programmer a very
    * simple means of quickly determining the number of Data Bytes that
    * will be required to hold an entire A2DP SBC Media Payload data of
    * the specified length (byte length of SBC Payload).
    */
#define A2DP_SBC_MEDIA_PAYLOAD_SIZE(_x)                 (sizeof(A2DP_SBC_Media_Payload_t) - sizeof(unsigned char) + (unsigned int)(_x))

   /*! The following constants represent the defined bit masks that are
    * present in the SBC Payload Header.
    */
#define A2DP_SBC_HEADER_FRAGMENTED_MASK                                 (0x80)
#define A2DP_SBC_HEADER_START_MASK                                      (0x40)
#define A2DP_SBC_HEADER_LAST_MASK                                       (0x20)
#define A2DP_SBC_HEADER_NUMBER_FRAMES_MASK                              (0x0F)

   /*! The following MACROs are a utility MACROs that exists to set,
    * reset, and test the Fragmented bit flag in the SBC Media Payload
    * Header.  These MACROs accept as their only parameter a pointer to
    * the SBC Media Payload Header.
    */
#define A2DP_SBC_HEADER_SET_FRAGMENTED_BIT(_x)          (((A2DP_SBC_Media_Payload_t *)(_x))->Header |= A2DP_SBC_HEADER_FRAGMENTED_MASK)
#define A2DP_SBC_HEADER_RESET_FRAGMENTED_BIT(_x)        (((A2DP_SBC_Media_Payload_t *)(_x))->Header &= (~A2DP_SBC_HEADER_FRAGMENTED_MASK))
#define A2DP_SBC_HEADER_TEST_FRAGMENTED_BIT(_x)         ((((A2DP_SBC_Media_Payload_t *)(_x))->Header & A2DP_SBC_HEADER_FRAGMENTED_MASK)?TRUE:FALSE)

   /*! The following MACROs are a utility MACROs that exists to set,
    * reset, and test the Start Fragmented bit flag in the SBC Media
    * Payload Header.  These MACROs accept as their only parameter a
    * pointer to the SBC Media Payload Header.
    */
#define A2DP_SBC_HEADER_SET_START_BIT(_x)               (((A2DP_SBC_Media_Payload_t *)(_x))->Header |= A2DP_SBC_HEADER_START_MASK)
#define A2DP_SBC_HEADER_RESET_START_BIT(_x)             (((A2DP_SBC_Media_Payload_t *)(_x))->Header &= (~A2DP_SBC_HEADER_START_MASK))
#define A2DP_SBC_HEADER_TEST_START_BIT(_x)              ((((A2DP_SBC_Media_Payload_t *)(_x))->Header & A2DP_SBC_HEADER_START_MASK)?TRUE:FALSE)

   /*! The following MACROs are a utility MACROs that exists to set,
    * reset, and test the Last Fragmented bit flag in the SBC Media
    * Payload Header.  These MACROs accept as their only parameter a
    * pointer to the SBC Media Payload Header.
    */
#define A2DP_SBC_HEADER_SET_LAST_BIT(_x)                (((A2DP_SBC_Media_Payload_t *)(_x))->Header |= A2DP_SBC_HEADER_LAST_MASK)
#define A2DP_SBC_HEADER_RESET_LAST_BIT(_x)              (((A2DP_SBC_Media_Payload_t *)(_x))->Header &= (~A2DP_SBC_HEADER_LAST_MASK))
#define A2DP_SBC_HEADER_TEST_LAST_BIT(_x)               ((((A2DP_SBC_Media_Payload_t *)(_x))->Header & A2DP_SBC_HEADER_LAST_MASK)?TRUE:FALSE)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * specified number of frames value into the specified SBC Media
    * Payload variable.  This MACRO accepts as it's first parameter a
    * pointer to the SBC Media Payload structure. The second parameter
    * is the actual number of frames value to store into the SBC Media
    * Payload structure.
    */
#define A2DP_SBC_ASSIGN_NUMBER_FRAMES(_x, _y)           (((A2DP_SBC_Media_Payload_t *)(_x))->Header |= ((Byte_t)((_y) & A2DP_SBC_HEADER_NUMBER_FRAMES_MASK)))

   /*! The following MACRO is a utility MACRO that exists to read the
    * specified number of frames value from the specified SBC Media
    * Payload variable.  This MACRO accepts as it's only parameter a
    * pointer to the SBC Media Payload structure.
    */
#define A2DP_SBC_READ_NUMBER_FRAMES(_x)                 (((A2DP_SBC_Media_Payload_t *)(_x))->Header & A2DP_SBC_HEADER_NUMBER_FRAMES_MASK)

   /* MPEG-1,2 Audio Codec Type Definitions/Constants.
    * The following structure represents the Codec Specific Information
    * Element for the MPEG-1,2 Audio codec.
    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagA2DP_MPEG_1_2_Codec_Specific_Information_Element_t
{
   Byte_t LayerCRCChannelMode;
   Byte_t RFAMPFSamplingFrequency;
   Byte_t VBRBitRate;
   Byte_t BitRate;
} __PACKED_STRUCT_END__ A2DP_MPEG_1_2_Codec_Specific_Information_Element_t;

#define A2DP_MPEG_1_2_CODEC_SPECIFIC_INFORMATION_ELEMENT_SIZE  (sizeof(A2DP_MPEG_1_2_Codec_Specific_Information_Element_t))

   /*! The following constants represent the defined layers supported by
    * MPEG-1,2 Audio Codec.  These values are as they would appear in
    * Octet0 of the Codec Specific Information Element for the MPEG-1,2
    * Audio Codec.
    */
#define A2DP_MPEG_1_2_LAYER_MASK                                  (0xE0)
#define A2DP_MPEG_1_2_LAYER_1_VALUE                               (0x80)
#define A2DP_MPEG_1_2_LAYER_2_VALUE                               (0x40)
#define A2DP_MPEG_1_2_LAYER_3_VALUE                               (0x20)

   /*! The following constant represents the defined CRC protection bit
    * supported by MPEG-1,2 Audio Codec.  This value is as it would
    * appear in Octet0 of the Codec Specific Information Element for the
    * MPEG-1,2 Audio Codec.
    */
#define A2DP_MPEG_1_2_CRC_PROTECTION_MASK                               (0x10)
#define A2DP_MPEG_1_2_CRC_PROTECTION_VALUE                              (0x10)

   /*! The following constants represents the defined channel modes
    * supported by MPEG-1,2 Audio Codec.  These values are as they would
    * appear in Octet0 of the Codec Specific Information Element for the
    * MPEG-1,2 Audio Codec.
    */
#define A2DP_MPEG_1_2_CHANNEL_MODE_MASK                                 (0x0F)
#define A2DP_MPEG_1_2_CHANNEL_MODE_MONO_VALUE                           (0x08)
#define A2DP_MPEG_1_2_CHANNEL_MODE_DUAL_CHANNEL_VALUE                   (0x04)
#define A2DP_MPEG_1_2_CHANNEL_MODE_STEREO_VALUE                         (0x02)
#define A2DP_MPEG_1_2_CHANNEL_MODE_JOINT_STEREO_VALUE                   (0x01)

   /*! The following constant represents the defined Media Payload Format
    * bit supported by MPEG-1,2 Audio Codec.  This value is as it would
    * appear in Octet1 of the Codec Specific Information Element for the
    * MPEG-1,2 Audio Codec.
    */
#define A2DP_MPEG_1_2_MEDIA_PAYLOAD_FORMAT_MASK                         (0x40)
#define A2DP_MPEG_1_2_MEDIA_PAYLOAD_FORMAT_VALUE                        (0x40)

   /*! The following constants represent the defined sampling frequencies
    * supported by the MPEG-1,2 Audio Codec.  These values are as they
    * would appear in Octet1 of the Codec Specific Information Element
    * for the MPEG-1,2 Audio Codec.
    */
#define A2DP_MPEG_1_2_SAMPLING_FREQUENCY_MASK                           (0x3F)
#define A2DP_MPEG_1_2_SAMPLING_FREQUENCY_16_KHZ_VALUE                   (0x20)
#define A2DP_MPEG_1_2_SAMPLING_FREQUENCY_22_05_KHZ_VALUE                (0x10)
#define A2DP_MPEG_1_2_SAMPLING_FREQUENCY_24_KHZ_VALUE                   (0x08)
#define A2DP_MPEG_1_2_SAMPLING_FREQUENCY_32_KHZ_VALUE                   (0x04)
#define A2DP_MPEG_1_2_SAMPLING_FREQUENCY_44_1_KHZ_VALUE                 (0x02)
#define A2DP_MPEG_1_2_SAMPLING_FREQUENCY_48_KHZ_VALUE                   (0x01)

   /*! The following constant represents the defined Variable Bit Rate
    * Support bit supported by MPEG-1,2 Audio Codec.  This value is as
    * it would appear in Octet2 of the Codec Specific Information
    * Element for the MPEG-1,2 Audio Codec.
    */
#define A2DP_MPEG_1_2_VARIABLE_BIT_RATE_SUPPORT_MASK                    (0x80)
#define A2DP_MPEG_1_2_VARIABLE_BIT_RATE_SUPPORT_VALUE                   (0x80)

   /*! The following constants represent the defined bit rate indexes
    * supported by the MPEG-1,2 Audio Codec.  These values are as they
    * would appear in Octet2-3 of the Codec Specific Information Element
    * for the MPEG-1,2 Audio Codec.
    */
#define A2DP_MPEG_1_2_BIT_RATE_INDEX_UPPER_BYTE_MASK                    (0x7F)
#define A2DP_MPEG_1_2_BIT_RATE_INDEX_LOWER_BYTE_MASK                    (0xFF)
#define A2DP_MPEG_1_2_BIT_RATE_INDEX_1110_VALUE                         (0x4000)
#define A2DP_MPEG_1_2_BIT_RATE_INDEX_1101_VALUE                         (0x2000)
#define A2DP_MPEG_1_2_BIT_RATE_INDEX_1100_VALUE                         (0x1000)
#define A2DP_MPEG_1_2_BIT_RATE_INDEX_1011_VALUE                         (0x0800)
#define A2DP_MPEG_1_2_BIT_RATE_INDEX_1010_VALUE                         (0x0400)
#define A2DP_MPEG_1_2_BIT_RATE_INDEX_1001_VALUE                         (0x0200)
#define A2DP_MPEG_1_2_BIT_RATE_INDEX_1000_VALUE                         (0x0100)
#define A2DP_MPEG_1_2_BIT_RATE_INDEX_0111_VALUE                         (0x0080)
#define A2DP_MPEG_1_2_BIT_RATE_INDEX_0110_VALUE                         (0x0040)
#define A2DP_MPEG_1_2_BIT_RATE_INDEX_0101_VALUE                         (0x0020)
#define A2DP_MPEG_1_2_BIT_RATE_INDEX_0100_VALUE                         (0x0010)
#define A2DP_MPEG_1_2_BIT_RATE_INDEX_0011_VALUE                         (0x0008)
#define A2DP_MPEG_1_2_BIT_RATE_INDEX_0010_VALUE                         (0x0004)
#define A2DP_MPEG_1_2_BIT_RATE_INDEX_0001_VALUE                         (0x0002)
#define A2DP_MPEG_1_2_BIT_RATE_INDEX_0000_VALUE                         (0x0001)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * layer value into the specified Codec Specific Information Element
    * variable.  This MACRO accepts as it's first parameter a pointer to
    * the Codec Specific Information Element structure. The second parameter is the actual layer value to store into the Codec
    * Specific Information Element structure.
    */
#define A2DP_MPEG_1_2_ASSIGN_LAYER(_x, _y)              (((A2DP_MPEG_1_2_Codec_Specific_Information_Element_t *)(_x))->LayerCRCChannelMode |= (Byte_t)((Byte_t)(_y) & A2DP_MPEG_1_2_LAYER_MASK))

   /*! The following MACRO is a utility MACRO that exists to read the
    * layer value from the specified Codec Specific Information Element
    * variable.  This MACRO accepts as its only parameter a pointer to
    * the Codec Specific Information Element structure.  This MACRO
    * returns the layer value.
    */
#define A2DP_MPEG_1_2_READ_LAYER(_x)                    (((A2DP_MPEG_1_2_Codec_Specific_Information_Element_t *)(_x))->LayerCRCChannelMode & A2DP_MPEG_1_2_LAYER_MASK)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * CRC protection value into the specified Codec Specific Information
    * Element variable.  This MACRO accepts as it's first parameter a
    * pointer to the Codec Specific Information Element structure. The second parameter is the actual CRC protection value to store into
    * the Codec Specific Information Element structure.
    */
#define A2DP_MPEG_1_2_ASSIGN_CRC_PROTECTION(_x, _y)     (((A2DP_MPEG_1_2_Codec_Specific_Information_Element_t *)(_x))->LayerCRCChannelMode |= (Byte_t)((Byte_t)(_y) & A2DP_MPEG_1_2_CRC_PROTECTION_MASK))

   /*! The following MACRO is a utility MACRO that exists to read the CRC
    * protection value from the specified Codec Specific Information
    * Element variable.  This MACRO accepts as its only parameter a
    * pointer to the Codec Specific Information Element structure.  This
    * MACRO returns the CRC protection value.
    */
#define A2DP_MPEG_1_2_READ_CRC_PROTECTION(_x)           (((A2DP_MPEG_1_2_Codec_Specific_Information_Element_t *)(_x))->LayerCRCChannelMode & A2DP_MPEG_1_2_CRC_PROTECTION_MASK)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * channel mode value into the specified Codec Specific Information
    * Element variable.  This MACRO accepts as it's first parameter a
    * pointer to the Codec Specific Information Element structure. The second parameter is the actual channel mode value to store into
    * the Codec Specific Information Element structure.
    */
#define A2DP_MPEG_1_2_ASSIGN_CHANNEL_MODE(_x, _y)       (((A2DP_MPEG_1_2_Codec_Specific_Information_Element_t *)(_x))->LayerCRCChannelMode |= (Byte_t)((Byte_t)(_y) & A2DP_MPEG_1_2_CHANNEL_MODE_MASK))

   /*! The following MACRO is a utility MACRO that exists to read the
    * channel mode value from the specified Codec Specific Information
    * Element variable.  This MACRO accepts as its only parameter a
    * pointer to the Codec Specific Information Element structure.  This
    * MACRO returns the channel mode value.
    */
#define A2DP_MPEG_1_2_READ_CHANNEL_MODE(_x)             (((A2DP_MPEG_1_2_Codec_Specific_Information_Element_t *)(_x))->LayerCRCChannelMode & A2DP_MPEG_1_2_CHANNEL_MODE_MASK)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * media payload format value into the specified Codec Specific
    * Information Element variable.  This MACRO accepts as it's first
    * parameter a pointer to the Codec Specific Information Element
    * structure. The second parameter is the actual media payload
    * format value to store into the Codec Specific Information Element
    * structure.
    */
#define A2DP_MPEG_1_2_ASSIGN_MEDIA_PAYLOAD_FORMAT(_x, _y) (((A2DP_MPEG_1_2_Codec_Specific_Information_Element_t *)(_x))->RFAMPFSamplingFrequency |= (Byte_t)((Byte_t)(_y) & A2DP_MPEG_1_2_MEDIA_PAYLOAD_FORMAT_MASK))

   /*! The following MACRO is a utility MACRO that exists to read the
    * media payload format value from the specified Codec Specific
    * Information Element variable.  This MACRO accepts as its only
    * parameter a pointer to the Codec Specific Information Element
    * structure.  This MACRO returns the media payload format value.
    */
#define A2DP_MPEG_1_2_READ_MEDIA_PAYLOAD_FORMAT(_x)     (((A2DP_MPEG_1_2_Codec_Specific_Information_Element_t *)(_x))->RFAMPFSamplingFrequency & A2DP_MPEG_1_2_MEDIA_PAYLOAD_FORMAT_MASK)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * sampling frequency value into the specified Codec Specific
    * Information Element variable.  This MACRO accepts as it's first
    * parameter a pointer to the Codec Specific Information Element
    * structure. The second parameter is the actual sampling frequency
    * value to store into the Codec Specific Information Element
    * structure.
    */
#define A2DP_MPEG_1_2_ASSIGN_SAMPLING_FREQUENCY(_x, _y) (((A2DP_MPEG_1_2_Codec_Specific_Information_Element_t *)(_x))->RFAMPFSamplingFrequency |= (Byte_t)((Byte_t)(_y) & A2DP_MPEG_1_2_SAMPLING_FREQUENCY_MASK))

   /*! The following MACRO is a utility MACRO that exists to read the
    * sampling frequency value from the specified Codec Specific
    * Information Element variable.  This MACRO accepts as its only
    * parameter a pointer to the Codec Specific Information Element
    * structure.  This MACRO returns the sampling frequency value.
    */
#define A2DP_MPEG_1_2_READ_SAMPLING_FREQUENCY(_x)       (((A2DP_MPEG_1_2_Codec_Specific_Information_Element_t *)(_x))->RFAMPFSamplingFrequency & A2DP_MPEG_1_2_SAMPLING_FREQUENCY_MASK)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * variable bit rate support value into the specified Codec Specific
    * Information Element variable.  This MACRO accepts as it's first
    * parameter a pointer to the Codec Specific Information Element
    * structure. The second parameter is the actual variable bit rate
    * support value to store into the Codec Specific Information Element
    * structure.
    */
#define A2DP_MPEG_1_2_ASSIGN_VARIABLE_BIT_RATE_SUPPORT(_x, _y) (((A2DP_MPEG_1_2_Codec_Specific_Information_Element_t *)(_x))->VBRBitRate |= (Byte_t)((Byte_t)(_y) & A2DP_MPEG_1_2_VARIABLE_BIT_RATE_SUPPORT_MASK))

   /*! The following MACRO is a utility MACRO that exists to read the
    * variable bit rate support value from the specified Codec Specific
    * Information Element variable.  This MACRO accepts as its only
    * parameter a pointer to the Codec Specific Information Element
    * structure.  This MACRO returns the variable bit rate support
    * value.
    */
#define A2DP_MPEG_1_2_READ_VARIABLE_BIT_RATE_SUPPORT(_x) (((A2DP_MPEG_1_2_Codec_Specific_Information_Element_t *)(_x))->VBRBitRate & A2DP_MPEG_1_2_VARIABLE_BIT_RATE_SUPPORT_MASK)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * bit rate index value into the specified Codec Specific Information
    * Element variable.  This MACRO accepts as it's first parameter a
    * pointer to the Codec Specific Information Element structure. The second parameter is the actual variable bit rate index value to
    * store into the Codec Specific Information Element structure.
    */
#define A2DP_MPEG_1_2_ASSIGN_BIT_RATE_INDEX(_x, _y)                                                                                                                     \
{                                                                                                                                                                       \
   (((A2DP_MPEG_1_2_Codec_Specific_Information_Element_t *)(_x))->VBRBitRate |= (Byte_t)((Byte_t)((Word_t)(_y) >> 8) & A2DP_MPEG_1_2_BIT_RATE_INDEX_UPPER_BYTE_MASK));  \
   (((A2DP_MPEG_1_2_Codec_Specific_Information_Element_t *)(_x))->BitRate     = (Byte_t)((Word_t)(_y) & A2DP_MPEG_1_2_BIT_RATE_INDEX_LOWER_BYTE_MASK));                 \
}

   /*! The following MACRO is a utility MACRO that exists to read the bit
    * rate index value from the specified Codec Specific Information
    * Element variable.  This MACRO accepts as its only parameter a
    * pointer to the Codec Specific Information Element structure.  This
    * MACRO returns the bit rate index value.
    */
#define A2DP_MPEG_1_2_READ_BIT_RATE_INDEX(_x)           ((Word_t)(((Word_t)(((A2DP_MPEG_1_2_Codec_Specific_Information_Element_t *)(_x))->VBRBitRate & A2DP_MPEG_1_2_BIT_RATE_INDEX_UPPER_BYTE_MASK) << 8) | (Word_t)(((A2DP_MPEG_1_2_Codec_Specific_Information_Element_t *)(_x))->BitRate & A2DP_MPEG_1_2_BIT_RATE_INDEX_LOWER_BYTE_MASK)))

   /*! MPEG-2,4 AAC Type Definitions/Constants.
    * The following structure represents the Codec Specific Information
    * Element for the MPEG-2,4 Audio codec.
    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagA2DP_MPEG_2_4_Codec_Specific_Information_Element_t
{
   Byte_t ObjectType;
   Byte_t SamplingFrequency;
   Byte_t SamplingFrequencyChannelsRFA;
   Byte_t VBRBitRate;
   Byte_t BitRate0;
   Byte_t BitRate1;
} __PACKED_STRUCT_END__ A2DP_MPEG_2_4_Codec_Specific_Information_Element_t;

#define A2DP_MPEG_2_4_CODEC_SPECIFIC_INFORMATION_ELEMENT_SIZE  (sizeof(A2DP_MPEG_2_4_Codec_Specific_Information_Element_t))

   /*! The following constants represent the defined object types
    * supported by MPEG-2,4 Audio Codec.  These values are as they would
    * appear in Octet0 of the Codec Specific Information Element for the
    * MPEG-2,4 Audio Codec.
    */
#define A2DP_MPEG_2_4_OBJECT_TYPE_MASK                                  (0xF0)
#define A2DP_MPEG_2_4_OBJECT_TYPE_MPEG_2_AAC_LC_VALUE                   (0x80)
#define A2DP_MPEG_2_4_OBJECT_TYPE_MPEG_4_AAC_LC_VALUE                   (0x40)
#define A2DP_MPEG_2_4_OBJECT_TYPE_MPEG_4_AAC_LTP_VALUE                  (0x20)
#define A2DP_MPEG_2_4_OBJECT_TYPE_MPEG_4_AAC_SCALABLE_VALUE             (0x10)

   /*! The following constants represent the defined sampling frequencies
    * supported by MPEG-2,4 Audio Codec.  These values are as they would
    * appear in Octet1-2 of the Codec Specific Information Element for
    * the MPEG-2,4 Audio Codec.
    */
#define A2DP_MPEG_2_4_SAMPLING_FREQUENCY_UPPER_BYTE_MASK                (0xFF)
#define A2DP_MPEG_2_4_SAMPLING_FREQUENCY_LOWER_BYTE_MASK                (0xF0)
#define A2DP_MPEG_2_4_SAMPLING_FREQUENCY_8_KHZ_VALUE                    (0x8000)
#define A2DP_MPEG_2_4_SAMPLING_FREQUENCY_11_025_KHZ_VALUE               (0x4000)
#define A2DP_MPEG_2_4_SAMPLING_FREQUENCY_12_KHZ_VALUE                   (0x2000)
#define A2DP_MPEG_2_4_SAMPLING_FREQUENCY_16_KHZ_VALUE                   (0x1000)
#define A2DP_MPEG_2_4_SAMPLING_FREQUENCY_22_05_KHZ_VALUE                (0x0800)
#define A2DP_MPEG_2_4_SAMPLING_FREQUENCY_24_KHZ_VALUE                   (0x0400)
#define A2DP_MPEG_2_4_SAMPLING_FREQUENCY_32_KHZ_VALUE                   (0x0200)
#define A2DP_MPEG_2_4_SAMPLING_FREQUENCY_44_1_KHZ_VALUE                 (0x0100)
#define A2DP_MPEG_2_4_SAMPLING_FREQUENCY_48_KHZ_VALUE                   (0x0080)
#define A2DP_MPEG_2_4_SAMPLING_FREQUENCY_64_KHZ_VALUE                   (0x0040)
#define A2DP_MPEG_2_4_SAMPLING_FREQUENCY_88_2_KHZ_VALUE                 (0x0020)
#define A2DP_MPEG_2_4_SAMPLING_FREQUENCY_96_KHZ_VALUE                   (0x0010)

   /*! The following constants represent the defined channel modes
    * supported by MPEG-2,4 Audio Codec.  These values are as they would
    * appear in Octet2 of the Codec Specific Information Element for the
    * MPEG-2,4 Audio Codec.
    */
#define A2DP_MPEG_2_4_CHANNELS_MASK                                     (0x0C)
#define A2DP_MPEG_2_4_CHANNELS_1_VALUE                                  (0x08)
#define A2DP_MPEG_2_4_CHANNELS_2_VALUE                                  (0x04)

   /*! The following constant represents the defined Variable Bit Rate
    * Support bit supported by MPEG-2,4 Audio Codec.  This value is as
    * it would appear in Octet3 of the Codec Specific Information
    * Element for the MPEG-2,4 Audio Codec.
    */
#define A2DP_MPEG_2_4_VARIABLE_BIT_RATE_SUPPORT_MASK                    (0x80)
#define A2DP_MPEG_2_4_VARIABLE_BIT_RATE_SUPPORT_VALUE                   (0x80)

   /*! The following constants represent bit masks used to aid in adding
    * the bit rate to the Codec Specific Information Element for
    * MPEG-2,4 Audio Codec.
    */
#define A2DP_MPEG_2_4_BIT_RATE_UPPER_BYTE_MASK                          (0x7F)
#define A2DP_MPEG_2_4_BIT_RATE_MIDDLE_BYTE_MASK                         (0xFF)
#define A2DP_MPEG_2_4_BIT_RATE_LOWER_BYTE_MASK                          (0xFF)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * object type value into the specified Codec Specific Information
    * Element variable.  This MACRO accepts as it's first parameter a
    * pointer to the Codec Specific Information Element structure. The second parameter is the actual object type value to store into the
    * Codec Specific Information Element structure.
    */
#define A2DP_MPEG_2_4_ASSIGN_OBJECT_TYPE(_x, _y)        (((A2DP_MPEG_2_4_Codec_Specific_Information_Element_t *)(_x))->ObjectType |= (Byte_t)((Byte_t)(_y) & A2DP_MPEG_2_4_OBJECT_TYPE_MASK))

   /*! The following MACRO is a utility MACRO that exists to read the
    * object type value from the specified Codec Specific Information
    * Element variable.  This MACRO accepts as its only parameter a
    * pointer to the Codec Specific Information Element structure.  This
    * MACRO returns the object type value.
    */
#define A2DP_MPEG_2_4_READ_OBJECT_TYPE(_x)              (((A2DP_MPEG_2_4_Codec_Specific_Information_Element_t *)(_x))->ObjectType & A2DP_MPEG_2_4_OBJECT_TYPE_MASK)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * sampling frequency value into the specified Codec Specific
    * Information Element variable.  This MACRO accepts as it's first
    * parameter a pointer to the Codec Specific Information Element
    * structure. The second parameter is the actual sampling frequency
    * index value to store into the Codec Specific Information Element
    * structure.
    */
#define A2DP_MPEG_2_4_ASSIGN_SAMPLING_FREQUENCY(_x, _y)                                                                                                                                      \
{                                                                                                                                                                                            \
   (((A2DP_MPEG_2_4_Codec_Specific_Information_Element_t *)(_x))->SamplingFrequency             = (Byte_t)((Byte_t)((Word_t)(_y) >> 8) & A2DP_MPEG_2_4_SAMPLING_FREQUENCY_UPPER_BYTE_MASK)); \
   (((A2DP_MPEG_2_4_Codec_Specific_Information_Element_t *)(_x))->SamplingFrequencyChannelsRFA |= (Byte_t)((Word_t)(_y) & A2DP_MPEG_2_4_SAMPLING_FREQUENCY_LOWER_BYTE_MASK));                \
}

   /*! The following MACRO is a utility MACRO that exists to read the
    * sampling frequency value from the specified Codec Specific
    * Information Element variable.  This MACRO accepts as its only
    * parameter a pointer to the Codec Specific Information Element
    * structure.  This MACRO returns the sampling frequency value.
    */
#define A2DP_MPEG_2_4_READ_SAMPLING_FREQUENCY(_x)       ((Word_t)(((Word_t)(((A2DP_MPEG_2_4_Codec_Specific_Information_Element_t *)(_x))->SamplingFrequency & A2DP_MPEG_2_4_SAMPLING_FREQUENCY_UPPER_BYTE_MASK) << 8) | (Word_t)(((A2DP_MPEG_2_4_Codec_Specific_Information_Element_t *)(_x))->SamplingFrequencyChannelsRFA & A2DP_MPEG_2_4_SAMPLING_FREQUENCY_LOWER_BYTE_MASK)))

   /*! The following MACRO is a utility MACRO that exists to assign the
    * channels value into the specified Codec Specific Information
    * Element variable.  This MACRO accepts as it's first parameter a
    * pointer to the Codec Specific Information Element structure. The second parameter is the actual channels value to store into the
    * Codec Specific Information Element structure.
    */
#define A2DP_MPEG_2_4_ASSIGN_CHANNELS(_x, _y)           (((A2DP_MPEG_2_4_Codec_Specific_Information_Element_t *)(_x))->SamplingFrequencyChannelsRFA |= (Byte_t)((Byte_t)(_y) & A2DP_MPEG_2_4_CHANNELS_MASK))

   /*! The following MACRO is a utility MACRO that exists to read the
    * channels value from the specified Codec Specific Information
    * Element variable.  This MACRO accepts as its only parameter a
    * pointer to the Codec Specific Information Element structure.  This
    * MACRO returns the channels value.
    */
#define A2DP_MPEG_2_4_READ_CHANNELS(_x)                 (((A2DP_MPEG_2_4_Codec_Specific_Information_Element_t *)(_x))->SamplingFrequencyChannelsRFA & A2DP_MPEG_2_4_CHANNELS_MASK)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * variable bit rate support value into the specified Codec Specific
    * Information Element variable.  This MACRO accepts as it's first
    * parameter a pointer to the Codec Specific Information Element
    * structure. The second parameter is the actual variable bit rate
    * support value to store into the Codec Specific Information Element
    * structure.
    */
#define A2DP_MPEG_2_4_ASSIGN_VARIABLE_BIT_RATE_SUPPORT(_x, _y) (((A2DP_MPEG_2_4_Codec_Specific_Information_Element_t *)(_x))->VBRBitRate |= (Byte_t)((Byte_t)(_y) & A2DP_MPEG_2_4_VARIABLE_BIT_RATE_SUPPORT_MASK))

   /*! The following MACRO is a utility MACRO that exists to read the
    * variable bit rate support value from the specified Codec Specific
    * Information Element variable.  This MACRO accepts as its only
    * parameter a pointer to the Codec Specific Information Element
    * structure.  This MACRO returns the variable bit rate support
    * value.
    */
#define A2DP_MPEG_2_4_READ_VARIABLE_BIT_RATE_SUPPORT(_x) (((A2DP_MPEG_2_4_Codec_Specific_Information_Element_t *)(_x))->VBRBitRate & A2DP_MPEG_2_4_VARIABLE_BIT_RATE_SUPPORT_MASK)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * bit rate value into the specified Codec Specific Information
    * Element variable.  This MACRO accepts as it's first parameter a
    * pointer to the Codec Specific Information Element structure. The second parameter is the actual bit rate value to store into the
    * Codec Specific Information Element structure.
    */
#define A2DP_MPEG_2_4_ASSIGN_BIT_RATE(_x, _y)                                                                                                                        \
{                                                                                                                                                                    \
   (((A2DP_MPEG_2_4_Codec_Specific_Information_Element_t *)(_x))->VBRBitRate |= (Byte_t)((Byte_t)((DWord_t)(_y) >> 16) & A2DP_MPEG_2_4_BIT_RATE_UPPER_BYTE_MASK));   \
   (((A2DP_MPEG_2_4_Codec_Specific_Information_Element_t *)(_x))->BitRate0    = (Byte_t)(((DWord_t)(_y) >> 8) & A2DP_MPEG_2_4_BIT_RATE_MIDDLE_BYTE_MASK));           \
   (((A2DP_MPEG_2_4_Codec_Specific_Information_Element_t *)(_x))->BitRate1    = (Byte_t)((DWord_t)(_y) & A2DP_MPEG_2_4_BIT_RATE_LOWER_BYTE_MASK));                   \
}

   /*! The following MACRO is a utility MACRO that exists to read the bit
    * rate value from the specified Codec Specific Information Element
    * variable.  This MACRO accepts as its only parameter a pointer to
    * the Codec Specific Information Element structure.  This MACRO
    * returns the bit rate value.
    */
#define A2DP_MPEG_2_4_READ_BIT_RATE(_x)                 ((DWord_t)(((DWord_t)(((A2DP_MPEG_2_4_Codec_Specific_Information_Element_t *)(_x))->VBRBitRate & A2DP_MPEG_2_4_BIT_RATE_UPPER_BYTE_MASK) << 16) | ((DWord_t)(((A2DP_MPEG_2_4_Codec_Specific_Information_Element_t *)(_x))->BitRate0 & A2DP_MPEG_2_4_BIT_RATE_MIDDLE_BYTE_MASK) << 8) | (((A2DP_MPEG_2_4_Codec_Specific_Information_Element_t *)(_x))->BitRate1 & A2DP_MPEG_2_4_BIT_RATE_LOWER_BYTE_MASK)))

   /*! ATRAC Type Definitions/Constants.
    * The following structure represents the Codec Specific Information
    * Element for the ATRAC Audio codec.
    */
typedef __PACKED_STRUCT_BEGIN__ struct _tagA2DP_ATRAC_Codec_Specific_Information_Element_t
{
   Byte_t VersionChannelModeRFA;
   Byte_t RFASamplingFrequencyVBRBitRate;
   Byte_t BitRate0;
   Byte_t BitRate1;
   Byte_t MaximumSUL0;
   Byte_t MaximumSUL1;
   Byte_t RFA;
} __PACKED_STRUCT_END__ A2DP_ATRAC_Codec_Specific_Information_Element_t;

#define A2DP_ATRAC_CODEC_SPECIFIC_INFORMATION_ELEMENT_SIZE  (sizeof(A2DP_ATRAC_Codec_Specific_Information_Element_t))

   /*! The following constants represent the defined versions supported
    * by ATRAC Audio Codec.  These values are as they would appear in
    * Octet0 of the Codec Specific Information Element for the ATRAC
    * Audio Codec.
    */
#define A2DP_ATRAC_VERSION_MASK                                         (0xE0)
#define A2DP_ATRAC_VERSION_ATRAC_VALUE                                  (0x10)
#define A2DP_ATRAC_VERSION_ATRAC2_VALUE                                 (0x20)
#define A2DP_ATRAC_VERSION_ATRAC3_VALUE                                 (0x30)

   /*! The following constants represent the defined channel modes
    * supported by ATRAC Audio Codec.  These values are as they would
    * appear in Octet0 of the Codec Specific Information Element for the
    * ATRAC Audio Codec.
    */
#define A2DP_ATRAC_CHANNEL_MODE_MASK                                    (0x1C)
#define A2DP_ATRAC_CHANNEL_MODE_SINGLE_CHANNEL_VALUE                    (0x10)
#define A2DP_ATRAC_CHANNEL_MODE_DUAL_CHANNEL_VALUE                      (0x08)
#define A2DP_ATRAC_CHANNEL_MODE_JOINT_STEREO_VALUE                      (0x04)

   /*! The following constants represent the defined sampling frequencies
    * supported by ATRAC Audio Codec.  These values are as they would
    * appear in Octet1 of the Codec Specific Information Element for the
    * ATRAC Audio Codec.
    */
#define A2DP_ATRAC_SAMPLING_FREQUENCY_MASK                              (0x30)
#define A2DP_ATRAC_SAMPLING_FREQUENCY_44_1_KHZ_VALUE                    (0x20)
#define A2DP_ATRAC_SAMPLING_FREQUENCY_48_KHZ_VALUE                      (0x10)

   /*! The following constant represents the defined Variable Bit Rate
    * Support bit supported by ATRAC Audio Codec.  This value is as it
    * would appear in Octet1 of the Codec Specific Information Element
    * for the ATRAC Audio Codec.
    */
#define A2DP_ATRAC_VARIABLE_BIT_RATE_SUPPORT_MASK                       (0x08)
#define A2DP_ATRAC_VARIABLE_BIT_RATE_SUPPORT_VALUE                      (0x08)

   /*! The following constants represent the defined bit rate indexes
    * supported by ATRAC Audio Codec.  These values are as they would
    * appear in Octet1-3 of the Codec Specific Information Element for
    * the ATRAC Audio Codec.
    */
#define A2DP_ATRAC_BIT_RATE_INDEX_UPPER_BYTE_MASK                       (0x07)
#define A2DP_ATRAC_BIT_RATE_INDEX_MIDDLE_BYTE_MASK                      (0xFF)
#define A2DP_ATRAC_BIT_RATE_INDEX_LOWER_BYTE_MASK                       (0xFF)
#define A2DP_ATRAC_BIT_RATE_INDEX_0000_VALUE                            (0x040000)
#define A2DP_ATRAC_BIT_RATE_INDEX_0001_VALUE                            (0x020000)
#define A2DP_ATRAC_BIT_RATE_INDEX_0002_VALUE                            (0x010000)
#define A2DP_ATRAC_BIT_RATE_INDEX_0003_VALUE                            (0x008000)
#define A2DP_ATRAC_BIT_RATE_INDEX_0004_VALUE                            (0x004000)
#define A2DP_ATRAC_BIT_RATE_INDEX_0005_VALUE                            (0x002000)
#define A2DP_ATRAC_BIT_RATE_INDEX_0006_VALUE                            (0x001000)
#define A2DP_ATRAC_BIT_RATE_INDEX_0007_VALUE                            (0x000800)
#define A2DP_ATRAC_BIT_RATE_INDEX_0008_VALUE                            (0x000400)
#define A2DP_ATRAC_BIT_RATE_INDEX_0009_VALUE                            (0x000200)
#define A2DP_ATRAC_BIT_RATE_INDEX_000A_VALUE                            (0x000100)
#define A2DP_ATRAC_BIT_RATE_INDEX_000B_VALUE                            (0x000080)
#define A2DP_ATRAC_BIT_RATE_INDEX_000C_VALUE                            (0x000040)
#define A2DP_ATRAC_BIT_RATE_INDEX_000D_VALUE                            (0x000020)
#define A2DP_ATRAC_BIT_RATE_INDEX_000E_VALUE                            (0x000010)
#define A2DP_ATRAC_BIT_RATE_INDEX_000F_VALUE                            (0x000008)
#define A2DP_ATRAC_BIT_RATE_INDEX_0010_VALUE                            (0x000004)
#define A2DP_ATRAC_BIT_RATE_INDEX_0011_VALUE                            (0x000002)
#define A2DP_ATRAC_BIT_RATE_INDEX_0012_VALUE                            (0x000001)

   /*! The following constants represent bit masks used to aid in adding
    * the maximum SUL to the Codec Specific Information Element for
    * ATRAC Audio Codec.
    */
#define A2DP_ATRAC_MAXIMUM_SUL_UPPER_BYTE_MASK                          (0xFF)
#define A2DP_ATRAC_MAXIMUM_SUL_LOWER_BYTE_MASK                          (0xFF)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * version value into the specified Codec Specific Information
    * Element variable.  This MACRO accepts as it's first parameter a
    * pointer to the Codec Specific Information Element structure. The second parameter is the actual version value to store into the
    * Codec Specific Information Element structure.
    */
#define A2DP_ATRAC_ASSIGN_VERSION(_x, _y)               (((A2DP_ATRAC_Codec_Specific_Information_Element_t *)(_x))->VersionChannelModeRFA |= (Byte_t)((Byte_t)(_y) & A2DP_ATRAC_VERSION_MASK))

   /*! The following MACRO is a utility MACRO that exists to read the
    * version value from the specified Codec Specific Information
    * Element variable.  This MACRO accepts as its only parameter a
    * pointer to the Codec Specific Information Element structure.  This
    * MACRO returns the version value.
    */
#define A2DP_ATRAC_READ_VERSION(_x)                     (((A2DP_ATRAC_Codec_Specific_Information_Element_t *)(_x))->VersionChannelModeRFA & A2DP_ATRAC_VERSION_MASK)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * channel mode value into the specified Codec Specific Information
    * Element variable.  This MACRO accepts as it's first parameter a
    * pointer to the Codec Specific Information Element structure. The second parameter is the actual channel mode value to store into
    * the Codec Specific Information Element structure.
    */
#define A2DP_ATRAC_ASSIGN_CHANNEL_MODE(_x, _y)          (((A2DP_ATRAC_Codec_Specific_Information_Element_t *)(_x))->VersionChannelModeRFA |= (Byte_t)((Byte_t)(_y) & A2DP_ATRAC_CHANNEL_MODE_MASK))

   /*! The following MACRO is a utility MACRO that exists to read the
    * channel mode value from the specified Codec Specific Information
    * Element variable.  This MACRO accepts as its only parameter a
    * pointer to the Codec Specific Information Element structure.  This
    * MACRO returns the channel mode value.
    */
#define A2DP_ATRAC_READ_CHANNEL_MODE(_x)                (((A2DP_ATRAC_Codec_Specific_Information_Element_t *)(_x))->VersionChannelModeRFA & A2DP_ATRAC_CHANNEL_MODE_MASK)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * sampling frequency value into the specified Codec Specific
    * Information Element variable.  This MACRO accepts as it's first
    * parameter a pointer to the Codec Specific Information Element
    * structure. The second parameter is the actual sampling frequency
    * value to store into the Codec Specific Information Element
    * structure.
    */
#define A2DP_ATRAC_ASSIGN_SAMPLING_FREQUENCY(_x, _y)    (((A2DP_ATRAC_Codec_Specific_Information_Element_t *)(_x))->RFASamplingFrequencyVBRBitRate |= (Byte_t)((Byte_t)(_y) & A2DP_ATRAC_SAMPLING_FREQUENCY_MASK))

   /*! The following MACRO is a utility MACRO that exists to read the
    * sampling frequency value from the specified Codec Specific
    * Information Element variable.  This MACRO accepts as its only
    * parameter a pointer to the Codec Specific Information Element
    * structure.  This MACRO returns the sampling frequency value.
    */
#define A2DP_ATRAC_READ_SAMPLING_FREQUENCY(_x)          (((A2DP_ATRAC_Codec_Specific_Information_Element_t *)(_x))->RFASamplingFrequencyVBRBitRate & A2DP_ATRAC_SAMPLING_FREQUENCY_MASK)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * variable bit rate support value into the specified Codec Specific
    * Information Element variable.  This MACRO accepts as it's first
    * parameter a pointer to the Codec Specific Information Element
    * structure. The second parameter is the actual variable bit rate
    * support value to store into the Codec Specific Information Element
    * structure.
    */
#define A2DP_ATRAC_ASSIGN_VARIABLE_BIT_RATE_SUPPORT(_x, _y) (((A2DP_ATRAC_Codec_Specific_Information_Element_t *)(_x))->RFASamplingFrequencyVBRBitRate |= (Byte_t)((Byte_t)(_y) & A2DP_ATRAC_VARIABLE_BIT_RATE_SUPPORT_MASK))

   /*! The following MACRO is a utility MACRO that exists to read the
    * variable bit rate support value from the specified Codec Specific
    * Information Element variable.  This MACRO accepts as its only
    * parameter a pointer to the Codec Specific Information Element
    * structure.  This MACRO returns the variable bit rate support
    * value.
    */
#define A2DP_ATRAC_READ_VARIABLE_BIT_RATE_SUPPORT(_x)   (((A2DP_ATRAC_Codec_Specific_Information_Element_t *)(_x))->RFASamplingFrequencyVBRBitRate & A2DP_ATRAC_VARIABLE_BIT_RATE_SUPPORT_MASK)

   /*! The following MACRO is a utility MACRO that exists to assign the
    * bit rate index value into the specified Codec Specific Information
    * Element variable.  This MACRO accepts as it's first parameter a
    * pointer to the Codec Specific Information Element structure. The second parameter is the actual bit rate index value to store into
    * the Codec Specific Information Element structure.
    */
#define A2DP_ATRAC_ASSIGN_BIT_RATE_INDEX(_x, _y)                                                                                                                                       \
{                                                                                                                                                                                      \
   (((A2DP_ATRAC_Codec_Specific_Information_Element_t *)(_x))->RFASamplingFrequencyVBRBitRate |= (Byte_t)((Byte_t)((DWord_t)(_y) >> 16) & A2DP_ATRAC_BIT_RATE_INDEX_UPPER_BYTE_MASK)); \
   (((A2DP_ATRAC_Codec_Specific_Information_Element_t *)(_x))->BitRate0                        = (Byte_t)(((DWord_t)(_y) >> 8) & A2DP_ATRAC_BIT_RATE_INDEX_MIDDLE_BYTE_MASK));         \
   (((A2DP_ATARC_Codec_Specific_Information_Element_t *)(_x))->BitRate1                        = (Byte_t)((DWord_t)(_y) & A2DP_ATRAC_BIT_RATE_INDEX_LOWER_BYTE_MASK));                 \
}

   /*! The following MACRO is a utility MACRO that exists to read the bit
    * rate index value from the specified Codec Specific Information
    * Element variable.  This MACRO accepts as its only parameter a
    * pointer to the Codec Specific Information Element structure.  This
    * MACRO returns the bit rate index value.
    */
#define A2DP_ATRAC_READ_BIT_RATE_INDEX(_x)              ((DWord_t)(((DWord_t)(((A2DP_ATARC_Codec_Specific_Information_Element_t *)(_x))->RFASamplingFrequencyVBRBitRate & A2DP_ATRAC_BIT_RATE_UPPER_BYTE_MASK) << 16) | ((DWord_t)(((A2DP_ATRAC_Codec_Specific_Information_Element_t *)(_x))->BitRate0 & A2DP_ATRAC_BIT_RATE_MIDDLE_BYTE_MASK) << 8) | (((A2DP_ATRAC_Codec_Specific_Information_Element_t *)(_x))->BitRate1 & A2DP_ATRAC_BIT_RATE_LOWER_BYTE_MASK)))

   /*! The following MACRO is a utility MACRO that exists to assign the
    * maximum SUL value into the specified Codec Specific Information
    * Element variable.  This MACRO accepts as it's first parameter a
    * pointer to the Codec Specific Information Element structure. The second parameter is the actual maximum SUL value to store into the
    * Codec Specific Information Element structure.
    */
#define A2DP_ATRAC_ASSIGN_MAXIMUM_SUL(_x, _y)                                                                                                                  \
{                                                                                                                                                              \
   (((A2DP_ATRAC_Codec_Specific_Information_Element_t *)(_x))->MaximumSUL0 = (Byte_t)((Byte_t)((Word_t)(_y) >> 8) & A2DP_ATRAC_MAXIMUM_SUL_UPPER_BYTE_MASK));  \
   (((A2DP_ATRAC_Codec_Specific_Information_Element_t *)(_x))->MaximumSUL1 = (Byte_t)(((Word_t)(_y)) & A2DP_ATRAC_MAXIMUM_SUL_LOWER_BYTE_MASK));               \
}

   /*! The following MACRO is a utility MACRO that exists to read the
    * maximum SUL value from the specified Codec Specific Information
    * Element variable.  This MACRO accepts as its only parameter a
    * pointer to the Codec Specific Information Element structure.  This
    * MACRO returns the maximum SUL value.
    */
#define A2DP_ATRAC_READ_MAXIMUM_SUL(_x)                 ((Word_t)(((Word_t)(((A2DP_ATRAC_Codec_Specific_Information_Element_t *)(_x))->MaximumSUL0) << 8) | (((A2DP_ATRAC_Codec_Specific_Information_Element_t *)(_x))->MaximumSUL1)))

#endif
