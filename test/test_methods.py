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


def test_islower_numeric():
    target = cstring('123')
    assert target.islower() == False


def test_islower_alnum_lc():
    target = cstring('hello123')
    assert target.islower() == True


def test_islower_alnum_uc():
    target = cstring('Hello123')
    assert target.islower() == False


def test_isprintable_True():
    target = cstring('hello, world')
    assert target.isprintable() == True


def test_isprintable_False():
    target = cstring(b'hello, world\x01')
    assert target.isprintable() == False


def test_isspace_empty():
    target = cstring('')
    assert target.isspace() == False


def test_isspace_True():
    target = cstring('\t\n ')
    assert target.isspace() == True


def test_isspace_False():
    target = cstring('hello, world\n')
    assert target.isspace() == False


def test_isupper_numeric():
    target = cstring('123')
    assert target.isupper() == False


def test_isupper_alnum_uc():
    target = cstring('HELLO123')
    assert target.isupper() == True


def test_isupper_alnum_lc():
    target = cstring('HELLo123')
    assert target.isupper() == False


def test_join():
    sep = cstring(', ')
    items = [cstring('hello'), cstring('world')]
    assert sep.join(items) == cstring('hello, world')


def test_lower():
    target = cstring('HELLO123')
    assert target.lower() == cstring('hello123')


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


def test_strip_noargs():
    target = cstring('\r\n\n\n\t  hello, world  \t\r\n\n  ')
    assert target.strip() == cstring('hello, world')


def test_strip_None():
    target = cstring('\r\n\n\n\t  hello, world  \t\r\n\n  ')
    assert target.strip(None) == cstring('hello, world')


def test_strip_arg():
    target = cstring('hello, world')
    assert target.strip('held') == cstring('o, wor')


def test_lstrip_arg():
    target = cstring('hello, world')
    assert target.lstrip('held') == cstring('o, world')


def test_rstrip_arg():
    target = cstring('hello, world')
    assert target.rstrip('held') == cstring('hello, wor')


def test_partition():
    target = cstring('hello, world')
    result = (cstring('hello'), cstring(', '), cstring('world'))
    assert target.partition(cstring(', ')) == result


def test_partition_sep_not_found():
    target = cstring('hello, world')
    result = (cstring('hello, world'), cstring(''), cstring(''))
    assert target.partition(cstring(': ')) == result


def test_rpartition():
    target = cstring('hello, world')
    result = (cstring('hello, wor'), cstring('l'), cstring('d'))
    assert target.rpartition(cstring('l')) == result


def test_rpartition_sep_not_found():
    target = cstring('hello, world')
    result = (cstring(''), cstring(''), cstring('hello, world'))
    assert target.rpartition(cstring(': ')) == result


def test_split():
    assert cstring('hello, world').split(cstring(', ')) == [cstring('hello'), cstring('world')]
    assert cstring('1,2,3').split(cstring(',')) == [
        cstring('1'), cstring('2'), cstring('3')]
    assert cstring('hello, world').split() == [cstring('hello,'), cstring('world')]
    assert cstring('hello\t \n  world').split() == [cstring('hello'), cstring('world')]
    assert cstring('1,2,3').split(cstring(','), maxsplit=1) == [
        cstring('1'), cstring('2,3')]
    assert cstring('1   2   3').split(maxsplit=1) == [
        cstring('1'), cstring('2   3')]


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


def test_upper():
    target = cstring('hello123')
    assert target.upper() == cstring('HELLO123')


def test_swapcase():
    target = cstring('hElLo, WoRlD 123')
    assert target.swapcase() == cstring('HeLlO, wOrLd 123')

