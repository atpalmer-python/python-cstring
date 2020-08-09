from cstring import cstring


def test_eq():
    a = cstring('hello')
    b = cstring('hello')
    assert a is not b
    assert a == b


def test_neq():
    assert cstring('hello') != cstring('world')


def test_ne():
    assert cstring('hello') != cstring('world')


def test_lt():
    assert cstring('a') < cstring('b')
    assert cstring('a') < cstring('aa')


def test_le():
    assert cstring('a') <= cstring('a')
    assert cstring('a') <= cstring('b')


def test_gt():
    assert cstring('b') > cstring('a')
    assert cstring('aa') > cstring('a')


def test_ge():
    assert cstring('a') >= cstring('a')
    assert cstring('b') >= cstring('a')

