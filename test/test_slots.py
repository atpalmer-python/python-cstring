from cstring import cstring


def test_new_from_bytes():
    assert cstring(b'hello, world') == cstring('hello, world')


def test_new_from_bytearray():
    assert cstring(bytearray('hello, world', 'utf8')) == cstring('hello, world')


def test_new_from_cstring():
    assert cstring(cstring('hello, world')) == cstring('hello, world')


def test_str():
    result = cstring('hello, world')
    assert str(result) == 'hello, world'


def test_repr():
    assert repr(cstring('"hello" "world" 🙂')) == repr('"hello" "world" 🙂')


def test_hash():
    a = cstring('hello')
    b = cstring('hello')
    assert a is not b
    assert set((a, b)) == set((a,)) == set((b,))

