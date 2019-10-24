#include <perfect_hashing/MphBuilder.h>

alex::hashing::MphBuilder& alex::hashing::MphBuilder::set_key_source(
    FILE* file, size_t nkeys)
{
    if (nkeys == 0)
        _keysource = cmph_io_nlfile_adapter(file);
    else
        _keysource = cmph_io_nlnkfile_adapter(file, nkeys);
    return *this;
}

alex::hashing::MphBuilder& alex::hashing::MphBuilder::set_key_source(
    unsigned char** keys, size_t nkeys)
{
    _keysource = cmph_io_vector_adapter(keys);
    return *this;
}

alex::hashing::MphBuilder& alex::hashing::MphBuilder::set_verbose(bool on)
{
    _verbose = on;
    return *this;
}

alex::hashing::MphBuilder& alex::hashing::MphBuilder::set_load_factor(
    double load)
{
    if (load < .5)
        load = .5;
    if (load > 1)
        load = 1.;
    _load = load;

    return *this;
}

alex::hashing::MphBuilder& alex::hashing::MphBuilder::set_algorithm(
    CMPH_ALGO algo)
{
    _algo = algo;
}

alex::hashing::Mph alex::hashing::MphBuilder::build()
{
    if (_keysource == nullptr)
        return Mph();

    auto config = cmph_config_new();
    if (verbose)
        cmph_config_set_verbosity(config, 0xFFFF);
    else
        cmph_config_set_verbosity(config, 0);

    cmph_config_set_algo(config, _algo);
    cmph_config_set_load_factor(config, _load);

    auto mphf = cmph_new(config, _keysource);
    auto size = cmph_packed_size(mphf);
    auto packed_mphf = new char[size];

    cmph_pack(mphf, packed_mphf);
    cmph_config_destroy(config);
    cmph_destroy(mphf);

    _keysource->dispose();
    _keysource = nullptr;

    return Mph(packed_mphf, size);
}

/*
template <typename Iterator>
alex::hashing::MphBuilder& alex::hashing::MphBuilder::set_key_source(
    Iterator begin, Iterator end)
{
    const auto& n = end - begin;
    if (n == 0)

        auto data = new char*[n];
    size_type i = 0;
    for (auto it = begin; it != end; ++it)
    {
        data[i] = new char[it->size() + 1];
        std::memcpy(data[i], it->c_str(), it->size());
        data[i][it->size()] = 0;
    }
    for (size_t i = 0; i < n; ++i)
        delete[] data[i];
    delete[] data;
}
*/