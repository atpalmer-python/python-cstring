# cstring Module

Alternate string representation to the built-in `str` type.

* Uses C-string representation internally.
* Memory is allocated in one continuous buffer to reduce pointer-hopping.
* UTF-8 encoding.
* `len` returns size in _bytes_ (not including terminating zero-byte).
* Random access (to _bytes_, *not* Unicode code points) is supported with indices and slices.

## Methods


### count(substring [,start [,end]])

See: https://docs.python.org/3/library/stdtypes.html#str.count

Notes:

* `substring` may be a `cstring` or Python `str` object.
* `start` and `end`, if provided, are _byte_ indexes.


### find(substring [,start [,end]])

See: https://docs.python.org/3/library/stdtypes.html#str.find

Notes:

* `substring` may be a `cstring` or Python `str` object.
* `start` and `end`, if provided, are _byte_ indexes.


### index(substring [,start [,end]])

See: https://docs.python.org/3/library/stdtypes.html#str.index

Notes:

* `substring` may be a `cstring` or Python `str` object.
* `start` and `end`, if provided, are _byte_ indexes.


## TODO

* Write docs (see `str` type docs)
* Write docstrings
* Fill out setup.py
* Allow initialization from bytes, bytearray, other cstrings, memoryview?, other?
* Read `__cstring__` "dunder" on objects, if available?
* Implement iter (iterate over Unicode code points, "runes")
* Implement str methods
* Include start/end indexes as byte indexes? Calculate code points? Or just don't support?
* Implement buffer interface?
* Decide subclassing protocol
