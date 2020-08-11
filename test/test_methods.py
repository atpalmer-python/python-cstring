from cstring import cstring


def test_count():
    target = cstring('hello, world')
    assert target.count(cstring('l')) == 3


def test_count_start():
    target = cstring('hello, world')
    assert target.count(cstring('l'), 10) == 1


def test_count_end():
    target = cstring('hello, world')
    assert target.count(cstring('l'), 0, 4) == 2


def test_count_str_unicode():
    target = cstring('🙂 🙃 🙂 🙂 🙃 🙂 🙂')
    assert target.count('🙂') == 5

