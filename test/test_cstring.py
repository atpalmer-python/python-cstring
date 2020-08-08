import pytest
from cstring import cstring


def test_str():
    result = cstring('hello, world')
    assert str(result) == 'hello, world'


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
    assert str(result) == str(cstring(''))  # TODO: implement cstring equality!


def test_repeat_five():
    result = cstring('hello') * 5
    assert str(result) == str(cstring('hellohellohellohellohello'))  # TODO: implement cstring equality!


def test_item_IndexError_too_small():
    with pytest.raises(IndexError):
        result = cstring('hello')[-6]


def test_item_ok_neg():
    result = cstring('hello')[-5]
    assert isinstance(result, cstring)
    assert str(result) == str(cstring('h'))  # TODO: implement cstring equality!


def test_item_IndexError_too_big():
    with pytest.raises(IndexError):
        result = cstring('hello')[5]


def test_item_ok_pos():
    result = cstring('hello')[4]
    assert isinstance(result, cstring)
    assert str(result) == str(cstring('o'))  # TODO: implement cstring equality!

