#pragma once 
#include <iostream>
#include <vector>

template<typename Iterator>
class IteratorRange {
public:

    IteratorRange() = default;
    IteratorRange(Iterator begin_page, Iterator end_page):begin_(begin_page), end_(end_page){};
    
    Iterator begin() const{
        return begin_;
    }
        
    Iterator end( ) const{
        return end_;
    }
        
    size_t size() const{
        return static_cast<size_t>(distance(end_,begin_));  
    }
    
private:
    Iterator begin_;
    Iterator end_;
};


template <typename Iterator>
class Paginator {
public:
        Paginator() = default;
        Paginator (Iterator it_begin, Iterator it_end, size_t page_size){
            if (it_end - page_size <= it_begin ) {
                iterator_range_vector.push_back(IteratorRange(it_begin, it_end));
            } else {
                Iterator it = it_begin + page_size;
                while (it < it_end){
                   iterator_range_vector.push_back(IteratorRange(it_begin, it)); 
                   it_begin += page_size;
                   it += page_size;
                }
                iterator_range_vector.push_back(IteratorRange(it_begin, it_end));
                
            }
        }
        
       auto begin() const{
           return iterator_range_vector.begin();
       }
       auto end() const{
           return iterator_range_vector.end();
       }
       size_t size() {
           return iterator_range_vector.size();
       }
        
    private:
   std::vector <IteratorRange<Iterator>> iterator_range_vector; 
     
};


template <typename Iterator>
std::ostream& operator << (std::ostream& out, const IteratorRange<Iterator>& page){
    for (auto it = page.begin(); it < page.end(); ++it){
        out << "{ " << *it << " }";
    }
    return out;
}

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}
