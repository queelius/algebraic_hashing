#include <cmph/cmph.h>
#include <vector>
#include <string_view>

class CmphVectorKeySource
{
public:
    ~CmphVectorKeySource()
    {
        cmph_io_vector_adapter_destroy(_source);
    };

    CmphVectorKeySource(char** keys, size_t nkeys)
    {
        _source = cmph_io_vector_adapter(keys, nkeys)   
    };

    CmphVectorKeySource(std::vector<std::string_view> keys)
    {
        _source = cmph_io_vector_adapter(keys.data(), keys.size());
    };

    cmph_io_adapter_t* raw()
    {
        return _source;
    };

    class ForwardIterator
    {  
    public:
        Iterator(cmph_io_adapter_t* source)
        {
        };

        Iterator(cmph_io_adapter_t* source, uint32_t offset = 0)
        {
        };

        BidirectionalIterator& operator++()
        {
            _source->read
        };

        bool operator==(const Iterator& rhs) const
        {
            return _source->nkeys == rhs._source->nkeys;
        };

        bool operator!=(const Iterator& rhs) const
        {
            return _source->nkeys != rhs._source->nkeys;
        };

    Iterator begin()
    {
        _source->rewind(_source);
        return Iterator(_source);
    };

    Iterator end()
    {
        return Iterator(nullptr);
    };


    private:
        cmph_io_adapter_t* _source;
    };

private:
    cmph_io_adapter_t* _source;
}














/*

cmph_io_adapter_t* cmph_io_vector_adapter(char** vector, uint32_t nkeys)
{
    cmph_io_adapter_t* key_source = cmph_io_vector_new(vector, nkeys);
    key_source->read = key_vector_read;
    key_source->dispose = key_vector_dispose;
    key_source->rewind = key_vector_rewind;
    return key_source;
}

void cmph_io_vector_adapter_destroy(cmph_io_adapter_t* key_source)
{
    cmph_io_vector_destroy(key_source);
}

static cmph_io_adapter_t* cmph_io_iterator_new(void* vector, uint32_t nkeys)
{
    cmph_io_adapter_t* key_source
        = (cmph_io_adapter_t*)malloc(sizeof(cmph_io_adapter_t));
    cmph_vector_t* cmph_vector = (cmph_vector_t*)malloc(sizeof(cmph_vector_t));
    assert(key_source);
    assert(cmph_vector);
    cmph_vector->vector = vector;
    cmph_vector->position = 0;
    key_source->data = (void*)cmph_vector;
    key_source->nkeys = nkeys;
    return key_source;
}

static void cmph_io_vector_destroy(cmph_io_adapter_t* key_source)
{
    cmph_vector_t* cmph_vector = (cmph_vector_t*)key_source->data;
    cmph_vector->vector = NULL;
    free(cmph_vector);
    free(key_source);
}

static void key_vector_rewind(void* data)
{
    cmph_vector_t* cmph_vector = (cmph_vector_t*)data;
    cmph_vector->position = 0;
}

static int key_vector_read(void* data, char** key, uint32_t* keylen)
{
    cmph_vector_t* cmph_vector = (cmph_vector_t*)data;
    char** keys_vd = (char**)cmph_vector->vector;
    size_t size;
    *keylen = (uint32_t)strlen(keys_vd[cmph_vector->position]);
    size = *keylen;
    *key = (char*)malloc(size + 1);
    strcpy(*key, keys_vd[cmph_vector->position]);
    cmph_vector->position = cmph_vector->position + 1;
    return (int)(*keylen);
}

static void key_vector_dispose(void* data, char* key, uint32_t keylen)
{
    free(key);
}
*/
