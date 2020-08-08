from cstring import cstring


def test_str():
    result = cstring('hello, world')
    assert str(result) == 'hello, world'

