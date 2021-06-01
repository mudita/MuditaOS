// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <purefs/fs/notifier.hpp>
#include <purefs/fs/inotify_message.hpp>

namespace purefs::fs
{

    struct notifier_mock final : public internal::notifier
    {
        virtual ~notifier_mock() = default;
        auto send_notification(std::shared_ptr<sys::Service> svc,
                               inotify_flags flags,
                               std::string_view name,
                               std::string_view name_dst) const -> void override
        {
            messages.emplace_back(flags, name, name_dst);
            xsvc.push_back(svc.get());
        }
        mutable std::vector<message::inotify> messages;
        mutable std::vector<sys::Service *> xsvc;
    };
} // namespace purefs::fs

namespace sys
{
    class Service
    {};
} // namespace sys

TEST_CASE("Notifier base class test")
{
    using namespace purefs::fs;
    auto svc = std::make_shared<sys::Service>();
    notifier_mock notify;
    notify.register_path("/sys/ala",
                         svc,
                         inotify_flags::open | inotify_flags::close_nowrite | inotify_flags::close_write |
                             inotify_flags::attrib | inotify_flags::move_src | inotify_flags::move_dst);
    SECTION("open and close simple")
    {
        notify.notify_open("/sys/ala", 77, true);
        notify.notify_close(77);
        REQUIRE(notify.messages.size() == 2);
        REQUIRE(notify.messages[0].name == "/sys/ala");
        REQUIRE(notify.messages[0].name_prev.empty());
        REQUIRE((notify.messages[0].flags && inotify_flags::open));
        REQUIRE(notify.messages[1].name == "/sys/ala");
        REQUIRE(notify.messages[1].name_prev.empty());
        REQUIRE((notify.messages[1].flags && inotify_flags::close_nowrite));
        notify.messages.clear();
    }
    SECTION("open and close pairing")
    {
        notify.notify_open("/sys/ala/1", 100, true);
        notify.notify_open("/sys/ala/2", 200, false);
        notify.notify_open("/sys/ala/3", 300, true);
        REQUIRE(notify.messages.size() == 3);
        notify.notify(200, inotify_flags::attrib);
        REQUIRE(notify.messages.size() == 4);
        notify.notify_close(200);
        REQUIRE(notify.messages.size() == 5);
        notify.notify_close(200);
        REQUIRE(notify.messages.size() == 5);
        REQUIRE(notify.messages[0].name == "/sys/ala/1");
        REQUIRE((notify.messages[0].flags && inotify_flags::open));
        REQUIRE(notify.messages[1].name == "/sys/ala/2");
        REQUIRE((notify.messages[1].flags && inotify_flags::open));
        REQUIRE(notify.messages[2].name == "/sys/ala/3");
        REQUIRE((notify.messages[2].flags && inotify_flags::open));
        REQUIRE(notify.messages[3].name == "/sys/ala/2");
        REQUIRE((notify.messages[3].flags && inotify_flags::attrib));
        REQUIRE(notify.messages[4].name == "/sys/ala/2");
        REQUIRE((notify.messages[4].flags && inotify_flags::close_write));
        notify.messages.clear();
        notify.notify_close(100);
        notify.notify_close(300);
        REQUIRE(notify.messages.size() == 2);
        REQUIRE(notify.messages[0].name == "/sys/ala/1");
        REQUIRE((notify.messages[0].flags && inotify_flags::close_nowrite));
        REQUIRE(notify.messages[1].name == "/sys/ala/3");
        REQUIRE((notify.messages[1].flags && inotify_flags::close_nowrite));
        notify.notify_close(300);
        notify.notify_close(300);
        REQUIRE(notify.messages.size() == 2);
        notify.messages.clear();
    }
    SECTION("Other notifies tests")
    {
        notify.notify("/sys/ala/100", "/sys/ala/555", inotify_flags::move_src);
        notify.notify("/sys/ala/555", "/sys/ala/100", inotify_flags::move_dst);
        notify.notify("/sys/ala/999/989", inotify_flags::attrib);
        REQUIRE(notify.messages.size() == 3);
        REQUIRE(notify.messages[0].name == "/sys/ala/100");
        REQUIRE(notify.messages[0].name_prev == "/sys/ala/555");
        REQUIRE((notify.messages[0].flags && inotify_flags::move_src));
        REQUIRE(notify.messages[1].name == "/sys/ala/555");
        REQUIRE(notify.messages[1].name_prev == "/sys/ala/100");
        REQUIRE((notify.messages[1].flags && inotify_flags::move_dst));
    }
}

TEST_CASE("Filtering test")
{
    using namespace purefs::fs;
    SECTION("Different mask for services")
    {
        notifier_mock notify;
        auto svc1 = std::make_shared<sys::Service>();
        auto svc2 = std::make_shared<sys::Service>();
        notify.register_path(
            "/sys/path1", svc1, inotify_flags::open | inotify_flags::close_nowrite | inotify_flags::close_write);
        notify.register_path("/sys/path1/", svc2, inotify_flags::open | inotify_flags::close_write);
        notify.notify_open("/sys/path1/file.txt", 100, false);
        notify.notify_open("/sys/path1/file1.txt", 200, true);
        REQUIRE(notify.messages.size() == 4);
        notify.notify_close(100);
        REQUIRE(notify.messages.size() == 6);
        notify.notify_close(200);
        REQUIRE(notify.messages.size() == 7);
        // Checking values in the queue
        REQUIRE(notify.messages[0].name == "/sys/path1/file.txt");
        REQUIRE((notify.messages[0].flags && inotify_flags::open));
        REQUIRE(notify.xsvc[0] == svc1.get());
        REQUIRE(notify.messages[1].name == "/sys/path1/file.txt");
        REQUIRE((notify.messages[1].flags && inotify_flags::open));
        REQUIRE(notify.xsvc[1] == svc2.get());
        REQUIRE(notify.messages[2].name == "/sys/path1/file1.txt");
        REQUIRE((notify.messages[2].flags && inotify_flags::open));
        REQUIRE(notify.xsvc[2] == svc1.get());
        REQUIRE(notify.messages[3].name == "/sys/path1/file1.txt");
        REQUIRE((notify.messages[3].flags && inotify_flags::open));
        REQUIRE(notify.xsvc[3] == svc2.get());
        REQUIRE(notify.messages[4].name == "/sys/path1/file.txt");
        REQUIRE((notify.messages[4].flags && inotify_flags::close_write));
        REQUIRE(notify.xsvc[4] == svc1.get());
        REQUIRE(notify.messages[5].name == "/sys/path1/file.txt");
        REQUIRE((notify.messages[5].flags && inotify_flags::close_write));
        REQUIRE(notify.xsvc[5] == svc2.get());
        REQUIRE(notify.messages[6].name == "/sys/path1/file1.txt");
        REQUIRE((notify.messages[6].flags && inotify_flags::close_nowrite));
        REQUIRE(notify.xsvc[6] == svc1.get());
    }
    SECTION("Different paths for services")
    {
        notifier_mock notify;
        auto svc1 = std::make_shared<sys::Service>();
        auto svc2 = std::make_shared<sys::Service>();
        notify.register_path("/sys/path1", svc1, inotify_flags::attrib);
        notify.register_path("/sys/path2", svc2, inotify_flags::attrib);
        notify.notify("/sys/path1/file.txt", inotify_flags::attrib);
        REQUIRE(notify.messages.size() == 1);
        notify.notify("/sys/path1/file2.txt", inotify_flags::attrib);
        REQUIRE(notify.messages.size() == 2);
        notify.notify("/sys/path3", inotify_flags::attrib);
        REQUIRE(notify.messages.size() == 2);
        notify.notify("/sys/path2/", inotify_flags::attrib);
        REQUIRE(notify.messages.size() == 3);
        REQUIRE(notify.messages[0].name == "/sys/path1/file.txt");
        REQUIRE(notify.xsvc[0] == svc1.get());
        REQUIRE(notify.messages[1].name == "/sys/path1/file2.txt");
        REQUIRE(notify.xsvc[1] == svc1.get());
        REQUIRE(notify.messages[2].name == "/sys/path2");
        REQUIRE(notify.xsvc[2] == svc2.get());
    }
}
