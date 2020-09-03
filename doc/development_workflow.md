# Development workflow

When working on a project change one needs to take following steps in order to get
changes merged into the master branch:

1. Create feature branch, which name must consist of JIRA number of a feature and 
short, general description of its contents, e.g. `EGD-5555-some-feature-fixes`.

2. Commit your work. Each commit *must* start with JIRA number in square brackets. It can also point
which component you are working on, e.g. `[EGD-5555] phonebook: fix contact details`
but it is not required. A component list is not defined yet so please follow names
that has been used already. Each commit should have description explaining:
   * what are you changing,
   * why are you making changes (motivation),
   * what is a result of your changes.
 If it is impossible to provide such information it means that your commit is likely
 excessive or redundant and should be squashed in other commit. Do not add
 commits that are out of scope of JIRA issue you are working on.

3. Before submitting a PR it is required to do basic development checks:
   * test your change on both Linux and RT1051 platforms. Please pay special attention to
 things you might unintentionally broke, e.g.: when working on calling check call log
 too,
   * include changelog description if applicable,
   * run unit tests,
   * check code formatting.

4. Submit a PR. PR name must include JIRA number in square brackets and a short
 description, e.g. `[EGD-5555] Fix contact details`. You can add labels which will be
 helpful for people who know part of code you are modifying to spot the change you are submitting and do a review. Try to explain in a PR description what and why are you
 changing, what decisions you had to take and event what you had been considering
 but eventually did not do. Include screenshots and gifs if applicable. The more
 information you provide, the bigger chance that you'll receive a quick review.

5. During PR review team members will be asking questions regarding your solution.
Answer them explaining your reasoning. Even if comment suggests an obvious change
answer with a simple OK to show that you have not missed it.

6. Add changes to your PR that are requested by reviewers and push feature branch
once again. Update comments requesting changes with a simple 'done'. Do not resolve
discussion on you own, it is reviewer responsibility to do so.

7. When PR is accepted by two or more reviewers it is time to add PR changes to master branch. Merging to
master is prohibited, do a rebase instead. **Do not add to master any commits that
are not required, less is more. An ideal situation is when you have a single commit
for a PR and a single JIRA issue.** If you have multiple commits, you can squash
them and force push a branch before merging PR to master - **make sure that commit message contains only valuable information.**

1. Merging PR causes feature branch to be deleted. Switch to master branch on
your local machine and do a pull or fetch to get your changes.

**If you need any assistance do not hesitate to ask others**

[What is rebase and how - by atlassian](https://www.atlassian.com/git/tutorials/merging-vs-rebasing)
[Simple case flow example](https://makandracards.com/makandra/36003-recommended-git-workflow-for-feature-branches)

# Git rebase commits and how it works

*Git rebase works different than Git merge.*

With rebase, you update your commits one by one. So:
```
git rebase origin/master  # will update your commits one by one to master.
```

To update it to top fresh version of master:
```
git fetch                 # update remote branches on your pc
git rebase origin/master  # update branch you are at to origin/master
```

*Git rebase works on actual branch copy*

Rebase changes your commit history (moves them on top) this means two things:  
* when you did a lot of changes in lot of places - either `git push` your branch on server, or make it's copy  
* when you are happy of rebase results - you will need to push your branch with force to server - as you changed it's history (updated it)  

Minimal commands:
```
git fetch                                             # update all remote branches  
git status                                            # check if there are not changes dangling, or waiting to commit, no merge in progress etc  
git push origin your_branch_name:your_branch_name     # push your branch to server  
git rebase origin/master                              # update your local branch to origin master  
# while rebase not done
git mergetool                                         # if rebase didn't succeed it will ask to merge each commit it can't fast forward on top, othervise it won't happen  
git rebase --continue                                 # update of this commit to master is done, proceed  
git status                                            # when done - check that you have really done it  
# endwhile  
# rebuild repository and check if your changes are intact
```

If there was something that broke your chanes you can now either:
* fix it in next commit
* try rebase again with:
    * `git reset origi/your_branch_name --hard`

If your rebase succeed:
* `git push origin your_branch_name:your_branch_name`

*Please remember:*
* Rebase changes history - don't rebase branches other people are working on
    * It's doable all right, but if you don't know the tool that well please check first how
* Always rebase your feature branches to origin master

# How to change some commit somewhere in history of my PR branch I'm ashamed

```
git log -S 'stupid log I added'     # search in history when some text was added
git rebase COMMIT_SHA~ -i           # interractive rebase to commit sha I found in git log -S
# select option edit in interactive mode on commit that's wrong
# fix file that's wrong
git add FIXED_FILE                  # add your fixups (i.e. removal of dumb log)
git rebase --continue               # continue with rebase
```

If you added file that you didn't want you can use:
```
# instead fixing this file manually
git reset HEAD~ --hard
# or if you want to keep part of changes, and add changes you want to add:
git reset HEAD~
# than:
git rebase --continue
```

# PS: good git browser will do it all for you, if you can do it all in git-kraken or sourcetree!*
