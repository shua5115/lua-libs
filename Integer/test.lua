---@diagnostic disable: lowercase-global
local int = require "integer"

assert(type(int) == "function")

local mt = getmetatable(int())
print("Integer metatable:", mt)
for k, v in pairs(mt) do
    print(k, v)
end

local a = int()
assert(a == int(0)) -- Light userdata do not allow overriding the eq method... that sucks
local b = int(5)
a = int(3)

assert(-a == int(-3))
assert(a + b == int(8))
assert(a - b == int(-2))
assert(a * b == int(15))
assert(a / b == int(0))
assert(b / a == int(1))
assert(b % a == int(2))
assert(b ^ a == int(125))
assert(b:shl(a) == int(40)) -- 0b101 << 3 = 0b101000 = 32 + 8 = 40
assert(b:shr(1) == int(2)) -- 0b101 >> 1 = 0b010
assert(b:band(a) == int(1)) -- 0b101 & 0b011 = 0b001
assert(b:bor(a) == int(7)) -- 0b101 | 0b011 = 0b111
assert(b:bxor(a) == int(6)) -- 0b101 ^ 0b011 = 0b110
-- assert(b:bnot() == int(2)) -- depends on how many bits in an ptrdiff_t
assert(int(-1):bnot() == int(0))
assert(a:min(b) == a)
assert(a:max(b) == b)
assert(mt.abs(-a) == a)
assert(a:pow(2) == int(9))
assert(b:sqrt() == int(2))
assert(not pcall(function() return a/int(0) end))
assert(not pcall(function() return a%int(0) end))

local function numeric_for_test()
    local sum = int()
    for i=int(1), 5 do
        sum = sum + i
    end
    return sum
end

local success = pcall(numeric_for_test)
print("Numeric for loop syntax " .. (success and "works" or "doesn't work"))

-- Benchmarks
local curtime = os.clock
local start_time = 0

t1 = 0
t2 = 0
local function tick()
    start_time = curtime()
end
local function tock()
    return curtime() - start_time
end
local function analyze()
    print("Numbers: "..t1.."s")
    print("Integers: "..t2.."s")
    print("Change:", t2/t1)
end
local N = int(1):shl(24):tonumber()
print("Performing operations " .. N .. " times...")
local temp = 0
tick()
for i=1,N do
    temp = temp + 1
    local _ = temp * 2
end
t1 = tock()
temp = int(0)
tick()
for i=1,N do
    temp = temp + 1
    local _ = temp * 2
end
t2 = tock()
analyze()