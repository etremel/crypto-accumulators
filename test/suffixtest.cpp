#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string.h>
#include <cassert>
#include <string>

#include <suffix/SuffixTree.hpp>

using std::cout;
using std::cin;
using std::cerr;
using std::setw;
using std::flush;
using std::endl;
using std::string;

int main()
{


    cout << "Normally, suffix trees require that the last\n"
         << "character in the input string be unique.  If\n"
         << "you don't do this, your tree will contain\n"
         << "suffixes that don't end in leaf nodes.  This is\n"
         << "often a useful requirement. You can build a tree\n"
         << "in this program without meeting this requirement,\n"
         << "but the validation code will flag it as being an\n"
         << "invalid tree\n\n";
    cout << "Enter string: " << flush;
    string input;
    getline(cin, input);
    SuffixTree stree(std::move(input));

    stree.build();
    //
    // Once all N prefixes have been added, the resulting table
    // of edges is printed out, and a validation step is
    // optionally performed.
    //
    stree.dump_edges();
    cout << "Would you like to validate the tree? "
            << flush;
    std::string s;
    getline( cin, s );
    if ( s.size() > 0 && s[ 0 ] == 'Y' || s[ 0 ] == 'y' )
        stree.validate();

    return 1;
}
