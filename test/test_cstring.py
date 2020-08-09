from cstring import cstring


def test_str():
    result = cstring('hello, world')
    assert str(result) == 'hello, world'


def test_hash():
    a = cstring('hello')
    b = cstring('hello')
    assert a is not b
    assert set((a, b)) == set((a,)) == set((b,))

