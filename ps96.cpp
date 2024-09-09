#include <bits/stdc++.h>
using namespace std;

//////////////////////////////////////Machine Coding Question: Dynamic Multilevel Caching System////////////////////////////////////////////

// Cache Entry structure
struct CacheEntry {
    string key;
    string value;
    int frequency;
    list<string>::iterator lruIt;
};

// Base Cache Level class
class CacheLevel {
public:
    virtual ~CacheLevel() {}
    virtual string get(string key) = 0;
    virtual void put(string key, string value) = 0;
    virtual void display() = 0;
};

// LRU Cache Level implementation
class LRUCache : public CacheLevel {
private:
    int capacity;
    list<string> lruList; 
    unordered_map<string, CacheEntry> cacheMap; 

public:
    LRUCache(int size) : capacity(size) {}

    string get(string key) override {
        if (cacheMap.find(key) != cacheMap.end()) {
            // Move accessed key to front of LRU list
            lruList.splice(lruList.begin(), lruList, cacheMap[key].lruIt); 
            return cacheMap[key].value;
        }
        return ""; 
    }

    void put(string key, string value) override {
        if (cacheMap.find(key) != cacheMap.end()) {
            // Updating the value and moving to front of LRU list
            cacheMap[key].value = value;
            lruList.splice(lruList.begin(), lruList, cacheMap[key].lruIt);
            return;
        }

        if (lruList.size() >= capacity) { 
            // Evicting LRU entry
            string lruKey = lruList.back();
            lruList.pop_back();
            cacheMap.erase(lruKey); 
        }

        // Adding new entry le
        lruList.push_front(key);
        cacheMap[key] = {key, value, 0, lruList.begin()};
    }

    void display() override {
        cout << "LRU Cache:" << endl;
        for (auto it = lruList.begin(); it != lruList.end(); ++it) {
            cout << "  Key: " << *it << ", Value: " << cacheMap[*it].value << endl;
        }
    }
};

// LFU Cache Level implementation very similar to LRU implementaion
class LFUCache : public CacheLevel {
private:
    int capacity;
    unordered_map<string, CacheEntry> cacheMap;
    unordered_map<int, list<string>> freqListMap; 
    int minFrequency; 

public:
    LFUCache(int size) : capacity(size), minFrequency(0) {}

    string get(string key) override {
        if (cacheMap.find(key) == cacheMap.end()) {
            return "";
        }

        // Updating frequency and list position
        CacheEntry& entry = cacheMap[key];
        freqListMap[entry.frequency].erase(entry.lruIt);
        entry.frequency++;
        freqListMap[entry.frequency].push_front(key);
        entry.lruIt = freqListMap[entry.frequency].begin();

        // Updating minimum frequency if needed
        if (freqListMap[minFrequency].empty()) {
            minFrequency++;
        }

        return entry.value;
    }

    void put(string key, string value) override {
        if (cacheMap.find(key) != cacheMap.end()) {
            // Update existing entry
            cacheMap[key].value = value;
            get(key); 
            return;
        }

        if (cacheMap.size() >= capacity) {
            // Evicting LFU entry
            string lfuKey = freqListMap[minFrequency].back();
            freqListMap[minFrequency].pop_back();
            cacheMap.erase(lfuKey);
        }

        // Adding new entry
        minFrequency = 1;
        freqListMap[1].push_front(key);
        cacheMap[key] = {key, value, 1, freqListMap[1].begin()};
    }

    void display() override {
        cout << "LFU Cache:" << endl;
        for (auto& freqList : freqListMap) {
            for (auto& key : freqList.second) {
                cout << "  Key: " << key << ", Value: " << cacheMap[key].value << ", Frequency: " << cacheMap[key].frequency << endl;
            }
        }
    }
};

///////////////////// Multilevel Cache System ////////////////////////
class MultiLevelCache {
private:
    vector<CacheLevel*> cacheLevels;

public:
    ~MultiLevelCache() {
        for (CacheLevel* level : cacheLevels) {
            delete level;
        }
    }

    void addCacheLevel(int size, string evictionPolicy) {
        CacheLevel* newLevel;
        if (evictionPolicy == "LRU") {
            newLevel = new LRUCache(size);
        } else if (evictionPolicy == "LFU") {
            newLevel = new LFUCache(size);
        } else {
            throw invalid_argument("Invalid eviction policy");
        }
        cacheLevels.push_back(newLevel);
    }

    string get(string key) {
        // Searching for key in each cache level
        for (int i = 0; i < cacheLevels.size(); ++i) {
            string value = cacheLevels[i]->get(key);
            if (!value.empty()) { 
                // If found, promoting to higher levels
                for (int j = i - 1; j >= 0; --j) {
                    cacheLevels[j]->put(key, value); 
                }
                return value;
            }
        }

        // Simulating the  fetching from main memory ????????????Sample Example
        string value = "Value from main memory"; 

        // Store in L1 cache and eviction of needed one
        cacheLevels[0]->put(key, value);
        return value;
    }

    void put(string key, string value) {
        // Insert into L1 cache (and potentially evict)
        cacheLevels[0]->put(key, value); 
    }

    void removeCacheLevel(int level) {
        if (level < 1 || level > cacheLevels.size()) {
            throw invalid_argument("Invalid cache level index");
        }
        delete cacheLevels[level - 1]; // Delete the cache level object
        cacheLevels.erase(cacheLevels.begin() + level - 1); 
    }

    void displayCache() {
        for (int i = 0; i < cacheLevels.size(); ++i) {
            cout << "Level " << i + 1 << ":" << endl;
            cacheLevels[i]->display();
            cout << endl;
        }
    }
};

///////////// main function ///////////////
int main() {
    //Creating multilevel cache
    MultiLevelCache cacheSystem;


    //adding levels in multilevel cache system
    cacheSystem.addCacheLevel(3, "LRU"); 
    cacheSystem.addCacheLevel(2, "LFU");

    //updating the values in the cache
    cacheSystem.put("A", "1");
    cacheSystem.put("B", "2");
    cacheSystem.put("C", "3");

    //printing the output
    cout << "Get 'A': " << cacheSystem.get("A") << endl; 

    cacheSystem.put("D", "4"); 

    cout << "Get 'C': " << cacheSystem.get("C") << endl; 


    cout<<endl<<"Displaying the Multilevel Cache ";
    cout << "\nCache State:\n";
    cacheSystem.displayCache();
    
    return 0;
}