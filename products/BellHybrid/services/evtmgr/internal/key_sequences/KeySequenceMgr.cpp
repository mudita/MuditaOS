// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "KeySequenceMgr.hpp"

KeySequenceMgr::KeySequenceMgr(std::vector<std::unique_ptr<AbstractKeySequence>> collection)
    : sequenceCollection(std::move(collection))
{
    auto onIdle = [this](auto &seq) {
        removeFromInProgressList(seq);
        actionIfPossible();
    };
    auto onTriggered = [this](auto &seq) { addToInProgressList(seq); };
    auto onReady     = [this](auto &seq) {
        moveToReadyList(seq);
        actionIfPossible();
    };
    auto onAbort = [this](auto &seq) {
        removeFromInProgressList(seq);
        removeFromReadyList(seq);
    };

    for (auto &seq : sequenceCollection) {
        seq->setCallbacks(onTriggered, onReady, onIdle, onAbort);
    }
}
void KeySequenceMgr::process(const RawKey &key)
{
    for (const auto &seq : sequenceCollection) {
        seq->process(key);
    }
}
void KeySequenceMgr::moveToReadyList(const AbstractKeySequence &seq)
{
    const auto res = std::find(inProgressSequences.begin(), inProgressSequences.end(), &seq);
    if (res != inProgressSequences.end()) {
        readySequences.emplace_back(&seq);
        inProgressSequences.remove(&seq);
    }
}
void KeySequenceMgr::actionIfPossible()
{
    if (inProgressSequences.empty()) {
        if (const auto s = readySequences.back(); s != nullptr) {
            s->action();
            readySequences.clear();
        }
    }
}
bool KeySequenceMgr::searchInReadyList(const AbstractKeySequence &seq)
{
    return std::find(readySequences.begin(), readySequences.end(), &seq) != readySequences.end();
}
void KeySequenceMgr::removeFromInProgressList(const AbstractKeySequence &seq)
{
    const auto result = std::find(inProgressSequences.begin(), inProgressSequences.end(), &seq);
    if (result != inProgressSequences.end()) {
        inProgressSequences.remove(&seq);
    }
}
void KeySequenceMgr::removeFromReadyList(const AbstractKeySequence &seq)
{
    if (searchInReadyList(seq)) {
        readySequences.remove(&seq);
    }
}
void KeySequenceMgr::addToInProgressList(const AbstractKeySequence &seq)
{
    inProgressSequences.emplace_back(&seq);
}
