#include <algorithm>
#include <initializer_list>

template<class T>
class Set {
public:
    Set() { _sz = 0; _root = nullptr; }

    template<typename Iterator>
    Set(Iterator first, Iterator last)
    {
        _sz = 0; _root = nullptr;
        for (; first != last; ++first)
            insert(*first);
    }

    Set(std::initializer_list<T> elems)
    {
        _sz = 0; _root = nullptr;
        for (auto i : elems)
            insert(i);
    }

    Set& operator=(const Set& s)
    {
        if (this == &s)
            return *this;

        _sz = s._sz;
        if (_root)
            delete _root;
        _root = nullptr;
        if (s._root)
        {
            _root = new Node();
            *_root = *s._root;
        }

        return *this;
    }

    Set(const Set& s)
    {
        _sz = 0;
        _root = nullptr;
        *this = s;
    }

    ~Set()
    {
        if (_root)
            delete _root;
    }

    size_t size() const {
        return _sz;
    }

    bool empty() const {
        return _sz == 0;
    }

    void insert(const T& elem) {
        if (find(elem) == end())
            ++_sz;
        _root = insert(_root, elem);
    }

    void erase(const T& elem) {
        if (find(elem) != end())
            --_sz;
        _root = erase(_root, elem);
    }

private:
    struct Node
    {
        T data;
        Node* l, * r, * p;
        size_t h;

        Node() { l = r = p = nullptr; h = 1; };
        Node(T data) : data(data) { l = r = p = nullptr; h = 1; };
        Node& operator=(const Node& node)
        {
            data = node.data;
            h = node.h;
            /*if (l)
                delete l;*/
            l = nullptr;
            /*if (r)
                delete r;*/
            r = nullptr;
            if (node.l)
            {
                l = new Node();
                *l = *node.l;
                l->p = this;
            }
            if (node.r)
            {
                r = new Node();
                *r = *node.r;
                r->p = this;
            }

            return *this;
        }

        ~Node()
        {
            if (this->l)
                delete this->l;
            if (this->r)
                delete this->r;
        }
    };

public:
    class iterator
    {
    public:
        friend class Set;

        iterator() { _current = nullptr; }

        iterator(const iterator& it) 
        {
            _current = it._current;
        }

        ~iterator() {}

        iterator& operator=(const iterator& it) 
        {
            _current = it._current;

            return *this;
        }

        iterator& operator++() 
        {
            Node* res = _current;
            if (res->r)
            {
                res = res->r;
                while (res->l)
                    res = res->l;

                return *this = iterator(res, _root);
            }
            while (true)
            {
                Node* parent = res->p;
                if (!parent)
                    return *this = iterator(nullptr, _root);

                if (parent->l == res)
                    return *this = iterator(parent, _root);

                res = parent;
            }
        }

        iterator operator++(int)
        {
            iterator res = *this;
            ++*this;

            return res;
        }

        iterator& operator--() 
        {
            if (!_current)
            {
                Node* res = _root;
                while (res->r)
                    res = res->r;

                return *this = iterator(res, _root);
            }

            Node* res = _current;
            if (res->l)
            {
                res = res->l;
                while (res->r)
                    res = res->r;

                return *this = iterator(res, _root);
            }
            while (true)
            {
                Node* parent = res->p;
                if (!parent)
                    return *this = iterator(nullptr, _root);

                if (parent->r == res)
                    return *this = iterator(parent, _root);

                res = parent;
            }
        }

        iterator operator--(int)
        {
            iterator res = *this;
            --*this;

            return res;
        }

        const T& operator*() const {
            if (!_current)
                return _root->data;
            return _current->data;
        }

        T const* operator->() const {
            return &_current->data;
        }

        friend bool operator==(const iterator& first, const iterator& second) {
            return first._current == second._current;
        }

        friend bool operator!=(const iterator& first, const iterator& second) {
            return first._current != second._current;
        }

    private:
        iterator(Node* current, Node* root) : _current(current), _root(root) {}
        Node* _current,* _root;
    };

    iterator begin() const {
        if (!_root) return end();

        Node* res = _root;
        while (res->l)
            res = res->l;
        return iterator(res, _root);
    }

    iterator end() const {
        return iterator(nullptr, _root);
    }

    iterator find(const T& elem) const {
        iterator res = lower_bound(elem);
        if (res != end() && (!(*res < elem) && !(elem < *res)))
            return res;

        return end();
    }

    iterator lower_bound(const T& elem) const {
        Node* res = _root;
        while (res)
        {
            if (res->data < elem)
            {
                if (!res->r)
                    break;
                else
                    res = res->r;
            }
            else
            {
                if (!res->l)
                    break;
                else
                    res = res->l;
            }
        }

        while (res && res->data < elem)
            res = res->p;

        return iterator(res, _root);
    }

private:
    Node* LRotate(Node* t) {
        Node* r = t->r;
        Node* L = r->l;
        r->p = t->p;
        r->l = t; if (t) t->p = r;
        t->r = L; if (L) L->p = t;
        t->h = std::max(Height(t->l), Height(t->r)) + 1;
        r->h = std::max(Height(r->l), Height(r->r)) + 1;

        return r;
    }

    Node* RRotate(Node* t) {
        Node* l = t->l;
        Node* R = l->r;
        l->p = t->p;
        l->r = t; if (t) t->p = l;
        t->l = R; if (R) R->p = t;
        t->h = std::max(Height(t->l), Height(t->r)) + 1;
        l->h = std::max(Height(l->l), Height(l->r)) + 1;

        return l;
    }

    size_t Height(Node* t)
    {
        if (!t) return 0;
        return t->h;
    }
    
    Node* insert(Node* t, T elem)
    {
        if (!t)
        {
            return new Node(elem);;
        }

        if (t->data < elem)
        {
            t->r = insert(t->r, elem);
            if (t->r)
                t->r->p = t;
        }
        else if (elem < t->data)
        {
            t->l = insert(t->l, elem);
            if (t->l)
                t->l->p = t;
        }
        else
            return t;

        t->h = std::max(Height(t->l), Height(t->r)) + 1;

        t = Balance(t);

        return t;
    }

    Node* erase(Node* t, T elem)
    {
        if (!t)
        {
            return nullptr;
        }

        if (t->data < elem)
        {
            t->r = erase(t->r, elem);
            if (t->r)
                t->r->p = t;
        }
        else if (elem < t->data)
        {
            t->l = erase(t->l, elem);
            if (t->l)
                t->l->p = t;
        }
        else
        {
            /*if (!t->l)
            {
                Node* h = t->r;
                t->r = nullptr;
                if (h)
                    h->p = t->p;
                delete t;
                return h;
            }
            else */
            if (!t->r)
            {
                Node* h = t->l;
                t->l = nullptr;
                if (h)
                    h->p = t->p;
                delete t;
                return h;
            }
            else {
                Node* h = t->r;
                while (h->l)
                    h = h->l;
                t->data = h->data;
                t->r = erase(t->r, h->data);
                if (t->r)
                    t->r->p = t;
            }
        }

        t->h = std::max(Height(t->l), Height(t->r)) + 1;

        t = Balance(t);

        return t;
    }

    int Dh(Node* t) {
        if (!t)
            return 0;
        return Height(t->l) - Height(t->r);
    }

    Node* Balance(Node* t)
    {
        int dh = Dh(t);

        if (dh > 1 && Dh(t->l) >= 0) {
            return RRotate(t);
        }
        if (dh > 1 && Dh(t->l) < 0) {
            t->l = LRotate(t->l);
            return RRotate(t);
        }

        if (dh < -1 && Dh(t->r) <= 0) {
            return LRotate(t);
        }
        if (dh < -1 && Dh(t->r) > 0) {
            t->r = RRotate(t->r);
            return LRotate(t);
        }

        return t;
    }

    Node* _root;
    size_t _sz;
};