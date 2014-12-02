#include <boost/tokenizer.hpp>
#include <string>
#include <iostream>
#include <unistd.h>

using namespace std;
using namespace boost;

int main(){
    string str;
    getline(cin, str);

    char_separator<char> sep("&|");
    tokenizer<char_separator<char> > tokens(str, sep);
    for(tokenizer<char_separator<char> >::iterator it = tokens.begin();
        it != tokens.end(); it++){

        cout << *it << "." << endl;
    }


    return 0;
}
