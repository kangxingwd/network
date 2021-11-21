#!/bin/bash

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


# 删除
sed '2,$d' test_file/user.txt
sed '/111/d' test_file/user.txt
# 删除 name=test1 行开始到 name=pp行
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



gawk '/name=nn/{print $0}' test_file/user.txt


# 正则表达式特殊字符 .*[]^${}\+?|()


# 扩展的正则表达式，gawk具备大多扩展正则， 而sed没有
# + ？ {} | ()


# 电话

# 邮件
#　^([a-zA-Z0-9_\-\.\+]+)@([a-zA-Z0-9_\-\.]+)\.([a-zA-Z]{2,5})$


