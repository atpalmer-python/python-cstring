import pytest
from cstring import cstring


def test_str():
    result = cstring('hello, world')
    assert str(result) == 'hello, world'


def test_len():
    result = cstring('hello, world')
    assert len(result) == 12


def test_concat():
    result = cstring('hello') + cstring(', ') + cstring('world')
    assert 'hello, world' == str(result)


def test_concat_TypeError():
    with pytest.raises(TypeError, match='^Object must have type cstring, not str.$'):
        cstring('hello') + 'world'

