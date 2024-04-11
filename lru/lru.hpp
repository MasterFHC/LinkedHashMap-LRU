#ifndef SJTU_LRU_HPP
#define SJTU_LRU_HPP

#include "utility.hpp"
#include "exceptions.hpp"
#include "class-integer.hpp"
#include "class-matrix.hpp"
using std::cout, std::endl;
class Hash {
public:
	unsigned int operator () (Integer lhs) const {
		int val = lhs.val;
		return std::hash<int>()(val);
	}
};
class Equal {
public:
	bool operator () (const Integer &lhs, const Integer &rhs) const {
		return lhs.val == rhs.val;
	}
};

namespace sjtu {
template<class T> class Node{
public:
    T* info;
    Node* pre;
    Node* nxt;
    Node(): info(nullptr), nxt(nullptr), pre(nullptr) {}
    Node(Node* pre, Node* nxt) : info(nullptr), nxt(nxt), pre(pre) {}
    Node(T* info, Node* pre, Node* nxt) : info(info), nxt(nxt), pre(pre) {}
};
template<class T> class double_list{
public:
    class iterator;
	
    iterator head, tail;
    Node<T> *head_node, *tail_node;
// --------------------------
    inline void initialize(){
        head_node = new Node<T>(), tail_node = new Node<T>();
        head_node->nxt = tail_node, head_node->pre = nullptr;
        tail_node->pre = head_node, tail_node->nxt = nullptr;
        head.ptr = head_node, head.cont = this;
        tail.ptr = tail_node, tail.cont = this;
    }
	double_list(){
        initialize();
	}
	double_list(const double_list<T> &other){
        initialize();
        for(auto it=other.begin();it!=other.end();it++){
            insert_tail((*it));
        }
	}
    double_list<T> & operator=(const double_list<T> &other){
        if(&other == this) return (*this);
        while(!empty()){
            delete_head();
        }
        head_node->nxt = head_node->pre = tail_node->nxt = tail_node->pre = nullptr;
        delete head_node;
        delete tail_node;
        initialize();
        for(auto it=other.begin();it!=other.end();it++){
            insert_tail((*it));
        }
        return (*this);
	}
	~double_list(){
        while(!empty()){
            delete_head();
        }
        head_node->nxt = head_node->pre = tail_node->nxt = tail_node->pre = nullptr;
        delete head_node;
        delete tail_node;
	}

	class iterator{
	public:
    	Node<T>* ptr;//pointing node
        double_list* cont;//container
    
	    // --------------------------
		iterator() : ptr(nullptr), cont(nullptr) {}
        iterator(Node<T>* t, double_list* cont) : ptr(t), cont(cont) {}
		iterator(const iterator &t) : ptr(t.ptr), cont(t.cont) {}
		~iterator(){
            ptr = nullptr;
            cont = nullptr;
        }
        /**
		 * iter++
		 */
		iterator operator++(int) {
            if(ptr == cont->get_tail()) throw("segmentation fault it++");
            iterator ret = *this;
            ptr = ptr->nxt;
            return ret;
		}
        /**
		 * ++iter
		 */
		iterator &operator++() {
            if(ptr == cont->get_tail()) throw("segmentation fault ++it");
            ptr = ptr->nxt;
            return (*this);
		}
        /**
		 * iter--
		 */
		iterator operator--(int) {
            if(ptr == cont->get_head()->nxt) throw("segmentation fault it--");
            iterator ret = *this;
            ptr = ptr->pre;
            return ret;
		}
        /**
		 * --iter
		 */
		iterator &operator--() {
            if(ptr == cont->get_head()->nxt) throw("segmentation fault --it");
            ptr = ptr->pre;
            return (*this);
		}
		T &operator*() const {
            if(ptr == nullptr or ptr == cont->get_head() or ptr == cont->get_tail()) throw("invalid when *it");
            return (*(ptr->info));
		}
        Node<T>* get() const{
            return ptr;
        }
		T *operator->() const noexcept {
            return (ptr->info);
		}
		bool operator==(const iterator &rhs) const {
            return (ptr == rhs.ptr and cont == rhs.cont);
    	}
		bool operator!=(const iterator &rhs) const {
            return (ptr != rhs.ptr or cont != rhs.cont);
		}
	};
    Node<T>* get_head() const{
        return head_node;
    }
    Node<T>* get_tail() const{
        return tail_node;
    }
	iterator begin() const{
        iterator ret(head);
        ++ret;
        return (ret);
	}
	iterator end() const{
        iterator ret(tail);
        return ret;
	}
	iterator erase(iterator &pos){
        if(pos == head or pos == tail) throw("invalid iterator erase");
        Node<T> *now_ele = pos.ptr;
        Node<T> *pre_ele = now_ele->pre, *nxt_ele = now_ele->nxt;
        now_ele->nxt = now_ele->pre = nullptr;
        delete now_ele->info;
        delete now_ele;
        pre_ele->nxt = nxt_ele, nxt_ele->pre = pre_ele;
        pos.ptr = nxt_ele;
        return pos;
	}

	/**
	 * the following are operations of double list
	*/
	void insert_head(const T &val){
        Node<T>* new_node = new Node<T>(new T(val), head_node, head_node->nxt);
        head_node->nxt->pre = new_node;
        head_node->nxt = new_node;
	}
	void insert_tail(const T &val){
        Node<T>* new_node = new Node<T>(new T(val), tail_node->pre, tail_node);
        tail_node->pre->nxt = new_node;
        tail_node->pre = new_node;
	}
	void delete_head(){
        iterator it = begin();
        erase(it);
	}
	void delete_tail(){
        iterator it = end();
        erase(--it);
	}
	bool empty(){
        return begin() == end();
	}
};

template<
	class Key,
	class T,
	class Hash = std::hash<Key>, 
	class Equal = std::equal_to<Key>
> class hashmap{
public:
	using value_type = pair<const Key, T>;
	double_list<value_type>* hash_table;
    int cap = 16;
    double fac = 8;
    int size = 0;
// --------------------------
	hashmap() {
        hash_table = new double_list<value_type>[cap];
	}
	hashmap(const hashmap &other){
        cap = other.cap;
        size = other.size;
        hash_table = new double_list<value_type>[cap];
        for(int i=0;i<cap;i++){
            hash_table[i] = other.hash_table[i];
        }
	}
	~hashmap(){
        delete [] hash_table;
	}
	hashmap & operator=(const hashmap &other){
        if(&other == this) return (*this);
        delete [] hash_table;
        cap = other.cap;
        size = other.size;
        hash_table = new double_list<value_type>[cap];
        for(int i=0;i<cap;i++){
            hash_table[i] = other.hash_table[i];
        }
        return (*this);
	}

	class iterator{
	public:
        value_type* ptr;
// --------------------------
        /**
         * the follows are constructors and destructors
         * you can also add some if needed.
        */
		iterator(){
            ptr = nullptr;
		}
        iterator(const value_type* &t){
            ptr = t;
        }
		iterator(const iterator &t){
            ptr = t.ptr;
		}
		~iterator(){
            ptr = nullptr;
        }

        /**
		 * if point to nothing
		 * throw 
		*/
		value_type &operator*() const {
            if(ptr == nullptr) throw("hashmap point to nothing");
            return (*ptr);
		}

        /**
		 * other operation
		*/
		value_type *operator->() const noexcept {
            return ptr;
		}
		bool operator==(const iterator &rhs) const {
            return (ptr == rhs.ptr);
    	}
		bool operator!=(const iterator &rhs) const {
            return (ptr != rhs.ptr);
		}
	};
    int get_hash(Key key) const{
        return (Hash()(key) % cap);
    }
	void clear(){
        cap = 16;
        size = 0;
        delete [] hash_table;
        hash_table = new double_list<value_type>[cap];
	}
	/**
	 * you need to expand the hashmap dynamically
	*/
	void expand(){
        int org_cap = cap;
        cap <<= 1;
        auto new_table = new double_list<value_type>[cap];
        for(int i=0;i<org_cap;i++){
            for(auto it=hash_table[i].begin();it!=hash_table[i].end();it++){
                new_table[get_hash((*it).first)].insert_head((*it));
            }
        }
        delete [] hash_table;
        hash_table = new_table;
	}

    /**
     * the iterator point at nothing
    */
	iterator end() const{
        iterator it;
        it.ptr = nullptr;
        return it;
	}
	/**
	 * find, return a pointer point to the value
	 * not find, return the end (point to nothing)
	*/
	iterator find(const Key &key)const{
        for(auto it=hash_table[get_hash(key)].begin();it!=hash_table[get_hash(key)].end();it++){
            if(Equal()((*it).first, key)){
                iterator ret;
                ret.ptr = &(*it);
                return ret;
            }
        }
        return end();
	}
	/**
	 * already have a value_pair with the same key
	 * -> just update the value, return false
	 * not find a value_pair with the same key
	 * -> insert the value_pair, return true
	*/
	sjtu::pair<iterator,bool> insert(const value_type &value_pair){
        size++;
        if(size >= cap*fac){
            expand();
        }
        auto it = find(value_pair.first);
        if(it == end()){
            hash_table[get_hash(value_pair.first)].insert_head(value_pair);
            return sjtu::pair<iterator,bool>(it, false);
        }
        (*it).second = value_pair.second;
        return sjtu::pair<iterator,bool>(it, true);
	}
	/**
	 * the value_pair exists, remove and return true
	 * otherwise, return false
	*/
	bool remove(const Key &key){
        size--;
        auto it = find(key);
        if(it == end()) return false;
        for(auto del_it=hash_table[get_hash(key)].begin();del_it!=hash_table[get_hash(key)].end();del_it++){
            if(Equal()((*del_it).first, key)){
                hash_table[get_hash(key)].erase(del_it);
                return true;
            }
        }
        return true;
	}
};

template<
	class Key,
	class T,
	class Hash = std::hash<Key>, 
	class Equal = std::equal_to<Key>
> class linked_hashmap{
public:
	typedef pair<const Key, T> value_type;
    class iterator;
	double_list<value_type>* linker = nullptr;
    hashmap<Key,iterator,Hash,Equal>* hash_map;

// --------------------------
	class const_iterator;
	class iterator{
	public:
        Node<value_type>* ptr;
        double_list<value_type>* cont;
		iterator(){
            ptr = nullptr;
		}
        iterator(Node<value_type>* node, double_list<value_type>* cont) : ptr(node), cont(cont) {}
		iterator(const iterator &other){
            ptr = other.ptr;
            cont = other.cont;
		}
		~iterator(){
            ptr = nullptr;
		}

		/**
		 * iter++
		 */
		iterator operator++(int) {
            if(ptr == cont->end().ptr) throw("linked hashmap invalid it++");
            iterator ret((*this));
            ptr = ptr->nxt;
            return ret;
        }
		/**
		 * ++iter
		 */
		iterator &operator++() {
            if(ptr == cont->end().ptr) throw("linked hashmap invalid ++it");
            ptr = ptr->nxt;
            return (*this);
        }
		/**
		 * iter--
		 */
		iterator operator--(int) {
            if(ptr == cont->begin().ptr) throw("linked hashmap invalid it--");
            iterator ret((*this));
            ptr = ptr->pre;
            return ret;
        }
		/**
		 * --iter
		 */
		iterator &operator--() {
            if(ptr == cont->begin().ptr) throw("linked hashmap invalid --it");
            ptr = ptr->pre;
            return (*this);
        }

		/**
		 * if the iter didn't point to a value
		 * throw "star invalid"
		*/
		value_type &operator*() const {
            if(ptr == nullptr or ptr == cont->end().ptr) throw("star invalid");
            return (*(ptr->info));
		}
		value_type *operator->() const noexcept {
            return ptr->info;
		}

		/**
		 * operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr;
        }
		bool operator!=(const iterator &rhs) const {
            return ptr != rhs.ptr;
        }
		bool operator==(const const_iterator &rhs) const {
            return ptr != rhs.ptr;
        }
		bool operator!=(const const_iterator &rhs) const {
            return ptr != rhs.ptr;
        }
	};

	class const_iterator {
	public:
        Node<value_type>* ptr;
        double_list<value_type>* cont;
    // --------------------------   
		const_iterator() {
            ptr = nullptr;
		}
        const_iterator(Node<value_type>* node, double_list<value_type>* cont) : ptr(node), cont(cont) {}
		const_iterator(const iterator &other) {
            ptr = other.ptr;
            cont = other.cont;
		}

		/**
		 * iter++
		 */
		const_iterator operator++(int) {
            if(ptr == cont->end().ptr) throw("linked hashmap invalid const it++");
            const_iterator ret((*this));
            ptr = ptr->nxt;
            return ret;
        }
		/**
		 * ++iter
		 */
		const_iterator &operator++() {
            if(ptr == cont->end().ptr) throw("linked hashmap invalid const ++it");
            ptr = ptr->nxt;
            return (*this);
        }
		/**
		 * iter--
		 */
		const_iterator operator--(int) {
            if(ptr == cont->begin().ptr) throw("linked hashmap invalid const it--");
            const_iterator ret((*this));
            ptr = ptr->pre;
            return ret;
        }
		/**
		 * --iter
		 */
		const_iterator &operator--() {
            if(ptr == cont->begin().ptr) throw("linked hashmap invalid const --it");
            ptr = ptr->pre;
            return (*this);
        }

		/**
		 * if the iter didn't point to a value
		 * throw 
		*/
		const value_type &operator*() const {
            if(ptr == nullptr or ptr == cont->end().ptr) throw("star invalid");
            return (*(ptr->info));
		}
		const value_type *operator->() const noexcept {
            return ptr->info;
		}

		/**
		 * operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr;
        }
		bool operator!=(const iterator &rhs) const {
            return ptr != rhs.ptr;
        }
		bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr;
        }
		bool operator!=(const const_iterator &rhs) const {
            return ptr != rhs.ptr;
        }
	};

	linked_hashmap() {
        linker = new double_list<value_type>();
        hash_map = new hashmap<Key,iterator,Hash,Equal>();
	}
	linked_hashmap(const linked_hashmap &other){
        linker = new double_list<value_type>(*(other.linker));
        hash_map = new hashmap<Key,iterator,Hash,Equal>(*(other.hash_map));
	}
	~linked_hashmap() {
        delete linker;
        delete hash_map;
        linker = nullptr;
        hash_map = nullptr;
	}
	linked_hashmap & operator=(const linked_hashmap &other) {
        if(&other == this) return (*this);
        delete linker;
        delete hash_map;
        linker = new double_list<value_type>(*(other.linker));
        hash_map = new hashmap<Key,iterator,Hash,Equal>(*(other.hash_map));
        return (*this);
	}

 	/**
	 * return the value connected with the Key(O(1))
	 * if the key not found, throw 
	*/
	T & at(const Key &key) {
        auto check_ptr = (hash_map->find(key)).ptr;
        if(check_ptr == nullptr) throw("at nullptr");
        return ((check_ptr->second).ptr->info->second);
	}
	const T & at(const Key &key) const {
        auto check_ptr = (hash_map->find(key)).ptr;
        if(check_ptr == nullptr) throw("at nullptr");
        return ((check_ptr->second).ptr->info->second);
	}
	T & operator[](const Key &key) {
        auto check_ptr = (hash_map->find(key)).ptr;
        if(check_ptr == nullptr) throw("[] nullptr");
        return ((check_ptr->second).ptr->info->second);
	}
	const T & operator[](const Key &key) const {
        auto check_ptr = (hash_map->find(key)).ptr;
        if(check_ptr == nullptr) throw("[] nullptr");
        return ((check_ptr->second).ptr->info->second);
	}

	/**
	 * return an iterator point to the first 
	 * inserted and existed element
	 */
	iterator begin() {
        return iterator(linker->begin().ptr, linker);
	}
	const_iterator cbegin() const {
        return const_iterator(linker->begin().ptr, linker);
	}
    /**
	 * return an iterator after the last inserted element
	 */
	iterator end() {
        return iterator(linker->end().ptr, linker);
	}
	const_iterator cend() const {
        return const_iterator(linker->end().ptr, linker);
	}
  	/**
	 * if didn't contain anything, return true, 
	 * otherwise false.
	 */
	bool empty() const {
        return linker->begin() == linker->end();
	}

    void clear(){
        while(!linker->empty()){
            linker->delete_head();
        }
        hash_map->clear();
	}

	size_t size() const {
        return (size_t)(hash_map->size);
	}
 	/**
	 * insert the value_piar
	 * if the key of the value_pair exists in the map
	 * update the value instead of adding a new element，
     * then the order of the element moved from inner of the 
     * list to the head of the list
	 * and return false
	 * if the key of the value_pair doesn't exist in the map
	 * add a new element and return true
	*/
	sjtu::pair<iterator, bool> insert(const value_type &value) {
        auto hash_it = hash_map->find(value.first);
        if(hash_it != hash_map->end()){
            typename double_list<value_type>::iterator to_delete(hash_it.ptr->second.ptr, linker);
            linker->erase(to_delete);
            linker->insert_tail(value);
            auto ret = linker->end();
            ret--;
            hash_map->insert(pair<Key, iterator>(value.first, iterator(ret.ptr, linker)));
            return sjtu::pair<iterator, bool>(iterator(ret.ptr, linker),false);
        }
        else{
            linker->insert_tail(value);
            auto ret = linker->end();
            ret--;
            hash_map->insert(pair<Key, iterator>(value.first, iterator(ret.ptr, linker)));
            return sjtu::pair<iterator, bool>(iterator(ret.ptr, linker),true);
        }
	}
 	/**
	 * erase the value_pair pointed by the iterator
	 * if the iterator points to nothing
	 * throw 
	*/
	void remove(iterator pos) {
        if(pos.ptr == nullptr or pos == end()) throw("linked hash map nullptr error!");
        typename double_list<value_type>::iterator to_delete(pos.ptr, linker);
        Key to_remove_value = pos.ptr->info->first;
        linker->erase(to_delete);
        hash_map->remove(to_remove_value);
	}
	/**
	 * return how many value_pairs consist of key
	 * this should only return 0 or 1
	*/
	size_t count(const Key &key) const {
        if(hash_map->find(key) != hash_map->end()) return 1;
        else return 0;
	}
	/**
	 * find the iterator points at the value_pair
	 * which consist of key
	 * if not find, return the iterator 
	 * point at nothing
	*/
	iterator find(const Key &key) {
        auto it = hash_map->find(key);
        if(it == hash_map->end()) return end();
        else return it.ptr->second;
	}
};

class lru{
    using lmap = sjtu::linked_hashmap<Integer,Matrix<int>,Hash,Equal>;
    using value_type = sjtu::pair<const Integer, Matrix<int> >;
public:
    lmap* memo;
    int cap = 0;
    lru(int size){
        memo = new lmap();
        cap = size;
    }
    ~lru(){
        delete memo;
        memo = nullptr;
    }
    /**
     * save the value_pair in the memory
     * delete something in the memory if necessary
    */
    void save(const value_type &v) const{
        if(memo->find(v.first) != memo->end()){
            memo->insert(v);
        }
        else{
            if(memo->size() == cap){
                memo->remove(memo->begin());
                memo->insert(v);
            }
            else{
                memo->insert(v);
            }
        }
    }
    /**
     * return a pointer contain the value
    */
    Matrix<int>* get(const Integer &v) const{
        if(memo->find(v) != memo->end()){
            Matrix<int> sec = memo->find(v).ptr->info->second;
            memo->remove(memo->find(v));
            memo->insert(value_type(v, sec));
            return &(memo->find(v).ptr->info->second);
        }
        else{
            return nullptr;
        }
    }
    /**
     * just print everything in the memory
     * to debug or test.
     * this operation follows the order, but don't
     * change the order.
    */
    void debug(){
        memo->debug();
    }
    void print (){
        sjtu::linked_hashmap <Integer,Matrix<int>,Hash,Equal>::iterator it ;
        for (it=memo->begin();it!=memo->end();it++){
            std::cout<<(*it).first.val<<" "<<(*it).second<<std::endl;
        }
    }

};
}

#endif