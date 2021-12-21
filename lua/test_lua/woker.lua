local PEOPLE = require("people")

local worker = {}
worker.__index = worker
setmetatable(worker, PEOPLE)

function worker:new(name, age, sex, id)
    local workPeople = PEOPLE:new(name, age, sex)
    workPeople.id = id

    setmetatable(workPeople, worker)
    return workPeople
end


function worker:print()
    print("name: " .. self.name)
    print("age: " .. self.age)
    print("sex: " .. self.sex)
    print("id: " .. self.id)
    print()
end

return worker