#include "page_policy.hpp"

#include <stdio.h>


ARC::ARC(size_t mem_size) : PagePolicy(mem_size) { }

bool ARC::add_memtrace_(const Record &record) {
    long vpn = record.addr << 14;
    int index;

    // case 1) page is in the cache(T1) : hit
    if( (index = T1.find(vpn)) != -1 ) {
        T1.remove(index, vpn);
	T2.push(vpn);
        return true;
    // case 2) page is in the cache(T2) : hit
    } else if( (index = T2.find(vpn)) != -1 ) {
        T2.remove(index, vpn);
	T2.push(vpn);
        return true;
    // case 3) page is in ghost cache(B1) : miss
    } else if( (index = B1.find(vpn)) != -1 ) {
	increase_P();
	replace(vpn);
	// move page : B1 --> T2
	B1.remove(index, vpn);
	T2.push(vpn);
        return false;
    // case 4) page is in ghost cache(B2) : miss
    } else if( (index = B2.find(vpn)) != -1 ) {
	decrease_P();
	replace(vpn);
	// move page : B2 --> T2
	B2.remove(index, vpn);
	T2.remove(vpn);
        return false;
    // case 5) page is not in both : miss
    } else {
	size_t T1_size = T1.get_size();
	size_t T2_size = T2.get_size();
	size_t B1_size = B1.get_size();
	size_t B2_size = B2.get_size();
	size_t L1_size = T1_size + B1_size;

	// if L1 (T1+B1) has exactly c pages
	if( L1_size == C ) {
		if( T1_size < C ) {
		    // delete LRU page in B1
		    B1.pop();
		    replace(vpn);
		// if B1 is empty
		} else {
		    T1.pop();
		}
	// if L1 (T1+B1) has less than c pages
	} else if( L1_size < C ) {
      	    size_t total_size = T1_size + T2_size + B1_size + B2_size;
    	    if( total_size >= C ) {
	    	if( total_size == 2*C ) {
		    // delete LRU page in B2
		    B2.pop();
		}
		replace(vpn);
	    }	    
	}

	T1.push(vpn);
        return false;
    }
}


void ARC::increase_P() {
    size_t B1_size = B1.get_size();
    size_t B2_size = B2.get_size();

    if( B1_size >= B2_size ) {
        P++;
    } else {
    	P += B2_size/B1_size;
    }
}

void ARC::decrease_P() {
    size_t B1_size = B1.get_size();
    size_t B2_size = B2.get_size();

    if( B1_size >= B2_size ) {
        P--;
    } else {
    	P -= B1_size/B2_size;
    }
}
void ARC::replace(long vpn) {
    size_t T1_size = T1.get_size();
    long victim_vpn;
    if( T1_size != 0 && (T1_size > P || (B2.find(vpn) != -1 && T1_size == P)) ) {
        // move page : T1 --> B1
	victim_vpn = T1.pop(); // delete the LRU page in T1
        B1.push(victim_vpn); // push page to B1
    } else {
    	// move page : T2 --> B2
	victim_vpn = T2.pop(); // delte the LRU page in T2
	B2.push(victim_vpn); // push page to B2
    }
}

///////////////////////////////// List

List::List() { count = 0; }

int List::find(long vpn) {
    if (vpn_to_index.find(vpn) == vpn_to_index.end()) {
        return -1;
    } else {
	vpn_to_index[vpn] = count;
    	return (int)count;
    }
}

void List::remove(long vpn) {	
    int index = vpn_to_index[vpn];
    index_to_vpn.erase(index);
    vpn_to_index.erase(vpn);
}

void List::remove(size_t index, long vpn) {
    index_to_vpn.erase(index);
    vpn_to_index.erase(vpn);
}


long List::pop() {
    // erase fisrt page
    long target = index_to_vpn.begin()->second;
    index_to_vpn.erase(index_to_vpn.begin());
    vpn_to_index.erase(target);

    return target;
}

void List::push(long vpn) {
    count++;
    vpn_to_index[vpn] = count;
    index_to_vpn[count] = vpn;
}

size_t List::get_size() {
    return index_to_vpn.size();
}


