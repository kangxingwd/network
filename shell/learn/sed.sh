#!/bin/bash

# 替换  s/pattern/replacement/flag
# 寻址  [address]command
#   address 有两种方式, 一种是行区间, 一种是模式匹配(模式匹配也可以匹配区间)


# sed
# 替换每行的第一个
sed 's/kx/wd/' test_file/user.txt
# 替换每行的第二个
sed 's/test/trial/2' test_file/user.txt
# 替换每行所有
sed 's/test/trial/g' test_file/user.txt

# 作用与特定行, 两种方式，一种时指定行数，另一种时模式匹配（可以匹配区间）
# 第2行
sed '2s/test/trial/g' test_file/user.txt
# 2-4 行
sed '2,4s/test/trial/g' test_file/user.txt
# 2到结尾
sed '2,$s/test/trial/g' test_file/user.txt
# 修改匹配到yyy的行，并将该行中 test 替换为trial
sed '/yyy/s/test/trial/g' test_file/user.txt
sed '/name=test1/s/test/trial/g' test_file/user.txt


sed '/name="kx"/s/passwd="[^"]*"/passwd="222222"/' test_file/test.xml
sed '/name="kx"/'


# 删除
sed '2,$d' test_file/user.txt
sed '/111/d' test_file/user.txt
# 删除 name=test1 行开始到 name=pp行   匹配区间
sed '/name=test1/,/name=pp/d' test_file/user.txt

# 插入
# 每一行前面插入
sed 'i\ppppppp' test_file/user.txt
# 第二行前面插入
sed '2i\ppppppp' test_file/user.txt
# 匹配行前面插入
sed '/name=nn/i\ppppppp' test_file/user.txt


# 附加
# 每行后面附加
sed 'a\ppppppp' test_file/user.txt
# 第二行后面附加
sed '2a\ppppppp' test_file/user.txt

# 修改整行
sed '3c\tttttttttttttt' test_file/user.txt
sed '/name=nn/c\name=tttttttttttt' test_file/user.txt
# 2-3行整体替换，不是每行替换
sed '2,3c\ttttttttttttt' test_file/user.txt

# 转换， sed中唯一一个处理单个字符的命令， [address]y/inchars/outchars/
# 转换命令是一个全局命令, 它会文本行中找到的所有指定字符自动进行转换
# 如下命令， 会将每行中1替换为7， 2替换为8， 3替换为9
sed 'y/123/789/' test_file/user.txt

# 打印  p
# 其中 -n 禁止输出其他行
sed -n '/name=nn/p' test_file/user.txt

# 等号 = 列出行号
sed -n '/name=nn/=' test_file/user.txt

# 小写l, 列出行
sed -n '/name=nn/l' test_file/user.txt

# 多个命令, 两种方法
sed -e 's/value="[^"]*"/value="iiiii"/ ; s/experience/qqqqqq/' test_file/test.xml
sed '{s/value="[^"]*"/value="iiiii"/ ; s/experience/qqqqqq/}' test_file/test.xml


# 获取xml字段
# 正则提取字符串  -- 利用 () 提取后, 将整行替换成  提取后的字符串 然后显示
echo "xxx  number : 123456 xxx" | sed  's/.*number : \([0-9]*\).*/\1/g'
name=kx
sed -n '/name="'${name}'"/s/.*passwd="\([^"]*\)" .*/\1/p' test_file/test.xml



# 正则表达式特殊字符 .*[]^${}\+?|()


# 扩展的正则表达式，gawk具备大多扩展正则， 而sed没有
# + ？ {} | ()

# 邮件
#　^([a-zA-Z0-9_\-\.\+]+)@([a-zA-Z0-9_\-\.]+)\.([a-zA-Z]{2,5})$


# 高级

# 删除匹配行的下一行   n
sed '/name="kx"/{n ; d}'  test_file/test.xml
# 删除匹配行的第二行
sed '/name="kx"/{n ; n ; d}'  test_file/test.xml

# 修改匹配行的下一行
sed '/name="kx"/{n; s/name="[^"]*"/name="pppppp"/ ; s/value="[^"]*"/value="ooooooo"/}'  test_file/test.xml

# sed, 格外的空间
# h 将模式空间复制到保持空间
# H 将模式空间附加到保持空间
# g 将保持空间复制到模式空间
# G 将保持空间附加到模式空间
# x 交换模式空间和保持空间的内容

# 先匹配 name="kx";  -- 放到模式空间
# h  然后将给该行复制到保持空间; 
# p  打印模式空间当前行的内容
# n  然后提取下一行内容
# p  打印模式空间当前的内容
# g  将保持空间复制到模式空间替换当前文本
# p  打印当前模式空间的内容
sed -n '/name="kx"/{h ; p ; n ; p ; g ; p}' test_file/test.xml

# 排除命令 !,  打印没有too的行
sed -n '/too/!p' test_file/test.xml

# 每行后面加空行,   利用了 保持空间 的默认值
sed 'G' test_file/test.xml
# 每行后面加空行, 最后一行后面不加
sed '$!G' test_file/test.xml
# 如果文本本身就有空行, 可以先删除空行,再在每行后面添加空行
sed '{/^$/d ; $!G}' test_file/test.xml

# 反转文本
sed -n '{1!G ; h ; $p }' test_file/test.xml

# 模式替代

# &符号, &符号可以用来代表替换命令中的匹配的模式
sed 's/kx/new_&/g' test_file/test.xml
# 上面命令等同于
sed 's/kx/new_kx/g' test_file/test.xml

# 替代单独的单词, 当在替换命令中使用圆括号时, 替代模式中就可以用 \1   \2
sed 's/name="\([^"]*\)".*age="\([^"]*\)"/new_name="\1" new_age="\2"/' test_file/test.xml

# 测试命令t, 可以判断前面的命令是否执行成功,  可以做到类似于 if else的功能; 
# 如果前面的命令替换成功,就不会执行后面的命令, sed会跳转到脚本的结尾
# 如果前面的命令没有执行成功, 就执行后面的命令
sed '{s/kx/new_kx/ ; t ; s/wd/new_wd/}' test_file/test.xml

# t也可以跳转到指定的标记
echo "1234567" | sed '{:start ; s/\(.*[0-9]\)\([0-9]\{3\}\)/\1,\2/ ; t start}'
# 1,234,567

# N 命令允许跨行匹配
# q 命令退出, N命令合并下一行到当前行, D命令删除模式空间第一行
# 显示最后3行
sed '{:start ; $q ; N; 3,$D ; b start}'  test_file/test.xml

# 删除连续的空白行 
sed '/./,/^$/!d' test_file/test.xml
# 删除开头的空白行
sed '/./,$!d' test_file/test.xml
# 删除结尾的空白行 --  这个命令不行
sed '{:start ; /^\n*$/{$d ; N ; b start } }' test_file/test.xml

gname=g1
sed '/GroupName='$gname'/s/enable="[0-9]*"/enable="1"/' username.xml

