#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm>
#include <queue>
#include <climits>

const char PATH_TO_DICT[] = "word_rus_2.txt";
typedef std::vector<std::string> dict_t;

const std::string SOURCE_WORD("муха"), TARGET_WORD("слон");

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
        bool flag = false;
        flag |= (rhs[i] == 'е' and lhs[i] == 'ё');
        flag |= (rhs[i] == 'ё' and lhs[i] == 'е');

//        flag |= (rhs[i] == 'и' and lhs[i] == 'й');
//        flag |= (rhs[i] == 'й' and lhs[i] == 'и');

        diff -= (int)flag;
    }
    return diff;
}


bool words_has_link(std::string const& rhs, std::string const& lhs) {
    return words_diff_count(rhs, lhs) == 1;
}

struct Node {
    unsigned index;
    unsigned cc;
    typedef std::vector<Node> V;
    std::vector<unsigned> link;
    std::string data;
    operator std::string const&() const { return data; }
    Node(std::string const& s) : data(s) {}

    bool operator==(std::string const& s) const { return data == s; }

    static Node::V make_graph(std::vector<std::string> const& dict, unsigned n) {
        Node::V nodes;
        for(auto& word : dict) {
            if(word.size() == n) {
                nodes.push_back(word);
            }
        }
        const unsigned size = nodes.size();
        for(unsigned i=0; i<size; i++) {
            nodes[i].index = i;
            for(unsigned j=i+1; j<size; j++) {
                if(words_has_link(nodes[i], nodes[j])) {
                    nodes[i].link.push_back(j);
                    nodes[j].link.push_back(i);
                }
            }
        }

        unsigned ccnum = 0;
        std::vector<bool> visited(size, false);

        std::function<void(unsigned id, unsigned cc)> bfs =
        [&](unsigned id, unsigned cc) {
            if(not visited[id]) {
                visited[id] = true;
                nodes[id].cc = cc;
                for(unsigned nearest : nodes[id].link) {
                    bfs(nearest, cc);
                }
            }
        };

        for(unsigned i=0; i<size; i++) {
            if(not visited[i]) {
                bfs(i, ccnum);
                ccnum++;
            }
        }

        std::cout << "ccnum: " << ccnum << std::endl;
        std::cout << "size: " << size << std::endl;

        return std::move(nodes);
    }
};

int main() {
    using std::cout;
    using std::endl;

    setlocale(LC_ALL, "Russian");
    auto dict = file_to_dict(PATH_TO_DICT);
    std::vector<Node> nodes = Node::make_graph(dict, 4);
    std::vector<bool> visited(nodes.size(), false);
    std::vector<unsigned> prev(nodes.size(), UINT_MAX);


    auto source = std::find_if(nodes.begin(), nodes.end(), [](Node const& n) -> bool { return n == SOURCE_WORD; });
    auto target = std::find_if(nodes.begin(), nodes.end(), [](Node const& n) -> bool { return n == TARGET_WORD; });
    std::queue<unsigned> queue;
    queue.push(source->index);
    visited[source->index] = true;

    unsigned current;
    while(not queue.empty()) {
        current = queue.front();
        queue.pop();
        for(unsigned nearest : nodes[current].link) {
            if(not visited[nearest]) {
                queue.push(nearest);
                visited[nearest] = true;
                prev[nearest] = current;
            }
        }
    }

    if(not visited[target->index]) {
        cout << "We have not a path from \"" << SOURCE_WORD << "\" to \"" << TARGET_WORD << "\"." << endl;
        cout << "This words have different connected component: ";
        cout << source->cc << " and " << target->cc << endl;
    }
    else {
        cout << "We have a path from \"" << SOURCE_WORD << "\" to \"" << TARGET_WORD << "\"." << endl;
        current = target->index;
        while(current != UINT_MAX) {
            cout << nodes[current].data << endl;
            current = prev[current];
        }
    }

    return 0;
}
