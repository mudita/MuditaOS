// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ATURCStream.hpp"
#include "ATCommon.hpp"

#include <module-os/RTOSWrapper/include/ticks.hpp>
#include <cstring>

namespace at
{

    /**
     * Look for first possible urc begin \r\n+
     * remove everything in front(should not happen but possible communication errors)
     * @return true if have urc begin to process
     */
    bool ATURCStream::search()
    {
        if (!urcBeginFound) {
            auto pos = atBuffer.find(urcStart);
            if (pos == std::string::npos) {
                atBuffer = {};
                return false;
            }

            urcBeginFound = true;

            atBuffer.erase(0, pos);
            return true;
        }
        return true;
    }

    /**
     * check that quotation mark is open or close in current buffer
     * to wait for next chunk or if closed and urc is closed by \r\n ad to ready urc list
     * @return true if found urc line
     */
    bool ATURCStream::checkQuotation()
    {
        if (!openQuotationMark) {
            auto posDelimiter = atBuffer.find(at::delimiter, posMark + 1);
            if (posDelimiter != std::string::npos) {
                auto posMark2 = atBuffer.find("\"", posMark + 1);
                if ((posMark2 != std::string::npos) && (posDelimiter > posMark2)) {
                    posMark           = posMark2;
                    openQuotationMark = !openQuotationMark;
                    return false;
                }

                urcList.push_back(atBuffer.substr(0, posDelimiter + std::strlen(at::delimiter)));
                atBuffer.erase(0, posDelimiter + std::strlen(at::delimiter));
                lookForNextURC = true;
                return true;
            }
        }
        return false;
    }

    /**
     * Check for next quotation and urc, in general could look like
     * \r\n+URC: "literal \r\n...."\r\n\r\n+URC: "param",4,4\r\n ...
     * or
     * \r\nURC... \r\n
     * @return true if found and we could look for next URCin buffer
     */
    bool ATURCStream::lookForNext()
    {
        lookForNextURC    = false;
        openQuotationMark = false;
        posMark           = 0; /// start from \r\n(+) from search
        do {
            if (checkQuotation())
                break;

            openQuotationMark = !openQuotationMark;
            posMark           = atBuffer.find("\"", posMark + 1);
        } while (posMark != std::string::npos);

        return lookForNextURC;
    }
    /**
     * try to find URC in current buffer,
     * found results could be get by getURCList
     */
    void ATURCStream::tryParse()
    {

        while (search()) {

            if (!lookForNext()) {
                urcBeginFound = false;
                break;
            }
        }
    }

    /**
     * This function reset buffer in case it is too big or too old.
     * it is dictated only by the possibility related to transmission errors.
     */
    void ATURCStream::fuse()
    {
        /**
         * multiple frame have around hundreds char, so they should be delayed only by
         * transmission time and for one URC should be (from experiment) around 1ms
         * if frame delay is too long, there is a possibility that we have some transmission
         * errors, eg.frames
         * [1ms]Frame1: \r\n+URC: param,"long string .....
         * [2ms]Frame2: error .... and some part without end of quotation mark  ... \r\n
         * [100ms] Frame3:\r\n+NextURC:ABC\r\n
         *
         * In case checking only data we omit NextURC, because we could also get URC which look like
         * \r\n+URC: param,"\r\n+NextURC:ABC\r\n"\r\n
         */

        if (atBuffer.empty())
            return;

        constexpr auto maxTimeout = 5;
        constexpr auto maxSize    = 2048;
        uint32_t timeoutNeeded    = lastWriteTime + maxTimeout;
        uint32_t timeElapsed      = cpp_freertos::Ticks::GetTicks();
        if ((timeElapsed >= timeoutNeeded) || (atBuffer.length() > maxSize)) {
            LOG_ERROR("AT URC Stream timeout ");
            atBuffer = {};
        }
    }

    bool ATURCStream::write(const std::string &buffer)
    {
        fuse();
        lastWriteTime = cpp_freertos::Ticks::GetTicks();
        atBuffer += buffer;
        tryParse();
        return true;
    }

} // namespace at
