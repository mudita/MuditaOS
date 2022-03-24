// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Sink.hpp"
#include <fstream>
#include <string>

class FileSink : public Sink
{
  private:
    std::ofstream fs;

  public:
    explicit FileSink(const std::string &filepath);
    ~FileSink();
    void put(const char *data, size_t size) final;
};
