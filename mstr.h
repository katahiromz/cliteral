// mstr.h --- C++ string class manipulation
// License: MIT
//////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename T_STR_CONTAINER>
void mstr_split(T_STR_CONTAINER& container,
                const typename T_STR_CONTAINER::value_type& str,
                const typename T_STR_CONTAINER::value_type::value_type ch)
{
    container.clear();
    size_t i = 0, k = str.find_first_of(ch);
    while (k != T_STR_CONTAINER::value_type::npos)
    {
        container.push_back(str.substr(i, k - i));
        i = k + 1;
        k = str.find_first_of(ch, i);
    }
    container.push_back(str.substr(i));
}

template <typename T_STR_CONTAINER>
void mstr_split(T_STR_CONTAINER& container,
                const typename T_STR_CONTAINER::value_type& str,
                const typename T_STR_CONTAINER::value_type& chars)
{
    container.clear();
    size_t i = 0, k = str.find_first_of(chars);
    while (k != T_STR_CONTAINER::value_type::npos)
    {
        container.push_back(str.substr(i, k - i));
        i = k + 1;
        k = str.find_first_of(chars, i);
    }
    container.push_back(str.substr(i));
}

template <typename T_STRING>
void mstr_trim(T_STRING& str, const typename T_STRING::value_type ch)
{
    const auto i = str.find_first_not_of(ch);
    const auto j = str.find_last_not_of(ch);
    if ((i == str.npos) || (j == str.npos))
    {
        str.clear();
    }
    else
    {
        str = str.substr(i, j - i + 1);
    }
}

template <typename T_STRING>
void mstr_trim(T_STRING& str, const typename T_STRING::value_type *spaces)
{
    const auto i = str.find_first_not_of(spaces);
    const auto j = str.find_last_not_of(spaces);
    if ((i == str.npos) || (j == str.npos))
    {
        str.clear();
    }
    else
    {
        str = str.substr(i, j - i + 1);
    }
}

template <typename T_STR_CONTAINER>
inline typename T_STR_CONTAINER::value_type
mstr_join(const T_STR_CONTAINER& container,
          const typename T_STR_CONTAINER::value_type& sep)
{
    typename T_STR_CONTAINER::value_type result;
    typename T_STR_CONTAINER::const_iterator it, end;
    it = container.begin();
    end = container.end();
    if (it != end)
    {
        result = *it;
        for (++it; it != end; ++it)
        {
            result += sep;
            result += *it;
        }
    }
    return result;
}

template <typename T_STR>
bool mstr_replace_all(T_STR& str, const T_STR& from, const T_STR& to)
{
    bool ret = false;
    size_t i = 0;
    for (;;) {
        i = str.find(from, i);
        if (i == T_STR::npos)
            break;
        ret = true;
        str.replace(i, from.size(), to);
        i += to.size();
    }
    return ret;
}

template <typename T_STR>
inline bool mstr_replace_all(T_STR& str,
                             const typename T_STR::value_type *from,
                             const typename T_STR::value_type *to)
{
    return mstr_replace_all(str, T_STR(from), T_STR(to));
}
