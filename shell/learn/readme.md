


# sort
- sort 默认是按照字符进行排序的
- sort -n 按照数字进行排序
```s
-b --ignore-leading-blanks 排序时忽略起始的空白
-C --check=quiet 不排序，如果数据无序也不要报告
-c --check 不排序，但检查输入数据是不是已排序；未排序的话，报告
-d --dictionary-order 仅考虑空白和字母，不考虑特殊字符
-f --ignore-case 默认情况下，会将大写字母排在前面；这个参数会忽略大小写
-g --general-number-sort 按通用数值来排序（跟-n不同，把值当浮点数来排序，支持科学计数法表示的值）
-i --ignore-nonprinting 在排序时忽略不可打印字符
-k --key=POS1[,POS2] 排序从POS1位置开始；如果指定了POS2的话，到POS2位置结束
-M --month-sort 用三字符月份名按月份排序
-m --merge 将两个已排序数据文件合并
-n --numeric-sort 按字符串数值来排序（并不转换为浮点数）
-o --output=file 将排序结果写出到指定的文件中
-R --random-sort 按随机生成的散列表的键值排序
 --random-source=FILE 指定-R参数用到的随机字节的源文件
-r --reverse 反序排序（升序变成降序）
-S --buffer-size=SIZE 指定使用的内存大小
-s --stable 禁用最后重排序比较
-T --temporary-directory=DIR 指定一个位置来存储临时工作文件
-t --field-separator=SEP 指定一个用来区分键位置的字符
-u --unique 和-c参数一起使用时，检查严格排序；不和-c参数一起用时，仅输出第一例相似的两行
-z --zero-terminated 用NULL字符作为行尾，而不是用换行符
```

```s
# -k和-t参数在对按字段分隔的数据进行排序
$ sort -t ':' -k 3 -n /etc/passwd 
root:x:0:0:root:/root:/bin/bash 
bin:x:1:1:bin:/bin:/sbin/nologin

# 对目录大小进行排序
du -sh * | sort -rn
```

# soproc 协程
```s
# soproc 将命令至于后台模式, 可通过ps 或者 jobs 查看, 默认启动的协程名称是  COPROC
soproc sleep 400

# 自定义协程的名字 My_Job
coproc My_Job { sleep 10; }

# 自定义名字可用来通信
```

# 算数 expr
```s
# expr 相关
ARG1 | ARG2 如果ARG1既不是null也不是零值，返回ARG1；否则返回ARG2
ARG1 & ARG2 如果没有参数是null或零值，返回ARG1；否则返回0
ARG1 < ARG2 如果ARG1小于ARG2，返回1；否则返回0
ARG1 <= ARG2 如果ARG1小于或等于ARG2，返回1；否则返回0
ARG1 = ARG2 如果ARG1等于ARG2，返回1；否则返回0
ARG1 != ARG2 如果ARG1不等于ARG2，返回1；否则返回0
ARG1 >= ARG2 如果ARG1大于或等于ARG2，返回1；否则返回0
ARG1 > ARG2 如果ARG1大于ARG2，返回1；否则返回0
ARG1 + ARG2 返回ARG1和ARG2的算术运算和
ARG1 - ARG2 返回ARG1和ARG2的算术运算差
ARG1 * ARG2 返回ARG1和ARG2的算术乘积
ARG1 / ARG2 返回ARG1被ARG2除的算术商
ARG1 % ARG2 返回ARG1被ARG2除的算术余数
STRING : REGEXP 如果REGEXP匹配到了STRING中的某个模式，返回该模式匹配
match STRING REGEXP 如果REGEXP匹配到了STRING中的某个模式，返回该模式匹配
substr STRING POS LENGTH 返回起始位置为POS（从1开始计数）、长度为LENGTH个字符的子字符串
index STRING CHARS 返回在STRING中找到CHARS字符串的位置；否则，返回0
length STRING 返回字符串STRING的数值长度
+ TOKEN 将TOKEN解释成字符串，即使是个关键字
(EXPRESSION) 返回EXPRESSION的值
```

# 小数运算  bc
```s
#!/bin/bash 
# scale=4 代表设置保留小数点后多少位
var1=100 
var2=45 
var3=$(echo "scale=4; $var1 / $var2" | bc) 
echo The answer for this is $var3

# 多个运算时， 可以使用 内联重定向 EOF 标记起始位置
var1=10.46 
var2=43.67 
var3=33.2 
var4=71 
var5=$(bc << EOF 
scale = 4 
a1 = ( $var1 * $var2) 
b1 = ($var3 * $var4) 
a1 + b1 
EOF 
) 
echo The final answer for this mess is $var5
```
 
# 用于高级数学表达式的双括号
```s
val++ 后增
val-- 后减
++val 先增
--val 先减
! 逻辑求反
~ 位求反
** 幂运算
<< 左位移
>> 右位移
& 位布尔和
| 位布尔或
&& 逻辑和
|| 逻辑或

#!/bin/bash
val1=10 
if (( $val1 ** 2 > 90 )) 
then 
 (( val2 = $val1 ** 2 )) 
 echo "The square of $val1 is $val2" 
fi 
```

# 用于高级字符串处理功能的双方括号
```s
# 双方括号里的expression使用了test命令中采用的标准字符串比较。但它提供了test命令未提供的另一个特性——模式匹配（pattern matching）。

#!/bin/bash
# 双等号将右边的字符串（r*）视为一个模式，并应用模式匹配规则
# if [[ $UU = r* ]] 
if [[ $USER == r* ]] 
then 
 echo "Hello $USER" 
else 
 echo "Sorry, I do not know you" 
fi
```

# shell注释
```s
: << COMMENTBLOCK
   shell脚本代码段
COMMENTBLOCK
```



