

#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <pgen2/types/data_container.h>


namespace pgen {
namespace types {
        


data_container::data_container(size_t pivot) : _pivot(pivot) { 
    _vec.resize(_pivot, 0);
}

const uint8_t* data_container::data() const {
    return _vec.data();       
}

size_t data_container::size() const {
    return _vec.size(); 
}

size_t data_container::pivot() const {
    return _pivot;      
}

void data_container::write_before(int index, void* buf, size_t buflen) {
    std::vector<uint8_t>::iterator it = _vec.begin();
    it += index - buflen;
    uint8_t* p = (uint8_t*)buf;
    for (size_t i=0; i<buflen; i++, it++) {
        *it = p[i];
    }
}

void data_container::set_content(const void* buf, size_t buflen) {
    _vec.insert(_vec.end(), (uint8_t*)buf, (uint8_t*)buf+buflen);
}
void data_container::delete_content() {
    _vec.resize(pivot());
}



} /* namespace types */
} /* namespace pgen */
