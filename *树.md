# 线索二叉树
## NOTE
* 对于一个有n个节点的二叉链表 每个节点有两个指针域 共2n个指针域 n个节点的二叉树共n-1条分支数 也就是说存在n+1个空指针域 **存在资源的浪费**  

在不遍历的情况下 我们不能知道某个节点的前驱是谁  
所以 考虑建立时 (***利用那些空地址***)记住前驱和后继  
我们把这种指向前驱和后继的指针称作 **线索** 
>加上线索的二叉链表称作 线索链表  
>相应的二叉树是线索二叉树  

我们将一颗二叉树遍历后 将所有空指针域的rightchild,更改为它的后继节点 便写出了一颗线索二叉树  

其实线索二叉树 等于是吧一颗二叉树转变成了一个双项链表,这样对我们插入删除查找节点都带来了方便. **我们将二叉树的某种次序遍历使其变为线索二叉树的过程是称作*线索化***  

> *但相应的 我们并不知到 孩子节点是否是指向孩子还是后继节点*  
**我们在每个结点增设两个标志域(bool常量)仅仅 1*2 位,其分别为ltag与rtag**  
```c
typedef TYPE int
struct tree_node
{
    struct tree_node * leftchild;
    bool ltag;
    TYPE data;
    bool rtag;
    struct tree_node * rightchild;
}
```
* ltag 为0时 leftchild指向左孩子 为1时 指向该节点的前驱
* rtag 为0时 rightchild指向右孩子 为1时 指向该节点的后驱
 
 ### 中序遍历线索化的递归代码如下
 * 这是声明
 ```c
typedef enum {Link,Thread}PointerTag;// 0 & 1  

typedef struct BiThrNode
{
    TElemType data;
    struct BiThrNode *lchild ,*rchild;
    PointerTag LTag ;
    PointerYag RTag ;
}BiTheNode,*BiThrNode;
```
***
```c
 BiThrTree pre;//全局变量,始终指向刚刚访问过的节点
 void Inthreading(BiThrTree p){
    if(p)
    {
        InThreading(p->lchild);//递归左子树线索化
        if(!p->lchild)          //没有左孩子
        {
            p->LTag=Thread;     //前驱线索
            p->lchild= pre;     //左孩子指针指向前驱 
        }
            if(!pre->rchild)    //前驱没有右孩子
        {
            pre -> RTag= Thread;//后继线索
            pre -> rchild=p;    //前驱右孩子指针指向后继(当前节点 p)
        }
        pre=p;                  //保持pre指向p的前驱
        InThreading (p->rchild);//递归右子树线索化
    }
 }
 ```