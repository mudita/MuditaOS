# How to fill changelog entries

## This howto is based on [keep a changelog](https://keepachangelog.com/en/1.0.0/)

## 1. The purpose of a changelog:
To let the end user (not necesairly technically skilled) what your PR changes. This means if some spectacular crash has been fixed, changelog entry should say e.g. _Fixed system crash on saving SMS draft_.
On the other hand if several minor fixes or refactors has been made simple information is sufficent eg. _Minor fixes in Text widget_. Nothing deeply-technical - just information understandable by reader.

## 2. What not to put in the changelog
_Changed gui items according to last UI change_. This doesn't really explain anything. Another example: _Fixed crash on Sunday_. It should be short but understandable
to anyone.

## 3. When to fill the changelog
At every PR. If PR contains multiple meaningful workpieces - list them.

## 4. How to add an entry to the changelog

A few rules to follow:
1. Add your entry in the _Current release_ section. If it does not exist - create it by yourself.
2. Do not remove anything, especially changes from previoues releases.
3. Add _"tags"_ to mark what you are chaning, e.g.: `[sms]`, `[phonebook]`. There
is no fixed list of tags, so if you can't find anything that suits you in the previous
entries, add it.
4. If you are not sure about formatting - see how previous entries are formatted.
5. Put changes in Added, Changed, Removed, Fixed subcategories.
6. If your change won't be visible to the end user, put it in "Other" category (e.g.
improvements to the build system).