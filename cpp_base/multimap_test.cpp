#include <iostream>
#include <map>
 
int main()
{  
    std::multimap<int,char> example = {{1,'a'},{2,'b'},{2,'c'}};
    for(auto &it:example)
    {
        std::cout << "Found " << it.first << " " << it.second << '\n';
    }
    std::cout<<"-----------\n";
    
    auto search = example.find(2);
    std::multimap<int,char>::iterator it;
    for(it=search;it!=example.end();++it)
    {
        std::cout << "Found " << it->first << " " << it->second << '\n';
    }
    
    std::cout<<"-----------\n";
    if (search != example.end()) {
        std::cout << "Found " << search->first << " " << search->second << '\n';
    } else {
        std::cout << "Not found\n";
    }
    std::cout<<"-----------\n";

    
}