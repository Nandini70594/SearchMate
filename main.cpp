#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <limits>

using namespace std;

struct Topic{
    string title;
    string shortDesc;
    string detailDesc;
    string imagePath;
    string keywords;
};

struct TrieNode{
    TrieNode* children[26];
    bool isEnd;

    TrieNode(){
        isEnd = false;
        for(int i = 0; i < 26; i++)
            children[i] = NULL;
    }
};

string toLower(string s){
    for(char &c : s) c = tolower(c);
    return s;
}

string cleanWord(string word){
    string clean = "";
    for(char c : word){
        if(c >= 'a' && c <= 'z')
            clean += c;
    }
    return clean;
}

vector<string> tokenizeAndClean(string text){
    text = toLower(text);
    stringstream ss(text);
    string word;
    vector<string> tokens;

    while(ss >> word){
        word = cleanWord(word);
        if(!word.empty())
            tokens.push_back(word);
    }

    return tokens;
}

void insert(TrieNode* root, string word){
    TrieNode* node = root;

    for(char c : word){
        int index = c - 'a';
        if(index < 0 || index >= 26) continue;

        if(node->children[index] == NULL)
            node->children[index] = new TrieNode();

        node = node->children[index];
    }

    node->isEnd = true;
}

TrieNode* searchPrefix(TrieNode* root, string prefix){
    TrieNode* node = root;

    for(char c : prefix){
        int index = c - 'a';
        if(index < 0 || index >= 26) return NULL;

        if(node->children[index] == NULL)
            return NULL;

        node = node->children[index];
    }

    return node;
}

void getSuggestions(TrieNode* node, string prefix, int &count){
    if(node == NULL || count >= 5) return;

    if(node->isEnd){
        cout << prefix << endl;
        count++;
    }

    for(int i = 0; i < 26; i++){
        if(node->children[i] != NULL){
            getSuggestions(node->children[i], prefix + char('a' + i), count);
        }
    }
}

int editDistance(string a, string b){
    int n = a.size(), m = b.size();
    vector<vector<int>> dp(n + 1, vector<int>(m + 1));

    for(int i = 0; i <= n; i++) dp[i][0] = i;
    for(int j = 0; j <= m; j++) dp[0][j] = j;

    for(int i = 1; i <= n; i++){
        for(int j = 1; j <= m; j++){
            if(a[i - 1] == b[j - 1])
                dp[i][j] = dp[i - 1][j - 1];
            else
                dp[i][j] = 1 + min(min(dp[i - 1][j], dp[i][j - 1]), dp[i - 1][j - 1]);
        }
    }

    return dp[n][m];
}

void addToInvertedIndex(unordered_map<string, vector<int>> &invertedIndex, const string &word, int topicIndex){
    vector<int> &vec = invertedIndex[word];
    for(int id : vec){
        if(id == topicIndex) return;
    }
    vec.push_back(topicIndex);
}

int main(){

    ifstream file("topics.txt");
    string line;
    vector<Topic> topics;

    if(!file){
        cout << "File not found" << endl;
        return 0;
    }

    while(getline(file, line)){
        if(line.empty()) continue;

        stringstream ss(line);
        Topic t;

        getline(ss, t.title, '|');
        getline(ss, t.shortDesc, '|');
        getline(ss, t.detailDesc, '|');
        getline(ss, t.imagePath, '|');
        getline(ss, t.keywords, '|');

        topics.push_back(t);
    }

    file.close();

    TrieNode* root = new TrieNode();
    unordered_map<string, vector<int>> invertedIndex;
    set<string> uniqueKeywordSet;

    vector<vector<string>> topicTokens(topics.size());

    for(int i = 0; i < (int)topics.size(); i++){
        vector<string> titleWords = tokenizeAndClean(topics[i].title);
        vector<string> keywordWords = tokenizeAndClean(topics[i].keywords);

        set<string> mergedUnique;

        for(string word : titleWords) mergedUnique.insert(word);
        for(string word : keywordWords) mergedUnique.insert(word);

        for(string word : mergedUnique){
            topicTokens[i].push_back(word);
            uniqueKeywordSet.insert(word);

            insert(root, word);
            addToInvertedIndex(invertedIndex, word, i);
        }
    }

    vector<string> allKeywords(uniqueKeywordSet.begin(), uniqueKeywordSet.end());

    vector<vector<int>> graph(topics.size());

    for(int i = 0; i < (int)topics.size(); i++){
        set<string> setA(topicTokens[i].begin(), topicTokens[i].end());

        for(int j = i + 1; j < (int)topics.size(); j++){
            int common = 0;
            for(string word : topicTokens[j]){
                if(setA.count(word)) common++;
            }

            if(common > 0){
                graph[i].push_back(j);
                graph[j].push_back(i);
            }
        }
    }

    cout << "Total topics loaded: " << topics.size() << endl;

    int choice;

    do{
        cout << "\n===== MENU =====\n";
        cout << "1. Show All Topics\n";
        cout << "2. Autocomplete Suggestions\n";
        cout << "3. Search Topics\n";
        cout << "4. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if(choice == 1){
            for(int i = 0; i < (int)topics.size(); i++){
                cout << "\nTopic: " << topics[i].title << endl;
                cout << "Short Description: " << topics[i].shortDesc << endl;
                cout << "Detailed Description: " << topics[i].detailDesc << endl;
                cout << "-----------------------------------" << endl;
            }
        }

        else if(choice == 2){
            string input;
            cout << "Enter prefix: ";
            getline(cin, input);

            vector<string> words = tokenizeAndClean(input);

            if(words.empty()){
                cout << "Please enter something" << endl;
                continue;
            }

            string lastWord = words.back();

            TrieNode* node = searchPrefix(root, lastWord);

            if(node == NULL){
                cout << "No suggestions found." << endl;
            }
            else{
                cout << "\nSuggestions:\n";
                int count = 0;
                getSuggestions(node, lastWord, count);
            }
        }

        else if(choice == 3){
            string query;
            cout << "Enter search query: ";
            getline(cin, query);

            vector<string> queryWords = tokenizeAndClean(query);

            if(queryWords.empty()){
                cout << "Please enter something valid." << endl;
                continue;
            }

            map<int, int> score;

            for(string word : queryWords){

                if(invertedIndex.count(word)){
                    for(int topicId : invertedIndex[word]){
                        score[topicId] += 5;
                    }
                }

                for(string key : allKeywords){
                    if(key == word) continue;

                    int dist = editDistance(word, key);
                    if(dist <= 2){
                        for(int topicId : invertedIndex[key]){
                            score[topicId] += 2;
                        }
                    }
                }

                TrieNode* node = searchPrefix(root, word);
                if(node != NULL){
                    score[-1] += 0;
                }
            }

            if(score.empty()){
                cout << "No results found." << endl;
                continue;
            }

            priority_queue<pair<int,int>> pq;

            for(auto &entry : score){
                pq.push({entry.second, entry.first});
            }

            cout << "\nTop Results:\n";
            int shown = 0;

            while(!pq.empty() && shown < 5){
                int topicIndex = pq.top().second;
                int topicScore = pq.top().first;
                pq.pop();

                cout << "\nTopic: " << topics[topicIndex].title << endl;
                cout << "Short Description: " << topics[topicIndex].shortDesc << endl;
                cout << "Score: " << topicScore << endl;

                if(!graph[topicIndex].empty()){
                    cout << "Related Topics: ";
                    int relCount = 0;
                    for(int rel : graph[topicIndex]){
                        cout << topics[rel].title;
                        relCount++;
                        if(relCount >= 3) break;
                        if(relCount < 3 && relCount < (int)graph[topicIndex].size()) cout << ", ";
                    }
                    cout << endl;
                }

                shown++;
            }
        }

        else if(choice == 4){
            cout << "Exiting program..." << endl;
        }

        else{
            cout << "Invalid choice!" << endl;
        }

    }while(choice != 4);

    return 0;
}
