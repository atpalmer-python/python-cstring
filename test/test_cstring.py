import pytest
from cstring import cstring


def test_str():
    result = cstring('hello, world')
    assert str(result) == 'hello, world'


def test_hash():
    a = cstring('hello')
    b = cstring('hello')
    assert a is not b
    assert set((a, b)) == set((a,)) == set((b,))


# Rich Compare


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


# Sequence API


def test_len():
    result = cstring('hello, world')
    assert len(result) == 12


def test_concat():
    result = cstring('hello') + cstring(', ') + cstring('world')
    assert 'hello, world' == str(result)


def test_concat_TypeError():
    with pytest.raises(TypeError, match='^Object must have type cstring, not str.$'):
        cstring('hello') + 'world'


def test_repeat_zero():
    result = cstring('hello') * 0
    assert result == cstring('')


def test_repeat_five():
    result = cstring('hello') * 5
    assert result == cstring('hellohellohellohellohello')


def test_item_IndexError_too_small():
    with pytest.raises(IndexError):
        result = cstring('hello')[-6]


def test_item_ok_neg():
    result = cstring('hello')[-5]
    assert isinstance(result, cstring)
    assert result == cstring('h')


def test_item_IndexError_too_big():
    with pytest.raises(IndexError):
        result = cstring('hello')[5]


def test_item_ok_pos():
    result = cstring('hello')[4]
    assert isinstance(result, cstring)
    assert result == cstring('o')


def test_contains_True():
    assert cstring('ell') in cstring('hello')


def test_contains_False():
    assert cstring('hello') not in cstring('world')



