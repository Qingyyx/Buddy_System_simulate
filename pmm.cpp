#include <bits/stdc++.h>

using namespace std;


#define HEAP_BITS 32
#define HEAP_SIZE (1ll << HEAP_BITS)

#define ALLOC_MAX (1 << 24) // 16 MB

struct Area {
    uint32_t start, end;
    Area(uint32_t s, uint32_t e) : start(s), end(e) { }
};

struct PMM {
    vector<vector<int>> block;

    PMM(size_t bits) {
        block.resize(bits + 1);
        block[bits].push_back(0);
    }

    size_t getfriend(const Area& area) {
        size_t size = area.end - area.start + 1;
        size_t bits = __lg(size);
        size_t pos = area.start ^ (1 << bits);
        return pos;
    }


    Area split_block(size_t bits) {
        if (bits > HEAP_BITS) {
            return Area(0, 0); // No available block
        }
        if (block[bits].empty()) {
            // Try to split a larger block
            Area area = split_block(bits + 1);
            if (area.start == 0 && area.end == 0) {
                return Area(0, 0); // No available block
            }
            size_t pos = area.start;
            block[bits].push_back(pos + (1 << bits));
            return Area(pos, pos + (1 << bits));
        } else {
            size_t pos = block[bits].back();
            block[bits].pop_back();
            return Area(pos, pos + (1 << bits));
        }
    }

    void merge_block(size_t bits, size_t pos) {
        if (bits > HEAP_BITS) {
            return; // Cannot merge further
        }
        size_t friend_pos = getfriend(Area(pos, pos + (1 << bits)));
        auto it = remove(block[bits].begin(), block[bits].end(), friend_pos);
        if (it != block[bits].end()) {
            block[bits].erase(it);
            merge_block(bits + 1, min(pos, friend_pos)); // Merge with the friend block
        } else {
            block[bits].push_back(pos);
        }
    }


    Area Malloc(size_t size) {
        if (size == 0 || size > ALLOC_MAX) {
            return Area(0, 0); // Invalid size
        }
        size_t bits = __lg(size - 1) + 1; // Round up to the next power of two
        return split_block(bits);
    }

    void Mfree(const Area& area) {
        size_t size = area.end - area.start + 1;
        size_t bits = __lg(size);
        merge_block(bits, area.start);
    }

};


random_device rd;
mt19937 gen(rd());

signed main(const signed argc, const char* argv[]) {

    PMM heap(HEAP_BITS);
    int test = 1000;
    if (argc > 1) {
        test = atoi(argv[1]);
    }
    vector<Area> areas;
    while (test--) {
        if (gen() & 1) {
            size_t size = gen() % (ALLOC_MAX)+1; // Allocate between 1 and 16 MB
            Area current = heap.Malloc(size);
            if (current.start == 0 && current.end == 0) {
                cout << "Allocation failed." << endl;
            } else {
                cout << "Allocated area: [" << current.start << ", " << current.end << ")" << endl;
                areas.push_back(current);
            }
        } else {
            if (areas.empty()) {
                cout << "No areas to free." << endl;
                continue;
            }
            size_t index = gen() % areas.size();
            heap.Mfree(areas[index]);
            cout << "Freed area: [" << areas[index].start << ", " << areas[index].end << ")" << endl;
            areas.erase(areas.begin() + index);
        }
    }
}


