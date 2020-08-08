from cstring import cstring


def test_str():
    result = cstring('hello, world')
    assert str(result) == 'hello, world'


def test_len():
    result = cstring('hello, world')
    assert len(result) == 12

