#include <algorithm>
#include <initializer_list>

template<class T>
class Set {
public:
    Set() { _sz = 0; _root = nullptr; }

    template<typename Iterator>
    Set(Iterator first, Iterator last) {
        _sz = 0; _root = nullptr;
        for (; first != last; ++first)
            insert(*first);
    }

    Set(std::initializer_list<T> elems) {
        _sz = 0; _root = nullptr;
        for (auto i : elems)
            insert(i);
    }

    Set& operator=(const Set& s) {
        if (this == &s)
            return *this;

        _sz = s._sz;
        if (_root)
            delete _root;
        _root = nullptr;
        if (s._root) {
            _root = new Node();
            *_root = *s._root;
        }

        return *this;
    }

    Set(const Set& s) {
        _sz = 0;
        _root = nullptr;
        *this = s;
    }

    ~Set() {
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
        if (find(elem) == end()) {
            ++_sz;
        }
        _root = insert(_root, elem);
    }

    void erase(const T& elem) {
        if (find(elem) != end()) {
            --_sz;
        }
        _root = erase(_root, elem);
    }

private:
    struct Node
    {
        T data;
        Node *left, *right, *parrent;
        size_t h;

        Node() { 
            left = right = parrent = nullptr; 
            h = 1; 
        };
        Node(T data) : data(data) { 
            left = right = parrent = nullptr; 
            h = 1; 
        };
        Node& operator=(const Node& node) {
            data = node.data;
            h = node.h;
            left = nullptr;
            right = nullptr;
            if (node.left) {
                left = new Node();
                *left = *node.left;
                left->parrentarrent = this;
            }
            if (node.right) {
                right = new Node();
                *right = *node.right;
                right->parrentarrent = this;
            }

            return *this;
        }

        ~Node() {
            if (this->left) {
                delete this->left;
            }
            if (this->right) {
                delete this->right;
            }
        }
    };

public:
    class iterator {
    public:
        friend class Set;

        iterator() { 
            _current = nullptr; 
        }

        iterator(const iterator& it) {
            _current = it._current;
        }

        ~iterator() {
        }

        iterator& operator=(const iterator& it) {
            _current = it._current;

            return *this;
        }

        iterator& operator++() {
            Node* res = _current;
            if (res->right) {
                res = res->right;
                while (res->left)
                    res = res->left;

                return *this = iterator(res, _root);
            }
            while (true) {
                Node* parent = res->parrent;
                if (!parent) {
                    return *this = iterator(nullptr, _root);
                }

                if (parent->left == res) {
                    return *this = iterator(parent, _root);
                }

                res = parent;
            }
        }

        iterator operator++(int) {
            iterator res = *this;
            ++* this;

            return res;
        }

        iterator& operator--() {
            if (!_current) {
                Node* res = _root;
                while (res->right)
                    res = res->right;

                return *this = iterator(res, _root);
            }

            Node* res = _current;
            if (res->left) {
                res = res->left;
                while (res->right)
                    res = res->right;

                return *this = iterator(res, _root);
            }
            while (true) {
                Node* parent = res->parrent;
                if (!parent) {
                    return *this = iterator(nullptr, _root);
                }

                if (parent->right == res) {
                    return *this = iterator(parent, _root);
                }

                res = parent;
            }
        }

        iterator operator--(int) {
            iterator res = *this;
            --* this;

            return res;
        }

        const T& operator*() const {
            if (!_current) {
                return _root->data;
            }
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
        iterator(Node* current, Node* root) : _current(current), _root(root) {
        }
        Node* _current, * _root;
    };

    iterator begin() const {
        if (!_root) return end();

        Node* res = _root;
        while (res->left) {
            res = res->left;
        }
        return iterator(res, _root);
    }

    iterator end() const {
        return iterator(nullptr, _root);
    }

    iterator find(const T& elem) const {
        iterator res = lower_bound(elem);
        if (res != end() && (!(*res < elem) && !(elem < *res))) {
            return res;
        }

        return end();
    }

    iterator lower_bound(const T& elem) const {
        Node* res = _root;
        while (res) {
            if (res->data < elem) {
                if (!res->right) {
                    break;
                } else {
                    res = res->right;
                }
            } else {
                if (!res->left) {
                    break;
                } else {
                    res = res->left;
                }
            }
        }

        while (res && res->data < elem) {
            res = res->parrent;
        }

        return iterator(res, _root);
    }

private:
    Node* LRotate(Node* t) {
        Node* r = t->right;
        Node* L = r->left;
        r->parrent = t->parrent;
        r->left = t; if (t) t->parrent = r;
        t->right = L; if (L) L->parrent = t;
        t->h = std::max(Height(t->left), Height(t->right)) + 1;
        r->h = std::max(Height(r->left), Height(r->right)) + 1;

        return r;
    }

    Node* RRotate(Node* t) {
        Node* l = t->left;
        Node* R = l->right;
        l->parrent = t->parrent;
        l->right = t; if (t) t->parrent = l;
        t->left = R; if (R) R->parrent = t;
        t->h = std::max(Height(t->left), Height(t->right)) + 1;
        l->h = std::max(Height(l->left), Height(l->right)) + 1;

        return l;
    }

    size_t Height(Node* t) {
        if (!t) return 0;
        return t->h;
    }

    Node* insert(Node* t, T elem) {
        if (!t) {
            return new Node(elem);
        }

        if (t->data < elem) {
            t->right = insert(t->right, elem);
            if (t->right) {
                t->right->parrent = t;
            }
        } else if (elem < t->data) {
            t->left = insert(t->left, elem);
            if (t->left) {
                t->left->parrent = t;
            }
        } else {
            return t;
        }

        t->h = std::max(Height(t->left), Height(t->right)) + 1;

        t = Balance(t);

        return t;
    }

    Node* erase(Node* t, T elem) {
        if (!t) {
            return nullptr;
        }

        if (t->data < elem) {
            t->right = erase(t->right, elem);
            if (t->right) {
                t->right->parrent = t;
            }
        } else if (elem < t->data) {
            t->left = erase(t->left, elem);
            if (t->left) {
                t->left->parrent = t;
            }
        } else {
            if (!t->right) {
                Node* h = t->left;
                t->left = nullptr;
                if (h) {
                    h->parrent = t->parrent;
                }
                delete t;
                return h;
            } else {
                Node* h = t->right;
                while (h->left)
                    h = h->left;
                t->data = h->data;
                t->right = erase(t->right, h->data);
                if (t->right)
                    t->right->parrent = t;
            }
        }

        t->h = std::max(Height(t->left), Height(t->right)) + 1;

        t = Balance(t);

        return t;
    }

    int HeightDifference(Node* t) {
        if (!t) {
            return 0;
        }
        return Height(t->left) - Height(t->right);
    }

    Node* Balance(Node* t) {
        int dh = HeightDifference(t);

        if (dh > 1 && HeightDifference(t->left) >= 0) {
            return RRotate(t);
        }
        if (dh > 1 && HeightDifference(t->left) < 0) {
            t->left = LRotate(t->left);
            return RRotate(t);
        }

        if (dh < -1 && HeightDifference(t->right) <= 0) {
            return LRotate(t);
        }
        if (dh < -1 && HeightDifference(t->right) > 0) {
            t->right = RRotate(t->right);
            return LRotate(t);
        }

        return t;
    }

    Node* _root;
    size_t _sz;
};