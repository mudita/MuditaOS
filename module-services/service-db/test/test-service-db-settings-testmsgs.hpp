// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

namespace settings
{
    namespace UTMsg
    {
        class SettingsUTMsg : public ::settings::Messages::SettingsMessage
        {
          public:
            explicit SettingsUTMsg(MessageType type = MessageType::Settings)
                : ::settings::Messages::SettingsMessage(type){};
            ~SettingsUTMsg() override = default;
        };

        class UTMsgReq : public SettingsUTMsg
        {
          public:
            std::string name, value;
            UTMsgReq() = default;
            UTMsgReq(std::string name, std::string value) : name(std::move(name)), value(std::move(value))
            {}
        };
        class UTMsgCnf : public SettingsUTMsg
        {
          public:
            std::string name, value;
            UTMsgCnf() = default;
            UTMsgCnf(std::string name, std::string value) : name(std::move(name)), value(std::move(value))
            {}
        };
        class UTMsgStart : public SettingsUTMsg
        {
          public:
            UTMsgStart() = default;
        };
        class UTMsgStop : public SettingsUTMsg
        {
          public:
            UTMsgStop() = default;
        };

        class ReqRegValChg : public UTMsgReq
        {
          public:
            ReqRegValChg(std::string name, std::string value) : UTMsgReq(std::move(name), std::move(value))
            {}
        };
        class ReqUnRegValChg : public UTMsgReq
        {
          public:
            ReqUnRegValChg(std::string name, std::string value) : UTMsgReq(std::move(name), std::move(value))
            {}
        };
        class ReqSetVal : public UTMsgReq
        {
          public:
            ReqSetVal(std::string name, std::string value) : UTMsgReq(std::move(name), std::move(value))
            {}
        };
        class ReqGetVal : public UTMsgReq
        {
          public:
            ReqGetVal(std::string name, std::string value) : UTMsgReq(std::move(name), std::move(value))
            {}
        };
        class CnfRegValChg : public UTMsgCnf
        {
          public:
            CnfRegValChg() = default;
            CnfRegValChg(std::string name, std::string value) : UTMsgCnf(std::move(name), std::move(value))
            {}
        };
        class CnfUnRegValChg : public UTMsgCnf
        {
          public:
            CnfUnRegValChg() = default;
            CnfUnRegValChg(std::string name, std::string value) : UTMsgCnf(std::move(name), std::move(value))
            {}
        };
        class CnfReqSetVal : public UTMsgCnf
        {
          public:
            CnfReqSetVal() = default;
            CnfReqSetVal(std::string name, std::string value) : UTMsgCnf(std::move(name), std::move(value))
            {}
        };
        class CnfReqGetVal : public UTMsgCnf
        {
          public:
            CnfReqGetVal() = default;
            CnfReqGetVal(std::string name, std::string value) : UTMsgCnf(std::move(name), std::move(value))
            {}
        };
        class CnfValChg : public UTMsgCnf
        {
          public:
            CnfValChg() = default;
            CnfValChg(std::string name, std::string value) : UTMsgCnf(std::move(name), std::move(value))
            {}
        };

        class ReqRegProfileChg : public UTMsgReq
        {
          public:
            ReqRegProfileChg() = default;
        };
        class ReqUnRegProfileChg : public UTMsgReq
        {
          public:
            ReqUnRegProfileChg() = default;
        };
        class ReqSetCurrentProfile : public UTMsgReq
        {
          public:
            ReqSetCurrentProfile(std::string profile) : UTMsgReq(std::move(profile), "")
            {}
        };
        class ReqGetAllProfiles : public UTMsgReq
        {
          public:
            ReqGetAllProfiles() = default;
        };
        class ReqAddProfile : public UTMsgReq
        {
          public:
            ReqAddProfile(std::string profile) : UTMsgReq(std::move(profile), "")
            {}
        };
        class CnfRegProfileChg : public UTMsgCnf
        {
          public:
            CnfRegProfileChg() = default;
        };
        class CnfUnRegProfileChg : public UTMsgCnf
        {
          public:
            CnfUnRegProfileChg() = default;
        };
        class CnfSetCurrentProfile : public UTMsgCnf
        {
          public:
            CnfSetCurrentProfile(std::string profile) : UTMsgCnf(std::move(profile), "")
            {}
        };
        class CnfGetAllProfiles : public UTMsgCnf
        {
          public:
            CnfGetAllProfiles() = default;
        };
        class CnfAddProfile : public UTMsgCnf
        {
          public:
            CnfAddProfile(std::string profile) : UTMsgCnf(std::move(profile), "")
            {}
        };
        class CnfReqProfileChg : public UTMsgCnf
        {
          public:
            CnfReqProfileChg(std::string profile) : UTMsgCnf(std::move(profile), "")
            {}
        };
        class ProfileChg : public UTMsgCnf
        {
          public:
            ProfileChg(std::string profile) : UTMsgCnf(std::move(profile), "")
            {}
        };
        class ProfilesChg : public UTMsgCnf
        {
          public:
            std::list<std::string> profiles;
            ProfilesChg(std::list<std::string> profiles) : UTMsgCnf(), profiles(std::move(profiles))
            {}
        };
    }; // namespace UTMsg
} // namespace settings
