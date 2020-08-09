from cstring import cstring


def test_len():
    result = cstring('hello, world')
    assert len(result) == 12

