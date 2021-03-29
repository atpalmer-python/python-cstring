import pytest
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


def test_find():
    target = cstring('hello')
    assert target.find('lo') == 3


def test_find_with_start():
    target = cstring('hello')
    assert target.find('lo', 3) == 3


def test_find_missing():
    target = cstring('hello')
    assert target.find('lo', 0, 4) == -1


def test_index():
    target = cstring('hello')
    assert target.index('lo') == 3


def test_index_with_start():
    target = cstring('hello')
    assert target.index('lo', 3) == 3


def test_index_missing():
    target = cstring('hello')
    with pytest.raises(ValueError):
        return target.index('lo', 0, 4)


def test_isalnum_True():
    target = cstring('hello123')
    assert target.isalnum() == True


def test_isalnum_False():
    target = cstring('hello_123')
    assert target.isalnum() == False


def test_isalpha_True():
    target = cstring('hello')
    assert target.isalpha() == True


def test_isalpha_False():
    target = cstring('hello123')
    assert target.isalpha() == False


def test_isdigit_True():
    target = cstring('123')
    assert target.isdigit() == True


def test_isdigit_False():
    target = cstring('123.4')
    assert target.isdigit() == False


def test_rfind():
    target = cstring('hello')
    assert target.rfind('o') == 4


def test_rfind_empty():
    target = cstring('hello')
    assert target.rfind('') == 5


def test_rfind_with_start():
    target = cstring('hello')
    assert target.rfind('lo', 3) == 3


def test_rfind_missing():
    target = cstring('hello')
    assert target.rfind('lo', 0, 4) == -1


def test_rindex():
    target = cstring('hello')
    assert target.rindex('o') == 4


def test_rindex_empty():
    target = cstring('hello')
    assert target.rindex('') == 5


def test_rindex_with_start():
    target = cstring('hello')
    assert target.rindex('lo', 3) == 3


def test_rindex_missing():
    target = cstring('hello')
    with pytest.raises(ValueError):
        return target.rindex('lo', 0, 4)


def test_startswith():
    target = cstring('hello, world')
    assert target.startswith('hello,') is True


def test_startswith_not():
    target = cstring('hello, world')
    assert target.startswith('world') is False


def test_startswith_with_start():
    target = cstring('hello, world')
    assert target.startswith('world', 7) is True


def test_startswith_with_start_and_end():
    target = cstring('hello, world')
    assert target.startswith('wo', 7, 8) is False


def test_endswith():
    target = cstring('hello, world')
    assert target.endswith('world') is True


def test_endswith_not():
    target = cstring('hello, world')
    assert target.endswith('hello') is False


def test_endswith_with_start():
    target = cstring('hello, world')
    assert target.endswith('world', 8) is False


def test_endswith_with_start_and_end():
    target = cstring('hello, world')
    assert target.endswith('wo', 7, 9) is True

