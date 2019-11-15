# development workflow

![alt text](./Images/workflow.png "workflow")

Additional info, one feature at the time:
* 1: create branch - branch name `EDG-<NUM>` i.e. `EDG-123`
* 2: add more commits - each commit should have in short message `[EDG-<NUM>] short message`
    * for WIP and FIX - this should be too added in commit message `[EDG-<NUM>]` i.e
        * `[WIP] [EDG-123] Progress on progressbar`
        * `[FIX] [EDG-123] Progress on progressbar`
* 3: rebase to master
    * When updating own branch one should rather rebase (which moves own commits on top) then merge (which puts changes by dates as it is)
    * This will provide better history and git conflict solving

After PR - feature branch is frozen in development (only fixes)
If you need further development - fork and update this fork to PR till its closed (after closing to master)

**If you need any assistance on how to do it @Adam will be glad to show you and ansver questions in scope of his ability.**

[What is rebase and how - by atlassian](https://www.atlassian.com/git/tutorials/merging-vs-rebasing)
[Simple case flow example](https://makandracards.com/makandra/36003-recommended-git-workflow-for-feature-branches)

# Cleaning mess on branch prior to PR and work on branch 

* when you have mixed new fetures with PR features use `git rebase to HEAD~N` to move commits in proper order

