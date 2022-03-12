#include <algorithm>
#include <cstddef>
#include <initializer_list>

// AA-tree

template<class T>
class Set {
public:
    struct Node {
        Node *left_;
        Node *right_;
        Node *parent_;
        T value_;
        size_t level_;
        
        Node() {
            left_ = nullptr;
            right_ = nullptr;
            parent_ = nullptr;
            level_ = 1;
        }
        
        Node(const T& value): value_(value) {
            left_ = nullptr;
            right_ = nullptr;
            parent_ = nullptr;
            level_ = 1;
        }
        
        ~Node() {
            parent_ = nullptr;
            delete left_;
            left_ = nullptr;
            delete right_;
            right_ = nullptr;
        }
    };

    class TreeIterator {
        Node *node;
        bool is_end = true;
    
    public:
        TreeIterator() {}
        
        TreeIterator(Node *node): node(node), is_end(false) {}
        
        TreeIterator(Node *node, bool is_end): node(node), is_end(is_end) {}
        
        ~TreeIterator() {
            node = nullptr;
            is_end = false;
        }
        
        T& operator*() const {
            return node->value_;
        }
        
        T* operator->() const {
            return &node->value_;
        }
        
        TreeIterator& operator++() {
            if (node->right_ != nullptr) {
                node = node->right_;
                while (node->left_ != nullptr) {
                    node = node->left_;
                }
                return *this;
            } else {
                if (node->parent_ == nullptr) {
                    is_end = true;
                    return *this;
                }
                Node *start_node = node;
                while (node->parent_ != nullptr) {
                    if (node->parent_->left_ == node) {
                        node = node->parent_;
                        return *this;
                    }
                    node = node->parent_;
                }
                node = start_node;
                is_end = true;
                return *this;
            }
        }
        
        TreeIterator operator++(int) {
            TreeIterator result = *this;
            ++(*this);
            return result;
        }
        
        TreeIterator& operator--() {
            if (is_end) {
                is_end = false;
                return *this;
            }
            if (node->left_ != nullptr) {
                node = node->left_;
                while (node->right_ != nullptr) {
                    node = node->right_;
                }
                return *this;
            } else {
                while (node->parent_ != nullptr) {
                    if (node->parent_->right_ == node) {
                        node = node->parent_;
                        return *this;
                    }
                    node = node->parent_;
                }
                return *this;
            }
        }
        
        TreeIterator operator--(int) {
            TreeIterator result = *this;
            --(*this);
            return result;
        }
        
        friend bool operator==(const TreeIterator& it1, const TreeIterator& it2) {
            return it1.node == it2.node && it1.is_end == it2.is_end;
        }
        
        friend bool operator!=(const TreeIterator& it1, const TreeIterator& it2) {
            return !(it1 == it2);
        }
    };
    
    using iterator = TreeIterator;
    using const_iterator = TreeIterator;

    Set() {}

    template<typename Iterator>
    Set(Iterator first, Iterator last) {
        for (; first != last; ++first) {
            insert(*first);
        }
    }

    Set(std::initializer_list<T> elements) {
        for (const T& value : elements) {
            insert(value);
        }
    }
    
    Set(const Set<T>& s) {
        delete root_;
        
        root_ = copyNode(s.root_);
        size_ = s.size();
        updateEnd();
        updateBegin();
    }
    
    ~Set() {
        delete root_;
        delete end_node_;
        end_ = iterator();
        begin_ = end_;
        size_ = 0;
    }
    
    Set<T>& operator=(const Set<T>& other) {
        if (this == &other) {
            return *this;
        }
        
        delete root_;
        
        root_ = copyNode(other.root_);
        size_ = other.size();
        updateEnd();
        updateBegin();
        return *this;
    }

    size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    void insert(const T& elem) {
        root_ = insert(root_, elem);
        updateEnd();
        updateBegin();
    }

    void erase(const T& elem) {
        root_ = erase(root_, elem);
        updateEnd();
        updateBegin();
    }

    iterator begin() const {
        return begin_;
    }

    iterator end() const {
        return end_;
    }

    iterator find(const T& value) const {
        Node *result = find(root_, value);
        if (result == nullptr) {
            return end();
        }
        return iterator(result);
    }

    iterator lower_bound(const T& value) const {
        Node *result = lower_bound(root_, value);
        if (result == nullptr) {
            return end();
        }
        return iterator(result);
    }

private:
    Node* copyNode(Node *root) const {
        if (root == nullptr) {
            return root;
        }
        Node *result = new Node(root->value_);
        result->level_ = root->level_;
        
        result->left_ = copyNode(root->left_);
        if (result->left_ != nullptr) {
            result->left_->parent_ = result;
        }
        result->right_ = copyNode(root->right_);
        if (result->right_ != nullptr) {
            result->right_->parent_ = result;
        }
        return result;
    }
    
    Node* skew(Node *root) {
        if (root == nullptr) {
            return root;
        } else if (root->left_ == nullptr) {
            return root;
        } else if (root->left_->level_ == root->level_) {
            Node *new_root = root->left_;
            root->left_ = new_root->right_;
            if (new_root->right_ != nullptr) {
                new_root->right_->parent_ = root;
            }
            
            new_root->right_ = root;
            root->parent_ = new_root;
            
            new_root->parent_ = nullptr;
            return new_root;
        } else {
            return root;
        }
    }

    Node* split(Node *root) {
        if (root == nullptr) {
            return root;
        } else if (root->right_ == nullptr || root->right_->right_ == nullptr) {
            return root;
        } else if (root->right_->right_->level_ == root->level_) {
            Node *new_root = root->right_;
            root->right_ = new_root->left_;
            if (new_root->left_ != nullptr) {
                new_root->left_->parent_ = root;
            }
            
            new_root->left_ = root;
            root->parent_ = new_root;
            
            new_root->parent_ = nullptr;
            ++new_root->level_;
            return new_root;
        } else {
            return root;
        }
    }
    
    Node* insert(Node *root, const T& value) {
        if (root == nullptr) {
            ++size_;
            return new Node(value);
        }
        if (value < root->value_) {
            root->left_ = insert(root->left_, value);
            root->left_->parent_ = root;
        } else if (root->value_ < value) {
            root->right_ = insert(root->right_, value);
            root->right_->parent_ = root;
        }
        
        root = skew(root);
        root = split(root);
        return root;
    }

    inline size_t get_level(Node *root) const {
        if (root == nullptr) {
            return 0;
        }
        return root->level_;
    }

    Node* decreaseLevel(Node *root) {
        size_t required_level = std::min(get_level(root->left_), get_level(root->right_)) + 1;
        if (required_level < root->level_) {
            root->level_ = required_level;
            if (root->right_ != nullptr && required_level < root->right_->level_) {
                root->right_->level_ = required_level;
            }
        }
        return root;
    }

    inline bool is_leaf(Node *root) const {
        return root->left_ == nullptr && root->right_ == nullptr;
    }

    Node* predessor(Node *root) const {
        root = root->left_;
        while (root->right_ != nullptr) {
            root = root->right_;
        }
        return root;
    }

    Node* successor(Node *root) const {
        root = root->right_;
        while (root->left_ != nullptr) {
            root = root->left_;
        }
        return root;
    }

    Node* erase(Node *root, const T& value) {
        if (root == nullptr) {
            return root;
        }
        if (value < root->value_) {
            root->left_ = erase(root->left_, value);
            if (root->left_ != nullptr) {
                root->left_->parent_ = root;
            }
        } else if (root->value_ < value) {
            root->right_ = erase(root->right_, value);
            if (root->right_ != nullptr) {
                root->right_->parent_ = root;
            }
        } else {
            if (is_leaf(root)) {
                --size_;
                delete root;
                return nullptr;
            } else if (root->left_ == nullptr) {
                T new_value = successor(root)->value_;
                root->right_ = erase(root->right_, new_value);
                if (root->right_ != nullptr) {
                    root->right_->parent_ = root;
                }
                root->value_ = new_value;
            } else {
                T new_value = predessor(root)->value_;
                root->left_ = erase(root->left_, new_value);
                if (root->left_ != nullptr) {
                    root->left_->parent_ = root;
                }
                root->value_ = new_value;
            }
        }
        
        root = decreaseLevel(root);
        root = skew(root);
        root->right_ = skew(root->right_);
        if (root->right_ != nullptr) {
            root->right_->parent_ = root;
        }
        if (root->right_ != nullptr) {
            root->right_->right_ = skew(root->right_->right_);
            if (root->right_->right_ != nullptr) {
                root->right_->right_->parent_ = root->right_;
            }
        }
        root = split(root);
        root->right_ = split(root->right_);
        if (root->right_ != nullptr) {
            root->right_->parent_ = root;
        }
        
        root->parent_ = nullptr;
        return root;
    }
    
    void updateBegin() {
        Node *result = root_;
        if (result != nullptr) {
            while (result->left_ != nullptr) {
                result = result->left_;
            }
            begin_ = iterator(result);
        } else {
            begin_ = end_;
        }
    }
    
    void updateEnd() {
        Node *result = root_;
        if (result != nullptr) {
            while (result->right_ != nullptr) {
                result = result->right_;
            }
            end_ = iterator(result, true);
        } else {
            end_ = iterator(end_node_, true);
        }
    }
    
    Node* find(Node* root, const T& value) const {
        if (root == nullptr) {
            return root;
        }
        if (value < root->value_) {
            return find(root->left_, value);
        } else if (root->value_ < value) {
            return find(root->right_, value);
        } else {
            return root;
        }
    }
    
    Node* lower_bound(Node* root, const T& value) const {
        if (root == nullptr) {
            return root;
        }
        if (value < root->value_) {
            Node *result = lower_bound(root->left_, value);
            if (result == nullptr) {
                result = root;
            }
            return result;
        } else if (root->value_ < value) {
            return lower_bound(root->right_, value);
        } else {
            return root;
        }
    }
    
    Node *root_ = nullptr;
    size_t size_ = 0;
    
    Node *end_node_ = new Node();
    iterator end_ = iterator(end_node_, true);
    iterator begin_ = end_;
};
