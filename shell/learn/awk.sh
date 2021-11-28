#!/bin/bash

# 内建变量
# FIELDWIDTHS 由空格分隔的一列数字，定义了每个数据字段确切宽度
# FS 输入字段分隔符
# RS 输入记录分隔符     gawk默认将RS和ORS设为换行符
# OFS 输出字段分隔符
# ORS 输出记录分隔符

# ARGC 当前命令行参数个数
# ARGIND 当前文件在ARGV中的位置
# ARGV 包含命令行参数的数组
# CONVFMT 数字的转换格式（参见printf语句），默认值为%.6 g
# ENVIRON 当前shell环境变量及其值组成的关联数组
# ERRNO 当读取或关闭输入文件发生错误时的系统错误号
# FILENAME 用作gawk输入数据的数据文件的文件名
# FNR 当前数据文件中的数据行数
# IGNORECASE 设成非零值时，忽略gawk命令中出现的字符串的字符大小写
# NF 数据文件中的字段总数
# NR 已处理的输入记录数
# OFMT 数字的输出格式，默认值为%.6 g
# RLENGTH 由match函数所匹配的子字符串的长度
# RSTART 由match函数所匹配的子字符串的起始位置


# gawk
echo "My name is Rich" | gawk '{$4="Christine"; print $0}'

# gawk -F: 'BEGIN {$1} {print $1 $7} END {}'

# BEGIN 和 END 分别在遍历文件前后执行命令,  -F: 指定 FS=":"
gawk -F: 'BEGIN {print "---start----"} {print $1,$7} END {print "---end--"}' test_file/passwd

gawk 'BEGIN{FS=":"; OFS="--"} {print $1,$6,$7}' test_file/passwd

# FIELDWIDTHS变量允许你不依靠字段分隔符来读取记录,  根据提供的字段宽度来计算字段
gawk 'BEGIN{FIELDWIDTHS="3 5 2 5"}{print $1,$2,$3,$4}' test_file/passwd

gawk 'BEGIN{print ARGC,ARGIND,ARGV[0],ARGV[1],ENVIRON["PWD"]}' test_file/passwd
# 2 0 gawk test_file/passwd

gawk 'BEGIN{print ENVIRON["PWD"] ; print ENVIRON["USER"],ENVIRON["HOME"]}'

# NF 用来获取最后一个记录
gawk -F: '{print $1,$NF}' test_file/passwd

# 自定义变量  任意数目的字母、数字和下划线，但不能以数字开头
gawk 'BEGIN{x=4; x=x ** 2 + 3; print x}'

# 将脚本 script1 中的 变量 n 变为2
gawk -f script1 n=2 data1

# 循环, 数组
gawk 'BEGIN{var["a"]=1; var["b"]=2; for (test in var) { print test":"var[test] }}'

# 删除数组变量  delete array[index]

# 匹配行
gawk -F: '/kx/{print $0}' test_file/passwd


# 打印 用:分割后第7个字段是以2 /ust/sbin 开头的行
gawk -F: '$7 ~ /^\/usr\/sbin/{print $0}' test_file/passwd

gawk -F: '$1 ~ /kx/{print $1,$NF}' test_file/passwd
# 使用 ! 排除匹配的
gawk -F: '$1 !~ /kx/{print $1,$NF}' test_file/passwd


# 数学表达式
# x == y：值x等于y
# x <= y：值x小于等于y
# x < y：值x小于y
# x >= y：值x大于等于y
# x > y：值x大于y
gawk -F: '$4 == 0{print $1}' test_file/passwd
gawk -F: '$1 == "sync"{print $0}' test_file/passwd

# if else
gawk -F: '{if ($1 == "root" ) print $0; else print $1 }' test_file/passwd

# while
gawk -F: '{i = 1; while (i < 4) {print $0; i++;} }' test_file/passwd

# break
# do{ xxx;} while ()

# for 
gawk -F"," '{total = 0; for (i = 1; i <= 3; i++) {total += $i}; avg = total / 3; print avg}' test_file/num.txt


# 格式化打印    printf "format string", var1, var2 . . .    
# %[modifier]control-letter, 
# 其中control-letter是一个单字符代码，用于指明显示什么类型的数据, 
# 而modifier则定义了可选的格式化特性
# c 将一个数作为ASCII字符显示
# d 显示一个整数值
# i 显示一个整数值（跟d一样）
# e 用科学计数法显示一个数
# f 显示一个浮点值
# g 用科学计数法或浮点数显示（选择较短的格式）
# o 显示一个八进制值
# s 显示一个文本字符串
# x 显示一个十六进制值
# X 显示一个十六进制值，但用大写字母A~F

# %后面可以添加一个数字,代表宽度, 默认右对齐, 不足的补上空格, 超过的会全部显示; 可以使用 - 来使其左对齐
gawk -F: '{printf "%20s %-20s\n",$1,$NF}' test_file/passwd

# %-5.1 表示左对齐, 宽度为5, 保留小数点后1位
gawk -F"," '{ret = 10/3; printf "9 / 3 = %-5.1f\n",ret }' test_file/num.txt


# 内建函数
# atan2(x, y) x/y的反正切，x和y以弧度为单位
# cos(x) x的余弦，x以弧度为单位
# exp(x) x的指数函数
# int(x) x的整数部分，取靠近零一侧的值
# log(x) x的自然对数
# rand( ) 比0大比1小的随机浮点值
# sin(x) x的正弦，x以弧度为单位
# sqrt(x) x的平方根
# srand(x) 为计算随机数指定一个种子值


# x = int(10 * rand())   产生0-9之间的随机数
gawk -F: '{x = int(10 * rand()); print x}' test_file/num.txt

# and(v1, v2)：执行值v1和v2的按位与运算。
# compl(val)：执行val的补运算。
# lshift(val, count)：将值val左移count位。
# or(v1, v2)：执行值v1和v2的按位或运算。
# rshift(val, count)：将值val右移count位。
# xor(v1, v2)：执行值v1和v2的按位异或运算。


# 字符串函数
# asort(s [,d]) 将数组s按数据元素值排序。索引值会被替换成表示新的排序顺序的连续数字。另外，如果指定了d，则排序后的数组会存储在数组d中
# asorti(s [,d]) 将数组s按索引值排序。生成的数组会将索引值作为数据元素值，用连续数字索引来表明排序顺序。另外如果指定了d，排序后的数组会存储在数组d中
# gensub(r, s, h [, t]) 查找变量$0或目标字符串t（如果提供了的话）来匹配正则表达式r。如果h是一个以g 或G开头的字符串，就用s替换掉匹配的文本。如果h是一个数字，它表示要替换掉第h 处r匹配的地方
# gsub(r, s [,t]) 查找变量$0或目标字符串t（如果提供了的话）来匹配正则表达式r。如果找到了，就全部替换成字符串s
# index(s, t) 返回字符串t在字符串s中的索引值，如果没找到的话返回0
# length([s]) 返回字符串s的长度；如果没有指定的话，返回$0的长度
# match(s, r [,a]) 返回字符串s中正则表达式r出现位置的索引。如果指定了数组a，它会存储s中匹配正则表达式的那部分
# split(s, a [,r]) 将s用FS字符或正则表达式r（如果指定了的话）分开放到数组a中。返回字段的总数
# sprintf(format, variables) 用提供的format和variables返回一个类似于printf输出的字符串
# sub(r, s [,t]) 在变量$0或目标字符串t中查找正则表达式r的匹配。如果找到了，就用字符串s替换掉第一处匹配
# substr(s, i [,n]) 返回s中从索引值i开始的n个字符组成的子字符串。如果未提供n，则返回s剩下的部分
# tolower(s) 将s中的所有字符转换成小写
# toupper(s) 将s中的所有字符转换成大写

gawk 'BEGIN{x = "testing"; print toupper(x); print length(x) }'

# mktime(datespec) 将一个按YYYY MM DD HH MM SS [DST]格式指定的日期转换成时间戳值①
# strftime(format [,timestamp])  将当前时间的时间戳或timestamp（如果提供了的话）转化格式化日期（采用shell函数date()的格式）
# systime( ) 返回当前时间的时间戳

