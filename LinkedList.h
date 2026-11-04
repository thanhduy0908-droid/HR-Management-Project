#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <cstddef>

/*************************************************************
 * @Description Danh sach lien ket don, cai dat theo class template.
 * @attention   Moi Node chua mot gia tri DataType va con tro den Node tiep theo.
 * @attention   Du lieu duoc them vao cuoi danh sach va duyet theo thu tu da them.
 *************************************************************/
template <typename DataType>
class LinkedList {
private:
    struct NODE {
        DataType Data;
        NODE* Next;

        explicit NODE(const DataType& data) : Data(data), Next(nullptr) {}
    };

    NODE* Head;
    NODE* Tail;
    std::size_t Size;

public:
    class const_iterator {
    private:
        const NODE* Current;

    public:
        explicit const_iterator(const NODE* current) : Current(current) {}

        const DataType& operator*() const {
            return Current->Data;
        }

        const_iterator& operator++() {
            Current = Current->Next;
            return *this;
        }

        bool operator!=(const const_iterator& other) const {
            return Current != other.Current;
        }

        bool operator==(const const_iterator& other) const {
            return Current == other.Current;
        }
    };

    LinkedList();
    ~LinkedList();

    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    void add(const DataType& item);
    void clear();
    bool empty() const;
    std::size_t size() const;
    const_iterator begin() const;
    const_iterator end() const;
};

template <typename DataType>
LinkedList<DataType>::LinkedList() : Head(nullptr), Tail(nullptr), Size(0) {}

template <typename DataType>
LinkedList<DataType>::~LinkedList() {
    clear();
}

template <typename DataType>
void LinkedList<DataType>::add(const DataType& item) {
    NODE* newNode = new NODE(item);
    if (Tail == nullptr) {
        Head = newNode;
    } else {
        Tail->Next = newNode;
    }
    Tail = newNode;
    ++Size;
}

template <typename DataType>
void LinkedList<DataType>::clear() {
    while (Head != nullptr) {
        NODE* nextNode = Head->Next;
        delete Head;
        Head = nextNode;
    }
    Tail = nullptr;
    Size = 0;
}

template <typename DataType>
bool LinkedList<DataType>::empty() const {
    return Size == 0;
}

template <typename DataType>
std::size_t LinkedList<DataType>::size() const {
    return Size;
}

template <typename DataType>
typename LinkedList<DataType>::const_iterator LinkedList<DataType>::begin() const {
    return const_iterator(Head);
}

template <typename DataType>
typename LinkedList<DataType>::const_iterator LinkedList<DataType>::end() const {
    return const_iterator(nullptr);
}

#endif
