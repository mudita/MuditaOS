// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <module-cellular/test/mock/AtCommon_channel.hpp>
#include <module-services/service-cellular/src/VolteHandler.hpp>
#include <queue>
#include <regex>
#include <utility>

using namespace at;
using namespace cellular::service;

using CommandAndResponse = std::pair<AT, Result>;

namespace
{
    struct DummyModemResponseParser
    {
        template <typename... Args>
        bool operator()(Args &&...args) const
        {
            FAIL("test depends on parser, but shouldn't");
            return false;
        }
    };

    struct QcfgImsThrowingParser
    {
        bool operator()(QcfgImsResult const &, bool) const
        {
            throw "IMS";
        }
    };

    template <bool ReturnValue>
    struct QcfgImsDummyParser
    {
        bool operator()(QcfgImsResult const &, bool) const
        {
            return ReturnValue;
        }
    };

    std::string extractCommandMnemonic(std::string const &input)
    {
        std::regex const extractor("AT\\+([A-Z_]+).*");
        std::smatch pieces;
        if (!std::regex_match(input, pieces, extractor) || pieces.size() != 2) {
            FAIL("bad command string given");
            return "";
        }

        return pieces[1].str();
    }

    template <typename Container, typename... Item>
    void push(Container &container, Item &&...items)
    {
        (container.push(items), ...);
    }
} // namespace

namespace at
{
    struct DummyOkResult : Result
    {
        DummyOkResult() : Result(Result::Code::OK, {})
        {}
    };

    struct DummyErrorResult : Result
    {
        DummyErrorResult() : Result(Result::Code::ERROR, {})
        {}
    };

    struct CommandSequentialChannel : ChannelMock
    {
        CommandSequentialChannel(std::queue<CommandAndResponse> &&chain) : chain_{chain}
        {}

        Result cmd(Cmd const &at)
        {
            if (chain_.empty()) {
                FAIL("all commands for channel already done");
            }

            auto const &currentCommand = chain_.front();
            auto currentCommandMnemonic =
                (currentCommand.first == AT::QCFG_IMS) ? "QCFG" : magic_enum::enum_name(currentCommand.first);
            if (currentCommandMnemonic != extractCommandMnemonic(at.getCmd())) {
                FAIL("unexpected command in sequence");
            }

            auto ret = currentCommand.second;
            chain_.pop();
            return ret;
        }

      protected:
        std::queue<CommandAndResponse> chain_;
    };
} // namespace at

TEST_CASE("VoLTE handler test")
{
    SECTION("ThrowsAboutVoiceDomainFailure_When_TryingToEnableVolte_And_VoiceDomainCommandRespondsNOK")
    {
        std::queue<CommandAndResponse> chain;
        push(chain, std::make_pair(AT::QNVFW, DummyErrorResult{}));
        CommandSequentialChannel channel(std::move(chain));

        VolteHandler<CommandSequentialChannel, DummyModemResponseParser> handler;

        REQUIRE_THROWS_WITH(handler.switchVolte(channel, true), Catch::Contains("voice domain"));
    }

    SECTION("ThrowsAboutMbnAutoselectFailure_When_TryingToEnableVolte_And_AutoselectCommandRespondsNOK")
    {
        std::queue<CommandAndResponse> chain;
        push(chain,
             std::make_pair(AT::QNVFW, DummyOkResult{}),
             std::make_pair(AT::QNVFW, DummyOkResult{}),
             std::make_pair(AT::QMBNCFG, DummyErrorResult{}));
        CommandSequentialChannel channel(std::move(chain));

        VolteHandler<CommandSequentialChannel, DummyModemResponseParser> handler;

        REQUIRE_THROWS_WITH(handler.switchVolte(channel, true), Catch::Contains("MBN"));
    }

    SECTION("ThrowsAboutImsCheckingFailure_When_TryingToEnableVolte_And_ImsConfigurationQueryRespondsNOK")
    {
        std::queue<CommandAndResponse> chain;
        push(chain,
             std::make_pair(AT::QNVFW, DummyOkResult{}),
             std::make_pair(AT::QNVFW, DummyOkResult{}),
             std::make_pair(AT::QMBNCFG, DummyOkResult{}),
             std::make_pair(AT::QCFG_IMS, DummyErrorResult{}));
        CommandSequentialChannel channel(std::move(chain));

        VolteHandler<CommandSequentialChannel, DummyModemResponseParser> handler;

        REQUIRE_THROWS_WITH(handler.switchVolte(channel, true), Catch::Contains("IMS"));
    }

    SECTION("ThrowsAboutImsCheckingFailure_When_TryingToEnableVolte_And_CantParseImsConfigurationQuery")
    {
        std::queue<CommandAndResponse> chain;
        push(chain,
             std::make_pair(AT::QNVFW, DummyOkResult{}),
             std::make_pair(AT::QNVFW, DummyOkResult{}),
             std::make_pair(AT::QMBNCFG, DummyOkResult{}),
             std::make_pair(AT::QCFG_IMS, DummyOkResult{}));
        CommandSequentialChannel channel(std::move(chain));

        VolteHandler<CommandSequentialChannel, QcfgImsThrowingParser> handler;

        REQUIRE_THROWS_WITH(handler.switchVolte(channel, true), Catch::Contains("IMS"));
    }

    SECTION("ReturnsOk_When_TryingToEnableVolte_And_AlreadyEnabled")
    {
        std::queue<CommandAndResponse> chain;
        push(chain,
             std::make_pair(AT::QNVFW, DummyOkResult{}),
             std::make_pair(AT::QNVFW, DummyOkResult{}),
             std::make_pair(AT::QMBNCFG, DummyOkResult{}),
             std::make_pair(AT::QCFG_IMS, DummyOkResult{}));
        CommandSequentialChannel channel(std::move(chain));

        VolteHandler<CommandSequentialChannel, QcfgImsDummyParser<true>> handler;

        REQUIRE(handler.switchVolte(channel, true));
    }

    SECTION("ReturnsOk_When_TryingToDisableVolte_And_AlreadyDisabled")
    {
        std::queue<CommandAndResponse> chain;
        push(chain, std::make_pair(AT::QNVFW, DummyOkResult{}), std::make_pair(AT::QCFG_IMS, DummyOkResult{}));
        CommandSequentialChannel channel(std::move(chain));

        VolteHandler<CommandSequentialChannel, QcfgImsDummyParser<true>> handler;

        REQUIRE(handler.switchVolte(channel, false));
    }

    SECTION("ReturnsFalse_When_TryingToDisableVolte_And_WasEnabledBefore")
    {
        std::queue<CommandAndResponse> chain;
        push(chain,
             std::make_pair(AT::QNVFW, DummyOkResult{}),
             std::make_pair(AT::QCFG_IMS, DummyOkResult{}),
             std::make_pair(AT::QCFG_IMS, DummyOkResult{}));
        CommandSequentialChannel channel(std::move(chain));

        VolteHandler<CommandSequentialChannel, QcfgImsDummyParser<false>> handler;

        REQUIRE_FALSE(handler.switchVolte(channel, false));
    }

    SECTION("ReturnsFalse_When_TryingToEnableVolte_And_WasEnabledBefore")
    {
        std::queue<CommandAndResponse> chain;
        push(chain,
             std::make_pair(AT::QNVFW, DummyOkResult{}),
             std::make_pair(AT::QCFG_IMS, DummyOkResult{}),
             std::make_pair(AT::QCFG_IMS, DummyOkResult{}));
        CommandSequentialChannel channel(std::move(chain));

        VolteHandler<CommandSequentialChannel, QcfgImsDummyParser<false>> handler;

        REQUIRE_FALSE(handler.switchVolte(channel, false));
    }

    SECTION("ThrowsAboutImsFailure_When_DisablingVolte_And_ImsSteeringCommandFailed")
    {
        std::queue<CommandAndResponse> chain;
        push(chain,
             std::make_pair(AT::QNVFW, DummyOkResult{}),
             std::make_pair(AT::QCFG_IMS, DummyOkResult{}),
             std::make_pair(AT::QCFG_IMS, DummyErrorResult{}));
        CommandSequentialChannel channel(std::move(chain));

        VolteHandler<CommandSequentialChannel, QcfgImsDummyParser<false>> handler;

        REQUIRE_THROWS_WITH(handler.switchVolte(channel, false), Catch::Contains("fail") && Catch::Contains("IMS"));
    }

    SECTION("ThrowsAboutImsFailure_When_EnablingVolte_And_ImsSteeringCommandFailed")
    {
        std::queue<CommandAndResponse> chain;
        push(chain,
             std::make_pair(AT::QNVFW, DummyOkResult{}),
             std::make_pair(AT::QNVFW, DummyOkResult{}),
             std::make_pair(AT::QMBNCFG, DummyOkResult{}),
             std::make_pair(AT::QCFG_IMS, DummyOkResult{}),
             std::make_pair(AT::QCFG_IMS, DummyErrorResult{}));
        CommandSequentialChannel channel(std::move(chain));

        VolteHandler<CommandSequentialChannel, QcfgImsDummyParser<false>> handler;

        REQUIRE_THROWS_WITH(handler.switchVolte(channel, true), Catch::Contains("fail") && Catch::Contains("IMS"));
    }
}
