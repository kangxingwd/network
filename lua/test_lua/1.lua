local common = require("common")

local a = {
    null = nil,
    bool = true,
    num = 20,
    str = "abc",
    subTab = {"111", "222"},
    sunTab = {"sun_a", {"sun_1", "sun_2"}, {you = "god", i = "man"}}
}


--common.print_dump(a)

print(_VERSION)

local co = coroutine.create(function ()
    for i = 1, 10, 2 do
        local curNum = i
        print(curNum)
        coroutine.yield(curNum, i, curNum+1)
    end

end)


print(type(co))

print(coroutine.status(co))

print(coroutine.resume(co))
print(coroutine.resume(co))
print(coroutine.resume(co))
print(coroutine.resume(co))
print(coroutine.resume(co))
print(coroutine.resume(co))

print(coroutine.status(co))


