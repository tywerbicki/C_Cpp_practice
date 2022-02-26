#include <iostream>
#include <vector>
#include <forward_list>
#include <algorithm>

template <typename K, typename V>
struct bt_node {
    bt_node<K, V> *parent, *left, *right;
    size_t height;
    K key;
    V *object;
};

template <typename K, typename V>
class AVL_tree {

    private:

        size_t alloc_size; unsigned alloc_exp;
        bt_node<K, V> *root, *free_store;
        std::forward_list< bt_node<K, V>* > released;
        std::vector< bt_node<K, V>* > allocations;
        size_t nodes_remaining, nodes_in_tree;
        
        AVL_tree(const AVL_tree<V, K>&);
        AVL_tree<K, V>& operator=(const AVL_tree<K, V>&);

        void allocate_nodes() {
            
            this->free_store = new bt_node<K, V>[ this->alloc_size ];
            this->allocations.push_back( this->free_store );
            this->nodes_remaining = this->alloc_size;
            this->alloc_size *= this->alloc_exp;
        }

        bt_node<K, V>* get_node() {

            if( ! this->released.empty() ) {
                bt_node<K, V> *new_node = this->released.front();
                this->released.pop_front();
                return new_node;
            }

            if( ! this->nodes_remaining ) allocate_nodes();
            
            this->nodes_remaining--;
            return this->free_store++;
        }

        void delete_node(bt_node<K, V> *node) {
            this->released.push_front(node);
        }

        bt_node<K, V>* leftmost_node(bt_node<K, V> *root) {

            bt_node<K, V> *tmp_node = root;
            while( tmp_node->left ) tmp_node = tmp_node->left;

            return tmp_node;
        }

        bt_node<K, V>* rightmost_node(bt_node<K, V> *root) {

            bt_node<K, V> *tmp_node = root;
            while( tmp_node->right ) tmp_node = tmp_node->right;

            return tmp_node;
        }

        void swap_node_contents(bt_node<K, V> *node1, bt_node<K, V> *node2) {

            K tmp_key = node1->key; V *tmp_value = node1->object;
            node1->key = node2->key; node1->object = node2->object;
            node2->key = tmp_key; node2->object = tmp_value;
        }

        void height_bubble_up(bt_node<K, V> *root) {
            
            size_t height_l = 0UL, height_r = 0UL;
            while( root ) {
                if( root->left ) height_l = root->left->height;
                if( root->right ) height_r = root->right->height;
                root->height = 1 + std::max<size_t>(height_l, height_r);
                root = root->parent;
            }   
        }

        int skew(bt_node<K, V> *node) {

            size_t height_l = 0UL, height_r = 0UL;
            if( node->left ) height_l = node->left->height;
            if( node->right ) height_r = node->right->height;
            return height_r - height_l;
        }

        void left_rotation(bt_node<K, V> *root) {
            
            if( root->parent ) {
                if( root->parent->left == root ) root->parent->left = root->right;
                else root->parent->right = root->right;
            } else this->root = root->right;

            root->right->parent = root->parent;
            root->parent = root->right;
            root->right = root->parent->left;
            if( root->right ) root->right->parent = root;
            root->parent->left = root;
            this->height_bubble_up(root);
        }

        void right_rotation(bt_node<K, V> *root) {
            
            if( root->parent ) {
                if( root->parent->left == root ) root->parent->left = root->left;
                else root->parent->right = root->left;
            } else this->root = root->left;

            root->left->parent = root->parent;
            root->parent = root->left;
            root->left = root->parent->right;
            if( root->left ) root->left->parent = root;
            root->parent->right = root;
            this->height_bubble_up(root);
        }

        void balance_tree(bt_node<K, V> *root) {

            this->height_bubble_up(root);
            while( root ) {
                int skew = this->skew(root);
                if( skew > 1 ) {
                    if( this->skew(root->right) > -1 ) this->left_rotation(root);
                    else {
                        this->right_rotation(root->right);
                        this->left_rotation(root);
                    }
                } else if( skew < -1 ) {
                    if( this->skew(root->left) < 1 ) this->right_rotation(root);
                    else {
                        this->left_rotation(root->left);
                        this->right_rotation(root);
                    }
                }
                root = root->parent;
            } 
        } 

    public:

        AVL_tree(size_t alloc_size = 10, unsigned alloc_exp = 2)
            : alloc_size(alloc_size), alloc_exp(alloc_exp) {

                this->root = NULL;
                this->nodes_in_tree = this->nodes_remaining = 0;
            }

        ~AVL_tree() {
            for(const auto &mem_address : this->allocations ) {
                delete[] mem_address;
            }
        }

        bt_node<K, V>* previous(bt_node<K, V> *node) {

            if( node->left ) return this->rightmost_node( node->left );
            if( node == this->leftmost_node(this->root) ) return NULL;

            bt_node<K, V> *tmp_node = node;
            while( tmp_node->parent->right != tmp_node ) tmp_node = tmp_node->parent;

            return tmp_node->parent;
        }

        bt_node<K, V>* next(bt_node<K, V> *node) {

            if( node->right ) return this->leftmost_node( node->right );
            if( node == this->rightmost_node(this->root) ) return NULL;

            bt_node<K, V> *tmp_node = node;
            while( tmp_node->parent->left != tmp_node ) tmp_node = tmp_node->parent;
            
            return tmp_node->parent;
        }

        void insert_previous(bt_node<K, V> *anchor_node, bt_node<K, V> *new_node) {
            
            if( ! anchor_node->left ) {
                anchor_node->left = new_node;
                new_node->parent = anchor_node;
                this->balance_tree(anchor_node);
                this->nodes_in_tree++;
                return;
            }

            anchor_node = this->previous(anchor_node);
            anchor_node->right = new_node;
            new_node->parent = anchor_node;
            this->balance_tree(anchor_node);
            this->nodes_in_tree++;
        }

        void insert_next(bt_node<K, V> *anchor_node, bt_node<K, V> *new_node) {

            if( ! anchor_node->right ) {
                anchor_node->right = new_node;
                new_node->parent = anchor_node;
                this->balance_tree(anchor_node);
                this->nodes_in_tree++;
            }

            anchor_node = this->next(anchor_node);
            anchor_node->left = new_node;
            new_node->parent = anchor_node;
            this->balance_tree(anchor_node);
            this->nodes_in_tree++;
        }

        bt_node<K, V>* find(const K key) {

            bt_node<K, V> *tmp_node = this->root;
            while( tmp_node ) {
                if( key < tmp_node->key ) tmp_node = tmp_node->left;
                else if ( key > tmp_node->key ) tmp_node = tmp_node->right;
                else return tmp_node;
            }

            return NULL;
        } 

        bt_node<K, V>* insert(const K key, V *value) {

            bt_node<K, V> *new_node = this->get_node(), *tmp_node = this->root, *prev_node;
            new_node->key = key; new_node->object = value;
            new_node->height = 1UL;
            new_node->left = new_node->right = NULL;

            if( ! this->root ) {
                this->root = new_node;
                new_node->parent = NULL;
                this->nodes_in_tree++;
                return new_node;
            }

            while( tmp_node ) {
                prev_node = tmp_node;
                if( key < tmp_node->key ) tmp_node = tmp_node->left;
                else if( key > tmp_node->key ) tmp_node = tmp_node->right;
                else return NULL;
            }

            if( key < prev_node->key ) prev_node->left = new_node;
            else prev_node->right = new_node;
            new_node->parent = prev_node;
            this->balance_tree(prev_node);
            this->nodes_in_tree++;
            return new_node;
        }

        int remove(const K key) {
            
            bt_node<K, V> *node_to_remove, *tmp_node;
            node_to_remove = this->find(key);
            if( ! node_to_remove ) return 0;

            while( node_to_remove->left || node_to_remove->right ) {
                if( node_to_remove->left ) tmp_node = this->previous( node_to_remove );
                else tmp_node = this->next( node_to_remove );
                this->swap_node_contents(node_to_remove, tmp_node);
                node_to_remove = tmp_node;
            }

            if( ! node_to_remove->parent ) this->root = NULL;
            else {
                if( node_to_remove == node_to_remove->parent->left ) node_to_remove->parent->left = NULL;
                else node_to_remove->parent->right = NULL;
                this->balance_tree(node_to_remove->parent);
            }
            this->delete_node(node_to_remove);
            this->nodes_in_tree--;
            return 1;
        }

        size_t height() {
            if( this->root ) return this->root->height;
            return 0UL;
        }

        size_t size() {
            return this->nodes_in_tree;
        }

        bt_node<K, V>* get_root() {
            return this->root;
        }

        void print() {

            if( ! this->root ) return;

            std::cout << "Size: " << this->size() << " ";
            std::cout << "Height: " << this->height() << " ";
            std::cout << "Skew: " << this->skew(this->root) << "\n";

            bt_node<K, V> *tmp_node = this->leftmost_node(this->root);
            while( tmp_node ) {
                std::cout << *( tmp_node->object ) << " ";
                tmp_node = this->next(tmp_node);
            }
            std::cout << std::endl;
        }

};




int main() {

    AVL_tree<int, double> my_tree;
    
    double one=1.1, two=2.2, three=3.3, four=4.4, five=5.5, six=6.6, seven=7.7;
    my_tree.insert(1, &one);
    my_tree.insert(2, &two);
    my_tree.insert(3, &three);
    my_tree.insert(5, &five);
    my_tree.insert(6, &six);
    my_tree.insert(4, &four);
    //my_tree.insert(7, &seven);
    
    //my_tree.remove(5);

    my_tree.print();

    //bt_node<int, double> *root = my_tree.get_root();
    //std::cout << root->key << "\n";
    //std::cout << root->left->key << " " << root->right->key << "\n";
    //std::cout << root->right->right->key << std::endl;

    //bt_node<int, double> *node = my_tree.find(5);
    //std::cout << node->parent->key << std::endl;
}
