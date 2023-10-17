# How to fill changelog entries

This article is based on [keep a changelog](https://keepachangelog.com/en/1.0.0/).

**Note:** MuditaOS changelog should be **edited by the MuditaOS core development team only**.

[View MuditaOS changelog](changelog.md).

## 1. The purpose of a changelog
To let the end user (not necessarily technically-skilled) know what your Pull Request changes. In practice, this means that if some spectacular crash has been fixed, changelog entry should say e.g. `Fixed system crash on saving SMS draft`.

On the other hand, if several minor fixes or refactors have been made, simple information is sufficient eg. `Minor fixes in Text widget`. Nothing deeply-technical - just information understandable to the reader.

## 2. What not to put in the changelog
Try to be informative when drafting an entry. `Changed GUI items according to last UI change` doesn't really say anything to the end user. Your entry should be short but informative and understandable to anyone.

## 3. When to fill the changelog
With every merged Pull Request. If a PR contains multiple meaningful workpieces - please list them all.

## 4. How to add an entry to the changelog

A few rules to follow:

1. Add your entry in the `Current release` section. If it doesn't exist - feel free to create it yourself.
2. Please don't remove anything, especially changes from previous releases.
3. Add "[tags]" to mark what you are changing, e.g.: `[sms]`, `[phonebook]`. There is no fixed list of tags, so if you can't find anything that suits you in the previous entries, please add a new tag.
4. If you're not sure about formatting - please see how previous entries are formatted.
5. Put relevant changes in `Added`, `Changed`, `Removed`, `Fixed` subsections.
6. If your change won't be visible to the end user, please put it in `Other` section (e.g. improvements to the build system).


