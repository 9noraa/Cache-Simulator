/* Name: Aaron Cohen
 * FSUID: ajc17d
 * Assignment: 3
 */

#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <list>

using namespace std;

			//Class that holds all of the cache information
class Cache{
	private:
		int tag;
		int set;
		int offset;
			//A vector of lists that has both the cache and the LRU information
		vector<list<string> > cache;
		vector<list<int> > LRU;
		int sets;
		int lines;

		int hits = 0;
		int misses = 0;
		int references = 0;
	public:
			//Default constructor
		Cache(){
			tag = 0;
			set = 0;
			offset = 0;
			sets = 0;
			lines = 0;
		}

			//Constructor that takes cache size, line or block size and associativity to create the cache
		Cache(int cache_size, int line_size, int associativity){
			//Calculations for offset, set and tag
			offset = log2(line_size);
			set = log2((cache_size / line_size) / associativity);
			tag = 32 - (offset + set);
			sets = (cache_size / line_size) / associativity;
			lines = (cache_size / line_size) / sets;

			//Initializing the cache and LRU containers
			vector<list<string> > temp1(sets, list<string>());
			vector<list<int> > temp2(sets, list<int>());
			cache = temp1;
			LRU = temp2;

			//Setting default values in the containers
			for(int i = 0; i < sets; i++){
				for(int j = 0; j < lines; j++){
					cache[i].push_back("");
					LRU[i].push_back(0);
				}
			}
		}

			//Function that takes a memory reference and checks the cache
		void Reference(string memory){
			//Incrementing the number of references
			references++;

			//String values for cache
			string memtag;
			string memset;
			string memoffset;

			int count = -1;
			int lrucount = 0;
			bool hit = false;
			bool noLRU = false;
			int setindex = 0;

			//Using sub string to get the tag, set and offset of the reference
			memtag = memory.substr(0, tag);
			memset = memory.substr(tag, set);
			memoffset = memory.substr((tag + set), offset);
			//Getting the set index of the reference
			setindex = stoi(memset, nullptr, 2);
			//cout << memtag << memset << memoffset << " " << setindex << endl;

			//Iterating through the vector list
			//Checking for a hit
			for(auto & val : cache[setindex]){
				count++;
				if(val == memtag){
					hit = true;
					lrucount = count;
				}
			}
			count = -1;
			if(hit){
			//LRU update
				for(auto & val : LRU[setindex]){
					count++;
					if(lrucount == count){
						val = references;
					}
				}
			}
			lrucount = 0;
			count = -1;

			if(hit){
				hits++;
			}else{
			//Case for a miss
				misses++;
			//Checking for a vacant spot in the cache
				for(auto & val : LRU[setindex]){
					count++;
					if(val == 0){
						noLRU = true;
						lrucount = count;
						val = references;
						break;
					}
				}
				count = -1;
			//If no LRU is needed insert in the cache
				if(noLRU){
					for(auto & val : cache[setindex]){
						count++;
						if(lrucount == count){
							val = memtag;
							hit = true;
						}
					}
				}
				noLRU = false;
				count = -1;
			//Using the LRU function to find which reference to evict
				if(!hit){
					int a = leastUsed(setindex);
					for(auto & val : cache[setindex]){
						count++;
						if(count == a){
							val = memtag;
						}
					}
					count = -1;
					for(auto & val : LRU[setindex]){
						count++;
						if(count == a){
							val = references;
						}
					}
				}
			}
			hit = false;
		}

			//Printing the contents of the cache with os output stream
		void print(ostream & os){
			int setcount = 0;
			os << "Total memory references: " << references << endl;
			os << "Hits: " << hits << endl;
			os << "Misses: " << misses << endl;
			os << "----------------Cache----------------" << endl;
			os << "\t\tTag" << endl;

			//Iterating through each vector entry
			for(auto & list : cache){
				os << "Set " << setcount << ":" << endl;
				setcount++;
			//Iterating through the list at the vector entry
				for(auto & val : list){
					if(val == ""){
						os << "\t\tNo value" << endl;
					}else{
						os << "\t\t" << val << endl;
					}
				}
			}

		}

			//Calculates the location of the LRU of the set
		int leastUsed(int setindex){
			int ref = 0;
			int count = 0;
			//Starting at the first entry of the list
			int curr = *(begin(LRU[setindex]));

			//Iterating and getting the index with the smallest reference
			//Which means it was last referenced earliest
			for(auto & val : LRU[setindex]){
				if(curr > val){
					curr = val;
					ref = count;
				}
				count++;
			}
			//Returning the location of the LRU to evict
			return ref;
		}
};

string dec_to_bin(int n);

int main(int argc, char ** argv){
	string reference;
	int ref;

			//Getting the command line arguents for parameters of the cache
	int cache_size = stoi(argv[1]);
	int line_size = stoi(argv[2]);
	int associativity = stoi(argv[3]);

			//Cache object
	Cache cache(cache_size, line_size, associativity);

			//Files for input and output
	fstream memory;
	fstream output;
	memory.open(argv[4]);
	output.open("output.txt", ios::out);

			//File error checking
	if(!memory){
		cerr << "Couldn't open file" << endl;
		exit(1);
	}

			//Checking to the end of the file
	while(!memory.eof()){
			//Getting references in decimal and converting to binary
		memory >> reference;
		ref = stoi(reference);
		reference = dec_to_bin(ref);
		cache.Reference(reference);
	}

	cache.print(output);
}

			//Decimal to binary conversion
string dec_to_bin(int n){
	string binary = "00000000000000000000000000000000";
	for(int i = 31; i >= 0; --i){
		if(n % 2){
			binary[i] = '1';
		}
		n /= 2;
	}
	return binary;
}
