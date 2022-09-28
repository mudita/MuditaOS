// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Pdu.hpp"

#include <Utils.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <variant>

namespace pdu::constants
{

    // PDU Types
    // http://www.openmobilealliance.org/release/Browser_Protocol_Stack/V2_1-20110315-A/OMA-WAP-TS-WSP-V1_0-20110315-A.pdf,
    // p98
    enum Type : std::uint8_t
    {
        Reserved = 0x00,
        Connect,
        ConnectReply,
        Redirect,
        Reply,
        Disconnect,
        Push,
        ConfirmedPush,
        Suspend,
        Resume,
        Get = 0x40,
        Options,
        Head,
        Delete,
        Trace,
        Post = 0x60,
        Put,
        DataFragment = 0x80
    };

    // Well-known Header Field Names
    // http://www.openmobilealliance.org/release/Browser_Protocol_Stack/V2_1-20110315-A/OMA-WAP-TS-WSP-V1_0-20110315-A.pdf,
    // p103
    enum FieldName : std::uint8_t
    {
        /* v1.1 */
        Accept = 0x00,
        AcceptCharset11,  // deprecated
        AcceptEncoding11, // deprecated
        AcceptLanguage,
        AcceptRanges,
        Age,
        Allow,
        Authorization,
        CacheControl11, // deprecated
        Connection,
        ContentBase11, // deprecated
        ContentEncoding,
        ContentLanguage,
        ContentLength,
        ContentLocation,
        ContentMD5,
        ContentRange11, // deprecated
        ContentType,
        Date,
        Etag,
        Expires,
        From,
        Host,
        IfModifiedSince,
        IfMatch,
        IfNoneMatch,
        IfRange,
        IfUnmodifiedSince,
        Location,
        LastModified,
        MaxForwards,
        Pragma,
        ProxyAuthenticate,
        ProxyAuthorization,
        Public,
        Range,
        Referer,
        RetryAfter,
        Server,
        TransferEncoding,
        Upgrade,
        UserAgent,
        Vary,
        Via,
        Warning,
        WWWAuthenticate,
        ContentDisposition11, // deprecated
        /* v1.2 */
        XWapApplicationId, // X-Wap-Application-ID
        XWapContentURI,
        XWapInitiatorURI,
        AcceptApplication,
        BearerIndication,
        PushFlag,
        Profile,
        ProfileDiff,
        ProfileWarning12, // deprecated
        /* v1.3 */
        Expect13,
        TE,
        Trailer,
        AcceptCharset,
        AcceptEncoding,
        CacheControl13, // deprecated
        ContentRange,
        XWapTod,
        ContentID,
        SetCookie,
        Cookie,
        EncodingVersion,
        /* v1.4 */
        ProfileWarning,
        ContentDisposition,
        XWAPSecurity,
        CacheControl,
        /* v1.5 */
        Expect,
        XWapLocInvocation,
        XWapLocDelivery
    };

    // Basic Well-Known Content Types
    // http://www.openmobilealliance.org/release/Browser_Protocol_Stack/V2_1-20110315-A/OMA-WAP-TS-WSP-V1_0-20110315-A.pdf,
    // p105
    // http://www.wapforum.org/wina/wsp-content-type.htm
    enum ContentType : std::uint8_t
    {
        AnyAny = 0x00,                         // "*/*"
        TextAny,                               // "text/*"
        TextHtml,                              // "text/html"
        TextPlain,                             // "text/plain"
        TextXHdml,                             // "text/x-hdml"
        TextXTtml,                             // "text/x-ttml"
        TextXVCalendar,                        // "text/x-vCalendar"
        TextXVCard,                            // "text/x-vCard"
        TextVndWapWml,                         // "text/vnd.wap.wml"
        TextVndWapWmlScript,                   // "text/vnd.wap.wmlscript"
        TextVndWapWtaEvent,                    // "text/vnd.wap.wta-event"
        MultipartAny,                          // "multipart/*"
        MultipartMixed,                        // "multipart/mixed"
        MultipartFormData,                     // "multipart/form-data"
        MultipartByteranges,                   // "multipart/byteranges"
        MultipartAlternative,                  // "multipart/alternative"
        ApplicationAny,                        // "application/*"
        ApplicationJavaVm,                     // "application/java-vm"
        ApplicationXWwwFormUrlencoded,         // "application/x-www-form-urlencoded"
        ApplicationXHdmlc,                     // "application/x-hdmlc"
        ApplicationVndWapWmlc,                 // "application/vnd.wap.wmlc"
        ApplicationVndWapWmlscriptc,           // "application/vnd.wap.wmlscriptc"
        ApplicationVndWapWtaEventc,            // "application/vnd.wap.wta-eventc"
        ApplicationVndWapUaprof,               // "application/vnd.wap.uaprof"
        ApplicationVndWapWtlsCaCertificate,    // "application/vnd.wap.wtls-ca-certificate"
        ApplicationVndWapWtlsUserCertificate,  // "application/vnd.wap.wtls-user-certificate"
        ApplicationXX509CaCert,                // "application/x-x509-ca-cert"
        ApplicationXX509UserCert,              // "application/x-x509-user-cert"
        ImageAny,                              // "image/*"
        ImageGif,                              // "image/gif"
        ImageJpeg,                             // "image/jpeg"
        ImageTiff,                             // "image/tiff"
        ImagePng,                              // "image/png"
        ImageVndWapWbmp,                       // "image/vnd.wap.wbmp"
        ApplicationVndWapMultipartAny,         // "application/vnd.wap.multipart.*"
        ApplicationVndWapMultipartMixed,       // "application/vnd.wap.multipart.mixed"
        ApplicationVndWapMultipartFormData,    // "application/vnd.wap.multipart.form-data"
        ApplicationVndWapMultipartByteranges,  // "application/vnd.wap.multipart.byteranges"
        ApplicationVndWapMultipartAlternative, // "application/vnd.wap.multipart.alternative"
        ApplicationXml,                        // "application/xml"
        TextXml,                               // "text/xml"
        ApplicationVndWapWbxml,                // "application/vnd.wap.wbxml"
        ApplicationXX968CrossCert,             // "application/x-x968-cross-cert"
        ApplicationXX968CaCert,                // "application/x-x968-ca-cert"
        ApplicationXX968UserCert,              // "application/x-x968-user-cert"
        TextVndWapSi,                          // "text/vnd.wap.si"
        ApplicationVndWapSic,                  // "application/vnd.wap.sic"
        TextVndWapSl,                          // "text/vnd.wap.sl"
        ApplicationVndWapSlc,                  // "application/vnd.wap.slc"
        TextVndWapCo,                          // "text/vnd.wap.co"
        ApplicationVndWapCoc,                  // "application/vnd.wap.coc"
        ApplicationVndWapRelated,              // "application/vnd.wap.multipart.related"
        ApplicationVndWapSia,                  // "application/vnd.wap.sia"
        TextVndWapConnectivityXml,             // "text/vnd.wap.connectivity-xml"
        ApplicationVndWapConnectivityWbxml,    // "application/vnd.wap.connectivity-wbxml"
        ApplicationPkcs7Mime,                  // "application/pkcs7-mime"
        ApplicationVndWapHashedCertificate,    // "application/vnd.wap.hashed-certificate"
        ApplicationVndWapSignedCertificate,    // "application/vnd.wap.signed-certificate"
        ApplicationVndWapCertResponse,         // "application/vnd.wap.cert-response"
        ApplicationXhtmlXml,                   // "application/xhtml+xml"
        ApplicationWmlXml,                     // "application/wml+xml"
        TextCss,                               // "text/css"
        ApplicationVndWapMmsMessage,           // "application/vnd.wap.mms-message"
        ApplicationVndWapRolloverCertificate,  // "application/vnd.wap.rollover-certificate"
        ApplicationVndWapLoccWbxml,            // "application/vnd.wap.locc+wbxml"
        ApplicationVndWapLocXml,               // "application/vnd.wap.loc+xml"
        ApplicationVndSyncmlDmWbxml,           // "application/vnd.syncml.dm+wbxml"
        ApplicationVndSyncmlDmXml,             // "application/vnd.syncml.dm+xml"
        ApplicationVndSyncmlNotification,      // "application/vnd.syncml.notification"
        ApplicationVndWapXhtmlXml,             // "application/vnd.wap.xhtml+xml"
        ApplicationVndWvXspCir,                // "application/vnd.wv.csp.cir"
        ApplicationVndOmaDdXml,                // "application/vnd.oma.dd+xml"
        ApplicationVndOmaDrmMessage,           // "application/vnd.oma.drm.message"
        ApplicationVndOmaDrmContent,           // "application/vnd.oma.drm.content"
        ApplicationVndOmaDrmRightsXml,         // "application/vnd.oma.drm.rights+xml"
        ApplicationVndOmaDrmRightsWbxml,       // "application/vnd.oma.drm.rights+wbxml"
    };

    enum PushApplicationId : std::uint8_t
    {
        XWapApplicationAny = 0x00, // x-wap-application:*
        XWapApplicationPushSia,    // x-wap-application:push.sia
        XWapApplicationWmlUa,      // x-wap-application:wml.ua
        XWapApplicationWtaUa,      // x-wap-application:wta.ua
        XWapApplicationMmsUa,      // x-wap-application:mms.ua
        XWapApplicationPushSyncml, // x-wap-application:push.syncml
        XWapApplicationLocUa,      // x-wap-application:loc.ua
        XWapApplicationSyncmlDm,   // x-wap-application:syncml.dm
        XWapApplicationDrmUa,      // x-wap-application:drm.ua
        XWapApplicationEmnUa,      // x-wap-application:emn.ua
        XWapApplicationWvUa,       // x-wap-application:wv.ua
    };

    // MMS
    // https://www.openmobilealliance.org/release/MMS/V1_3-20110913-A/OMA-TS-MMS_ENC-V1_3-20110913-A.pdf, p64
    enum MmsFieldName : std::uint8_t
    {
        MmsBcc = 0x01,
        MmsCc,
        XMmsContentLocation,
        MmsContentType,
        MmsDate,
        XMmsDeliveryReport,
        XMmsDeliveryTime,
        XMmsExpiry,
        MmsFrom,
        XMmsMessageClass,
        MmsMessageID,
        XMmsMessageType,
        XMmsMMSVersion,
        XMmsMessageSize,
        XMmsPriority,
        XMmsReadReport,
        XMmsReportAllowed,
        XMmsResponseStatus,
        XMmsResponseText,
        XMmsSenderVisibility,
        XMmsStatus,
        MmsSubject,
        MmsTo,
        XMmsTransactionId,
        XMmsRetrieveStatus,
        XMmsRetrieveText,
        XMmsReadStatus,
        XMmsReplyCharging,
        XMmsReplyChargingDeadline,
        XMmsReplyChargingID,
        XMmsReplyChargingSize,
        XMmsPreviouslySentBy,
        XMmsPreviouslySentDate,
        XMmsStore,
        XMmsMMState,
        XMmsMMFlags,
        XMmsStoreStatus,
        XMmsStoreStatusText,
        XMmsStored,
        XMmsAttributes,
        XMmsTotals,
        XMmsMboxTotals,
        XMmsQuotas,
        XMmsMboxQuotas,
        XMmsMessageCount,
        MmsContent,
        XMmsStart,
        MmsAdditionalheaders,
        XMmsDistributionIndicator,
        XMmsElementDescriptor,
        XMmsLimit,
        XMmsRecommendedRetrievalMode,
        XMmsRecommendedRetrievalModeText,
        XMmsStatusText,
        XMmsApplicID,
        XMmsReplyApplicID,
        XMmsAuxApplicInfo,
        XMmsContentClass,
        XMmsDRMContent,
        XMmsAdaptationAllowed,
        XMmsReplaceID,
        XMmsCancelID,
        XMmsCancelStatus
    };

    enum MmsMessageType : std::uint8_t
    {
        MmsMSendReq = 0x00,  // m-send-req
        MmsMSendConf,        // m-send-conf
        MmsMNotificationInd, // m-notification-ind
        MmsMNotifyrespInd,   // m-notifyresp-ind
        MmsMRetrieveConf,    // m-retrieve-conf
        MmsMAcknowledgeInd,  // m-acknowledge-ind
        MmsMDeliveryInd,     // m-delivery-ind
        MmsMReadRecInd,      // m-read-rec-ind
        MmsMReadOrigInd,     // m-read-orig-ind
        MmsMForwardReq,      // m-forward-req
        MmsMForwardComf,     // m-forward-conf
        MmsMMboxStoreReq,    // m-mbox-store-req
        MmsMMboxStoreConf,   // m-mbox-store-conf
        MmsMMboxViewReq,     // m-mbox-view-req
        MmsMMboxViewConf,    // m-mbox-view-conf
        MmsMMboxUploadReq,   // m-mbox-upload-req
        MmsMMboxUploadConf,  // m-mbox-upload-conf
        MmsMMboxDeleteReq,   // m-mbox-delete-req
        MmsMMboxDeleteConf,  // m-mbox-delete-conf
        MmsMMboxDescr,       // m-mbox-descr
        MmsMDeleteReq,       // m-delete-req
        MmsMDeleteConf,      // m-delete-conf
        MmsMCancelReq,       // m-cancel-req
        MmsMCancelConf       // m-cancel-conf
    };

} // namespace pdu::constants

namespace pdu
{

    // A range of characters
    struct CharRange
    {
        using Iterator = std::string::const_iterator;

        CharRange() : m_begin(), m_end()
        {}

        CharRange(Iterator begin, Iterator end) : m_begin(begin), m_end(end)
        {
            assert(m_begin <= m_end);
        }

        Iterator begin() const
        {
            return m_begin;
        }
        Iterator end() const
        {
            return m_end;
        }
        bool empty() const
        {
            return m_begin == m_end;
        }
        std::uint32_t size() const
        {
            return m_end - m_begin;
        }

        std::string str() const
        {
            return std::string(m_begin, m_end);
        }

        friend bool operator==(CharRange const &rng, std::string const &str)
        {
            return rng.size() == str.size() && std::equal(rng.m_begin, rng.m_end, str.begin());
        }

        friend bool operator==(std::string const &str, CharRange const &rng)
        {
            return rng == str;
        }

        friend bool operator==(CharRange const &rng, const char *cstr)
        {
            return cstr[rng.size()] == '\0' && std::equal(rng.m_begin, rng.m_end, cstr);
        }

        friend bool operator==(const char *cstr, CharRange const &rng)
        {
            return rng == cstr;
        }

      protected:
        Iterator m_begin;
        Iterator m_end;
    };

    // A buffer representing input octets
    struct Octets : CharRange
    {
        Octets() = default;

        Octets(Iterator begin, Iterator end) : CharRange(begin, end)
        {}

        bool peek(std::uint8_t &result) const
        {
            if (m_begin != m_end) {
                result = *m_begin;
                return true;
            }
            return false;
        }

        bool next(std::uint8_t &result)
        {
            if (m_begin != m_end) {
                result = *m_begin++;
                return true;
            }
            return false;
        }

        void ignore()
        {
            if (m_begin != m_end) {
                ++m_begin;
            }
        }

        void ignore(std::uint32_t n)
        {
            m_begin = advancedByN(n);
        }

        Octets subOctets(std::uint32_t n)
        {
            Iterator it = advancedByN(n);
            Octets result(m_begin, it);
            m_begin = it;
            return result;
        }

      private:
        Iterator advancedByN(std::uint32_t n) const
        {
            return n < size() ? m_begin + n : m_end;
        }
    };

    // A range representing text, excluding last \0 character
    struct Text : CharRange
    {
        Text() = default;

        Text(Iterator begin, Iterator end) : CharRange(begin, end)
        {}
    };

    // http://www.openmobilealliance.org/release/Browser_Protocol_Stack/V2_1-20110315-A/OMA-WAP-TS-WSP-V1_0-20110315-A.pdf,
    // p79
    using DefaultValue = std::variant<std::uint8_t, Text, Octets>;
    using ContentValue = DefaultValue;
    using HeaderName   = std::variant<std::uint8_t, Text>;
    using HeaderValue  = DefaultValue;

    // Representing known header names and header values
    // Intended to be used with string literals
    // If passing an object is needed replace const char*
    //   with std::string or std::string_view
    struct KnownHeader
    {
        std::uint8_t id;
        const char *idCStr;
    };

    template <typename NameOrValue>
    inline bool isKnown(NameOrValue const &nameOrValue, KnownHeader const &knownHeader)
    {
        return std::visit(
            [&](auto const &val) {
                using Val = utils::remove_cref_t<decltype(val)>;
                if constexpr (std::is_same_v<Val, std::uint8_t>) {
                    return val == knownHeader.id;
                }
                if constexpr (std::is_same_v<Val, Text>) {
                    return val == knownHeader.idCStr;
                }
                return false;
            },
            nameOrValue);
    }

    struct Parser
    {
        static constexpr std::uint8_t Zero          = 0;
        static constexpr std::uint8_t EndOfString   = 0;
        static constexpr std::uint8_t UintvarEscape = 31;
        static constexpr std::uint8_t TextMin       = 32;
        static constexpr std::uint8_t TextMax       = 127;
        static constexpr std::uint8_t TextEscape    = 127;
        static constexpr std::uint8_t PageShift     = 127;

        static constexpr std::uint32_t StringMaxLength = 1024;

        // http://www.openmobilealliance.org/release/Browser_Protocol_Stack/V2_1-20110315-A/OMA-WAP-TS-WSP-V1_0-20110315-A.pdf,
        // s63
        static bool parseUintvar(Octets &octets, std::uint32_t &val)
        {
            val             = 0x00000000;
            bool isEndFound = false;
            for (int i = 0; i < 5; ++i) {
                std::uint8_t octet = 0x00;
                if (!octets.next(octet)) {
                    return false;
                }
                val = (val << 7) | octet;
                if ((octet & 0x80) == 0x00) {
                    isEndFound = true;
                    break;
                }
            }
            return isEndFound;
        }

        // http://www.openmobilealliance.org/release/Browser_Protocol_Stack/V2_1-20110315-A/OMA-WAP-TS-WSP-V1_0-20110315-A.pdf,
        // s63
        static bool parseUint(Octets &octets, std::uint32_t &val, std::uint32_t length)
        {
            if (length > 4) {
                return false;
            }
            val = 0x00000000;
            for (std::uint32_t i = 0; i < length; ++i) {
                std::uint8_t octet = 0x00;
                if (!octets.next(octet)) {
                    return false;
                }
                val = (val << 8) | octet;
            }
            return true;
        }

        // http://www.openmobilealliance.org/release/Browser_Protocol_Stack/V2_1-20110315-A/OMA-WAP-TS-WSP-V1_0-20110315-A.pdf,
        // p79
        static bool parseTextN(Octets &octets, Text &text, std::uint32_t length)
        {
            if (length > StringMaxLength) {
                return false;
            }
            if (length == 0) {
                text = Text(octets.begin(), octets.begin());
                return true;
            }
            // Avoid including EoS character
            Octets::Iterator const begin = octets.begin();
            octets.ignore(length - 1);
            Octets::Iterator const end = octets.begin();
            std::uint8_t octet         = 0x00;
            if (!octets.next(octet)) {
                return false;
            }
            if (octet == EndOfString) {
                text = Text(begin, end);
            }
            else {
                text = Text(begin, octets.begin());
            }
            return true;
        }

        // http://www.openmobilealliance.org/release/Browser_Protocol_Stack/V2_1-20110315-A/OMA-WAP-TS-WSP-V1_0-20110315-A.pdf,
        // p79
        static bool parseText(Octets &octets, Text &text)
        {
            Octets::Iterator begin = octets.begin();
            std::uint8_t octet     = 0x00;
            if (!octets.next(octet)) {
                return false;
            }
            if (octet == TextEscape) {
                begin = octets.begin();
            }
            // Avoid including EoS character
            Octets::Iterator end = begin;
            while (octet != EndOfString) {
                end = octets.begin();
                if (!octets.next(octet)) {
                    return false;
                }
            }
            text = Text(begin, end);
            return true;
        }

        // http://www.openmobilealliance.org/release/Browser_Protocol_Stack/V2_1-20110315-A/OMA-WAP-TS-WSP-V1_0-20110315-A.pdf,
        // p79
        // Value may be:
        // - decoded short integer
        // - text
        // - data as parsable octets range
        struct DefaultPolicy
        {
            static bool data(Octets &octets, std::uint8_t lengthOctet, DefaultValue &value)
            {
                octets.ignore();
                std::uint32_t length = lengthOctet;
                if (lengthOctet == UintvarEscape) {
                    if (!parseUintvar(octets, length)) {
                        return false;
                    }
                }
                if (octets.size() < length) {
                    return false;
                }
                value = octets.subOctets(length);
                return true;
            }

            template <typename Value>
            static bool text(Octets &octets, std::uint8_t charOctet, Value &value)
            {
                // Do not ignore the first character, it's part of the text
                Text text(octets.begin(), octets.begin());
                if (!parseText(octets, text)) {
                    return false;
                }
                value = text;
                return true;
            }

            static bool shortInteger(Octets &octets, std::uint8_t uintOctet, DefaultValue &value)
            {
                octets.ignore();
                value = std::uint8_t(uintOctet & 0x7F);
                return true;
            }
        };

        // http://www.openmobilealliance.org/release/Browser_Protocol_Stack/V2_1-20110315-A/OMA-WAP-TS-WSP-V1_0-20110315-A.pdf,
        // p81
        // Name may be:
        // - not decoded short integer well-known field name [128, 255]
        // - text token starting from octet [32, 126]
        // - shortcut page shift delimiter [0, 31]
        // - page shift delimter 127 (then header value is one octet page id and
        //   shouldn't be parsed with parseHeaderValue)
        struct HeaderNamePolicy
        {
            static bool data(Octets &octets, std::uint8_t lengthOctet, HeaderName &value)
            {
                octets.ignore();
                value = lengthOctet;
                return true;
            }

            static bool text(Octets &octets, std::uint8_t charOctet, HeaderName &value)
            {
                if (charOctet == PageShift) {
                    octets.ignore();
                    value = charOctet;
                    return true;
                }
                else {
                    return DefaultPolicy::text(octets, charOctet, value);
                }
            }

            static bool shortInteger(Octets &octets, std::uint8_t uintOctet, HeaderName &value)
            {
                octets.ignore();
                value = uintOctet;
                return true;
            }
        };

        // http://www.openmobilealliance.org/release/Browser_Protocol_Stack/V2_1-20110315-A/OMA-WAP-TS-WSP-V1_0-20110315-A.pdf,
        // p81
        struct IgnoreHeaderValuePolicy
        {
            template <typename Value>
            static bool data(Octets &octets, std::uint8_t lengthOctet, Value &value)
            {
                octets.ignore();
                std::uint32_t length = lengthOctet;
                if (lengthOctet == UintvarEscape) {
                    if (!parseUintvar(octets, length)) {
                        return false;
                    }
                }
                if (octets.size() < length) {
                    return false;
                }
                octets.ignore(length);
                return true;
            }

            template <typename Value>
            static bool text(Octets &octets, std::uint8_t charOctet, Value &value)
            {
                octets.ignore();
                while (charOctet != EndOfString) {
                    if (!octets.next(charOctet)) {
                        return false;
                    }
                }
                return true;
            }

            template <typename Value>
            static bool shortInteger(Octets &octets, std::uint8_t uintOctet, Value &value)
            {
                octets.ignore();
                return true;
            }
        };

        // http://www.openmobilealliance.org/release/Browser_Protocol_Stack/V2_1-20110315-A/OMA-WAP-TS-WSP-V1_0-20110315-A.pdf,
        // p79
        template <typename Policy, typename Value>
        static bool parseGeneric(Octets &octets, Value &value)
        {
            std::uint8_t octet = 0x00;
            if (!octets.peek(octet)) {
                return false;
            }

            if (octet < TextMin) {
                return Policy::data(octets, octet, value);
            }
            else if (octet <= TextMax) {
                return Policy::text(octets, octet, value);
            }
            else { // octet > TextMax
                return Policy::shortInteger(octets, octet, value);
            }
        }

        // Value may be:
        // - decoded short integer well-known field name
        // - long integer well-known field name
        // - text
        // - integer/text type mashed together with optional parameter
        static bool parseContentType(Octets &octets, ContentValue &value)
        {
            return parseGeneric<DefaultPolicy>(octets, value);
        }

        // http://www.openmobilealliance.org/release/Browser_Protocol_Stack/V2_1-20110315-A/OMA-WAP-TS-WSP-V1_0-20110315-A.pdf,
        // p81
        template <typename PageShiftPred, typename NamePred, typename ValuePred>
        static bool parseHeader(Octets &octets, PageShiftPred pageShiftPred, NamePred namePred, ValuePred valuePred)
        {
            HeaderName name = std::uint8_t(0);
            if (!parseGeneric<HeaderNamePolicy>(octets, name)) {
                return false;
            }

            // Distinguish between page shift and short integer
            std::uint32_t nameUint = 0;
            std::visit(
                [&](auto const &n) {
                    using n_t = utils::remove_cref_t<decltype(n)>;
                    if constexpr (std::is_same_v<n_t, std::uint8_t>) {
                        nameUint = n;
                    }
                },
                name);
            if (nameUint > 0) {
                if (nameUint < PageShift) {
                    pageShiftPred(static_cast<std::uint8_t>(nameUint));
                    return true;
                }
                else if (nameUint == PageShift) {
                    std::uint8_t octet = 0;
                    if (!octets.next(octet)) {
                        return false;
                    }
                    pageShiftPred(octet);
                    return true;
                }
                else if (nameUint > TextMax && nameUint <= 0xFF) {
                    name = std::uint8_t(nameUint & 0x7F); // decode short integer
                }
                else if (nameUint > 0xFF) {
                    return false; // This should not happen
                }
            }

            if (!namePred(std::move(name))) {
                int foo;
                return parseGeneric<IgnoreHeaderValuePolicy>(octets, foo);
            }

            HeaderValue value = std::uint8_t(0);
            if (!parseGeneric<DefaultPolicy>(octets, value)) {
                return false;
            }

            valuePred(std::move(value));
            return true;
        }

        template <std::size_t N>
        static bool parseHeaders(Octets &octets,
                                 std::array<KnownHeader, N> const &headerNames,
                                 std::array<HeaderValue, N> &headerValues)
        {
            std::array<bool, N> headersFound;
            headersFound.fill(false);
            std::size_t foundCount = 0;

            while (!octets.empty()) {
                std::size_t nameId      = N;
                HeaderValue headerValue = std::uint8_t(0);
                bool pageShift          = false;

                auto pageShiftPred = [&](std::uint8_t page) { pageShift = true; };
                auto namePred      = [&](auto &&name) {
                    for (std::size_t i = 0; i < N; ++i) {
                        if (headersFound[i]) {
                            // Header already found
                            continue;
                        }
                        else if (isKnown(name, headerNames[i])) {
                            headersFound[i] = true;
                            ++foundCount;
                            nameId = i;
                            return true;
                        }
                    }
                    return false;
                };
                auto valuePred = [&](auto &&value) {
                    if (nameId < N) {
                        headerValues[nameId] = std::move(value);
                    }
                };

                if (!Parser::parseHeader(octets, pageShiftPred, namePred, valuePred)) {
                    return false;
                }

                // NOTE: Page shifts are currently not supported
                if (pageShift) {
                    return false;
                }

                if (foundCount == N) {
                    break;
                }
            }

            return true;
        }
    };

    // http://www.openmobilealliance.org/release/Browser_Protocol_Stack/V2_1-20110315-A/OMA-WAP-TS-WSP-V1_0-20110315-A.pdf,
    // p64
    struct CommonFields
    {
        std::uint8_t id   = 0x00; // set in connectionless mode
        std::uint8_t type = 0x00;
    };

    // http://www.openmobilealliance.org/release/Browser_Protocol_Stack/V2_1-20110315-A/OMA-WAP-TS-WSP-V1_0-20110315-A.pdf,
    // p70
    struct PushFields
    {
        std::uint32_t headersLen  = 0;
        ContentValue contentType  = std::uint8_t{0};
        HeaderValue applicationId = std::uint8_t{0};
    };

    // https://www.openmobilealliance.org/release/MMS/V1_3-20110913-A/OMA-TS-MMS_ENC-V1_3-20110913-A.pdf, p17
    struct MmsCommonFields
    {
        HeaderValue messageType = std::uint8_t{0};
    };

    // https://www.openmobilealliance.org/release/MMS/V1_3-20110913-A/OMA-TS-MMS_ENC-V1_3-20110913-A.pdf, p21, p55, p53
    struct MmsNotificationFields
    {
        HeaderValue fromAddress     = std::uint8_t{0};
        HeaderValue contentLocation = std::uint8_t{0};
    };

    enum ConnectionMode
    {
        Connectionless,
        Connectionmode
    };

    std::optional<MmsNotification> parse(std::string const &message, ConnectionMode connectionMode)
    {
        Octets octets(message.begin(), message.end());

        // http://www.openmobilealliance.org/release/Browser_Protocol_Stack/V2_1-20110315-A/OMA-WAP-TS-WSP-V1_0-20110315-A.pdf,
        // p64
        CommonFields commonFields;
        if (connectionMode == Connectionless && !octets.next(commonFields.id)) {
            return std::nullopt;
        }
        if (!octets.next(commonFields.type)) {
            return std::nullopt;
        }

        if (commonFields.type != constants::Push) {
            return std::nullopt; // unsupported PDU type
        }

        PushFields pushFields;
        if (!Parser::parseUintvar(octets, pushFields.headersLen)) {
            return std::nullopt;
        }
        Octets headersOctets = octets.subOctets(pushFields.headersLen);
        if (!Parser::parseContentType(headersOctets, pushFields.contentType)) {
            return std::nullopt;
        }

        if (!isKnown(pushFields.contentType,
                     {constants::ApplicationVndWapMmsMessage, "application/vnd.wap.mms-message"})) {
            return std::nullopt; // unsupported Push type
        }

        // Push headers
        {
            std::array<HeaderValue, 1> pushValues;
            if (!Parser::parseHeaders(
                    headersOctets,
                    std::array<KnownHeader, 1>{{{constants::XWapApplicationId, "X-Wap-Application-ID"}}},
                    pushValues)) {
                return std::nullopt;
            }
            pushFields.applicationId = std::move(pushValues[0]);
        }

        if (!isKnown(pushFields.applicationId, {constants::XWapApplicationMmsUa, "x-wap-application:mms.ua"})) {
            return std::nullopt; // unsupported WAP Push app
        }

        // MMS headers
        // https://www.openmobilealliance.org/release/MMS/V1_3-20110913-A/OMA-TS-MMS_ENC-V1_3-20110913-A.pdf, p21
        MmsCommonFields mmsCommonFields;
        MmsNotificationFields mmsNotificationFields;
        {
            std::array<HeaderValue, 3> mmsValues;
            if (!Parser::parseHeaders(
                    octets,
                    std::array<KnownHeader, 3>{{{constants::XMmsMessageType, "X-Mms-Message-Type"},
                                                {constants::MmsFrom, "From"},
                                                {constants::XMmsContentLocation, "X-Mms-Content-Location"}}},
                    mmsValues)) {
                return std::nullopt;
            }
            mmsCommonFields.messageType           = std::move(mmsValues[0]);
            mmsNotificationFields.fromAddress     = std::move(mmsValues[1]);
            mmsNotificationFields.contentLocation = std::move(mmsValues[2]);
        }

        // https://www.openmobilealliance.org/release/MMS/V1_3-20110913-A/OMA-TS-MMS_ENC-V1_3-20110913-A.pdf, p56
        if (!isKnown(mmsCommonFields.messageType, {constants::MmsMNotificationInd, "m-notification-ind"})) {
            return std::nullopt; // unsuported MMS type
        }

        // https://www.openmobilealliance.org/release/MMS/V1_3-20110913-A/OMA-TS-MMS_ENC-V1_3-20110913-A.pdf, p55
        Text address;
        if (std::holds_alternative<Octets>(mmsNotificationFields.fromAddress)) {
            Octets octets      = std::get<Octets>(mmsNotificationFields.fromAddress);
            std::uint8_t token = 0;
            if (!octets.next(token)) {
                return std::nullopt;
            }
            constexpr std::uint8_t AddressPresentToken = 128;
            if (token == AddressPresentToken) {
                if (!Parser::parseTextN(octets, address, octets.size())) {
                    return std::nullopt;
                }
            }
        }

        // https://www.openmobilealliance.org/release/MMS/V1_3-20110913-A/OMA-TS-MMS_ENC-V1_3-20110913-A.pdf, p53
        Text location;
        if (std::holds_alternative<Text>(mmsNotificationFields.contentLocation)) {
            location = std::get<Text>(mmsNotificationFields.contentLocation);
        }

        return std::optional<MmsNotification>(MmsNotification(address.str(), location.str()));
    }

    std::optional<MmsNotification> parse(std::string const &message)
    {
        return parse(message, Connectionless);
    }

} // namespace pdu
