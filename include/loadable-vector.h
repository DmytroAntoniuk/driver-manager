#pragma once

template <class T>
char *vector_save(std::vector<T> *v, char *p)
{
    size_t used = v->size() * sizeof(T);
    size_t val  = v->size();

    int *pi = reinterpret_cast<int *>(p);
    *pi++   = static_cast<int>(used);
    p += sizeof(int);
    *pi++ = static_cast<int>(val);
    p += sizeof(int);
    memcpy(p, &v->front(), used);
    p += used;
    return p;
}

template <class T>
char *vector_load(std::vector<T> *v, char *p)
{
    size_t sz = 0, num = 0;

    int *pi = reinterpret_cast<int *>(p);
    sz      = *pi++;
    p += sizeof(int);
    num = *pi++;
    p += sizeof(int);
    if (!num)
        num = sz;
    v->resize(num);
    memcpy(v->data(), p, sz);
    p += sz;
    return p;
}

template <class T>
class loadable_vector : public std::vector<T>
{
public:
    char *savedata(char *p) { return vector_save(this, p); }
    char *loaddata(char *p) { return vector_load(this, p); }
};
