//
// STREE2006.CPP - Suffix tree creation
//
// Mark Nelson, updated December, 2006
//
// This code has been tested with Borland C++ and
// Microsoft Visual C++.
//
// This program asks you for a line of input, then
// creates the suffix tree corresponding to the given
// text. Additional code is provided to validate the
// resulting tree after creation.
//
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <vector>
#include <string.h>
#include <cassert>
#include <unordered_map>
#include <memory>
#include <utility>
#include <suffix/SuffixTree.hpp>

using std::cout;
using std::cin;
using std::cerr;
using std::setw;
using std::flush;
using std::endl;
using std::make_pair;
using std::shared_ptr;
using std::make_shared;


int SuffixTreeNode::Count = 0;

SuffixTreeNode::SuffixTreeNode() :
        suffix_node(nullptr),
        root(false)
{
    Count++;
}

//
// The default ctor for Edge just sets start_node
// to the invalid value.  This is done to guarantee
// that the hash table is initially filled with unused
// edges.
//

SuffixTreeEdge::SuffixTreeEdge() : startNode(nullptr){}

//
// I create new edges in the program while walking up
// the set of suffixes from the active point to the
// endpoint.  Each time I create a new edge, I also
// add a new node for its end point.  The node's
// suffix node is set to -1 by the default Node() ctor,
// so I don't have to do anything with it at this point.
//
SuffixTreeEdge::SuffixTreeEdge(int initFirstCharIndex, int initLastCharIndex, shared_ptr<SuffixTreeNode>& parentNode , SuffixTree *containingTree) :
        firstCharIndex(initFirstCharIndex),
        lastCharIndex(initLastCharIndex),
        startNode(parentNode),
        endNode(make_shared<SuffixTreeNode>()),
        containingTree(containingTree)
{
}

//
// When a suffix ends on an implicit node, adding a new character
// means I have to split an existing edge.  This function is called
// to split an edge at the point defined by the Suffix argument.
// The existing edge loses its parent, as well as some of its leading
// characters.  The newly created edge descends from the original
// parent, and now has the existing edge as a child.
//
// Since the existing edge is getting a new parent and starting
// character, its hash table entry will no longer be valid.  That's
// why it gets removed at the start of the function.  After the parent
// and start char have been recalculated, it is re-inserted.
//
// The number of characters stolen from the original node and given
// to the new node is equal to the number of characters in the suffix
// argument, which is last - first + 1;
//

shared_ptr<SuffixTreeNode> SuffixTreeEdge::splitEdge( SuffixTree::Suffix &suffix )
{
    //Ugly hack to get a shared_ptr to this
    shared_ptr<SuffixTreeEdge> thisShared = this->containingTree->getEdge(startNode,
    		this->containingTree->text.at(firstCharIndex));
    this->containingTree->remove(*this);
    shared_ptr<SuffixTreeEdge> new_edge = make_shared<SuffixTreeEdge>(firstCharIndex,
            firstCharIndex + suffix.lastCharIndex - suffix.firstCharIndex,
            suffix.originNode,
            this->containingTree);
    this->containingTree->insert(new_edge);
    new_edge->endNode->suffix_node = suffix.originNode;
    firstCharIndex += suffix.lastCharIndex - suffix.firstCharIndex + 1;
    startNode = new_edge->endNode;
    this->containingTree->insert(thisShared);
    return new_edge->endNode;
}


///
/// A suffix in the tree is denoted by a Suffix structure
/// that denotes its last character.  The canonical
/// representation of a suffix for this algorithm requires
/// that the origin_node by the closest node to the end
/// of the tree.  To force this to be true, we have to
/// slide down every edge in our current path until we
/// reach the final node.
void SuffixTree::Suffix::canonize()
{
    if ( !isExplicit() ) {
        shared_ptr<SuffixTreeEdge> edge = containingTree.getEdge( originNode, containingTree.text.at(firstCharIndex) );
        int edge_span = edge->lastCharIndex - edge->firstCharIndex;
        while ( edge_span <= ( lastCharIndex - firstCharIndex ) ) {
            firstCharIndex = firstCharIndex + edge_span + 1;
            originNode = edge->endNode;
            if ( firstCharIndex <= lastCharIndex ) {
               edge = containingTree.getEdge( edge->endNode, containingTree.text.at(firstCharIndex) );
                edge_span = edge->lastCharIndex - edge->firstCharIndex;
            };
        }
    }
}


SuffixTree::SuffixTree(std::string&& text) :
        text(text),
        rootNode(make_shared<SuffixTreeNode>()),
        goodSuffixes(text.length())
{
    rootNode->root = true;
}

SuffixTree::SuffixTree(const std::string& text) :
        text(text),
        rootNode(make_shared<SuffixTreeNode>()),
        goodSuffixes(text.length())
{
    rootNode->root = true;
}

shared_ptr<SuffixTreeNode>& SuffixTree::getRootNode() {
    return rootNode;
}

//quoth a wizard, "Don't copy shared_ptr, if you don't need to."
void SuffixTree::insert(shared_ptr<SuffixTreeEdge>& edge) {
    edgeTable.insert(make_pair(
                         make_pair(edge->startNode, text.at(edge->firstCharIndex)),
                         edge));
}

void SuffixTree::remove(SuffixTreeEdge& edge) {
    //I don't know why, but the stupid temporary is required for this to compile
    std::pair<shared_ptr<SuffixTreeNode>, char> key = make_pair(edge.startNode, text.at(edge.firstCharIndex));
    edgeTable.erase(key);
}

shared_ptr<SuffixTreeEdge> SuffixTree::getEdge( shared_ptr<SuffixTreeNode>& node, char firstChar )
{
    auto foundEntry = edgeTable.find(make_pair(node, firstChar));
    if(foundEntry == edgeTable.end()) {
        return nullptr;
    } else {
        return foundEntry->second;
    }
}


//
// This routine prints out the contents of the suffix tree
// at the end of the program by walking through the
// hash table and printing out all used edges.  It
// would be really great if I had some code that will
// print out the tree in a graphical fashion, but I don't!
//

void SuffixTree::dump_edges()
{
    int current_n = text.length()-1;
    cout << "   Start      End       Suf     First Last  String\n";
    for ( auto& entry : edgeTable) {
        shared_ptr<SuffixTreeEdge> s = entry.second;
        if ( s->startNode == nullptr )
            continue;
        cout << setw( 9 ) << s->startNode << " "
             << setw( 9 ) << s->endNode << " "
             << setw( 9 ) << s->endNode->suffix_node << " "
             << setw( 5 ) << s->firstCharIndex << " "
             << setw( 6 ) << s->lastCharIndex << "  ";
        int top;
        if ( current_n > s->lastCharIndex )
            top = s->lastCharIndex;
        else
            top = current_n;
        for ( int l = s->firstCharIndex ;
                  l <= top;
                  l++ )
            cout << text.at(l);
        cout << "\n";
    }
}


//
// The active point is the first non-leaf suffix in the
// tree.  We start by setting this to be the empty string
// at node 0.  The addPrefix() function will update this
// value after every new prefix is added.
//
void SuffixTree::build() {
    Suffix active( getRootNode(), 0, -1, *this );  // The initial active prefix
    for ( size_t i = 0 ; i <= text.length()-1 ; i++ )
        addPrefix( active, i );
}


///
/// This routine constitutes the heart of the algorithm.
/// It is called repetitively, once for each of the prefixes
/// of the input string.  The prefix in question is denoted
/// by the index of its last character.
///
/// At each prefix, we start at the active point, and add
/// a new edge denoting the new last character, until we
/// reach a point where the new edge is not needed due to
/// the presence of an existing edge starting with the new
/// last character.  This point is the end point.
///
/// Luckily for us, the end point just happens to be the
/// active point for the next pass through the tree.  All
/// we have to do is update its last_char_index to indicate
/// that it has grown by a single character, and then this
/// routine can do all its work one more time.
///
void SuffixTree::addPrefix( Suffix &active, int last_char_index )
{
    shared_ptr<SuffixTreeNode> parent_node;
    shared_ptr<SuffixTreeNode> last_parent_node = nullptr;

    for ( ; ; ) {
        shared_ptr<SuffixTreeEdge> edge;
        parent_node = active.originNode;
//
// Step 1 is to try and find a matching edge for the given node.
// If a matching edge exists, we are done adding edges, so we break
// out of this big loop.
//
        if ( active.isExplicit() ) {
            edge = getEdge( active.originNode, text.at(last_char_index) );
            if ( edge != nullptr )
                break;
        } else { //implicit node, a little more complicated
            edge = getEdge( active.originNode, text.at(active.firstCharIndex) );
            int span = active.lastCharIndex - active.firstCharIndex;
            if ( text.at(edge->firstCharIndex + span + 1) == text.at(last_char_index) )
                break;
            parent_node = edge->splitEdge( active );
        }
//
// We didn't find a matching edge, so we create a new one, add
// it to the tree at the parent node position, and insert it
// into the hash table.  When we create a new node, it also
// means we need to create a suffix link to the new node from
// the last node we visited.
//
        shared_ptr<SuffixTreeEdge> new_edge = make_shared<SuffixTreeEdge>( last_char_index, text.length()-1, parent_node, this );
        this->insert(new_edge);
        if ( last_parent_node != nullptr )
            last_parent_node->suffix_node = parent_node;
        last_parent_node = parent_node;
//
// This final step is where we move to the next smaller suffix
//
        if ( active.originNode->isRoot() )
            active.firstCharIndex++;
        else
            active.originNode = active.originNode->suffix_node;
        active.canonize();
    }
    if ( last_parent_node != nullptr )
        last_parent_node->suffix_node = parent_node;
    active.lastCharIndex++;  //Now the endpoint is the next active point
    active.canonize();
};

//
// The validation code consists of two routines.  All it does
// is traverse the entire tree.  walk_tree() calls itself
// recursively, building suffix strings up as it goes.  When
// walk_tree() reaches a leaf node, it checks to see if the
// suffix derived from the tree matches the suffix starting
// at the same point in the input text.  If so, it tags that
// suffix as correct in the GoodSuffixes[] array.  When the tree
// has been traversed, every entry in the GoodSuffixes array should
// have a value of 1.
//
// In addition, the BranchCount[] array is updated while the tree is
// walked as well.  Every count in the array has the
// number of child edges emanating from that node.  If the node
// is a leaf node, the value is set to -1.  When the routine
// finishes, every node should be a branch or a leaf.  The number
// of leaf nodes should match the number of suffixes (the length)
// of the input string.  The total number of branches from all
// nodes should match the node count.
//
void SuffixTree::validate()
{

    shared_ptr<SuffixTreeNode> root = getRootNode();
    branchCount.insert(make_pair(root, 0));
    std::vector<char> validationString(text.size()+2);
    walk_tree( root, 0, validationString );
    int error = 0;
    for ( size_t i = 0 ; i < text.length()-1 ; i++ )
        if ( goodSuffixes[ i ] != 1 ) {
            cout << "Suffix " << i << " count wrong!\n";
            error++;
        }
    if ( error == 0 )
        cout << "All Suffixes present!\n";
    size_t leaf_count = 0;
    int branch_count = 0;
    for ( auto& countEntry : branchCount ) {
        if ( countEntry.second == 0 )
            cout << "Logic error on node "
                 << countEntry.first
                 << ", not a leaf or internal node!\n";
        else if ( countEntry.second == -1 )
            leaf_count++;
        else
            branch_count += countEntry.second;
    }
    cout << "Leaf count : "
         << leaf_count
         << ( leaf_count == ( text.length()-1 + 1 ) ? " OK" : " Error!" )
         << "\n";
    cout << "Branch count : "
         << branch_count
         << ( branch_count == (SuffixTreeNode::Count - 1) ? " OK" : " Error!" )
         << endl;
}

bool SuffixTree::walk_tree( shared_ptr<SuffixTreeNode>& start_node, int last_char_so_far, std::vector<char>& currentString )
{
    int edges = 0;
    for ( int i = 0 ; i < 256 ; i++ ) {
        shared_ptr<SuffixTreeEdge> edge = getEdge( start_node, i );
        if ( edge != nullptr ) {
            if ( branchCount[ edge->startNode ] < 0 )
                cerr << "Logic error on node "
                     << edge->startNode
                     << '\n';
            branchCount[ edge->startNode ]++;
            edges++;
            int l = last_char_so_far;
            for ( int j = edge->firstCharIndex ; j <= edge->lastCharIndex ; j++ ) {
                currentString.at(l++) = text.at(j);
            }
            currentString.at(l) = '\0';
            if ( walk_tree( edge->endNode, l , currentString) ) {
                if ( branchCount[ edge->endNode ] > 0 )
                        cerr << "Logic error on node "
                             << edge->endNode
                             << "\n";
                branchCount[ edge->endNode ]--;
            }
        }
    }
//
// If this node didn't have any child edges, it means we
// are at a leaf node, and can check on this suffix.  We
// check to see if it matches the input string, then tick
// off its entry in the GoodSuffixes list.
//
    if ( edges == 0 ) {
        char* currentStringCStyle = &currentString[0];
        cout << "Suffix : ";
        for ( int m = 0 ; m < last_char_so_far ; m++ )
            cout << currentString.at(m);
        cout << "\n";
        goodSuffixes.at(strlen( currentStringCStyle ) - 1 )++;
        cout << "comparing: " << ( text.c_str() + text.length() - strlen(currentStringCStyle) )
             << " to " << currentStringCStyle << endl;
        if ( strcmp(text.c_str() + text.length() - strlen(currentStringCStyle), currentStringCStyle ) != 0 )
            cout << "Comparison failure!\n";
        return 1;
    } else
        return 0;
}


