
-- export LUA_CPATH='$LUA_CPATH:/usr/local/lib/?.so;;'

package.path = package.path .. ";/usr/local/lualibs/?.lua;"
package.cpath = package.cpath .. ";/usr/local/lib/?.so;"

local enc = require("encrypt")

print(enc.md5("22222"))
