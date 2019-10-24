#include <cmph/cmph.h>
#include <perfect_hashing/Mph.h>

alex::hash::Mph::Mph()
    : _packed_mphf(nullptr)
    , _size(0)
{
}

alex::hash::Mph::Mph(void* packed_mphf, size_type size)
    : _packed_mphf(_packed_mphf)
    , _size(size)
{
    // Mph takes over the data pointed to by packed_mphf
}

alex::hash::Mph::Mph(Mph const& copy)
    : _size(copy._size)
    , _packed_mphf(new char[copy._size])
{
    std::memcpy(_packed_mphf, copy._packed_mphf, copy._size);
}

alex::hash::Mph::Mph(Mph&& rval)
    : _size(rval._size)
    , _packed_mphf(rval._packed_mphf)
{
    rval._size = 0;
    rval._packed_mphf = nullptr;
}

alex::hash::Mph& alex::hash::Mph::operator=(Mph& copy)
{
    if (this != &copy)
    {
        erase();
        _size = copy._size;
        _packed_mphf = new char[_size];
        std::memcpy(_packed_mphf, copy._packed_mphf, copy._size);
    }
    return *this;
}

alex::hash::Mph& alex::hash::Mph::operator=(Mph&& rval)
{
    if (this != &rval)
    {
        erase();

        _size = rval._size;
        _packed_mphf = rval._packed_mphf;
        rval._packed_mphf = nullptr;
        rval._size = 0;
    }
    return *this;
}

void alex::hash::Mph::erase()
{
    if (_size != 0)
    {
        delete[] _packed_mphf;
        _packed_mphf = nullptr;
        _size = 0;
    }
}

string_view alex::hash::Mph::data() { return _packed_mphf; }

alex::hash::Mph::size_type alex::hash::Mph::operator()(
    std::string_view key) const
{
    return operator()(key.c_str(), key.size());
}

alex::hash::Mph::size_type alex::hash::Mph::operator()(
    char const* key, size_t len) const
{
    return cmph_search_packed(_packed_mphf, key, len);
}

alex::hash::Mph::size_type alex::hash::Mph::operator()(std::string_view key)
{
    if (_size == nullptr)
        return 0;

    return cmph_search_packed(_packed_mphf, key.data(), key.size());
};

alex::hash::Mph::~Mph() { erase(); }

alex::hash::Mph::hash_type alex::hash::Mph::max_hash() const
{
    return cmph_packed_max_hash(_packed_mphf);
}

std::istream& operator<<(std::istream& in, Mph& mph)
{
    erase();

    in >> mph._size;
    if (_size != 0)
    {
        mph._packed_mphf = new char[mph._size];
        for (size_t i = 0; i < mph._size; ++i)
            in >> mph._packed_mphf[i];
    }
    return in;
}

std::ostream& operator>>(std::ostream& out, const Mph& mph)
{
    out << mph._size;
    for (size_t i = 0; i < mph._size; ++i)
        out << mph._packed_mphf[i];
    return out;
}