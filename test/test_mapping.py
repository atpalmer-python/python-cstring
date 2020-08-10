from cstring import cstring


def test_len():
    result = cstring('hello, world')
    assert len(result) == 12


def test_subscript():
    assert cstring('hello')[1] == cstring('e')


def test_subscript_slice_empty():
    assert cstring('hello')[2:1] == cstring('')


def test_subscript_slice():
    assert cstring('hello')[1:4] == cstring('ell')


def test_subscript_slice_skip():
    assert cstring('hello, world')[1:-1:2] == cstring('el,wr')


def test_subscript_slice_skip_back():
    assert cstring('hello, world')[-1:3:-3] == cstring('do,')

