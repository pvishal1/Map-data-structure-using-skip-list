#include <iostream>
#include <random>

namespace cs540 {
using namespace std;
uniform_int_distribution<int> distribution(0, 1);

template<typename key_t, typename mapped_t>
struct skiplistnode {
	skiplistnode<key_t, mapped_t> *up;
	skiplistnode<key_t, mapped_t> *down;
	skiplistnode<key_t, mapped_t> *left;
	skiplistnode<key_t, mapped_t> *right;
	typedef pair<const key_t, mapped_t> ValueType;

	ValueType *data;
	int boundary;
	skiplistnode() {
		up = NULL;
		down = NULL;
		left = NULL;
		right = NULL;
		boundary = -1;
		data = NULL;
	}

	skiplistnode(const ValueType &v) :
			data(new ValueType(v)) {
		up = NULL;
		down = NULL;
		left = NULL;
		right = NULL;
		boundary = -1;
	}
};

template<typename key_t, typename mapped_t>
class Map {
public:
	typedef pair<const key_t, mapped_t> ValueType;
	skiplistnode<key_t, mapped_t> *head, *tail;
	int h, n;
	default_random_engine generator;
	class Iterator;
	class ConstIterator;
	class ReverseIterator;

	Map() {
		head = new skiplistnode<key_t, mapped_t>();
		tail = new skiplistnode<key_t, mapped_t>();
		head->boundary = 0;
		tail->boundary = 1;
		head->right = tail;
		tail->left = head;
		n = 0;
		h = 0;
	}

	Map(const Map &m) {
		head = new skiplistnode<key_t, mapped_t>();
		tail = new skiplistnode<key_t, mapped_t>();
		head->boundary = 0;
		tail->boundary = 1;
		head->right = tail;
		tail->left = head;
		n = 0;
		h = 0;
		copy(&m, this);
	}

	Map &operator=(const Map &m) {
		if (this == &m) {
			return *this;
		}
		copy(&m, this);
		return *this;
	}

	Map(initializer_list<const ValueType> il) {
		head = new skiplistnode<key_t, mapped_t>();
		tail = new skiplistnode<key_t, mapped_t>();
		head->boundary = 0;
		tail->boundary = 1;
		head->right = tail;
		tail->left = head;
		n = 0;
		h = 0;
		for (const ValueType *var = il.begin(); var < il.end(); var++) {
			insert(*var);
		}
	}

	~Map() {
		destroy();
	}

	skiplistnode<key_t, mapped_t> *findPosition(const key_t key) const {
		skiplistnode<key_t, mapped_t> *p;
		p = head;
		while (true) {
			while (p->right->boundary != 1
					&& (p->right->data->first < key
							|| p->right->data->first == key)) {
				p = p->right;
			}
			if (p->down != NULL) {
				p = p->down;
			} else {
				break;
			}
		}
		return p;
	}

	size_t size() const {
		return (size_t) n;
	}

	bool empty() const {
		if (n == 0) {
			return true;
		}
		return false;
	}

	pair<Iterator, bool> insert(const ValueType &ele) {
		skiplistnode<key_t, mapped_t> *p, *q, *r;
		pair<Iterator, bool> pointer;
		int i = 0;
		p = findPosition(ele.first);

		if (p->boundary == 0 || !(p->data->first == ele.first)) {
			q = new skiplistnode<key_t, mapped_t>(ele);
			q->left = p;
			q->right = p->right;
			p->right->left = q;
			p->right = q;
			pointer.first.current_node = q;
			pointer.second = true;

			while (distribution(generator)) {
				if (i >= h) {
					skiplistnode<key_t, mapped_t> *p1, *p2;
					h++;
					p1 = new skiplistnode<key_t, mapped_t>();
					p2 = new skiplistnode<key_t, mapped_t>();
					p1->boundary = 0;
					p2->boundary = 1;
					p1->right = p2;
					p1->down = head;
					p2->left = p1;
					p2->down = tail;
					head->up = p1;
					tail->up = p2;
					head = p1;
					tail = p2;
				}

				while (p->up == NULL) {
					p = p->left;
				}

				p = p->up;
				r = new skiplistnode<key_t, mapped_t>(ele);
				r->left = p;
				p->right->left = r;
				r->right = p->right;
				p->right = r;
				r->down = q;
				q->up = r;
				q = r;
				i++;
			}
			n++;
		} else {
			pointer.first.current_node = p;
			pointer.second = false;
		}
		return pointer;
	}

	void erase(const key_t &ele) {
		skiplistnode<key_t, mapped_t> *p, *q;
		p = findPosition(ele);
		if (p->data->first == ele) {
			while (p != NULL) {
				p->left->right = p->right;
				p->right->left = p->left;
				q = p->left;
				if (p->up == NULL) {
					delete(p->data);
					delete (p);
					p = NULL;
				} else {
					p = p->up;
					delete(p->down->data);
					delete (p->down);
				}
			}
			while (q->boundary == 0 && q->right->boundary == 1 && h > 0) {
				head = q->down;
				head->up = NULL;
				tail = q->right->down;
				tail->up = NULL;
				delete(q->right->data);
				delete (q->right);
				delete(q->data);
				delete (q);
				h--;
				q = head;
			}
			n--;
		} else {
			throw out_of_range("Key not in Map");
		}
	}

	void erase(Iterator pos) {
		skiplistnode<key_t, mapped_t> *p, *q;
		p = pos.current_node;
		while (p != NULL) {
			p->left->right = p->right;
			p->right->left = p->left;
			q = p->left;
			if (p->up == NULL) {
				delete (p->data);
				delete (p);
				p = NULL;
			} else {
				p = p->up;
				delete (p->down->data);
				delete (p->down);
			}
		}
		while (q->boundary == 0 && q->right->boundary == 1 && h > 0) {
			head = q->down;
			head->up = NULL;
			tail = q->right->down;
			tail->up = NULL;
			delete (q->right->data);
			delete (q->data);
			delete (q->right);
			delete (q);
			h--;
			q = head;
		}
		n--;
	}

	void destroy() {
		skiplistnode<key_t, mapped_t> *p, *q;
		p = head;
		while (p != NULL) {
			q = p->down;
			destroyRow(p);
			p = q;
		}
	}

	void destroyRow(skiplistnode<key_t, mapped_t> *q) {
		skiplistnode<key_t, mapped_t> *r;
		while (q != NULL) {
			r = q;
			q = q->right;
			delete (r->data);
			delete (r);
		}
	}

	void copy(const Map<key_t, mapped_t> *old_map,
			Map<key_t, mapped_t> *new_map) {
		skiplistnode<key_t, mapped_t> *p;
		p = old_map->head;
		while (p->down != NULL) {
			p = p->down;
		}
		p = p->right;

		while (!(p->boundary == 1)) {
			new_map->insert(*(p->data));
			p = p->right;
		}
	}

	Iterator begin() {
		skiplistnode<key_t, mapped_t> *p;
		Iterator it;
		p = head;
		while (p->down != NULL) {
			p = p->down;
		}
		it.current_node = p->right;
		return it;
	}

	Iterator end() {
		skiplistnode<key_t, mapped_t> *p;
		Iterator it;
		p = tail;
		while (p->down != NULL) {
			p = p->down;
		}
		it.current_node = p;
		return it;
	}

	ConstIterator begin() const {
		skiplistnode<key_t, mapped_t> *p;
		ConstIterator it;
		p = head;
		while (p->down != NULL) {
			p = p->down;
		}
		it.current_node = p->right;
		return it;
	}

	ConstIterator end() const {
		skiplistnode<key_t, mapped_t> *p;
		ConstIterator it;
		p = tail;
		while (p->down != NULL) {
			p = p->down;
		}
		it.current_node = p;
		return it;
	}

	ReverseIterator rbegin() {
		skiplistnode<key_t, mapped_t> *p;
		ReverseIterator it;
		p = tail;
		while (p->down != NULL) {
			p = p->down;
		}
		it.current_node = p->left;
		return it;
	}

	ReverseIterator rend() {
		skiplistnode<key_t, mapped_t> *p;
		ReverseIterator it;
		p = head;
		while (p->down != NULL) {
			p = p->down;
		}
		it.current_node = p;
		return it;
	}

	void clear() {
		Iterator it1, it2, it3;
		it2 = end();
		for (it1 = begin(); it1.current_node != it2.current_node;) {
			it3 = it1;
			it3++;
			erase(it1);
			it1 = it3;
		}
	}

	Iterator find(const key_t &ele) {
		Iterator it;
		it.current_node = findPosition(ele);
		if (!(it.current_node->data->first == ele)) {
			return end();
		}
		return it;
	}

	ConstIterator find(const key_t &ele) const {
		ConstIterator it;
		it.current_node = findPosition(ele);
		if (!(it.current_node->data->first == ele)) {
			return end();
		}
		return it;
	}

	mapped_t &at(const key_t &ele) {
		skiplistnode<key_t, mapped_t> *p;
		p = findPosition(ele);
		if (!(p->data->first == ele)) {
			throw out_of_range("Key not in Map");
		}
		return p->data->second;
	}

	const mapped_t &at(const key_t &ele) const {
		skiplistnode<key_t, mapped_t> *p;
		p = findPosition(ele);
		if (!(p->data->first == ele)) {
			throw out_of_range("Key not in Map");
		}
		return p->data->second;
	}

	bool operator==(const Map &m) {
		if (this->n != m.n) {
			return false;
		}
		skiplistnode<key_t, mapped_t> *p, *q;
		p = m.head;
		q = this->head;
		while (p->down != NULL) {
			p = p->down;
		}
		while (q->down != NULL) {
			q = q->down;
		}
		p = p->right;
		q = q->right;
		while (p->boundary != 1 && q->boundary != 1) {
			if ((!(p->data->first == q->data->first))
					&& (!(p->data->second == q->data->second))) {
				return false;
			}
			p = p->right;
			q = q->right;
		}
		return true;
	}

	bool operator!=(const Map &m) {
		return (!(*this == m));
	}

	bool lexicographical_compare(skiplistnode<key_t, mapped_t> *p,
			skiplistnode<key_t, mapped_t> *q) {
		while (p->boundary != 1 && q->boundary != 1) {
			bool flag = true;
			cout << endl << "++" << endl;
			if ((!(p->data->first < q->data->first))
					&& (!(p->data->first == q->data->first))) {
				return false;
			} else if ((!(p->data->second < q->data->second))
					&& (!(p->data->second == q->data->second))) {
				return false;
			}
			p = p->right;
			q = q->right;
		}
		return true;
	}

	bool operator<(const Map &m) {
		skiplistnode<key_t, mapped_t> *p, *q;
		p = m.head;
		q = this->head;
		while (p->down != NULL) {
			p = p->down;
		}
		while (q->down != NULL) {
			q = q->down;
		}
		p = p->right;
		q = q->right;
		if (!(lexicographical_compare(p, q))) {
			return false;
		} else if (this->n > m.n) {
			return false;
		}
		return true;
	}

	mapped_t &operator[](const key_t &key) {
		skiplistnode<key_t, mapped_t> *p;
		p = findPosition(key);
		if ((p->data == NULL) || (!(p->data->first == key))) {
			mapped_t m;
			insert( { key, m });
			p = findPosition(key);
		}
		return p->data->second;
	}

	template<typename IT_T>
	void insert(IT_T range_beg, IT_T range_end) {
		IT_T it;
		for (it = range_beg; it!=range_end; it++) {
			insert(*it);
		}
	}

	class Iterator {
	public:
		//typedef pair<const key_t, mapped_t> ValueType;
		skiplistnode<key_t, mapped_t> *current_node;

		Iterator& operator=(const Iterator &m) {
			if (this == &m) {
				return *this;
			}
			this->current_node = m.current_node;
			return *this;
		}

		Iterator &operator++() {
			current_node = current_node->right;
			return *this;
		}

		Iterator operator++(int) {
			Iterator temp = (*this);
			++(*this);
			return temp;
		}

		Iterator &operator--() {
			current_node = current_node->left;
			return *(this);
		}

		Iterator operator--(int) {
			Iterator temp = (*this);
			operator--();
			return temp;
		}

		ValueType *operator->() const {
			return current_node->data;
		}

		ValueType &operator*() const {
			return *(current_node->data);
		}

		bool operator==(const Iterator &it2) {
			return (this->current_node == it2.current_node);
		}

		bool operator==(const ConstIterator &it2) {
			return (this->current_node == it2.current_node);
		}

		bool operator!=(const Iterator &it2) {
			return !(this->current_node == it2.current_node);
		}

		bool operator!=(const ConstIterator &it2) {
			return !(this->current_node == it2.current_node);
		}

		bool operator==(const ReverseIterator &it2) {
			return (this->current_node == it2.current_node);
		}

		bool operator!=(const ReverseIterator &it2) {
			return !(this->current_node == it2.current_node);
		}
	};

	class ConstIterator {
	public:
		//typedef pair<const key_t, mapped_t> ValueType;
		skiplistnode<key_t, mapped_t> *current_node;

		ConstIterator& operator=(const ConstIterator &m) {
			if (this == &m) {
				return *this;
			}
			this->current_node = m.current_node;
			return *this;
		}

		ConstIterator &operator++() {
			current_node = current_node->right;
			return *this;
		}

		ConstIterator operator++(int) {
			ConstIterator temp = (*this);
			++(*this);
			return temp;
		}

		ConstIterator &operator--() {
			current_node = current_node->left;
			return *(this);
		}

		ConstIterator operator--(int) {
			ConstIterator temp = (*this);
			operator--();
			return temp;
		}

		const ValueType *operator->() const {
			return current_node->data;
		}

		const ValueType &operator*() const {
			return *(current_node->data);
		}

		bool operator==(const ConstIterator &it2) {
			return (this->current_node == it2.current_node);
		}

		bool operator==(const Iterator &it2) {
			return (this->current_node == it2.current_node);
		}

		bool operator!=(const ConstIterator &it2) {
			return !(this->current_node == it2.current_node);
		}

		bool operator!=(const Iterator &it2) {
			return !(this->current_node == it2.current_node);
		}

		bool operator==(const ReverseIterator &it2) {
			return (this->current_node == it2.current_node);
		}

		bool operator!=(const ReverseIterator &it2) {
			return !(this->current_node == it2.current_node);
		}
	};

	class ReverseIterator {
	public:
		//typedef pair<const key_t, mapped_t> ValueType;
		skiplistnode<key_t, mapped_t> *current_node;

		ReverseIterator& operator=(const ReverseIterator &m) {
			if (this == &m) {
				return *this;
			}
			this->current_node = m.current_node;
			return *this;
		}

		ReverseIterator &operator++() {
			current_node = current_node->left;
			return *this;
		}

		ReverseIterator operator++(int) {
			ReverseIterator temp = (*this);
			++(*this);
			return temp;
		}

		ReverseIterator &operator--() {
			current_node = current_node->right;
			return *(this);
		}

		ReverseIterator operator--(int) {
			ReverseIterator temp = (*this);
			operator--();
			return temp;
		}

		ValueType *operator->() const {
			return current_node->data;
		}

		ValueType &operator*() const {
			return *(current_node->data);
		}

		bool operator==(const ReverseIterator &it2) {
			return (this->current_node == it2.current_node);
		}

		bool operator!=(const ReverseIterator &it2) {
			return !(this->current_node == it2.current_node);
		}

		bool operator==(const ConstIterator &it2) {
			return (this->current_node == it2.current_node);
		}

		bool operator!=(const ConstIterator &it2) {
			return !(this->current_node == it2.current_node);
		}

		bool operator==(const Iterator &it2) {
			return (this->current_node == it2.current_node);
		}

		bool operator!=(const Iterator &it2) {
			return !(this->current_node == it2.current_node);
		}
	};
};

}
