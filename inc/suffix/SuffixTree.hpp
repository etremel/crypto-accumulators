#ifndef SUFFIXTREE_H
#define SUFFIXTREE_H

#include <memory>
#include <unordered_map>
#include <string>
#include <vector>
#include <utils/StandardHashes.hpp>

//Forward declaration because there's more than one class in this header
class SuffixTreeNode;
class SuffixTreeEdge;


class SuffixTree {
        friend SuffixTreeEdge;
    public:
        std::string text;
        //Text is copied in
        SuffixTree(const std::string& text);
        //Move constructor
        SuffixTree(std::string&& text);
        /**
         * Builds the suffix tree from the text. This should be called after the
         * constructor and before calling any other methods.
         */
        void build();
        void dump_edges();
        /**
         * Inserts an edge into the suffix tree's table of edges
         * @param edge a pointer to an edge, by reference (the table contains pointers)
         */
        void insert(std::shared_ptr<SuffixTreeEdge>& edge);
        /**
         * Removes an edge from the suffix tree's table of edges
         * @param edge the edge to remove
         */
        void remove(SuffixTreeEdge& edge);
        /**
         * Looks for an edge with the given starting node and first character,
         * returning a pointer to it or NULL if the edge does not exist.
         * @param node a pointer to the desired edge's starting node
         * @param firstChar the first character of the substring represented by the desired edge
         * @return a shared_ptr to the edge, or NULL if such an edge does not exist
         */
        std::shared_ptr<SuffixTreeEdge> getEdge(std::shared_ptr<SuffixTreeNode>& node, char firstChar );
        std::shared_ptr<SuffixTreeNode>& getRootNode();
        void validate();
        bool walk_tree( std::shared_ptr<SuffixTreeNode>& start_node, int last_char_so_far, std::vector<char>& currentString);
    private:
        /**
         * When a new tree (edge??) is added to the table, we step through all the
         * currently defined suffixes from the active point to the end point.
         * This structure defines a Suffix by its final character. In the
         * canonical representation, we define that last character by starting
         * at a node in the tree, and following a string of characters,
         * represented by first_char_index and last_char_index.  The two
         * indices point into the input string.  Note that if a suffix ends at a
         * node, there are no additional characters needed to characterize its
         * last character position. When this is the case, we say the node is
         * Explicit, and set first_char_index > last_char_index to flag that.
         */
        class Suffix {
            public :
                Suffix( std::shared_ptr<SuffixTreeNode>& node, int start, int stop, SuffixTree& containingTree )
                : originNode( node ),
                  firstCharIndex( start ),
                  lastCharIndex( stop ),
                  containingTree(containingTree){};
                std::shared_ptr<SuffixTreeNode> originNode;
                int firstCharIndex;
                int lastCharIndex;
                bool isExplicit(){ return firstCharIndex > lastCharIndex; }
                bool isImplicit(){ return lastCharIndex >= firstCharIndex; }
                void canonize();
            private:
                SuffixTree& containingTree;
        };

        //Outside users shouldn't need to know about active suffixes
        void addPrefix( SuffixTree::Suffix &active, int last_char_index );
        /**
         * Key: (pointer to Node, character). Value: pointer to Edge with that ending node and first character
         */
        std::unordered_map<std::pair<std::shared_ptr<SuffixTreeNode>, char>, std::shared_ptr<SuffixTreeEdge>> edgeTable;
        std::shared_ptr<SuffixTreeNode> rootNode;
        //validation shit
        std::vector<char> goodSuffixes;
        std::unordered_map<std::shared_ptr<SuffixTreeNode>, char> branchCount;
};

/**
 * The suffix tree is made up of edges connecting nodes.
 * Each edge represents a string of characters starting
 * at first_char_index and ending at last_char_index.
 */
class SuffixTreeEdge {
        friend SuffixTree;
    public :
        int firstCharIndex;
        int lastCharIndex;
        std::shared_ptr<SuffixTreeNode> startNode;
        std::shared_ptr<SuffixTreeNode> endNode;
        SuffixTreeEdge();
        SuffixTreeEdge( int initFirstCharIndex,
              int initLastCharIndex,
              std::shared_ptr<SuffixTreeNode>& parentNode,
              SuffixTree* containingTree);
    private:
        /**
         * Splits this edge at the point defined by the suffix, creating a new
         * edge that descends from this edge's parent and has this edge as a
         * child.
         * @param suffix a suffix that ends in the middle of this edge
         * @return a pointer to the end node of the new edge
         */
        std::shared_ptr<SuffixTreeNode> splitEdge( SuffixTree::Suffix &suffix );
        /**
         * A pointer back to the tree containing this edge. No need for memory
         * management because the tree may exist after the edge is deleted, and
         * the edge will not exist after the tree is deleted.
         */
        SuffixTree* containingTree;
};

///
/// The only information contained in a node is the
/// suffix link. Each suffix in the tree that ends
/// at a particular node can find the next smaller suffix
/// by following the suffix_node link to a new node.  Nodes
/// are stored in a simple array.
///
class SuffixTreeNode {
        friend SuffixTree;
    public :
        std::shared_ptr<SuffixTreeNode> suffix_node;
        SuffixTreeNode();
        bool isRoot() { return root;}
    private :
        bool root;
        static int Count;
};

#endif // SUFFIXTREE_H
