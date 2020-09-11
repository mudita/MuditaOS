# Development workflow

When submitting code or documentation changes please follow these steps to have bigger chances of getting your piece of work merged into the master branch.

## Create a new branch

Create a branch, which name is connected to the feature, bug fix, or documentation change you want to contribute e.g. `feature/some-feature` or ``fix/some-fix`.

**Note:** If you're part of the MuditaOS core development team, please precede the branch name with a relevant Jira ticket number e.g. `EGD-5555-some-feature`.

## Commit changes

Commit your work. Each commit should have a description explaining:
	- what are you changing,
	- why are you making the changes (motivation),
	- what is the result of your changes.

If it's impossible to provide the above information then your commit is likely excessive or redundant and should be squashed with other commit(s).

Here's [a helpful article about writing good Git commit messages](https://chris.beams.io/posts/git-commit/).

**Note:** If you're part of the MuditaOS core development team, your commit *must* start with Jira ticket number in square brackets. It can also point to which component you are working on, e.g. `[EGD-5555] phonebook: fix contact details`. A list of components is not yet defined, so please follow the names that are already in use. Do not add commits that are out of the scope of the Jira issue you are working on.

## Basic checks before Pull Request

Before submitting a Pull Request please go through some basic checks:
	- test your changes on both Linux and RT1051 platforms (please pay special attention to the things you might break unintentionally, e.g. when working on calling funcionality, check call log too)
	- [include changelog description](changelog_howto.md) (if applicable),
	- run unit tests (`make check`),
	- check if your code formatting complies with [`.clang-format`](.clang-format).

## Submit a Pull Request

Submit a Pull Request. You can add labels that will help people who know the part of the code you are modifying in spotting the changes you are submitting and do a code review. 

Please use the PR description to:
	- explain what and why you are changing, 
	- what decisions you had to take, 
	- what you have been considering but eventually did not do (if applicable), 

Include screenshots and gifs/screencasts if applicable. The more information you provide, the bigger chance that you'll receive a quick review.

**Note:** If you're part of the MuditaOS core development team, please precede the PR name with a relevant Jira ticket number e.g. `[EGD-5555] Fixed contact details`.

## PR review - answer questions

During a PR review, team members will ask you questions regarding your solution. Please answer them by explaining the reasoning behind the suggested changes. Even if the feedback you receive regarding your PR is an obvious change, please answer with a simple OK to show that you haven't missed it.

## PR review - act on feedback

Add changes to your PR that are requested by reviewers and push the feature branch once again. Update comments requesting changes with a simple `Done`. Don't resolve discussion on your own, it's the reviewer's responsibility to do so.

## Merge to `master` branch

When a PR is accepted by two or more reviewers it's time to add PR changes to `master` branch. **Merging to
`master` is prohibited - [do a rebase instead](https://www.atlassian.com/git/tutorials/merging-vs-rebasing). Do not add any commits to `master` that are not required - less is more. An ideal situation is when you have a single commit for a PR and a single Jira issue (MuditaOS core development team).** 

If you have multiple commits, you can squash them and force push a branch before merging your PR to master. **Please make sure that your commit message contains only valuable information.**

Merging a PR causes the feature branch to be deleted. Switch to master branch on your local machine and do a pull or fetch to get your changes.

**If you need any assistance do not hesitate to ask others 🙌🏼**

### Commit using `git rebase`

**Git rebase works differently than Git merge.**

With `rebase`, you update your commits one by one.
```
git rebase origin/master  # will update your commits one by one to master.
```

To update it to the latest version of `master`:
```
git fetch                 # update remote branches on your local machine
git rebase origin/master  # update branch you are at to origin/master
```

**Git rebase works on an actual branch copy.**

`Rebase` changes your commit history (moves them on top). This means two things:  
 - when you did a lot of changes in a lot of places - either `git push` your branch on server, or make its copy
 - when you're happy of `git rebase` results - you'll need to push your branch with force to server - since you've changed its history (updated it) 

A minimal set of commands:
```
git fetch                                             # update all remote branches  
git status                                            # check if there are no changes dangling, or waiting to commit, no merge in progress, etc.  
git push origin your_branch_name:your_branch_name     # push your branch to server  
git rebase origin/master                              # update your local branch to origin master  

# while rebase not done
git mergetool                                         # if rebase didn't succeed it will ask to merge each commit it can't fast forward on top, othervise it won't happen  
git rebase --continue                                 # update of this commit to master is done, proceed  
git status                                            # when done - check that you have really done it  
# endwhile

# rebuild repository and check if your changes are intact
```

If there was something that broke your changes you can now either:
* fix it in your next commit
* try rebase again with:
    * `git reset origin/your_branch_name --hard`

If your rebase succeeds:
* `git push origin your_branch_name:your_branch_name`

**Note:**
 - Rebase changes history - don't rebase branches that other people are working on (it's doable, but don't do this if you're not familiar with how to do it properly)
 - Always rebase your feature branches to `origin master`

#### How to change some shameful commit somewhere in your PR branch history
```
git log -S 'shameful log I added'   # search in history when some message was added
git rebase COMMIT_SHA~ -i           # interactive rebase to commit sha I found in git log -S
# select option edit in interactive mode on commit that's wrong
# fix file that's wrong
git add FIXED_FILE                  # add your fixups (i.e. removal of the shameful log)
git rebase --continue               # continue with rebase
```

If you added a file that you didn't want to, you can use:
```
# instead fixing this file manually
git reset HEAD~ --hard
# or if you want to keep a part of the changes and add new changes:
git reset HEAD~
# then:
git rebase --continue
```

PS: A good GUI git browser will do it all for you.
