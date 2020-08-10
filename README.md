# cstring Module

Alternate string representation to the built-in `str` type.

* Uses C-string representation internally.
* Memory is allocated in one continuous buffer to reduce pointer-hopping.
* UTF-8 encoding.
* `len` returns size in _bytes_ (not including terminating zero-byte).
* Random access (to _bytes_, *not* Unicode code points) is supported with indices and slices.

## TODO

* Write docs (see `str` type docs)
* Write docstrings
* Fill out setup.py
* Implement iter (iterate over Unicode code points, "runes")
* Implement str methods
* Implement buffer interface?
* Decide subclassing protocol
