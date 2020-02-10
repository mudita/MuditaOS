#pragma once

#include <stdint.h>


class ParserUtils
{

public:

    /*! Enum for the HTTP method names.
     */
    enum class Method: uint8_t
    {
        Get,
        Post,
        Put,
        Delete
    };

    /*! Enum class for the HTTP status codes.
     *  source: https://github.com/j-ulrich/http-status-codes-cpp
     */
    enum class Code
    {
        /*####### 1xx - Informational #######*/
        /* Indicates an interim response for communicating connection status
         * or request progress prior to completing the requested action and
         * sending a final response.
         */
        Continue           = 100, //!< Indicates that the initial part of a request has been received and has not yet been rejected by the server.
        SwitchingProtocols = 101, //!< Indicates that the server understands and is willing to comply with the client's request, via the Upgrade header field, for a change in the application protocol being used on this connection.
        Processing         = 102, //!< Is an interim response used to inform the client that the server has accepted the complete request, but has not yet completed it.
        EarlyHints         = 103, //!< Indicates to the client that the server is likely to send a final response with the header fields included in the informational response.

        /*####### 2xx - Successful #######*/
        /* Indicates that the client's request was successfully received,
         * understood, and accepted.
         */
        OK                          = 200, //!< Indicates that the request has succeeded.
        Created                     = 201, //!< Indicates that the request has been fulfilled and has resulted in one or more new resources being created.
        Accepted                    = 202, //!< Indicates that the request has been accepted for processing, but the processing has not been completed.
        NonAuthoritativeInformation = 203, //!< Indicates that the request was successful but the enclosed payload has been modified from that of the origin server's 200 (OK) response by a transforming proxy.
        NoContent                   = 204, //!< Indicates that the server has successfully fulfilled the request and that there is no additional content to send in the response payload body.
        ResetContent                = 205, //!< Indicates that the server has fulfilled the request and desires that the user agent reset the \"document view\", which caused the request to be sent, to its original state as received from the origin server.
        PartialContent              = 206, //!< Indicates that the server is successfully fulfilling a range request for the target resource by transferring one or more parts of the selected representation that correspond to the satisfiable ranges found in the requests's Range header field.
        MultiStatus                 = 207, //!< Provides status for multiple independent operations.
        AlreadyReported             = 208, //!< Used inside a DAV:propstat response element to avoid enumerating the internal members of multiple bindings to the same collection repeatedly. [RFC 5842]
        IMUsed                      = 226, //!< The server has fulfilled a GET request for the resource, and the response is a representation of the result of one or more instance-manipulations applied to the current instance.

        /*####### 3xx - Redirection #######*/
        /* Indicates that further action needs to be taken by the user agent
         * in order to fulfill the request.
         */
        MultipleChoices   = 300, //!< Indicates that the target resource has more than one representation, each with its own more specific identifier, and information about the alternatives is being provided so that the user (or user agent) can select a preferred representation by redirecting its request to one or more of those identifiers.
        MovedPermanently  = 301, //!< Indicates that the target resource has been assigned a new permanent URI and any future references to this resource ought to use one of the enclosed URIs.
        Found             = 302, //!< Indicates that the target resource resides temporarily under a different URI.
        SeeOther          = 303, //!< Indicates that the server is redirecting the user agent to a different resource, as indicated by a URI in the Location header field, that is intended to provide an indirect response to the original request.
        NotModified       = 304, //!< Indicates that a conditional GET request has been received and would have resulted in a 200 (OK) response if it were not for the fact that the condition has evaluated to false.
        UseProxy          = 305, //!< \deprecated \parblock Due to security concerns regarding in-band configuration of a proxy. \endparblock
                                 //!< The requested resource MUST be accessed through the proxy given by the Location field.
        TemporaryRedirect = 307, //!< Indicates that the target resource resides temporarily under a different URI and the user agent MUST NOT change the request method if it performs an automatic redirection to that URI.
        PermanentRedirect = 308, //!< The target resource has been assigned a new permanent URI and any future references to this resource outght to use one of the enclosed URIs. [...] This status code is similar to 301 Moved Permanently (Section 7.3.2 of rfc7231), except that it does not allow rewriting the request method from POST to GET.

        /*####### 4xx - Client Error #######*/
        /* Indicates that the client seems to have erred.
         */
        BadRequest                  = 400, //!< Indicates that the server cannot or will not process the request because the received syntax is invalid, nonsensical, or exceeds some limitation on what the server is willing to process.
        Unauthorized                = 401, //!< Indicates that the request has not been applied because it lacks valid authentication credentials for the target resource.
        PaymentRequired             = 402, //!< *Reserved*
        Forbidden                   = 403, //!< Indicates that the server understood the request but refuses to authorize it.
        NotFound                    = 404, //!< Indicates that the origin server did not find a current representation for the target resource or is not willing to disclose that one exists.
        MethodNotAllowed            = 405, //!< Indicates that the method specified in the request-line is known by the origin server but not supported by the target resource.
        NotAcceptable               = 406, //!< Indicates that the target resource does not have a current representation that would be acceptable to the user agent, according to the proactive negotiation header fields received in the request, and the server is unwilling to supply a default representation.
        ProxyAuthenticationRequired = 407, //!< Is similar to 401 (Unauthorized), but indicates that the client needs to authenticate itself in order to use a proxy.
        RequestTimeout              = 408, //!< Indicates that the server did not receive a complete request message within the time that it was prepared to wait.
        Conflict                    = 409, //!< Indicates that the request could not be completed due to a conflict with the current state of the resource.
        Gone                        = 410, //!< Indicates that access to the target resource is no longer available at the origin server and that this condition is likely to be permanent.
        LengthRequired              = 411, //!< Indicates that the server refuses to accept the request without a defined Content-Length.
        PreconditionFailed          = 412, //!< Indicates that one or more preconditions given in the request header fields evaluated to false when tested on the server.
        PayloadTooLarge             = 413, //!< Indicates that the server is refusing to process a request because the request payload is larger than the server is willing or able to process.
        URITooLong                  = 414, //!< Indicates that the server is refusing to service the request because the request-target is longer than the server is willing to interpret.
        UnsupportedMediaType        = 415, //!< Indicates that the origin server is refusing to service the request because the payload is in a format not supported by the target resource for this method.
        RangeNotSatisfiable         = 416, //!< Indicates that none of the ranges in the request's Range header field overlap the current extent of the selected resource or that the set of ranges requested has been rejected due to invalid ranges or an excessive request of small or overlapping ranges.
        ExpectationFailed           = 417, //!< Indicates that the expectation given in the request's Expect header field could not be met by at least one of the inbound servers.
        ImATeapot                   = 418, //!< Any attempt to brew coffee with a teapot should result in the error code 418 I'm a teapot.
        UnprocessableEntity         = 422, //!< Means the server understands the content type of the request entity (hence a 415(Unsupported Media Type) status code is inappropriate), and the syntax of the request entity is correct (thus a 400 (Bad Request) status code is inappropriate) but was unable to process the contained instructions.
        Locked                      = 423, //!< Means the source or destination resource of a method is locked.
        FailedDependency            = 424, //!< Means that the method could not be performed on the resource because the requested action depended on another action and that action failed.
        UpgradeRequired             = 426, //!< Indicates that the server refuses to perform the request using the current protocol but might be willing to do so after the client upgrades to a different protocol.
        PreconditionRequired        = 428, //!< Indicates that the origin server requires the request to be conditional.
        TooManyRequests             = 429, //!< Indicates that the user has sent too many requests in a given amount of time (\"rate limiting\").
        RequestHeaderFieldsTooLarge = 431, //!< Indicates that the server is unwilling to process the request because its header fields are too large.
        UnavailableForLegalReasons  = 451, //!< This status code indicates that the server is denying access to the resource in response to a legal demand.

        /*####### 5xx - Server Error #######*/
        /* Indicates that the server is aware that it has erred
         * or is incapable of performing the requested method.
         */
        InternalServerError           = 500, //!< Indicates that the server encountered an unexpected condition that prevented it from fulfilling the request.
        NotImplemented                = 501, //!< Indicates that the server does not support the functionality required to fulfill the request.
        BadGateway                    = 502, //!< Indicates that the server, while acting as a gateway or proxy, received an invalid response from an inbound server it accessed while attempting to fulfill the request.
        ServiceUnavailable            = 503, //!< Indicates that the server is currently unable to handle the request due to a temporary overload or scheduled maintenance, which will likely be alleviated after some delay.
        GatewayTimeout                = 504, //!< Indicates that the server, while acting as a gateway or proxy, did not receive a timely response from an upstream server it needed to access in order to complete the request.
        HTTPVersionNotSupported       = 505, //!< Indicates that the server does not support, or refuses to support, the protocol version that was used in the request message.
        VariantAlsoNegotiates         = 506, //!< Indicates that the server has an internal configuration error: the chosen variant resource is configured to engage in transparent content negotiation itself, and is therefore not a proper end point in the negotiation process.
        InsufficientStorage           = 507, //!< Means the method could not be performed on the resource because the server is unable to store the representation needed to successfully complete the request.
        LoopDetected                  = 508, //!< Indicates that the server terminated an operation because it encountered an infinite loop while processing a request with "Depth: infinity". [RFC 5842]
        NotExtended                   = 510, //!< The policy for accessing the resource has not been met in the request. [RFC 2774]
        NetworkAuthenticationRequired = 511, //!< Indicates that the client needs to authenticate to gain network access.
    };


    bool isMethodValid(uint8_t);
};
