#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm>
#include <queue>
#include <climits>

const char PATH_TO_DICT[] = "word_rus.txt";
typedef std::vector<std::string> dict_t;

dict_t file_to_dict(std::string const& path) {
    std::ifstream file(PATH_TO_DICT);
    dict_t dict;
    while(not file.eof()) {
        dict.push_back(std::string());
        file >> dict.back();
    }
    file.close();
    return std::move(dict);
}

unsigned words_diff_count(std::string const& rhs, std::string const& lhs) {
    unsigned int size = rhs.size();
    assert(size == lhs.size());

    int diff = 0;
    for(unsigned i=0; i<size; i++) {
        diff += int(rhs[i] != lhs[i]);
    }
    return diff;
}


bool words_has_link(std::string const& rhs, std::string const& lhs) {
    return words_diff_count(rhs, lhs) == 1;
}

struct Node {
    typedef std::vector<Node> V;
    std::vector<V::const_iterator> links;
    std::string data;
    operator std::string const&() const { return data; }
    Node(std::string const& s) : data(s) {}

    static Node::V make_graph(std::vector<std::string> const& dict, unsigned n) {
        Node::V nodes;
        for(auto& word : dict) {
            if(word.size() == n) {
                nodes.push_back(word);
            }
        }
        unsigned size = nodes.size();
        for(unsigned i=0; i<size; i++) {
            for(unsigned j=i+1; j<size; j++) {
                if(words_has_link(nodes[i], nodes[j])) {
                    nodes[i].links.push_back(nodes.cbegin() + j);
                    nodes[j].links.push_back(nodes.cbegin() + i);
                }
            }
        }
        return std::move(nodes);
    }
};

int main() {
    using std::cout;
    using std::endl;

    setlocale(LC_ALL, "Russian");
    auto dict = file_to_dict(PATH_TO_DICT);
    auto nodes = Node::make_graph(dict, 4);
    typedef Node::V::const_iterator NodeIterator;
    std::vector<bool> visited(nodes.size(), false);
    std::vector<unsigned> prev(nodes.size(), UINT_MAX);

    auto is_fly = [](Node const& n) -> bool { return n.data == "муха"; };
    auto is_target = [](Node::V::const_iterator it) -> bool { return it->data == "слон"; };
    NodeIterator fly = std::find_if(nodes.begin(), nodes.end(), is_fly);
    std::queue<NodeIterator> queue;
    queue.push(fly);
    unsigned pos = std::distance(nodes.cbegin(), fly);
    cout << "pos: " << pos << " "<< nodes.size() << endl;
    visited[pos] = true;

    unsigned cur_pos, linked_pos;
    NodeIterator cur, target(nodes.cend());
    while(not queue.empty()) {
        cur = queue.front();
        cur_pos = std::distance(nodes.cbegin(), cur);
        queue.pop();

        for(NodeIterator linked_node : cur->links) {
//            cout << ">> " << linked_node->data << endl;
            linked_pos = std::distance(nodes.cbegin(), linked_node);
            prev[linked_pos] = cur_pos;
//            if(is_target(linked_node)) {
            if(0) {
                target = linked_node;
                cout << "Find " << target->data << endl;
                break;
            }

            if(not visited[linked_pos]) {
                visited[linked_pos] = true;
                queue.push(linked_node);
            }
        }
    }
    if(cur != nodes.cend()) {
            unsigned i = cur_pos;
            while(i != UINT_MAX) {
                cout << nodes[i].data << endl;
                i = prev[i];
            }
    }
    else cout << "Not found" << endl;


    return 0;
}
