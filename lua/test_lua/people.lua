local people = {
    age = nil,
    sex = nil,
    name = nil
}

people.__index = people

function people:new(name, age, sex)
    local pp = {}
    setmetatable(pp, self)

    pp.name = name
    pp.age = age
    pp.sex = sex
    return pp
end

function people:print()
    print("name: " .. self.name)
    print("age: " .. self.age)
    print("sex: " .. self.sex)
    print()
end

function people:setAge(age)
    self.age = age
end

function people:setSex(sex)
    self.sex = sex
end

function people:ageAdd(num)
    self.age = self.age + num
end

return people