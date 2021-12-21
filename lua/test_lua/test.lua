local PEOPLE = require("people")
local WORKER = require("woker")

local pp = PEOPLE:new("kx", 10, "boy")

pp:print()

pp:setAge(11)

pp:print()

pp:ageAdd(10)

pp:print()



local worker1 = WORKER:new("wd", 10, "girl", 10001)

worker1:print()

worker1:setAge(11)

worker1:ageAdd(11)

worker1:print()

