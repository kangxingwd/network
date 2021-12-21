
--[[
print_dump是一个用于调试输出数据的函数，能够打印出nil,boolean,number,string,table类型的数据，以及table类型值的元表
参数data表示要输出的数据
参数showMetatable表示是否要输出元表
参数lastCount用于格式控制，用户请勿使用该变量
]]
local function print_dump(data, showMetatable, lastCount)
    if type(data) ~= "table" then
        --Value
        if type(data) == "string" then
            io.write("\"", data, "\"")
        else
            io.write(tostring(data))
        end
    else
        --Format
        local count = lastCount or 0
        count = count + 1
        io.write("{\n")
        --Metatable
        if showMetatable then
            for i = 1,count do io.write("\t") end
            local mt = getmetatable(data)
            io.write("\"__metatable\" = ")
            print_dump(mt, showMetatable, count)    -- 如果不想看到元表的元表，可将showMetatable处填nil
            io.write(",\n")     --如果不想在元表后加逗号，可以删除这里的逗号
        end
        --Key
        for key,value in pairs(data) do
            for i = 1,count do io.write("\t") end
            if type(key) == "string" then
                io.write("\"", key, "\" = ")
            elseif type(key) == "number" then
                io.write("[", key, "] = ")
            else
                io.write(tostring(key))
            end
            print_dump(value, showMetatable, count) -- 如果不想看到子table的元表，可将showMetatable处填nil
            io.write(",\n")     --如果不想在table的每一个item后加逗号，可以删除这里的逗号
        end
        --Format
        for i = 1,lastCount or 0 do io.write("\t") end
        io.write("}")
    end
    --Format
    if not lastCount then
        io.write("\n")
    end
end

local function urlEncode(s)
    s = string.gsub(s, "([^%w%.%- ])", function(c) return string.format("%%%02X", string.byte(c)) end)
    return string.gsub(s, " ", "+")
end

local function urlDecode(s)
    s = string.gsub(s, "+", " ")
    s = string.gsub(s, '%%(%x%x)', function(h) return string.char(tonumber(h, 16)) end)
    return s
end

local function split(s, sep, whole)
    local array = {}
    if not s then
        return array
    end

    sep = sep or " "
    if whole ~= false then
        whole = true
    end

    local culIndex = 1;
    while culIndex <= #s do
        local strEnd = string.find(s, sep, culIndex)
        if not strEnd then
            break
        end

        if strEnd == culIndex then
            if whole then
                table.insert(array, "")
            end
        else
            local subStr = string.sub(s, culIndex, strEnd -1)
            table.insert(array, subStr)
        end
        culIndex = strEnd + 1
    end
    return array
end


local function spilt2(str,mode)
    local tab,cnt={},1
    for i in string.gmatch(str,string.format(".-%%f[%s]",mode,mode)) do
      tab[cnt]=i
      cnt=cnt+1
    end
    for i in string.gmatch(str,string.format("[^%s]+$",mode)) do
      tab[cnt]=i
      cnt=cnt+1
    end
    return tab
end

return {
    print_dump = print_dump,
    urlEncode = urlEncode,
    urlDecode = urlDecode,
    split = split,
    spilt2 = spilt2
}