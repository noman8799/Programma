#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

const size_t buck_sz = 4;
template <typename T>
class Deque {
public:
    ~Deque() {
        if (sz) {
            if (st != fin) {
                for (size_t j = fin_pos; j < buck_sz; j++)
                    (deq[fin] + j)->~T();
                for (size_t j = 0; j <= fin_pos; j++)
                    (deq[fin] + j)->~T();
                for (size_t j = st + 1; j < fin; j++)
                    for (size_t j1 = 0; j1 < buck_sz; ++j1)
                        (deq[j] + j1)->~T();
            }
            else {
                for (size_t j = st_pos; j < fin_pos; j++)
                    (deq[st] + j)->~T();
            }
        }
        for (size_t j = 0; j < deq.size(); ++j)
            delete[] reinterpret_cast<int8_t*>(deq[j]);
    }

    Deque() {
        size_t i = 0;
        try {
            sz = 0;
            st_pos = 0;
            fin_pos = 0;
            st = 1;
            fin = 1;
            deq.resize(3);
            for (i = 0; i < 3; ++i) {
                deq[i] = reinterpret_cast<T*>(new int8_t[sizeof(T) * buck_sz]);
            }
        }
        catch (...) {
            for (auto j = 0; j < i; ++j)
                delete[] reinterpret_cast<int8_t*>(deq[j]);
            throw;
        }
    }

    Deque(const Deque<T>& a) {
        fin = a.fin;
        fin_pos = a.fin_pos;
        sz = a.sz;
        st = a.st;
        st_pos = a.st_pos;
        deq.assign(a.deq.size(), nullptr);
        size_t g = st;
        size_t i = 0;
        for (size_t j = 0; j < deq.size(); ++j) {
            T* temp = reinterpret_cast<T*>(new int8_t[sizeof(T) * buck_sz]);
            deq[j] = temp;
        }
        try {
            for (; g <= fin; ++g) {
                if (g == st)i = st_pos;
                else i = 0;
                for (; (i < buck_sz) && (i + buck_sz * (g - st) < sz); ++i)
                    new(deq[g] + i)T(a.deq[g][i]);
            }
        }
        catch (...) {
            if (g == st) {
                for (size_t j = st_pos; j < i; j++)
                    (deq[g] + j)->~T();
            }
            else {
                for (size_t j = st_pos; j < buck_sz; j++)
                    (deq[st] + j)->~T();
                for (size_t j = 0; j < i; j++)
                    (deq[g] + j)->~T();
                for (size_t j = st + 1; j < g; j++)
                    for (size_t j1 = 0; j1 < buck_sz; ++j1)
                        (deq[j] + j1)->~T();
                for (size_t j = 0; j < deq.size(); ++j)
                    delete[] reinterpret_cast<int8_t*>(deq[j]);
            }
            deq.clear();
            deq.shrink_to_fit();
            throw;
        }

    }

    Deque& operator=(const Deque<T>& a) {
        if (st != fin) {
            for (size_t j = st_pos; j < buck_sz; j++)
                (deq[st] + j)->~T();
            for (size_t j = 0; j <= fin_pos; j++)
                (deq[fin] + j)->~T();
            for (size_t i = st + 1; i < fin; i++)
                for (size_t j = 0; j < buck_sz; ++j)
                    (deq[i] + j)->~T();
        }
        else {
            for (size_t j = st_pos; j < fin_pos; j++)
                (deq[st] + j)->~T();
        }
        for (size_t j = 0; j < deq.size(); ++j)
            delete[] reinterpret_cast<int8_t*>(deq[j]);
        st = a.st;
        st_pos = a.st_pos;
        fin = a.fin;
        fin_pos = a.fin_pos;
        sz = a.sz;
        deq.assign(a.deq.size(), nullptr);
        size_t j = st;
        size_t i = 0;
        for (size_t t = 0; t < deq.size(); ++t) {
            T* temp = reinterpret_cast<T*>(new int8_t[sizeof(T) * buck_sz]);
            deq[t] = temp;
        }
        try {
            for (; j <= fin; ++j) {
                if (j != st) i = 0;
                else i = st_pos;
                for (; (i < buck_sz) && (i + buck_sz * (j - st) < sz); ++i)
                    new(deq[j] + i)T(a.deq[j][i]);
            }
        }
        catch (...) {
            if (j == st)
                for (size_t t = st_pos; t < i; ++t)
                    (deq[j] + t)->~T();
            else {
                for (size_t j = st_pos; j < buck_sz; ++j)
                    (deq[st] + j)->~T();
                delete[] reinterpret_cast<int8_t*>(deq[st]);
                for (size_t t = 0; t < i; ++t)
                    (deq[j] + t)->~T();
                delete[] reinterpret_cast<int8_t*>(deq[j]);
                for (size_t t = st + 1; t < j; ++t)
                    delete[] deq[t];
            }
            deq.clear();
            deq.shrink_to_fit();
            throw;
        }
        return *this;
    }

    T& operator[](unsigned int i) {
        return deq[st + (i + st_pos) / buck_sz][(i + st_pos) % buck_sz];
    }

    T& at(unsigned int i) {
        if (i >= sz)throw out_of_range("");
        return deq[i / buck_sz + fin][i % buck_sz];
    }

    const T& operator[](unsigned int i) const {
        return deq[st + (i + st_pos) / buck_sz][(i + st_pos) % buck_sz];
    }

    const T& at(unsigned int i) const {
        if (i >= sz)throw out_of_range("");
        return deq[i / buck_sz + fin][i % buck_sz];
    }

    unsigned int size() const;

    void push_back(const T&);
    void pop_back();

    void push_front(const T&);

    void pop_front();

private:
    vector<T*> deq;
    size_t st, st_pos, fin, fin_pos, sz;
    void grow_buffer() {
        size_t i, j;
        vector<T*> newarr(2 * deq.size() - 1);
        try {
            for (i = 0; i < 2 * deq.size() - 1; ++i)
                newarr[i] = reinterpret_cast<T*>(new int8_t[buck_sz * sizeof(T)]);
            for (i = 0; i < deq.size() - 1; ++i) {
                newarr[i + deq.size() - 1] = deq[i];
                for (j = 0; j < buck_sz; ++j) {
                    (deq[i] + j)->~T();
                }
            }
        }
        catch (...) {
            for (size_t j = 0; j < i; ++j) {
                delete[] reinterpret_cast<int8_t*>(newarr[j]);
            }
            throw;
        }
        deq = newarr;
    }

};
template <typename T>
unsigned int Deque<T>::size() const {
    return sz;
}

template <typename T>
void Deque <T>::push_back(const T& a) {
    try {
        if (sz) {
            if (fin_pos == buck_sz - 1 && fin == deq.size() - 2) {
                try {
                    deq.push_back(reinterpret_cast<T*>(new int8_t[buck_sz * sizeof(T)]));
                    ++fin;
                    fin_pos = 0;
                    new (deq[fin] + fin_pos)T(a);
                }
                catch (...) {
                    fin_pos = buck_sz - 1;
                    fin--;
                    throw;
                }
            }
            else if (fin_pos == buck_sz - 1) {


                try {
                    fin_pos = 0;
                    ++fin;
                    new (deq[fin] + fin_pos)T(a);
                }
                catch (...) {
                    --fin;
                    fin_pos = buck_sz - 1;
                    throw;
                }
            }
            else {

                try {
                    ++fin_pos;
                    new (deq[fin] + fin_pos)T(a);
                }
                catch (...) {
                    fin_pos--;
                    throw;
                }
            }
        }
    }
    catch (...) {
        throw;
    }
    ++sz;
}

template <typename T>
void Deque <T>::push_front(const T& a) {
    if (sz) {
        if (st_pos == 0 && st == 0) {
            try {
                st += deq.size() - 2;
                fin += deq.size() - 1;
                grow_buffer();
                st_pos = buck_sz - 1;
                new (deq[st] + st_pos)T(a);
            }
            catch (...) {
                fin -= (st + 1);
                st_pos = 0;
                st = 0;
                throw;
            }
        }
        else if (st_pos == 0) {
            try {
                --st;
                st_pos = buck_sz - 1;
                new (deq[st] + st_pos)T(a);
            }
            catch (...) {
                st_pos = 0;
                ++st;
                throw;
            }
        }
        else {
            try {
                --st_pos;
                new (deq[st] + st_pos)T(a);
            }
            catch (...) {
                ++st_pos;
                throw;
            }

        }
    }
    ++sz;
}

template <typename T>
void Deque <T>::pop_back() {
    try {
        (deq[fin] + fin_pos)->~T();
        --sz;

        if (fin_pos)
            --fin_pos;
        else {
            fin_pos = buck_sz - 1;
            --fin;
        }
    }
    catch (...) {
        throw;
    }
}

template <typename T>
void Deque <T>::pop_front() {
    try {
        (deq[st] + st_pos)->~T();
        --sz;
        if (st_pos < buck_sz - 1)
            ++st_pos;
        else {
            st_pos = 0;
            ++st;
        }
    }
    catch (...) {
        throw;
    }
}