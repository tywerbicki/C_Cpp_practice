#include <stdlib.h>
#include <iostream>

// Doubly-linked node.
template <typename T>
struct dl_node {
    dl_node<T> *prev, *next;
    T item;
};

// Singly-linked pointer node.
template <typename T>
struct slp_node {
    slp_node<T> *next;
    T *item;
};

template <typename T>
class deque
{
    private:

        // Expansion magnitude and expansion factor.
        size_t exp_mag, exp_fac;
        // Head and tail of the doubly-linked list.
        dl_node<T> *head, *tail;
        // Free store points to an available node on the heap.
        // Released is the head of a singly-linked list of discarded nodes.
        dl_node<T> *free_store, *released;
        // Allocations is the head of a singly-linked list of pointers pointing 
        // to heap-allocated blocks of memory.
        slp_node< dl_node<T> > *allocations;
        // The number of nodes that remain available on the heap.
        size_t nodes_remaining;
        // The number of elements in the deque.
        size_t len;

        // Dynamically allocate 'exp_mag' nodes and record the memory address 
        // of the allocation so that it can be released in the destructor.
        void allocate_nodes() {
            
            // Dynamically allocate 'exp_mag' nodes.
            this->free_store = new dl_node<T>[this->exp_mag];
            // Allocate slp node to store memory address.
            slp_node< dl_node<T> > *store_node = new slp_node< dl_node<T> >;
            // Put memory address in node.
            store_node->item = this->free_store; 
            // Insert node at the front of the linked-list pointed to by this->allocations.
            store_node->next = this->allocations;
            this->allocations = store_node;
            // Update nodes remaining on the free store.
            this->nodes_remaining = this->exp_mag;
            // Update the expansion magnitude for the next allocation.
            this->exp_mag *= this->exp_fac;
        }

        // Request a node from the heap.
        dl_node<T>* get_node() {
            
            dl_node<T> *node;

            // If there is a previously released node available, select it.
            if ( this->released ) {
                node = released;
                released = node->next;
                return node;
            } else if (this->nodes_remaining == 0) {
                // If there isn't any released nodes and the free store is empty,
                // dynamically allocate room for 'exp_mag' more nodes.
                allocate_nodes();
            } 
            node = this->free_store++;
            this->nodes_remaining--;

            return node;
        }

        // Marks a node as released and makes it available for future addition to the deque.
        void delete_node(dl_node<T> *node) {
            node->next = this->released;
            this->released = node;
        }

    public:

        deque(unsigned exp_mag = 100, unsigned exp_fac = 2)
            : exp_mag(exp_mag), exp_fac(exp_fac)
        {   
            // Initialize the head and tail to NULL to indicate an empty deque.
            this->head = this->tail = NULL;
            // Initialize 'released' and 'allocations' to NULL to indicate an empty list.
            this->released = NULL; this->allocations = NULL;
            // Initialize length of deque to 0.
            this->len = 0UL;
            // Dynamically allocate room for 'exp_mag' nodes.
            allocate_nodes();
        }

        // Release all dynamically allocated memory.
        ~deque() {
            
            slp_node< dl_node<T> > *next_node, *current_node = this->allocations;

            do {
                next_node = current_node->next;
                delete[] current_node->item;
                delete current_node;
                current_node = next_node;
            } while( current_node );
        }

        // Provide global function with access to private members.
        template <typename T_s>
        friend std::ostream& std::operator<<(std::ostream& os, const deque<T_s>& deq);

        // Returns the number of elements in the deque.
        size_t length() {
            return this->len;
        }

        // Places 'item' at the end of the deque.
        void append(T item) {
            
            dl_node<T> *node = get_node();
            node->item = item;
            if ( ! this->len ) {
                node->prev = node->next = NULL;
                this->head = this->tail = node;
            } else {
                node->prev = this->tail;
                node->next = NULL;
                node->prev->next = this->tail = node;
            }

            ++this->len;
        }

        // Places 'item' at the front of the deque.
        void prepend(T item) {

            if ( ! this->len ) {
                append(item); return;
            } else {
                dl_node<T> *node = get_node();
                node->item = item;
                node->next = this->head;
                node->prev = NULL;
                node->next->prev = this->head = node;
            }

            ++this->len;
        }

        // Removes and returns the item at the end of the deque.
        T pop() {

            if ( ! this->len ) exit(EXIT_FAILURE);

            dl_node<T> *last_node = this->tail;
            T item = last_node->item;
            if (last_node->prev) {
                last_node->prev->next = NULL;
            } else {
                this->head = NULL;
            }
            this->tail = last_node->prev;
            delete_node(last_node);
            this->len--;
            return item;
        }

        // Removes and returns the item at the front of the deque.
        T prepop() {

            if ( ! this->len ) exit(EXIT_FAILURE);

            dl_node<T> *first_node = this->head;
            T item = first_node->item;
            if (first_node->next) { 
                first_node->next->prev = NULL; 
            } else {
                this->tail = NULL; 
            }
            this->head = first_node->next;
            delete_node(first_node);
            this->len--;
            return item;
        }
};

// Overload << operator to accept deque.
template <typename T>
std::ostream& std::operator<<(std::ostream& os, const deque<T>& deq) {
    
    dl_node<T> *tmp_node = deq.head;
    while ( tmp_node ) {
        os << tmp_node->item << " ";
        tmp_node = tmp_node->next;
    } 
    
    return os;
}



// For testing.
int main() {

    deque<int> my_deque(5, 2);
    my_deque.prepend(2);
    my_deque.prepend(3);
    my_deque.append(69);

    std::cout << my_deque << std::endl;
    std::cout << "Pop: " << my_deque.pop() << std::endl;
    std::cout << my_deque << std::endl;
    std::cout << "Prepop: " << my_deque.prepop() << std::endl;
    std::cout << my_deque << std::endl;
    std::cout << "Prepop: " << my_deque.prepop() << std::endl;
    std::cout << my_deque << std::endl;

    for (size_t i = 0; i < 5; i++) {
        my_deque.append(i + 15);
    }
    
    std::cout << my_deque << std::endl;
    std::cout << "Length: " << my_deque.length() << std::endl;
}
