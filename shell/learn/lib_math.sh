

function add {
    echo $(($1 + $2))
}

function mult {
    echo $(($1 * $2))
}

function div {
    if [ $2 -ne 0 ]; then
        echo $(($1 / $2))
    else
        echo -1
    fi
}

# 阶乘
function factorial {
    if [ $1 -eq 1 ]; then
        echo 1
    else
        # local next=$[ $1 - 1 ]
        # local ret=$(factorial $next)
        # echo $[ $1 * $ret ]
        echo $(($1 * $(factorial $(($1 - 1)))))
    fi
}
