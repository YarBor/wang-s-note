- git commit 
> Git 仓库中的提交记录保存的是你的目录下所有文件的快照，就像是把整个目录复制，然后再粘贴一样，但比复制粘贴优雅许多！
> Git 希望提交记录尽可能地轻量，因此在你每次进行提交时，它并不会盲目地复制整个目录。条件允许的情况下，它会将当前版本与仓库中的上一个版本进行对比，并把所有的差异打包到一起作为一个提交记录。
> Git 还保存了提交的历史记录。这也是为什么大多数提交记录的上面都有父节点的原因 —— 我们会在图示中用箭头来表示这种关系。对于项目组的成员来说，维护提交历史对大家都有好处。
关于提交记录太深入的东西咱们就不再继续探讨了，现在你可以把提交记录看作是项目的快照。提交记录非常轻量，可以快速地在这些提交记录之间切换！
```shell
$ git commit -m "********************************"
```
提交代码到本地仓库

---

- git branch
创建分支! `创建分支, 多用分支`
因为创建分支不会增加任何额外的开销 却可以按逻辑进行工作的分解与维护
```shell
$ git branch + name
```
在主分支下创建新的分支`name`

- 强制修改分支位置
你现在是相对引用的专家了，现在用它来做点实际事情。
我使用相对引用最多的就是移动分支。可以直接使用 -f 选项让分支指向另一个提交。例如:
`git branch -f main HEAD~3`
上面的命令会将 main 分支强制指向 HEAD 的第 3 级父提交。
---

- git checkout 
选择分支!
在进行`branch`创建分支后 
执行的`commit`操作选择的分支还是主分支`main`
若要将文件提交到新的/目标分支
需要进行`checkout`进行分支选择 
```shell
$ git checkout + name
```
> 如果想创建一个新分支的同时切换到该分支 可以通过`git checkout -b <your-branch-name>`操作来实现

---

-1- 分支与合并
- git merge
使用`git marge` 即在Git中合并两个分支时会产生一个特殊的提交记录, 它有两个父节点. 创建的记录相当于"我要把这两个父节点本身及它们所有的祖先都包含进来"
```shell
$ git merge + name 
```
**注意** 是将`name`分支合并到`所在分支`所在分支将包含`name`分支的所有祖先 但`name`分支不是

--- 

-2- 分支与合并
- git rebase
第二种合并分支的方法是 `git rebase`。Rebase 实际上就是取出一系列的提交记录，“复制”它们，然后在另外一个地方逐个的放下去
具体描述可以理解为==>
**或者说 将`所选分支`及其父节点追加在了name上**
> Rebase 的优势就是可以创造更线性的提交历史，这听上去有些难以理解。如果只允许使用 Rebase 的话，代码库的提交历史将会变得异常清晰。
```shell
$ git rebase + _name_
```
**注意** 是将`所在分支`提交节点及父节点合并**至**`name`
**注意** 在rebase完成后 要将`name`分支更新
```shell
样例
git checkout -b wang
git commit
git checkout main
git commit
git checkout wang
git rebase main #合并 #或者说 将`所选分支`及其父节点追加在了所选分支上
git rebase wang #因为 wang 是main分支下的 所以是更新mian至wang

```
---

- git log
查看提交记录的哈希值
> 通过指定提交记录哈希值的方式在 Git 中移动不太方便。在实际应用时，并没有像本程序中这么漂亮的可视化提交树供你参考，所以你就不得不用 git log 来查查看提交记录的哈希值。
> 并且哈希值在真实的 Git 世界中也会更长（译者注：基于 SHA-1，共 40 位）。例如前一关的介绍中的提交记录的哈希值可能是 fed2da64c0efc5293610bdd892f82a58e8cbc5d8。舌头都快打结了吧...
> 比较令人欣慰的是，Git 对哈希的处理很智能。你只需要提供能够唯一标识提交记录的前几个字符即可。因此我可以仅输入fed2 而不是上面的一长串字符。

---
相对引用 ^ 和 ~<num>
Node^ 指向该节点的父节点[可以叠加 ^^^ 指向3级父节点]
Node~<num> 指向该节点的num级节点

---

- HEAD 概念
HEAD默认引用上一次提交
```shell
可以使用`git checkout`进行指定HEAD节点
$ cat .git/HEAD  #查看HEAD指向
$ git symbolic-ref HEAD #查看HEAD指向(如果是引用)
```
可以通过 cat .git/HEAD 查看， 如果 HEAD 指向的是一个引用，还可以用 git symbolic-ref HEAD 查看它的指向

---

- 撤销变更
在 Git 里撤销变更的方法很多。和提交一样，撤销变更由底层部分（暂存区的独立文件或者片段）和上层部分（变更到底是通过哪种方式被撤销的）组成。我们这个应用主要关注的是后者。
主要有两种方法用来撤销变更 —— 一是 `git reset`，还有就是 `git revert`
git reset + `目标`--在本地删除目标下的子节点
`git reset HEAD^ `删除HEAD下的所有子节点

git revert `***`- 撤销更改并分享给别人 
换句话 -- 提交一次更改,这次更改的内容是回退到`***`位置;

- 删除某一次的commit
  `git rebase -i 「节点」`
  将需要删除的commit 前的pick 改为drop
   


- 贮藏修改
  https://git-scm.com/book/zh/v2/Git-%E5%B7%A5%E5%85%B7-%E8%B4%AE%E8%97%8F%E4%B8%8E%E6%B8%85%E7%90%86
  `git stash` / `git stash push`
  将当前的修改贮藏 类似压入栈中

  查看时可以用`git stash list`进行查看
  并通过`git stash apply` / `git stash apply satsh@{2}`将刚刚的贮藏重新应用 
  或者通过`git stash pop` 来应用并且丢弃贮藏 
  或只通过`git stash drop「name」`来丢弃

- 通过 `git pull`实现拉取操作
  `git pull --rebase`将以上游为准 忽略本地commit的不同 直接进行覆盖操作
  `git pull --no-rebase` 将以本地为准 忽略上游和本地commit的不同 进行覆盖「上游」
