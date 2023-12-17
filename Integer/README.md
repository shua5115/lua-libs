# Integer Library

Uses light userdata instead of full userdata to store integers.
This [one-file lua integer C library](http://codepad.org/LZpFrKbT) was used as a base.
Integers in lua are useful for enforcing that a number will be an integer.
It is easy to accumulate floating point errors when doing math with regular numbers,
so this new type will always convert operands to integers when doing math.
Certain math library functions, like abs, max, and min, are re-implemented for integers.

Light userdata is a bit more restrictive than full userdata, since
the __eq metamethod does not work, it always just does pointer comparison.
In this case that's what we want, it just means you can't convert numbers to integers inline.

Because the type syntax works through metatables, there is a performance hit to
integers when compared to regular numbers.
Using a basic benchmark on addition and multiplication, integers are about 20x slower
when tested on 32-bit LuaForWindows install on a Ryzen 3600 CPU.