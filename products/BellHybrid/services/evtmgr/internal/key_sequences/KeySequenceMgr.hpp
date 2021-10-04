// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <list>

#include "AbstractKeySequence.hpp"

class KeySequenceMgr
{
  public:
    using SequenceCollection = std::vector<std::unique_ptr<AbstractKeySequence>>;
    explicit KeySequenceMgr(SequenceCollection collection);

    void process(const RawKey &key);

  private:
    void actionIfPossible();

    void moveToReadyList(const AbstractKeySequence &seq);
    bool searchInReadyList(const AbstractKeySequence &seq);
    void removeFromReadyList(const AbstractKeySequence &seq);

    void addToInProgressList(const AbstractKeySequence &seq);
    void removeFromInProgressList(const AbstractKeySequence &seq);

    const SequenceCollection sequenceCollection;
    std::list<const AbstractKeySequence *> inProgressSequences;
    std::list<const AbstractKeySequence *> readySequences;
};
