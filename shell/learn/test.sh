#!/bin/bash

# $0 当前脚本文件名称
# $$ 当前脚本进程PID
# $# 参数数量
# ${!#} 最后一个参数,  不能用 ${$#}
# $* 所有参数的字符串
# $@ 所有参数， 分开保存在字符串中， 可以用 for 进行遍历
# shift 移动命令行参数, 默认移动一位, shift n 可以移动多位

echo "\$0: $0"
echo "\$\$: $$"
echo "\$#: $#"
echo "\$*: $*"
echo "\$@: $@"

echo $(basename $0)

count=1
for param in $@; do
    echo "param $count: $param"
    # count=$((count+1))
    count=$(($count + 1))
done

func1() {
    echo "enter func1"
}

function func2 {
    echo "enter func2"
    # 返回的值必须在 0-255 之间
    return 222
}

func1

# $? 获取的是return的值
func2
ret=$?
echo "func2 ret1 = $ret"

# $(func) 获取的是函数中所有 echo 的值
ret=$(func2)
echo "func2 ret2 = $ret"

# `func` 获取的是函数中所有 echo 的值
ret=$(func2)
echo "func2 ret3 = $ret"

function add {
    if [ $# -eq 0 -o $# -gt 2 ]; then
        echo -1
    elif [ $# -eq 1 ]; then
        echo $(($1 + $1))
    else
        echo $(($1 + $2))
    fi
}

ret=$(add 2)
echo "add 2 = $ret"
ret=$(add 2 3)
echo "add 2 3 = $ret"
ret=$(add 3 4 5)
echo "add 3 4 5 = $ret"

# 导入库， . 是 source 命令的缩写
. lib_math.sh

factorial 4

# 小数， 浮点数，scale=4 代表设置保留小数点后多少位
var1=100 
var2=45 
var3=$(echo "scale=4; $var1 / $var2" | bc) 
echo The answer for this is $var3

# 两个方括号中可以使用模式匹配字符串， 如果给模式匹配的字符串加上双引号不会进行模式匹配变成字符串匹配
# if [[ $UU = "r*" ]]  将不会匹配
UU=rrr11
if [[ $UU = r* ]]; then
    echo "Hello $UU"
else
    echo "Sorry, I do not know you, $UU"
fi

if [ x"$UU" = x"$KX" ]; then
    echo "Hello $UU"
else
    echo "Sorry, I do not know you, $UU"
fi

# case, 可以使用 | 和 * 进行模式匹配
case $USER in
rich | barbara)
    echo "Welcome, $USER"
    echo "Please enjoy your visit"
    ;;
testing)
    echo "Special testing account"
    ;;
jessica)
    echo "Do not forget to log off when you're done"
    ;;
*)
    echo "Sorry, you are not allowed here"
    ;;
esac

# for, 默认将空格、制表符、换行符当作字段的分隔符
list="Alabama Alaska Arizona Arkansas xxxx Colorado"
list=$list" Connecticut"
for test in $list; do
    echo "The next2 state is $test "
done

for i in $(seq -1 10); do
    echo file_$i
done

# for, 默认将空格、制表符、换行符当作字段的分隔符; 想要修改默认值，可修改 IFS 环境变量
IFS_OLD=$IFS
IFS=$'\n':
# <在代码中使用新的IFS值>
IFS=$IFS_OLD

IFS_OLD=$IFS
IFS=$'\n'
echo "file: tt.txt  -----------------"
for line in $(cat test_file/tt.txt); do
    echo $line
done
IFS=$IFS_OLD

# 遍历目录下文件
for file in $(ls .); do
    echo $file
done

for file in ./* /mnt/h/code/git/network/shell/*; do
    if [ -d "$file" ]; then
        echo "dir:  $file"
    elif [ -f "$file" ]; then
        echo "file: $file"
    fi
done

# for 循环
for ((i = 1; i <= 5; i++)); do
    echo "$i"
done

for ((a = 1, b = 2; a <= 4; a++, b += 2)); do
    echo "$a + $b = $(($a + $b))"
    echo "$a^2 + $b^2 = $(($a ** 2 + $b ** 2))"
    echo "$a^2 + $b^2 = $(($a ** 2 + $b ** 2))"
    echo "$a&$b = $(($a & $b))"
    echo "$a|$b = $(($a | $b))"
done

var=10
while [ $var -gt 0 ]; do
    echo "var: $var"
    var=$(($var - 1))
done

var=10
until [ $var -eq 0 ]; do
    echo "var: $var"
    var=$(($var - 1))
done

# 跳出循环，break,  默认只跳出当前循环，想要跳出外部多层循环，可指定跳出的循环层级 break n
for ((a = 1; a < 4; a++)); do
    echo "Outer loop: $a"
    for ((b = 1; b < 100; b++)); do
        if [ $b -gt 4 ]; then
            break 2
        fi
        echo " Inner loop: $b"
    done
done

# 继续循环， continue, continue 命令也允许通过命令行参数指定要继续执行哪一级循环 continue n
for ((a = 1; a <= 5; a++)); do
    echo "Iteration $a:"
    for ((b = 1; b < 3; b++)); do
        if [ $a -gt 2 ] && [ $a -lt 4 ]; then
            continue 2
        fi
        var3=$(($a * $b))
        echo " The result of $a * $b is $var3"
    done
done

# 循环的输出重定向，
for file in $(ls .); do
    echo $file
done >test_file/file.txt

for state in "North Dakota" Connecticut Illinois Alabama Tennessee; do
    echo "$state is the next place to go"
done | sort

# # 获取可执行
# IFS_OLD=$IFS
# IFS=:
# for path in $PATH; do
#     echo $path
#     for exec_file in "$path"/*; do
#         if [ -x "$exec_file" ]; then
#             echo "  $exec_file"
#         fi
#     done
# done >test_file/bin_file
# IFS=$IFS_OLD

# 读取每行
input="test_file/users.csv"
line_num=1
cat $input | while read line; do
    echo "line_num $line_num: $line"
    line_num=$(($line_num + 1))
done

while read -r line; do
    echo $line
done <"$input"

while IFS=',' read -r userid name; do
    # echo "name: $name, userid: $userid"  # 不知道这个为啥不行
    echo "userid: $userid, name: $name"
done <"$input"

# shell注释
: <<XXXXX
echo "-----------------------"

XXXXX

# 处理命令行参数
# :ab:c 的意思：
#   第一个: 表示去掉错误消息；
#   a  代表有个 -a 选项
#   b: 代表有个-b选项且带携带一个参数
#   c: 和a一样， 代表有个 -c 选项
while getopts :ab:c opt; do
    case "$opt" in
    a) echo "Found the -a option" ;;
    b) echo "Found the -b option, with value $OPTARG" ;;
    c) echo "Found the -c option" ;;
    *) echo "Unknown option: $opt" ;;
    esac
done

# 帮助文档
help() {
    cat <<EOF
$0 [OPTION]
OPTION:
    -a          xxx
    -b          xxx
    -c          xxx
EOF
}

# 用户输入
# read -p "Please Enter your username: " name
# read -s -p "Enter your password: " pass
# echo "user: $name, pass: $pass"

# 错误重定向
ls -l xxx >test_file/err.log 2>&1
ls -l xxx 1>test_file/out.log 2>test_file/err.log

# # 创建临时文件 (-t选项会强制mktemp命令来在系统的临时目录来创建该文件)
# tempfile=$(mktemp -t tmp.XXXXXX)
# # 创建临时目录
# tempdir=$(mktemp -d dir.XXXXXX)

# # 例子
# outfile='test_file/members.sql'
# IFS=','
# while read lname fname address city state zip; do
#     cat >>$outfile <<EOF
# INSERT INTO members (lname,fname,address,city,state,zip) VALUES ('$lname', '$fname', '$address', '$city', '$state', '$zip');
# EOF
# done <${1}



# 信号捕获
# trap "echo Goodbye..." EXIT
trap "echo Goodbye..." EXIT

# 恢复信号默认行为 --
trap -- SIGINT
echo "end---"

# # 后台运行，终端退出不关闭
# nohup ./test1.sh &

# # 调整进程优先级 [-20,19] , -20 优先级最高
# nice -n 10 ./test4.sh > test4.out &
# ps -p [pid] -o pid,ppid,ni,cmd
# 重新设置优先级
# renice -n 0 -p [pid]

# 数组
myarray=(1 2 3 4 5)

# 函数传递数组参数
function addarray {
    # local array=($(echo "$@"))
    local array=($@)
    local num=0
    for val in ${array[*]}; do
        echo "val: $val"
        num=$[ $num + $val ]
    done
    echo "$num"
}
array=(1 2 3 4 5)
addarray ${array[*]}

# 函数返回数组
function testarray {
    # local array=($(echo "$@"))
    local array=($@)
    local new_array=()
    local num=0
    for ((i = 0; i <= $[ $# - 1 ]; i++));do
        new_array[$i]=$(( ${array[$i]} * 2 ))
    done
    echo ${new_array[*]}
}
array=(1 2 3 4 5)
ret=$(testarray ${array[*]})
echo "ret = ${ret[*]}"


# gawk
echo "My name is Rich" | gawk '{$4="Christine"; print $0}'

# gawk -F: 'BEGIN {$1} {print $1 $7} END {}'

# BEGIN 和 END 分别在遍历文件前后执行命令
gawk -F: 'BEGIN {print "---start----"} {print $1,$7} END {print "---end--"}' /etc/passwd

